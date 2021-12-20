#ifndef LANGUAGESERVERCLUSTER_H
#define LANGUAGESERVERCLUSTER_H

#include "LSP/LSPEvent.h"
#include "LSP/basic_types.h"
#include "LanguageServerEntry.h"
#include "LanguageServerProtocol.h"
#include "clWorkspaceEvent.hpp"
#include "cl_command_event.h"
#include "entry.h"

#include <unordered_set>
#include <vector>
#include <wx/event.h>
#include <wx/sharedptr.h>
#include <wxStringHash.h>

class LSPOutlineViewDlg;
class LanguageServerPlugin;
class LanguageServerCluster : public wxEvtHandler
{
    struct CrashInfo {
        size_t times = 0;
        time_t last_crash = 0;
    };

    std::unordered_map<wxString, LanguageServerProtocol::Ptr_t> m_servers;
    std::unordered_map<wxString, CrashInfo> m_restartCounters;
    std::unordered_map<wxString, std::vector<LSP::SymbolInformation>> m_symbols_to_file_cache;
    LanguageServerPlugin* m_plugin = nullptr;
    LSPOutlineViewDlg* m_quick_outline_dlg = nullptr;

public:
    typedef wxSharedPtr<LanguageServerCluster> Ptr_t;

public:
    void StartServer(const LanguageServerEntry& entry);

    void RestartServer(const wxString& name);
    void StartServer(const wxString& entry);
    void StopServer(const wxString& entry);
    void DeleteServer(const wxString& name);

    void StopAll(const std::unordered_set<wxString>& languages = {});
    void StartAll(const std::unordered_set<wxString>& languages = {});
    void ClearAllDiagnostics();
    void SetWorkspaceType(FileExtManager::FileType type);

    /**
     * @brief covnert LSP::SignatureHelp class to TagEntryPtrVector_t
     */
    void LSPSignatureHelpToTagEntries(TagEntryPtrVector_t& tags, const LSP::SignatureHelp& sighelp);

protected:
    void OnSignatureHelp(LSPEvent& event);
    void OnHover(LSPEvent& event);
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
    void OnQuickOutlineView(LSPEvent& event);
    void OnShowQuickOutlineDlg(LSPEvent& event);
    void OnOulineViewSymbols(LSPEvent& event);
    void OnSemanticTokens(LSPEvent& event);
    void OnLogMessage(LSPEvent& event);
    void OnDocumentSymbolsForHighlight(LSPEvent& event);
    void OnBuildEnded(clBuildEvent& event);
    void OnOpenResource(wxCommandEvent& event);
    void OnEditorClosed(clCommandEvent& event);
    void OnActiveEditorChanged(wxCommandEvent& event);
    wxString GetEditorFilePath(IEditor* editor) const;
    /**
     * @brief find an editor either by local or remote path
     */
    IEditor* FindEditor(const wxString& path) const;
    IEditor* FindEditor(const LSPEvent& event) const;

    void DiscoverWorkspaceType();
    void UpdateNavigationBar();

public:
    LanguageServerCluster(LanguageServerPlugin* plugin);
    virtual ~LanguageServerCluster();
    /**
     * @brief restart language servers associated with a given languages
     */
    void Reload(const std::unordered_set<wxString>& languages = {});
    LanguageServerProtocol::Ptr_t GetServerForEditor(IEditor* editor);
    LanguageServerProtocol::Ptr_t GetServerByName(const wxString& name);
    LanguageServerProtocol::Ptr_t GetServerForLanguage(const wxString& lang);
    void ClearRestartCounters();
};

#endif // LANGUAGESERVERCLUSTER_H
