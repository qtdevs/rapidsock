#ifndef CLIENTWINDOW_H
#define CLIENTWINDOW_H

#include <QWidget>
#include "rapidclient.h"

namespace Ui {
class ClientWindow;
}

class ClientWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ClientWindow(QWidget *parent = 0);
    virtual ~ClientWindow();
private:
    QScopedPointer<Ui::ClientWindow> ui;

public Q_SLOTS:
    void error(QAbstractSocket::SocketError socketError);
    void stateChanged(QAbstractSocket::SocketState socketState);

private Q_SLOTS:
    void on_writeButton_clicked();
    void on_closeButton_clicked();
private:
    RapidClient *socket;

public:
    void connectToHost(quint16 port);
private:
    static int clientIndex;
private:
    int clientId;
};

#endif // CLIENTWINDOW_H
