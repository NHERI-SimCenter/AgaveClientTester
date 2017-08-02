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

#include "agavesetupdriver.h"

#include "utilWindows/errorpopup.h"
#include "utilWindows/authform.h"
#include "utilWindows/quickinfopopup.h"
#include "remoteFileOps/fileoperator.h"
#include "remoteFileOps/joboperator.h"

#include "../AgaveClientInterface/agaveInterfaces/agavehandler.h"

AgaveSetupDriver::AgaveSetupDriver(QObject *parent) : QObject(parent) {}

AgaveSetupDriver::~AgaveSetupDriver()
{
    if (theConnector != NULL) theConnector->deleteLater();
    if (authWindow != NULL) authWindow->deleteLater();
    if (myJobHandle != NULL) myJobHandle->deleteLater();
    if (myFileHandle != NULL) myFileHandle->deleteLater();
}

RemoteDataInterface * AgaveSetupDriver::getDataConnection()
{
    return theConnector;
}

JobOperator * AgaveSetupDriver::getJobHandler()
{
    return myJobHandle;
}

FileOperator * AgaveSetupDriver::getFileHandler()
{
    return myFileHandle;
}

void AgaveSetupDriver::getAuthReply(RequestState authReply)
{
    if ((authReply == RequestState::GOOD) && (authWindow != NULL) && (authWindow->isVisible()))
    {
        closeAuthScreen();
    }
}

void AgaveSetupDriver::getFatalInterfaceError(QString errText)
{
    ErrorPopup((QString)errText);
}

void AgaveSetupDriver::subWindowHidden(bool nowVisible)
{
    if (nowVisible == false)
    {
        shutdown();
    }
}

void AgaveSetupDriver::shutdown()
{
    if (doingShutdown) //If shutdown already in progress
    {
        return;
    }
    doingShutdown = true;
    qDebug("Beginning graceful shutdown.");
    if (theConnector != NULL)
    {
        RemoteDataReply * revokeTask = theConnector->closeAllConnections();
        if (revokeTask != NULL)
        {
            QObject::connect(revokeTask, SIGNAL(connectionsClosed(RequestState)), this, SLOT(shutdownCallback()));
            qDebug("Waiting on outstanding tasks");
            QuickInfoPopup * waitOnCloseMessage = new QuickInfoPopup("Waiting for network shutdown. Click OK to force quit.");
            QObject::connect(waitOnCloseMessage, SIGNAL(accepted()), this, SLOT(shutdownCallback()));
            waitOnCloseMessage->show();
            return;
        }
    }
    shutdownCallback();
}

void AgaveSetupDriver::shutdownCallback()
{
    QCoreApplication::instance()->exit(0);
}
