#ifndef AGAVESETUPDRIVER_H
#define AGAVESETUPDRIVER_H

#include <QObject>
#include <QCoreApplication>
#include <QWidget>
#include <QWindow>

enum class RequestState;

class RemoteDataInterface;

class AuthForm;

class ExplorerWindow;
class JobOperator;
class FileOperator;

class AgaveSetupDriver : public QObject
{
    Q_OBJECT
public:
    explicit AgaveSetupDriver(QObject *parent = nullptr);
    ~AgaveSetupDriver();
    virtual void startup() = 0;
    void shutdown();

    virtual void closeAuthScreen() = 0;

    RemoteDataInterface * getDataConnection();
    JobOperator * getJobHandler();
    FileOperator * getFileHandler();

private slots:
    void getAuthReply(RequestState authReply);
    void getFatalInterfaceError(QString errText);
    void subWindowHidden(bool nowVisible);
    void shutdownCallback();

protected:
    RemoteDataInterface * theConnector = NULL;
    AuthForm * authWindow = NULL;
    JobOperator * myJobHandle = NULL;
    FileOperator * myFileHandle = NULL;

    bool doingShutdown = false;

signals:

public slots:
};

#endif // AGAVESETUPDRIVER_H
