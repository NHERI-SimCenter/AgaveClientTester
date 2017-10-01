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

#include "remotefiletree.h"

#include "../AgaveClientInterface/remotedatainterface.h"
#include "../AgaveClientInterface/filemetadata.h"

#include "fileoperator.h"
#include "filetreenode.h"

RemoteFileTree::RemoteFileTree(QWidget *parent) :
    QTreeView(parent)
{
    QObject::connect(this, SIGNAL(expanded(QModelIndex)), this, SLOT(folderExpanded(QModelIndex)));
    QObject::connect(this, SIGNAL(clicked(QModelIndex)), this, SLOT(fileEntryTouched(QModelIndex)));

    selectedItem = NULL;
}

RemoteFileTree::~RemoteFileTree()
{
    //TODO: Delete entries in the file data tree
}

void RemoteFileTree::setFileOperator(FileOperator * theOperator)
{
    myFileOperator = theOperator;
    myFileOperator->linkToFileTree(this);
}

FileOperator * RemoteFileTree::getFileOperator()
{
    return myFileOperator;
}

void RemoteFileTree::setSelectedLabel(QLabel * selectedFileDisp)
{
    selectedFileDisplay = selectedFileDisp;
}

FileTreeNode * RemoteFileTree::getSelectedNode()
{
    return selectedItem;
}

void RemoteFileTree::setupFileView()
{
    selectedItem = NULL;
    emit newFileSelected(NULL);
    //TODO: reconsider needed columns
    this->hideColumn((int)FileColumn::MIME_TYPE);
    this->hideColumn((int)FileColumn::PERMISSIONS);
    this->hideColumn((int)FileColumn::FORMAT);
    this->hideColumn((int)FileColumn::LAST_CHANGED);
    //TODO: Adjust column size defaults;
}

void RemoteFileTree::folderExpanded(QModelIndex itemOpened)
{
    fileEntryTouched(itemOpened);

    if (selectedItem == NULL) return;
    if (!selectedItem->childIsUnloaded()) return;

    myFileOperator->enactFolderRefresh(selectedItem);
}

void RemoteFileTree::fileEntryTouched(QModelIndex fileIndex)
{
    selectedItem = myFileOperator->getNodeFromIndex(fileIndex);

    if (selectedFileDisplay != NULL)
    {
        if (selectedItem == NULL)
        {
            selectedFileDisplay->setText("No File Selected.");
        }
        else
        {
            FileMetaData newFileData = selectedItem->getFileData();

            QString fileString = "Filename: %1\nType: %2\nSize: %3";
            fileString = fileString.arg(newFileData.getFileName(),
                                newFileData.getFileTypeString(),
                                QString::number(newFileData.getSize()));
            selectedFileDisplay->setText(fileString);
        }
    }

    emit newFileSelected(selectedItem);
}
