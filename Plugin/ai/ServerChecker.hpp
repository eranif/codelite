#pragma once

#include "codelite_exports.h"
#include "file_logger.h"

#include <wx/socket.h>
#include <wx/timer.h>
#include <wx/url.h>

class WXDLLIMPEXP_SDK ServerChecker;
struct SockerHelper {
    std::unique_ptr<wxSocketClient> m_sock;
    ServerChecker* m_owner{nullptr};
    int m_socket_id{wxNOT_FOUND};
    SockerHelper(ServerChecker* owner, const wxString& server_url);
    ~SockerHelper();
};

class WXDLLIMPEXP_SDK ServerChecker : public wxEvtHandler
{
public:
    ServerChecker(std::atomic_bool& is_alive);
    ~ServerChecker() override;

    void Check(const wxString& server_url);

protected:
    void OnSocketEvent(wxSocketEvent& event);
    void OnTimer(wxTimerEvent& event);

private:
    std::atomic_bool& m_is_alive;
    std::unique_ptr<SockerHelper> m_helper{nullptr};
    wxTimer m_timer;
    wxString m_server_url;
    friend struct SockerHelper;
};
