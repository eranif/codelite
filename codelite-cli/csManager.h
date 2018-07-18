#ifndef CSMANAGER_H
#define CSMANAGER_H

#include "codelite_events.h"
#include "csCommandHandlerManager.h"
#include "csConfig.h"
#include "file_logger.h"
#include <cl_command_event.h>
#include <unordered_set>
#include <wx/event.h>

class csManager : public wxEvtHandler
{
    csConfig m_config;
    csCommandHandlerManager m_handlers;

    wxString m_command;
    wxString m_options;
    bool m_startupCalled;
    wxSharedPtr<JSONRoot> m_findInFilesMatches;

public:
    csManager();
    virtual ~csManager();
    bool Startup();

    wxString& GetCommand() { return m_command; }
    wxString& GetOptions() { return m_options; }
    const wxString& GetCommand() const { return m_command; }
    const wxString& GetOptions() const { return m_options; }
    const csConfig& GetConfig() const { return m_config; }
    void LoadCommandFromINI();

protected:
    // The handler completed
    void OnCommandProcessedCompleted(clCommandEvent& event);

    // Search events
    void OnSearchThreadMatch(wxCommandEvent& event);
    void OnSearchThreadStarted(wxCommandEvent& event);
    void OnSearchThreadCancelled(wxCommandEvent& event);
    void OnSearchThreadEneded(wxCommandEvent& event);
};

#endif // CSMANAGER_H
