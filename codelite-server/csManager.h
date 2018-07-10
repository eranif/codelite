#ifndef CSMANAGER_H
#define CSMANAGER_H

#include "codelite_events.h"
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

public:
    csManager();
    virtual ~csManager();
    void Startup();

protected:
    void OnWorkerThreadTerminated(clCommandEvent& event);
    void OnServerError(clCommandEvent& event);
    void OnNewConnection(clCommandEvent& event);
};

#endif // CSMANAGER_H
