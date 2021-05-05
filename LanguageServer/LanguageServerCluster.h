#ifndef LANGUAGESERVERCLUSTER_H
#define LANGUAGESERVERCLUSTER_H

#include "LSP/LSPEvent.h"
#include "LSP/basic_types.h"
#include "LanguageServerEntry.h"
#include "LanguageServerProtocol.h"
#include "clWorkspaceEvent.hpp"
#include "cl_command_event.h"
#include "entry.h"
#include <wx/event.h>
#include <wx/sharedptr.h>
#include <wxStringHash.h>

class LanguageServerCluster : public wxEvtHandler
{
    struct CrashInfo {
        size_t times = 0;
        time_t last_crash = 0;
    };

    std::unordered_map<wxString, LanguageServerProtocol::Ptr_t> m_servers;
    std::unordered_map<wxString, CrashInfo> m_restartCounters;

public:
    typedef wxSharedPtr<LanguageServerCluster> Ptr_t;

public:
    void StartServer(const LanguageServerEntry& entry);

    void RestartServer(const wxString& name);
    void StartServer(const wxString& entry);
    void StopServer(const wxString& entry);
    void DeleteServer(const wxString& name);

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
    void OnWorkspaceClosed(clWorkspaceEvent& event);
    void OnWorkspaceOpen(clWorkspaceEvent& event);
    void OnCompileCommandsGenerated(clCommandEvent& event);
    void OnSetDiagnostics(LSPEvent& event);
    void OnClearDiagnostics(LSPEvent& event);
    void OnOutlineSymbols(LSPEvent& event);
    void OnBuildEnded(clBuildEvent& event);
    wxString GetEditorFilePath(IEditor* editor) const;
    /**
     * @brief find an editor either by local or remote path
     */
    IEditor* FindEditor(const wxString& path) const;
    IEditor* FindEditor(const LSPEvent& event) const;

public:
    LanguageServerCluster();
    virtual ~LanguageServerCluster();
    void Reload();
    LanguageServerProtocol::Ptr_t GetServerForFile(const wxString& filename);
    LanguageServerProtocol::Ptr_t GetServerByName(const wxString& name);
    void ClearRestartCounters();
};

#endif // LANGUAGESERVERCLUSTER_H
