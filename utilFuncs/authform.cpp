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

#include "authform.h"
#include "ui_authform.h"

#include "remotedatainterface.h"
#include "copyrightdialog.h"

#include "agavesetupdriver.h"
#include "ae_globals.h"

AuthForm::AuthForm(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AuthForm)
{
    ui->setupUi(this);

    QLabel * versionLabel = new QLabel(ae_globals::get_Driver()->getVersion());

    ui->headerBox->setHeadingText(ae_globals::get_Driver()->getBanner());
    ui->headerBox->appendWidget(versionLabel);

    QPushButton * copyButton = new QPushButton("Click Here for Copyright and License");
    QObject::connect(copyButton, SIGNAL(clicked(bool)), this, SLOT(getCopyingInfo()));
    ui->headerBox->appendWidget(copyButton);

    ui->footerBox->condense();

    this->setTabOrder(ui->unameInput, ui->passwordInput);
    this->setTabOrder(ui->passwordInput, ui->loginButton);
    this->setTabOrder(ui->loginButton, ui->quitButton);
}

AuthForm::~AuthForm()
{
    delete ui;
}

void AuthForm::getCopyingInfo()
{
    CopyrightDialog copyrightPopup;
    copyrightPopup.exec();
}

void AuthForm::exitAuth()
{
    ae_globals::get_Driver()->shutdown();
}

void AuthForm::performAuth()
{
    if (ae_globals::get_connection()->getInterfaceState() != RemoteDataInterfaceState::READY_TO_AUTH) return;

    QString unameText = ui->unameInput->text();
    QString passText = ui->passwordInput->text();

    RemoteDataReply * authReply = ae_globals::get_connection()->performAuth(unameText, passText);

    if (authReply == nullptr)
    {
        ae_globals::displayFatalPopup("Unable to connect to DesignSafe. Please check internet connection.");
    }
    this->setCursor(QCursor(Qt::WaitCursor));

    ui->instructText->setText("Connecting to DesignSafe");
    QObject::connect(authReply,SIGNAL(haveAuthReply(RequestState)),this,SLOT(getAuthReply(RequestState)));
    QObject::connect(authReply,SIGNAL(haveAuthReply(RequestState)),ae_globals::get_Driver(), SLOT(getAuthReply(RequestState)));
    ui->loginButton->setEnabled(false);
}

void AuthForm::getAuthReply(RequestState authReply)
{
    if (authReply == RequestState::GOOD)
    {
        ui->instructText->setText("Loading . . .");
    }
    else if (authReply == RequestState::EXPLICIT_ERROR)
    {
        ui->instructText->setText("Username/Password combination incorrect, verify your credentials and try again.");
        ui->loginButton->setEnabled(true);
    }
    else
    {
        ui->instructText->setText("Unable to contact DesignSafe, verify your connection and try again.");
        ui->loginButton->setEnabled(true);
    }
    this->unsetCursor();
}
