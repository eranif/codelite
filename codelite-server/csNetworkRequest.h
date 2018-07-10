#ifndef CSNETWORKREQUEST_H
#define CSNETWORKREQUEST_H

#include "worker_thread.h"

class csNetworkRequest : public ThreadRequest
{
public:
    csNetworkRequest();
    virtual ~csNetworkRequest();
};

#endif // CSNETWORKREQUEST_H
