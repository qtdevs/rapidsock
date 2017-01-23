#ifndef RAPIDSERVER_H
#define RAPIDSERVER_H

#include <functional>
#include <QTcpServer>

class RapidClient;
class RapidServerPrivate;
class RapidServer : public QTcpServer
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(RapidServer)

public:
    explicit RapidServer(QObject *parent = Q_NULLPTR);
    virtual ~RapidServer();
private:
    QScopedPointer<RapidServerPrivate> d_ptr;

public:
    const QList<RapidClient *> &clients() const;

Q_SIGNALS:
    void datagramReady(qint16 type, RapidClient *client, const QByteArray &datagram);
protected:
    void incomingConnection(qintptr socketDescriptor) Q_DECL_FINAL;

protected:
    virtual void onError(QAbstractSocket::SocketError error);
    virtual void onStateChanged(QAbstractSocket::SocketState state);
    virtual void incomingDatagram(qint16 type, const QByteArray &datagram);
public:
    bool broadcastDatagram(qint16 type, const char *data, int size, QList<RapidClient *> *errorSockets = Q_NULLPTR);
    bool broadcastDatagram(qint16 type, const QByteArray &datagram, QList<RapidClient *> *errorSockets = Q_NULLPTR);

public:
    struct Event {
        RapidServer *server;
        RapidClient *client;
        const QByteArray &datagram;
    };
    typedef std::function<void(const Event &)> Filter;

public:
    void installRapidFilter(qint16 type, const Filter &filter);
    void removeRapidFilter(qint16 type);
    void close();
};

typedef RapidServer::Event RapidServerEvent;

#endif // RAPIDSERVER_H
