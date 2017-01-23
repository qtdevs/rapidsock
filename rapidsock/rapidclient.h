#ifndef RAPIDCLIENT_H
#define RAPIDCLIENT_H

#include <functional>
#include <QTcpSocket>

class RapidClientPrivate;
class RapidClient : public QTcpSocket
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(RapidClient)

public:
    explicit RapidClient(QObject *parent = Q_NULLPTR);
    virtual ~RapidClient();
private:
    QScopedPointer<RapidClientPrivate> d_ptr;

Q_SIGNALS:
    void datagramReady(qint16 type, const QByteArray &datagram);
protected:
    virtual void incomingDatagram(qint16 type, const QByteArray &datagram);
public:
    bool writeDatagram(qint16 type, const char *data, int size);
    bool writeDatagram(qint16 type, const QByteArray &datagram);
public:
    const qint16 &datagramType() const;
    const QByteArray &datagram() const;

public:
    struct Event {
        RapidClient *client;
        const QByteArray &datagram;
    };
    typedef std::function<void(const Event &)> Filter;
public:
    void installRapidFilter(qint16 type, const Filter &filter);
    void removeRapidFilter(qint16 type);
};

typedef RapidClient::Event RapidClientEvent;

#endif // RAPIDCLIENT_H
