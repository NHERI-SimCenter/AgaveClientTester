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
