#ifndef QUICKPROGRAMDRIVER_H
#define QUICKPROGRAMDRIVER_H

#include <QObject>

class QuickProgramDriver : public QObject
{
    Q_OBJECT
public:
    explicit QuickProgramDriver(QObject *parent = 0);

private slots:
    void printoutFatalErrors(QString errorText);
};

#endif // QUICKPROGRAMDRIVER_H
