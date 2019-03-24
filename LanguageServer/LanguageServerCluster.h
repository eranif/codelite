#ifndef LANGUAGESERVERCLUSTER_H
#define LANGUAGESERVERCLUSTER_H

#include <wx/event.h>
#include "LanguageServerProtocol.h"
#include <wxStringHash.h>
#include "cl_command_event.h"
#include <wx/sharedptr.h>
#include "LSP/LSPEvent.h"
#include "LanguageServerEntry.h"

class LanguageServerCluster : public wxEvtHandler
{
    std::unordered_map<wxString, LanguageServerProtocol::Ptr_t> m_servers;

public:
    typedef wxSharedPtr<LanguageServerCluster> Ptr_t;

protected:
    LanguageServerProtocol::Ptr_t GetServerForFile(const wxFileName& filename);
    LanguageServerProtocol::Ptr_t GetServerByName(const wxString& name);
    void RestartServer(const wxString& name);
    void StartServer(const LanguageServerEntry& entry);

    void StopAll();
    void StartAll();

protected:
    void OnSymbolFound(LSPEvent& event);
    void OnCompletionReady(LSPEvent& event);
    void OnReparseNeeded(LSPEvent& event);
    void OnRestartNeeded(LSPEvent& event);
    void OnLSPInitialized(LSPEvent& event);
    void OnMethodNotFound(LSPEvent& event);
    void OnWorkspaceClosed(wxCommandEvent& event);
    void OnWorkspaceOpen(wxCommandEvent& event);

public:
    LanguageServerCluster();
    virtual ~LanguageServerCluster();

    void Reload();
};

#endif // LANGUAGESERVERCLUSTER_H
