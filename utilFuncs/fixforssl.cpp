#include "fixforssl.h"
#include "ui_fixforssl.h"

#include "ae_globals.h"

FixForSSL::FixForSSL(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FixForSSL)
{
    ui->setupUi(this);
}

FixForSSL::~FixForSSL()
{
    delete ui;
}

bool FixForSSL::performSSLcheck()
{
    if (QSslSocket::supportsSsl() == false)
    {
        //TODO: Lines commented out for debug purposes
        //#ifdef Q_OS_WIN
        #ifndef Q_OS_WIN
            FixForSSL * sslWindow = new FixForSSL();
            sslWindow->show();
        #else

            ae_globals::displayFatalPopup("SSL support was not detected on this computer.\nPlease insure that some version of SSL is installed,\nNormally, this comes with all Linux and Mac computers, so your OS install may be broken.");
        #endif
        return false;
    }
    return true;
}
