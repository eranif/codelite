#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include "LSP/MessageWithParams.h"

namespace LSP
{

class WXDLLIMPEXP_CL Notification : public LSP::MessageWithParams
{
public:
    Notification();
    ~Notification() override = 0; // Make sure that this class cant be used directly
};
} // namespace LSP

#endif // NOTIFICATION_H
