#ifndef RAPIDCLIENT_P_H
#define RAPIDCLIENT_P_H

#include "rapidclient.h"

class RapidClientPrivate
{
    Q_DECLARE_PUBLIC(RapidClient)

public:
    RapidClientPrivate();
    virtual ~RapidClientPrivate();
private:
    RapidClient *q_ptr;

private:
    void onReadReady();
private:
    qint16 datagramType;
    qint64 datagramSize;
    QByteArray datagram;

private:
    QHash<qint16, RapidClient::Filter> filters;

private:
    template <typename Type>
    bool writeSocket(const Type *type, int size) {
        if (q_ptr->write(reinterpret_cast<const char *>(type), size) == size)
        { return true; } else { q_ptr->close(); return false; }
    }
    template <typename Type>
    bool readSocket(Type *type, int size) {
        if (q_ptr->read(reinterpret_cast<char *>(type), size) == size)
        { return true; } else { q_ptr->close(); return false; }
    }
};

#endif // RAPIDCLIENT_P_H
