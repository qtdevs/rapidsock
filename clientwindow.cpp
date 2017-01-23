#include "clientwindow.h"
#include "ui_clientwindow.h"

#include <QtDebug>
#include <QHostAddress>

int ClientWindow::clientIndex = 0;

ClientWindow::ClientWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ClientWindow())
{
    ui->setupUi(this);

    socket = new RapidClient(this);
    setAttribute(Qt::WA_DeleteOnClose, true);

    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(error(QAbstractSocket::SocketError)));
    connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this, SLOT(stateChanged(QAbstractSocket::SocketState)));

    clientId = ++clientIndex;
    setWindowTitle(tr("客户端 #%1").arg(clientId));

    socket->installRapidFilter(15, [=] (const RapidClientEvent &e) {
        QString text = QString::fromUtf8(e.datagram);
        ui->output->appendPlainText(text);
    });
}

ClientWindow::~ClientWindow()
{
}

void ClientWindow::error(QAbstractSocket::SocketError socketError)
{
    if (QTcpSocket::RemoteHostClosedError != socketError) {
        qDebug() << socketError;
    }

    close();
}

void ClientWindow::stateChanged(QAbstractSocket::SocketState socketState)
{
    switch (socketState) {
    case QAbstractSocket::ConnectedState: {
        QByteArray datagram;
        QDataStream ds(&datagram, QIODevice::WriteOnly);
        ds << clientId;
        socket->writeDatagram(8, datagram);
        break;
    }
    case QAbstractSocket::UnconnectedState:
        this->close();
        break;
    default:
        break;
    }
}

void ClientWindow::on_writeButton_clicked()
{
    QString text = ui->datagram->text();
    if (!text.isEmpty() && QAbstractSocket::ConnectedState == socket->state()) {
        socket->writeDatagram(15, text.toUtf8());
        ui->datagram->clear();
    }
}

void ClientWindow::on_closeButton_clicked()
{
    socket->close();
    close();
}

void ClientWindow::connectToHost(quint16 port)
{
    socket->connectToHost(QHostAddress::LocalHost, port);
}
