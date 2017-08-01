#ifndef AGAVESETUPDRIVER_H
#define AGAVESETUPDRIVER_H

#include <QObject>

class AgaveSetupDriver : public QObject
{
    Q_OBJECT
public:
    explicit AgaveSetupDriver(QObject *parent = nullptr);

signals:

public slots:
};

#endif // AGAVESETUPDRIVER_H