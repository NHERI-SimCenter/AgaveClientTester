/*********************************************************************************
**
** Copyright (c) 2018 The University of Notre Dame
** Copyright (c) 2018 The Regents of the University of California
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

#include "remotefilemodel.h"
#include "remotefileitem.h"

#include "../remoteFileOps/filenoderef.h"
#include "../remoteFileOps/fileoperator.h"
#include "../remoteFileOps/filetreenode.h"
#include "ae_globals.h"

RemoteFileModel::RemoteFileModel(QObject * parent) : QStandardItemModel(parent)
{
    QObject::connect(ae_globals::get_file_handle(), SIGNAL(fileSystemChange(FileNodeRef)),
                     this, SLOT(newFileData(FileNodeRef)));
}

void RemoteFileModel::newFileData(FileNodeRef newFileData)
{
    NodeState theState = newFileData.getNodeState();

    switch (theState) {
    case NodeState::DELETING:
    case NodeState::ERROR:
    case NodeState::NON_EXTANT:
        purgeItem(newFileData);
        return;
    case NodeState::FILE_BUFF_LOADED:
    case NodeState::FILE_BUFF_LOADING:
    case NodeState::FILE_BUFF_RELOADING:
    case NodeState::FILE_KNOWN:
    case NodeState::FOLDER_CONTENTS_LOADING:
    case NodeState::FOLDER_CONTENTS_RELOADING:
    case NodeState::FOLDER_KNOWN_CONTENTS_NOT:
        updateItem(newFileData);
        return;
    case NodeState::FOLDER_CONTENTS_LOADED:
        updateItem(newFileData,true);
        return;
    case NodeState::FILE_SPECULATE_IDLE:
    case NodeState::FILE_SPECULATE_LOADING:
    case NodeState::FOLDER_SPECULATE_IDLE:
    case NodeState::FOLDER_SPECULATE_LOADING:
    case NodeState::INIT:
    default:
        return;
    }
}

void RemoteFileModel::purgeItem(FileNodeRef toRemove)
{
    RemoteFileItem * targetItem = findItem(toRemove);
    if (targetItem == NULL) return;
    QStandardItem * parentNode = targetItem->parent();
    if (parentNode == NULL)
    {
        parentNode = this->invisibleRootItem();
    }

    parentNode->removeRow(targetItem->row());
    if (parentNode->hasChildren()) return;
    parentNode->appendRow(RemoteFileItem(true));
}

void RemoteFileModel::updateItem(FileNodeRef toUpdate, bool folderContentsLoaded)
{
    RemoteFileItem * targetItem = findItem(toUpdate);
    QList<RemoteFileItem *> itemList;
    if (targetItem == NULL)
    {
        RemoteFileItem * parentItem = findParentItem(toUpdate);
        if (parentItem == NULL) return;
        if (parentItem->parentOfPlaceholder())
        {
            while (parentItem->hasChildren())
            {
                parentItem->removeRow(0);
            }
        }

        targetItem = new RemoteFileItem(toUpdate);
        itemList.append(targetItem);
        while (itemList.size() < columnCount())
        {
            itemList.append(new RemoteFileItem(targetItem));
        }

        parentItem->appendRow(itemList);
    }
    else
    {
        itemList = targetItem->getRowList();
    }

    for (int i = 0; i < itemList.size(); i++)
    {
        QStandardItem * itemToUpdate = itemList.at(i);
        itemToUpdate->setText(getRawColumnData(itemToUpdate, i));
    }
    if (targetItem->hasChildren() || (toUpdate.getFileType() != FileType::DIR)) return;
    targetItem->appendRow(RemoteFileItem(!folderContentsLoaded));
}

RemoteFileItem * RemoteFileModel::findItem(FileNodeRef toFind)
{
    //TODO: Find item
}

RemoteFileItem * RemoteFileModel::findParentItem(FileNodeRef toFind)
{
    //TODO: Find parent of item, can be root
}

QString RemoteFileModel::getRawColumnData(FileNodeRef fileData, int i)
{
    if (fileData.isNil())
    {
        return "";
    }

    QStandardItem * headerItem = horizontalHeaderItem(i);
    if (headerItem == NULL)
    {
        return "";
    }
    QString headerText = headerItem->text();
    if (headerText == "File Name")
    {
        return fileData.getFileName();
    }
    if (headerText == "Type")
    {
        return fileData.getFileTypeString();
    }
    if (headerText == "Size")
    {
        return QString::number(fileData.getSize());
    }
    return "";
}
