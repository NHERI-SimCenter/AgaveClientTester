#ifndef FIXFORSSL_H
#define FIXFORSSL_H

#include <QMainWindow>
#include <QSslSocket>

namespace Ui {
class FixForSSL;
}

class FixForSSL : public QMainWindow
{
    Q_OBJECT

public:
    explicit FixForSSL(QWidget *parent = 0);
    ~FixForSSL();

    static bool performSSLcheck();

private:
    Ui::FixForSSL *ui;
};

#endif // FIXFORSSL_H
