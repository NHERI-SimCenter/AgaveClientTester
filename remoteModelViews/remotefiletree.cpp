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

#include "linkedstandarditem.h"
#include "../remoteFileOps/fileoperator.h"
#include "../remoteFileOps/filetreenode.h"
#include "../remoteFileOps/filenoderef.h"
#include "../ae_globals.h"
#include "../utilFuncs/agavesetupdriver.h"

RemoteFileTree::RemoteFileTree(QWidget *parent) :
    QTreeView(parent)
{
    ae_globals::get_file_handle()->linkToFileTree(this);

    QObject::connect(this, SIGNAL(expanded(QModelIndex)), this, SLOT(folderExpanded(QModelIndex)));
    QObject::connect(this, SIGNAL(clicked(QModelIndex)), this, SLOT(fileEntryTouched(QModelIndex)));
    this->setEditTriggers(QTreeView::NoEditTriggers);
}

FileNodeRef RemoteFileTree::getSelectedFile()
{
    FileNodeRef fail;
    QModelIndexList indexList = this->selectedIndexes();

    if (indexList.isEmpty()) return fail;
    QStandardItemModel * theModel = qobject_cast<QStandardItemModel *>(this->model());
    if (theModel == NULL) return fail;

    LinkedStandardItem * theLinkedItem = (LinkedStandardItem *)theModel->itemFromIndex(indexList.at(0));

    FileTreeNode * retNode = qobject_cast<FileTreeNode *>(theLinkedItem->getLinkedObject());
    if (retNode == NULL) return fail;
    return retNode->getFileData();
}

void RemoteFileTree::setupFileView()
{
    FileNodeRef fail;
    emit newFileSelected(fail);
    //TODO: reconsider needed columns
    this->hideColumn((int)FileColumn::MIME_TYPE);
    this->hideColumn((int)FileColumn::PERMISSIONS);
    this->hideColumn((int)FileColumn::FORMAT);
    this->hideColumn((int)FileColumn::LAST_CHANGED);
    //TODO: Adjust column size defaults;
}

void RemoteFileTree::folderExpanded(QModelIndex fileIndex)
{
    fileEntryTouched(fileIndex);
    FileNodeRef selectedItem = getSelectedFile();
    if (selectedItem.isNil()) return;
    if (selectedItem.folderContentsLoaded()) return;

    selectedItem.enactFolderRefresh();
}

void RemoteFileTree::fileEntryTouched(QModelIndex itemTouched)
{
    this->selectionModel()->clearSelection();
    QStandardItemModel * dataStore = (QStandardItemModel *)this->model();
    QStandardItem * selectedItem = dataStore->itemFromIndex(itemTouched);
    if (selectedItem == NULL)
    {
        return;
    }

    LinkedStandardItem * linkedItem = (LinkedStandardItem *)(selectedItem);

    selectRowByItem(linkedItem);
}

void RemoteFileTree::forceSelectionRefresh()
{
    emit newFileSelected(getSelectedFile());
}

void RemoteFileTree::selectRowByItem(LinkedStandardItem * linkedItem)
{
    FileTreeNode * clickedNode = qobject_cast<FileTreeNode *>(linkedItem->getLinkedObject());
    if (clickedNode == NULL)
    {
        return;
    }

    if (!clickedNode->nodeIsDisplayed())
    {
        return;
    }

    QStandardItem * parentNode;
    if (clickedNode->isRootNode())
    {
        parentNode = linkedItem->model()->invisibleRootItem();
    }
    else
    {
        parentNode = clickedNode->getParentNode()->getFirstDataNode();
    }

    int rowNum = clickedNode->getFirstDataNode()->row();

    this->selectionModel()->select(QItemSelection(parentNode->child(rowNum,0)->index(),
                                                  parentNode->child(rowNum,parentNode->columnCount()-1)->index()),
                                   QItemSelectionModel::Select);
    emit newFileSelected(getSelectedFile());
}
