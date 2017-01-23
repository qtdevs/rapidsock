#include "serverwindow.h"
#include "ui_serverwindow.h"

#include "clientwindow.h"

ServerWindow::ServerWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ServerWindow())
{
    ui->setupUi(this);
    server = new RapidServer(this);

    //
    server->installRapidFilter(8, [=] (const RapidServerEvent &e) {
        QDataStream ds(e.datagram);
        int socketId = -1;
        ds >> socketId;

        socketIds.insert(e.client, socketId);
    });

    server->installRapidFilter(15, [=] (const RapidServerEvent &e) {
        int socketId = socketIds.value(e.client);
        QString text = QString::fromUtf8(e.datagram);
        ui->output->appendPlainText(tr("#%1: %2").arg(socketId).arg(text));
    });
}

ServerWindow::~ServerWindow()
{
}

void ServerWindow::on_writeButton_clicked()
{
    QString text = ui->datagram->text();
    if (!text.isEmpty() && server->isListening()) {
        server->broadcastDatagram(15, text.toUtf8());
        ui->datagram->clear();
    }
}

void ServerWindow::on_stateButton_clicked()
{
    if (server->isListening()) {
        server->close();
        ui->stateButton->setText(tr("打开服务器"));
        ui->serverAddress->clear();
        ui->serverPort->clear();
    } else if (server->listen()) {
        ui->stateButton->setText(tr("关闭服务器"));
        ui->serverAddress->setText(server->serverAddress().toString());
        ui->serverPort->setText(QString::number(server->serverPort()));
    }
}

void ServerWindow::on_buildButton_clicked()
{
    if (server->isListening()) {
        auto socketWindow = new ClientWindow();
        socketWindow->connectToHost(server->serverPort());
        socketWindow->show();
    }
}
