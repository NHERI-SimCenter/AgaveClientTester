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

#include "explorerdriver.h"
#include "explorerwindow.h"

#include "utilWindows/errorpopup.h"
#include "utilWindows/authform.h"
#include "remoteFileOps/fileoperator.h"
#include "remoteFileOps/joboperator.h"

#include "../AgaveClientInterface/agaveInterfaces/agavehandler.h"

ExplorerDriver::ExplorerDriver(QObject *parent) : AgaveSetupDriver(parent)
{
    AgaveHandler * tmpHandle = new AgaveHandler(this);
    tmpHandle->registerAgaveAppInfo("compress", "compress-0.1u1",{"directory", "compression_type"},{},"directory");
    tmpHandle->registerAgaveAppInfo("extract", "extract-0.1u1",{"inputFile"},{},"");
    tmpHandle->registerAgaveAppInfo("openfoam","openfoam-2.4.0u11",{"solver"},{"inputDirectory"},"inputDirectory");

    theConnector = (RemoteDataInterface *) tmpHandle;

    QObject::connect(theConnector, SIGNAL(sendFatalErrorMessage(QString)), this, SLOT(getFatalInterfaceError(QString)));
}

ExplorerDriver::~ExplorerDriver()
{
    if (mainWindow != NULL) mainWindow->deleteLater();
}

void ExplorerDriver::startup()
{
    authWindow = new AuthForm(this);
    authWindow->show();
    QObject::connect(authWindow->windowHandle(),SIGNAL(visibleChanged(bool)),this, SLOT(subWindowHidden(bool)));

    mainWindow = new ExplorerWindow(this);
}

void ExplorerDriver::closeAuthScreen()
{
    if (mainWindow == NULL)
    {
        ErrorPopup anError(VWTerrorType::ERR_WINDOW_SYSTEM);
        return;
    }

    myJobHandle = new JobOperator(theConnector,this);
    myFileHandle = new FileOperator(theConnector,this);
    myFileHandle->resetFileData();

    mainWindow->startAndShow();

    //The dynamics of this may be different in windows. TODO: Find a more cross-platform solution
    QObject::connect(mainWindow->windowHandle(),SIGNAL(visibleChanged(bool)),this, SLOT(subWindowHidden(bool)));

    if (authWindow != NULL)
    {
        QObject::disconnect(authWindow->windowHandle(),SIGNAL(visibleChanged(bool)),this, SLOT(subWindowHidden(bool)));
        authWindow->hide();
        authWindow->deleteLater();
        authWindow = NULL;
    }
}

QString ExplorerDriver::getBanner()
{
    return "SimCenter Agave Explorer Program";
}

QString ExplorerDriver::getVersion()
{
    return "Version: 0.1";
}

QString ExplorerDriver::getLicense()
{
    QString ret;
    //Note: This is a messy way of doing this. Should find a cleaner way, once some stabilty reached.
    ret = ret.append("The SimCenter Agave Explorer and its underlying Client Interface to Agave, ");
    ret = ret.append("in source and binary forms, is copyright \"The University of Notre Dame\" ");
    ret = ret.append("and \"The Regents of the University of California\" and is licensed under the following copyright and license:\n\n");
    ret = ret.append("BSD 3-Clause License\n\nCopyright (c) 2017, The University of Notre Dame\n");
    ret = ret.append("Copyright (c) 2017, The Regents of the University of California\n\n");
    ret = ret.append("All rights reserved.\n\n");
    ret = ret.append("Redistribution and use in source and binary forms, with or without modification, ");
    ret = ret.append("are permitted provided that the following conditions are met:\n\n");
    ret = ret.append("* Redistributions of source code must retain the above copyright notice, this\n");
    ret = ret.append("  list of conditions and the following disclaimer.\n\n");
    ret = ret.append("* Redistributions in binary form must reproduce the above copyright notice,\n");
    ret = ret.append("  this list of conditions and the following disclaimer in the documentation\n");
    ret = ret.append("  and/or other materials provided with the distribution.\n\n");
    ret = ret.append("* Neither the name of the copyright holder nor the names of its\n");
    ret = ret.append("  contributors may be used to endorse or promote products derived from\n");
    ret = ret.append("  this software without specific prior written permission.\n\n");
    ret = ret.append("THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\"\n");
    ret = ret.append("AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE\n");
    ret = ret.append("IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE\n");
    ret = ret.append("DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE\n");
    ret = ret.append("FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL\n");
    ret = ret.append("DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR\n");
    ret = ret.append("SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER\n");
    ret = ret.append("CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,\n");
    ret = ret.append("OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE\n");
    ret = ret.append("OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n\n");
    ret = ret.append("------------------------------------------------------------------------------------\n\n");
    ret = ret.append("The SimCenter Agave Explorer Program makes use of the QT packages (as libraries, unmodified): core, gui, widgets and network\n\n");
    ret = ret.append("QT is copyright \"The Qt Company Ltd\" and licensed under the GNU Lesser General Public License (version 3) which references the GNU General Public License (version 3)\n\n");
    ret = ret.append("These Licenses can be found at: <http://www.gnu.org/licenses/>\n\n");
    ret = ret.append("------------------------------------------------------------------------------------\n\n");

    return ret;
}
