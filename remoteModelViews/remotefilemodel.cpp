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

#include "../remoteFileOps/fileoperator.h"
#include "ae_globals.h"

RemoteFileModel::RemoteFileModel(QObject * parent) : QStandardItemModel(parent)
{
    QObject::connect(ae_globals::get_file_handle(), SIGNAL(fileSystemChange(FileNodeRef)),
                     this, SLOT(newFileData(FileNodeRef)));
}

void RemoteFileModel::newFileData(FileNodeRef newFileData)
{
/*
 *
    if (!nodeIsDisplayed())
    {


        QList<QStandardItem *> appendList;

        firstDataNode = new LinkedStandardItem(this);
        appendList.append(firstDataNode);
        for (int i = 1; i < myModel->columnCount(); i++)
        {
            appendList.append(new LinkedStandardItem(this));
        }

        if (myParent == NULL)
        {
            myModel->appendRow(appendList);
        }
        else
        {
            myParent->firstDataNode->appendRow(appendList);
        }
    }
    */

    /*
     * QStandardItem * parentItem;
    if (myParent == NULL)
    {
        parentItem = myModel->invisibleRootItem();
    }
    else
    {
        parentItem = myParent->firstDataNode;
    }
    int rowNum = firstDataNode->row();

    for (int i = 0; i < parentItem->columnCount(); i++)
    {
        QStandardItem * itemToUpdate = parentItem->child(rowNum, i);
        itemToUpdate->setText(getRawColumnData(i, myModel));
    }
    if (mySpaceHolderNode != NULL)
    {
        if (mySpaceHolderNode->model() == NULL)
        {
            firstDataNode->appendRow(mySpaceHolderNode);
        }
    }
    */

    /*
     * QString spaceholderText = "MAJOR ERROR";
    switch (mySpaceHolderState)
    {
    case SpaceHolderState::LOADING:
        spaceholderText = "Loading . . . ";
        break;
    case SpaceHolderState::EMPTY:
        spaceholderText = "Empty Folder";
        break;
    default:
        spaceholderText = "MAJOR ERROR";
    }
    */
}
