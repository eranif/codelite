#ifndef CSMANAGER_H
#define CSMANAGER_H

#include "codelite_events.h"
#include "csCommandHandlerManager.h"
#include "csConfig.h"
#include "file_logger.h"
#include <cl_command_event.h>
#include <unordered_set>
#include <wx/event.h>

class csJoinableThread;
class csNetworkThread;
class csManager : public wxEvtHandler
{
    csConfig m_config;
    csNetworkThread* m_networkThread;
    std::unordered_set<csJoinableThread*> m_threads;
    csCommandHandlerManager m_handlers;

    wxString m_command;
    wxString m_options;
    bool m_startupCalled;

public:
    csManager();
    virtual ~csManager();
    bool Startup();

    wxString& GetCommand() { return m_command; }
    wxString& GetOptions() { return m_options; }

protected:
    void OnWorkerThreadTerminated(clCommandEvent& event);
    void OnServerError(clCommandEvent& event);
    void OnNewConnection(clCommandEvent& event);
    void OnCommandProcessedCompleted(clCommandEvent &event);
};

#endif // CSMANAGER_H
