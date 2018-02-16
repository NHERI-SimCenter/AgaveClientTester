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

#include "joblistnode.h"

#include "../utilFuncs/linkedstandarditem.h"

JobListNode::JobListNode(RemoteJobData newData, QStandardItemModel * theModel, QObject *parent) : QObject(parent)
{
    myModel = theModel;
    if (myModel == NULL)
    {
        this->deleteLater();
        return;
    }

    setData(newData);
}

JobListNode::~JobListNode()
{
    if (myModelItem != NULL)
    {
        myModel->removeRow(myModelItem->row());
        myModelItem = NULL;
    }
}

void JobListNode::setData(RemoteJobData newData)
{
    bool signalChange = false;
    if ((newData.getState() != myData.getState()) && (myData.getState() != "APP_INIT"))
    {
        signalChange = true;
    }
    myData = newData;

    QList<QStandardItem *> myModelRow;

    if (myModelItem == NULL)
    {
        for (int i = 0; i < myModel->columnCount(); i++)
        {
            myModelRow.append(new LinkedStandardItem(this));
        }

        myModel->insertRow(0, myModelRow);
    }
    else
    {
        for (int i = 0; i < myModel->columnCount(); i++)
        {
            myModelRow.append(myModel->item(myModelItem->row(),i));
        }
    }

    int i = 0;
    QStandardItem * headerItem = myModel->horizontalHeaderItem(i);
    while (headerItem != NULL)
    {
        QString headerText = headerItem->text();
        QStandardItem * dataEntry = myModelRow.at(i);
        if (dataEntry == NULL)
        {
            qDebug("ERROR: Column Mismatch in job list.");
            return;
        }

        if (headerText == "Task Name")
        {
            dataEntry->setText(myData.getName());
        }
        else if (headerText == "State")
        {
            dataEntry->setText(myData.getState());
        }
        else if (headerText == "Agave App")
        {
            dataEntry->setText(myData.getApp());
        }
        else if (headerText == "Time Created")
        {
            dataEntry->setText(myData.getTimeCreated().toString());
        }
        else if (headerText == "Agave ID")
        {
            dataEntry->setText(myData.getID());
        }

        i++;
        headerItem = myModel->horizontalHeaderItem(i);
    }

    if (signalChange)
    {
        emit jobStateChanged(&myData);
    }
}

RemoteJobData JobListNode::getData()
{
    return myData;
}

void JobListNode::setDetails(QMap<QString, QString> inputs, QMap<QString, QString> params)
{
    myData.setDetails(inputs, params);
}
