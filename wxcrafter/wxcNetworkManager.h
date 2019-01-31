#ifndef WXCNETWORKMANAGER_H
#define WXCNETWORKMANAGER_H

#include "wxcEvent.h"
#include "wxcLib/clSocketBase.h"
#include "wxcLib/wxcNetworkReply.h"
#include <wx/event.h>

class wxcNetworkManager : public wxEvtHandler
{
    clSocketBase::Ptr_t m_socket;

public:
    wxcNetworkManager();
    virtual ~wxcNetworkManager();

    void OnNetConnEstablished(wxcNetworkEvent& event);

    void SendReply(wxcNetworkReply& reply);
};

#endif // WXCNETWORKMANAGER_H
