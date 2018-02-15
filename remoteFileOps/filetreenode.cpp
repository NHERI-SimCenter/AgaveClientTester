/*********************************************************************************
**
** Copyright (c) 2017 The University of Notre Dame
** Copyright (c) 2017 The Regents of the University of California
**
** Redistribution and use in source and binary forms, with or without modification,
** are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice, this
** list of conditions and the following disclaimer.
**
** 2. Redistributions in binary form must reproduce the above copyright notice, this
** list of conditions and the following disclaimer in the documentation and/or other
** materials provided with the distribution.
**
** 3. Neither the name of the copyright holder nor the names of its contributors may
** be used to endorse or promote products derived from this software without specific
** prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
** EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
** SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
** TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
** BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
** IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
** SUCH DAMAGE.
**
***********************************************************************************/

// Contributors:
// Written by Peter Sempolinski, for the Natural Hazard Modeling Laboratory, director: Ahsan Kareem, at Notre Dame

#include "filetreenode.h"

#include "../utilFuncs/linkedstandarditem.h"

#include "../AgaveClientInterface/filemetadata.h"
#include "../AgaveClientInterface/remotedatainterface.h"

FileTreeNode::FileTreeNode(FileMetaData contents, FileTreeNode * parent):QObject((QObject *)parent)
{
    fileData = new FileMetaData(contents);
    myParent = parent;
    parent->childList.append(this);

    QObject::connect(this, SIGNAL(fileSystemChanged()), myParent, SLOT(fileTreeChanged()));

    getModelLink();
    addUpdateModelNodes();

    if (isFolder())
    {
        myLoadingNode = new LinkedStandardItem(this,"Loading . . .");
        myModelNodes.at(0)->appendRow(myLoadingNode);
    }
}

FileTreeNode::FileTreeNode(QStandardItemModel * stdModel, QString rootFolderName, QObject * parent):QObject((QObject *)parent)
{
    //This constructor is only to create the root node
    myModel = stdModel;
    fileData = new FileMetaData();

    QString fullPath = "/";
    fullPath = fullPath.append(rootFolderName);

    fileData->setFullFilePath(fullPath);
    fileData->setType(FileType::DIR);

    addUpdateModelNodes();

    myLoadingNode = new LinkedStandardItem(this,"Loading . . .");
    myModelNodes.at(0)->appendRow(myLoadingNode);
    displayNode();
}

FileTreeNode::~FileTreeNode()
{
    while (this->childList.size() > 0)
    {
        FileTreeNode * toDelete = this->childList.takeLast();
        delete toDelete;
    }
    if (fileData != NULL)
    {
        delete fileData;
    }
    if (this->fileDataBuffer != NULL)
    {
        delete this->fileDataBuffer;
    }
    //TODO: Verify order of loseing model elements on model teardown
    if (myLoadingNode != NULL)
    {
        delete myLoadingNode;
    }
    if (myEmptyNode != NULL)
    {
        delete myEmptyNode;
    }
    while (!myModelNodes.isEmpty())
    {
        QStandardItem * toDelete = myModelNodes.takeLast();
        delete toDelete; //This should also remove it from model
    }
    if (myParent != NULL)
    {
        if (myParent->childList.contains(this))
        {
            myParent->childList.removeAll(this);
        }
    }
}

bool FileTreeNode::isRootNode()
{
    return (myParent == NULL);
}

bool FileTreeNode::nodeIsDisplayed()
{
    if (myParent == NULL)
    {
        return true;
    }
    if (myModelNodes.isEmpty())
    {
        return false;
    }
    if (myParent->nodeIsDisplayed() == false)
    {
        return false;
    }

    LinkedStandardItem * parentNode = myParent->myModelNodes.at(0);
    if (!parentNode->hasChildren())
    {
        return false;
    }
    if (parentNode == myModelNodes.at(0)->parent())
    {
        return true;
    }
    return false;
}

void FileTreeNode::displayNode()
{
    if ((myParent != NULL) && !myParent->nodeIsDisplayed())
    {
        myParent->displayNode();
    }
    addUpdateModelNodes();
    QList<QStandardItem *> appendList;
    for (auto itr = myModelNodes.cbegin(); itr != myModelNodes.cend(); itr++)
    {
        appendList.append(*itr);
    }
    if (myParent == NULL)
    {
        myModel->appendRow(appendList);
    }
    else
    {
        myParent->myModelNodes.at(0)->appendRow(appendList);
    }
}

NodeState FileTreeNode::getNodeState()
{
    if (isFolder())
    {
        if (haveLStask())
        {
            if (!nodeIsDisplayed())
            {
                return NodeState::FOLDER_SPECULATE;
            }

            if (!childList.isEmpty())
            {
                return NodeState::FOLDER_CONTENTS_RELOADING;
            }

            return NodeState::FOLDER_CONTENTS_LOADING;
        }
        else
        {
            if (myLoadingNode != NULL)
            {
                return NodeState::FOLDER_KNOWN_CONTENTS_NOT;
            }
            return NodeState::FOLDER_CONTENTS_LOADED;
        }
    }
    else
    {
        if (haveBuffTask())
        {
            if (!nodeIsDisplayed())
            {
                return NodeState::FILE_SPECULATE;
            }

            if (fileDataBuffer != NULL)
            {
                return NodeState::FILE_BUFF_RELOADING;
            }

            return NodeState::FILE_BUFF_LOADING;
        }
        else
        {
            if (fileDataBuffer == NULL)
            {
                return NodeState::FILE_KNOWN;
            }

            return NodeState::FILE_BUFF_LOADED;
        }
    }
    return NodeState::ERROR;
}

FileMetaData FileTreeNode::getFileData()
{
    return *fileData;
}

QByteArray * FileTreeNode::getFileBuffer()
{
    return fileDataBuffer;
}

QList<LinkedStandardItem *> FileTreeNode::getModelNodes()
{
    return myModelNodes;
}

FileTreeNode * FileTreeNode::getNodeWithName(QString filename)
{
    return pathSearchHelper(filename,false);
}

FileTreeNode * FileTreeNode::getClosestNodeWithName(QString filename)
{
    return pathSearchHelper(filename,true);
}

FileTreeNode * FileTreeNode::getParentNode()
{
    return myParent;
}

void FileTreeNode::setFileBuffer(QByteArray * newFileBuffer)
{
    if (fileDataBuffer == NULL)
    {
        if (newFileBuffer != NULL)
        {
            fileDataBuffer = new QByteArray(*newFileBuffer);
            fileTreeChanged();
        }
        return;
    }

    if (newFileBuffer == NULL)
    {
        delete fileDataBuffer;
        fileDataBuffer = NULL;
        fileTreeChanged();
        return;
    }

    if ((fileDataBuffer->length() == newFileBuffer->length()) && (fileDataBuffer->endsWith(*newFileBuffer)))
    {
        return;
    }

    delete fileDataBuffer;
    fileDataBuffer = new QByteArray(*newFileBuffer);
    fileTreeChanged();
}

bool FileTreeNode::haveLStask()
{
    return (lsTask != NULL);
}

void FileTreeNode::setLStask(RemoteDataReply * newTask, bool clearData)
{
    if (lsTask != NULL)
    {
        QObject::disconnect(lsTask, 0, this, 0);
    }
    lsTask = newTask;
    QObject::connect(lsTask, SIGNAL(haveLSReply(RequestState,QList<FileMetaData>*)),
                     this, SLOT(deliverLSdata(RequestState,QList<FileMetaData>*)));

    if (clearData)
    {
        clearAllChildren();
    }
}

bool FileTreeNode::haveBuffTask()
{
    return (bufferTask != NULL);
}

void FileTreeNode::setBuffTask(RemoteDataReply * newTask)
{
    if (bufferTask != NULL)
    {
        QObject::disconnect(bufferTask, 0, this, 0);
    }
    bufferTask = newTask;
    QObject::connect(bufferTask, SIGNAL(haveBufferDownloadReply(RequestState,QByteArray*)),
                     this, SLOT(deliverBuffData(RequestState,QByteArray*)));
}

QList<FileTreeNode *> FileTreeNode::getChildList()
{
    return childList;
}

FileTreeNode * FileTreeNode::getChildNodeWithName(QString filename)
{
    for (auto itr = this->childList.begin(); itr != this->childList.end(); itr++)
    {
        FileMetaData toSearch = (*itr)->getFileData();
        if ((toSearch.getFileName() == filename) && (toSearch.getFileType() != FileType::INVALID))
        {
            return (*itr);
        }
    }
    return NULL;
}

bool FileTreeNode::fileNameMatches(QString fileToMatch)
{
    FileTreeNode * rootNode = this;
    while (rootNode->isRootNode() == false)
    {
        rootNode = rootNode->getParentNode();
    }

    FileTreeNode * checkNode = rootNode->getNodeWithName(fileToMatch);
    if (checkNode == NULL)
    {
        return false;
    }
    return (checkNode == this);
}

bool FileTreeNode::isFolder()
{
    if (fileData == NULL)
    {
        return false;
    }
    if (fileData->getFileType() == FileType::DIR)
    {
        return true;
    }
    return false;
}

void FileTreeNode::deliverLSdata(RequestState taskState, QList<FileMetaData>* dataList)
{
    if (lsTask == QObject::sender())
    {
        lsTask = NULL;
    }
    if (taskState != RequestState::GOOD)
    {
        return;
    }

    if (verifyControlNode(dataList) == false)
    {
        qDebug("ERROR: File tree data/node mismatch");
        return;
    }
    this->updateFileNodeData(dataList);
}

void FileTreeNode::deliverBuffData(RequestState taskState, QByteArray * bufferData)
{
    if (bufferTask == QObject::sender())
    {
        bufferTask = NULL;
    }
    if (taskState != RequestState::GOOD)
    {
        return;
    }

    qDebug("Download of buffer complete: %s", qPrintable(fileData->getFullPath()));

    if ((bufferData != NULL) && !nodeIsDisplayed())
    {
        displayNode();
    }

    setFileBuffer(bufferData);
}

void FileTreeNode::fileTreeChanged()
{
    emit fileSystemChanged();
}

void FileTreeNode::getModelLink()
{
    if (myModel == NULL)
    {
        if (myParent == NULL)
        {
            return;
        }
        myModel = myParent->myModel;
    }
}

FileTreeNode * FileTreeNode::pathSearchHelper(QString filename, bool stopEarly)
{//TODO: I am worried about how generic this function is.
    //Our current agave setup has a named root folder
    if (isRootNode() == false) return NULL;

    QStringList filePathParts = FileMetaData::getPathNameList(filename);
    FileTreeNode * searchNode = this;

    QString rootName = filePathParts.takeFirst();
    if (rootName != searchNode->getFileData().getFileName())
    {
        return NULL;
    }

    for (auto itr = filePathParts.cbegin(); itr != filePathParts.cend(); itr++)
    {
        FileTreeNode * nextNode = searchNode->getChildNodeWithName(*itr);
        if (nextNode == NULL)
        {
            if (stopEarly == true)
            {
                return searchNode;
            }
            return NULL;
        }
        searchNode = nextNode;
    }

    return searchNode;
}

bool FileTreeNode::verifyControlNode(QList<FileMetaData> * newDataList)
{
    QString controllerAddress = getControlAddress(newDataList);
    if (controllerAddress.isEmpty()) return false;

    FileTreeNode * myRootNode = this;
    while (myRootNode->myParent != NULL)
    {
        myRootNode = myRootNode->myParent;
    }

    FileTreeNode * controllerNode = myRootNode->getNodeWithName(controllerAddress);
    return (controllerNode == this);
}

QString FileTreeNode::getControlAddress(QList<FileMetaData> * newDataList)
{
    for (auto itr = newDataList->cbegin(); itr != newDataList->cend(); itr++)
    {
        if ((*itr).getFileName() == ".")
        {
            return (*itr).getContainingPath();
        }
    }

    return "";
}

void FileTreeNode::updateFileNodeData(QList<FileMetaData> * newDataList)
{
    if (myLoadingNode != NULL)
    {
        delete myLoadingNode;
        myLoadingNode = NULL;
    }

    if (myEmptyNode != NULL)
    {
        delete myEmptyNode;
        myEmptyNode = NULL;
    }

    //If the incoming list is empty, ie. has one entry (.), place empty file item
    if (newDataList->size() <= 1)
    {
        clearAllChildren();
        myLoadingNode = new LinkedStandardItem(this, "Empty Folder");
        fileTreeChanged();
        return;
    }

    purgeUnmatchedChildren(newDataList);

    for (auto itr = newDataList->begin(); itr != newDataList->end(); itr++)
    {
        insertFile(&(*itr));
    }

    if (newDataList->isEmpty())
    {
        myEmptyNode = new LinkedStandardItem(this,"Empty Folder");
        myModelNodes.at(0)->appendRow(myEmptyNode);
    }

    if (!nodeIsDisplayed())
    {
        displayNode();
    }

    for (auto itr = childList.begin(); itr != childList.end(); itr++)
    {
        (*itr)->displayNode();
    }

    fileTreeChanged();
}

void FileTreeNode::clearAllChildren()
{
    while (!childList.isEmpty())
    {
        FileTreeNode * aChild = childList.takeLast();
        delete aChild;
    }
}

void FileTreeNode::insertFile(FileMetaData * newData)
{
    if (newData->getFileName() == ".") return;

    for (auto itr = childList.begin(); itr != childList.end(); itr++)
    {
        if ((*newData) == (*itr)->getFileData())
        {
            return;
        }
    }

    new FileTreeNode(*newData,this);
}

void FileTreeNode::purgeUnmatchedChildren(QList<FileMetaData> * newChildList)
{
    if (childList.size() == 0) return;

    QList<FileTreeNode *> altList;

    while (!childList.isEmpty())
    {
        FileTreeNode * aNode = childList.takeLast();
        FileMetaData toCheck = aNode->getFileData();

        bool matchFound = false;

        for (auto itr = newChildList->begin(); itr != newChildList->end() && (matchFound == false); ++itr)
        {
            if ((*itr).getFileName() == ".") continue;

            if ((*itr) == toCheck)
            {
                matchFound = true;
            }
        }

        if (matchFound)
        {
            altList.append(aNode);
        }
        else
        {
            delete aNode;
        }
    }

    while (!altList.isEmpty())
    {
        childList.append(altList.takeLast());
    }
}

QString FileTreeNode::getRawColumnData(int i, QStandardItemModel * fullModel)
{
    if (fileData == NULL)
    {
        return "";
    }

    QStandardItem * headerItem = fullModel->horizontalHeaderItem(i);
    if (headerItem == NULL)
    {
        return "";
    }
    QString headerText = headerItem->text();
    if (headerText == "File Name")
    {
        return fileData->getFileName();
    }
    if (headerText == "Type")
    {
        return fileData->getFileTypeString();
    }
    if (headerText == "Size")
    {
        return QString::number(fileData->getSize());
    }
    return "";
}

void FileTreeNode::addUpdateModelNodes()
{
    for (int i = 0; i < myModel->columnCount(); i++)
    {
        QString dataText = getRawColumnData(i, myModel);
        if (myModelNodes.size() <= i)
        {
            myModelNodes.append(new LinkedStandardItem(this, dataText));
        }
        else
        {
            myModelNodes.at(i)->setText(dataText);
        }
    }


}
