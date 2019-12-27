#ifndef LANGUAGESERVERCLUSTER_H
#define LANGUAGESERVERCLUSTER_H

#include <wx/event.h>
#include "LanguageServerProtocol.h"
#include <wxStringHash.h>
#include "cl_command_event.h"
#include <wx/sharedptr.h>
#include "LSP/LSPEvent.h"
#include "LanguageServerEntry.h"
#include "LSP/basic_types.h"
#include "entry.h"

class LanguageServerCluster : public wxEvtHandler
{
    std::unordered_map<wxString, LanguageServerProtocol::Ptr_t> m_servers;

public:
    typedef wxSharedPtr<LanguageServerCluster> Ptr_t;

protected:
    void RestartServer(const wxString& name);
    void StartServer(const LanguageServerEntry& entry);

    void StopAll();
    void StartAll();
    void ClearAllDiagnostics();
    
    /**
     * @brief covnert LSP::SignatureHelp class to TagEntryPtrVector_t
     */
    void LSPSignatureHelpToTagEntries(TagEntryPtrVector_t& tags, const LSP::SignatureHelp& sighelp);

protected:
    void OnSignatureHelp(LSPEvent& event);
    void OnSymbolFound(LSPEvent& event);
    void OnCompletionReady(LSPEvent& event);
    void OnReparseNeeded(LSPEvent& event);
    void OnRestartNeeded(LSPEvent& event);
    void OnLSPInitialized(LSPEvent& event);
    void OnMethodNotFound(LSPEvent& event);
    void OnWorkspaceClosed(wxCommandEvent& event);
    void OnWorkspaceOpen(wxCommandEvent& event);
    void OnCompileCommandsGenerated(clCommandEvent& event);
    void OnSetDiagnostics(LSPEvent& event);
    void OnClearDiagnostics(LSPEvent& event);

public:
    LanguageServerCluster();
    virtual ~LanguageServerCluster();
    void Reload();
    LanguageServerProtocol::Ptr_t GetServerForFile(const wxFileName& filename);
    LanguageServerProtocol::Ptr_t GetServerByName(const wxString& name);
};

#endif // LANGUAGESERVERCLUSTER_H
