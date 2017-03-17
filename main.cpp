#include <QCoreApplication>

#include "AgaveClientInterface/remotedatainterface.h"
#include "AgaveClientInterface/agaveInterfaces/agavehandler.h"

#include "quickprogramdriver.h"


int main(int argc, char *argv[])
{
    QCoreApplication mainLoop(argc, argv);

    RemoteDataInterface * dataLink = (RemoteDataInterface *) new AgaveHandler((QObject *)&mainLoop);
    QuickProgramDriver * errorLink = new QuickProgramDriver((QObject *)&mainLoop);

    QObject::connect(dataLink,SIGNAL(sendFatalErrorMessage(QString)),errorLink,SLOT(printoutFatalErrors(QString)));

    return mainLoop.exec();
}
