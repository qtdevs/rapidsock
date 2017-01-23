#include "rapidserver.h"
#include "rapidserver_p.h"
#include "rapidclient.h"

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(qlcRapidServer, "RapidServer")

// class RapidServer

RapidServer::RapidServer(QObject *parent)
    : QTcpServer(parent)
    , d_ptr(new RapidServerPrivate())
{
    d_ptr->q_ptr = this;
}

RapidServer::~RapidServer()
{
}

const QList<RapidClient *> &RapidServer::clients() const
{
    Q_D(const RapidServer);
    return d->clients;
}

void RapidServer::incomingConnection(qintptr socketDescriptor)
{
    Q_D(RapidServer);

    QScopedPointer<RapidClient> client(new RapidClient(this));
    if (client->setSocketDescriptor(socketDescriptor)) {
        typedef void (QTcpSocket::* ErrorSignal)(QAbstractSocket::SocketError);
        ErrorSignal errorSignal = static_cast<ErrorSignal>(&QTcpSocket::error);

        connect(client.data(), errorSignal,
                this, &RapidServer::onError);
        connect(client.data(), &QTcpSocket::stateChanged,
                this, &RapidServer::onStateChanged);
        connect(client.data(), &RapidClient::datagramReady,
                this, &RapidServer::incomingDatagram);

        d->clients.append(client.take());
    }
}

void RapidServer::onError(QAbstractSocket::SocketError error)
{
    Q_D(RapidServer);

    auto client = static_cast<RapidClient *>(QObject::sender());

    if (QTcpSocket::RemoteHostClosedError != error) {
        qCWarning(qlcRapidServer) << "Socket error:" << client << error;
    }

    d->clients.removeOne(client);
    client->deleteLater();
}

void RapidServer::onStateChanged(QAbstractSocket::SocketState state)
{
    Q_D(RapidServer);

    if (QTcpSocket::UnconnectedState == state) {
        auto client = static_cast<RapidClient *>(QObject::sender());

        d->clients.removeOne(client);
        client->deleteLater();
    }
}

void RapidServer::incomingDatagram(qint16 type, const QByteArray &datagram)
{
    Q_D(RapidServer);

    auto client = static_cast<RapidClient *>(QObject::sender());
    Filter filter = d->filters.value(type);
    if (Q_LIKELY(filter)) {
        filter(Event{ this, client, datagram });
    } else {
        emit datagramReady(type, client, datagram);
    }
}

bool RapidServer::broadcastDatagram(qint16 type, const char *data, int size, QList<RapidClient *> *errorClients)
{
    Q_D(RapidServer);

    bool succeeded = true;
    if (!d->clients.isEmpty()) {
        for (auto client : d->clients) {
            if (!client->writeDatagram(type, data, size)) {
                if (errorClients) {
                    errorClients->append(client);
                }
                succeeded = false;
            }
        }
    }

    return succeeded;
}

bool RapidServer::broadcastDatagram(qint16 type, const QByteArray &datagram, QList<RapidClient *> *errorClients)
{
    return broadcastDatagram(type, datagram.constData(), datagram.size(), errorClients);
}

void RapidServer::installRapidFilter(qint16 type, const Filter &filter)
{
    Q_D(RapidServer);
    d->filters.insert(type, filter);
}

void RapidServer::removeRapidFilter(qint16 type)
{
    Q_D(RapidServer);
    d->filters.remove(type);
}

void RapidServer::close()
{
    Q_D(RapidServer);

    QTcpServer::close();
    for (auto client : d->clients)
        client->close();

    d->clients.clear();
}

// class RapidServerPrivate

RapidServerPrivate::RapidServerPrivate()
    : q_ptr(nullptr)
{
}

RapidServerPrivate::~RapidServerPrivate()
{
}
