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

// The ErrorPopup is used to halt execution with a useful error message.
// It has 2 constructors:
// 1) Uses a VWTerrorType which is an error code. This is meant for fatal errors which might happen from time to time,
// even if the code is correct.
// 2) Uses a QString text message. This is meant for fatal errors that should not be possible, except in the case of a logic
// error elsewhere in the code.

#ifndef ERRORPOPUP_H
#define ERRORPOPUP_H

#include <QtGlobal>
#include <QDialog>

enum class VWTerrorType : unsigned int {ERR_NO_DEF = 255,
                                        CUSTOM_ERROR = 254,
                                       ERR_NOT_IMPLEMENTED = 1,
                                       ERR_ACCESS_LOST = 2,
                                       ERR_WINDOW_SYSTEM = 3,
                                       ERR_AUTH_BLANK = 4};

namespace Ui {
class ErrorPopup;
}

class ErrorPopup : public QDialog
{
    Q_OBJECT

public:
    explicit ErrorPopup(VWTerrorType errNum = VWTerrorType::ERR_NO_DEF);
    explicit ErrorPopup(QString errorText = "No Text");
    ~ErrorPopup();

private slots:
    void closeByError();

private:
    void setErrorLabel(QString errorText);
    QString getErrorText(VWTerrorType errNum);

    Ui::ErrorPopup *ui;
    VWTerrorType errorVal;
};

#endif // ERRORPOPUP_H
