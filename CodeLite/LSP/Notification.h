#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include "LSP/MessageWithParams.h"

namespace LSP
{

class WXDLLIMPEXP_SDK Notification : public LSP::MessageWithParams
{
public:
    Notification();
    virtual ~Notification() = 0; // Make sure that this class cant be used directly
};
}; // namespace LSP

#endif // NOTIFICATION_H
