#ifndef INITIALIZEDNOTIFICATION_HPP
#define INITIALIZEDNOTIFICATION_HPP

#include "LSP/Notification.h"

namespace LSP
{

class WXDLLIMPEXP_CL InitializedNotification : public Notification
{
public:
    InitializedNotification();
    ~InitializedNotification() override = default;
};

} // namespace LSP

#endif // INITIALIZEDNOTIFICATION_HPP
