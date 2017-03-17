#include "quickprogramdriver.h"

#include <QtGlobal>

QuickProgramDriver::QuickProgramDriver(QObject *parent) : QObject(parent)
{

}

void QuickProgramDriver::printoutFatalErrors(QString errorText)
{
    qCritical("Agave Error: %s", qPrintable(errorText));
}
