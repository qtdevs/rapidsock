#include "rapidclient.h"
#include "rapidclient_p.h"

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(qlcRapidClient, "RapidClient")

// class RapidClient

RapidClient::RapidClient(QObject *parent)
    : QTcpSocket(parent)
    , d_ptr(new RapidClientPrivate())
{
    d_ptr->q_ptr = this;

    connect(this, &QTcpSocket::readyRead,
            this, [=] { d_ptr->onReadReady(); });
    connect(this, &QTcpSocket::stateChanged,
            this, [=] { d_ptr->datagramSize = 0; });
}

RapidClient::~RapidClient()
{
}

void RapidClient::incomingDatagram(qint16 type, const QByteArray &datagram)
{
    Q_D(RapidClient);

    Filter filter = d->filters.value(type);
    if (Q_LIKELY(filter)) {
        filter(Event{ this, datagram });
    } else {
        emit datagramReady(type, datagram);
    }
}

bool RapidClient::writeDatagram(qint16 type, const char *data, int size)
{
    Q_D(RapidClient);

    if (size < 0 || size > 0xFFFF) {
        qCCritical(qlcRapidClient) << "Socket error:"
                                   << QTcpSocket::DatagramTooLargeError;
        return false;
    }

    if (!d->writeSocket(&type, 2)) { return false; }
    if (!d->writeSocket(&size, 2)) { return false; }

    return d->writeSocket(data, size);
}

bool RapidClient::writeDatagram(qint16 type, const QByteArray &datagram)
{
    return writeDatagram(type, datagram.constData(), datagram.size());
}

const qint16 &RapidClient::datagramType() const
{
    Q_D(const RapidClient);
    return d->datagramType;
}

const QByteArray &RapidClient::datagram() const
{
    Q_D(const RapidClient);
    return d->datagram;
}

void RapidClient::installRapidFilter(qint16 type, const Filter &filter)
{
    Q_D(RapidClient);
    d->filters.insert(type, filter);
}

void RapidClient::removeRapidFilter(qint16 type)
{
    Q_D(RapidClient);
    d->filters.remove(type);
}

// RapidClientPrivate

RapidClientPrivate::RapidClientPrivate()
    : q_ptr(nullptr)
    , datagramType(0)
    , datagramSize(0)
{
}

RapidClientPrivate::~RapidClientPrivate()
{
}

void RapidClientPrivate::onReadReady()
{
    Q_Q(RapidClient);

    while (true) {
        qint64 size = q->bytesAvailable();
        if (0 == datagramSize) {
            if (size < 4) {
                return;
            }

            if (!readSocket(&datagramType, 2)) { return; }
            if (!readSocket(&datagramSize, 2)) { return; }
        } else {
            if (size < datagramSize) {
                return;
            }

            if (datagram.capacity() <= datagramSize)
                datagram.reserve(datagramSize);
            datagram.resize(datagramSize);

            if (!readSocket(datagram.data(), datagramSize)) { return; }
            q->incomingDatagram(datagramType, datagram); datagramSize = 0;
        }
    }
}
