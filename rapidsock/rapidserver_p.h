#ifndef RAPIDSERVER_P_H
#define RAPIDSERVER_P_H

#include "rapidserver.h"

class RapidServerPrivate
{
    Q_DECLARE_PUBLIC(RapidServer)

public:
    RapidServerPrivate();
    virtual ~RapidServerPrivate();
private:
    RapidServer *q_ptr;

public:
    QList<RapidClient *> clients;
    QHash<qint16, RapidServer::Filter> filters;
};

#endif // RAPIDSERVER_P_H
