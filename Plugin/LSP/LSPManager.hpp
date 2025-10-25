#pragma once

#include "CodeLiteRemoteHelper.hpp"
#include "LSP/LSPEvent.h"
#include "LSP/LanguageServerProtocol.h"
#include "LSP/basic_types.h"
#include "LanguageServerEntry.h"
#include "clWorkspaceEvent.hpp"
#include "cl_command_event.h"
#include "codelite_exports.h"
#include "database/entry.h"

#include <unordered_set>
#include <vector>
#include <wx/event.h>
#include <wx/sharedptr.h>
#include <wx/utils.h>

class LSPOutlineViewDlg;

namespace LSP
{
class WXDLLIMPEXP_SDK Manager : public wxEvtHandler
{
public:
    using Ptr_t = wxSharedPtr<Manager>;
    void StartServer(const LanguageServerEntry& entry);
    void StartServer(const wxString& entry);
    void RestartServer(const wxString& name);

    void StopServer(const wxString& entry);
    void DeleteServer(const wxString& name);

    void StopAll(const std::unordered_set<wxString>& languages = {});
    void StartAll(const std::unordered_set<wxString>& languages = {});

    inline void RestartAll(const std::unordered_set<wxString>& languages = {})
    {
        wxBusyCursor bc{};
        StopAll(languages);
        StartAll(languages);
    }

    void ClearAllDiagnostics();
    void SetWorkspaceType(FileExtManager::FileType type);

    /**
     * @brief convert LSP::SignatureHelp class to TagEntryPtrVector_t
     */
    void LSPSignatureHelpToTagEntries(TagEntryPtrVector_t& tags, const LSP::SignatureHelp& sighelp);
    /**
     * @brief restart language servers associated with a given languages
     */
    void Reload(const std::unordered_set<wxString>& languages = {});
    LanguageServerProtocol::Ptr_t GetServerForEditor(IEditor* editor);
    LanguageServerProtocol::Ptr_t GetServerByName(const wxString& name);
    LanguageServerProtocol::Ptr_t GetServerForLanguage(const wxString& lang);
    void ClearRestartCounters();
    static Manager& GetInstance();
    void Initialise();

    //----------------------------
    // Completion API.
    //----------------------------

    /**
     * @brief Shows an outline view for the specified editor using the LSP server.
     *
     * Retrieves the LSP server associated with @c editor. If the server exists and
     * supports document symbols, it requests the symbols for both quick‑outline and
     * outline‑view contexts and displays the result via @c ShowQuickOutlineDialog.
     *
     * If no server is available or document symbols are unsupported, a
     * @c wxEVT_CC_SHOW_QUICK_OUTLINE event is queued so that other components can
     * handle the request.
     *
     * @param editor Pointer to the editor for which the outline view should be displayed.
     */
    void ShowOutlineView(IEditor* editor);

    /**
     * @brief Attempts to locate the definition of the symbol under the caret in the given editor.
     *
     * The function first validates the @a editor pointer, then retrieves the appropriate LSP
     * server for the editor. If no server is available or the server does not support the
     * Document Symbols capability, the lookup is delegated to the traditional code‑completion
     * mechanism by posting a @c wxEVT_CC_FIND_SYMBOL event containing the word at the caret,
     * its position, and the file name.
     *
     * If a suitable server is found, the request is forwarded to the server via
     * @c server->FindDefinition(editor).
     *
     * @param editor Pointer to the editor instance where the symbol lookup should be performed.
     */
    void FindSymbol(IEditor* editor);

    /**
     * @brief Finds the declaration of a symbol in the given editor.
     *
     * This function attempts to find the declaration of a symbol at the current
     * cursor position in the specified editor. If no language server is available
     * for the editor, it fires a code completion event to handle the request
     * through alternative means.
     *
     * @param editor The editor instance to search for declarations in
     */
    void FindDeclaration(IEditor* editor);

    /**
     * @brief Initiates code completion for the specified editor.
     *
     * Attempts to obtain an LSP server for the given editor and request code
     * completion. If no server is available or the cursor is inside a comment,
     * a fallback simple word‑completion event is triggered. The method also
     * ensures the editor's control currently has focus before delegating to the
     * server.
     *
     * @param editor Pointer to the editor where completion is requested. Must not be nullptr.
     * @param kind   The trigger kind indicating how the completion was invoked (e.g., user‑initiated).
     */
    void CodeComplete(IEditor* editor, LSP::CompletionItem::eTriggerKind kind);

    /**
     * @brief Displays a function call tip (parameter hint) for the specified editor.
     *
     * The method obtains the appropriate language server for the editor. If a server
     * is available and the caret is not within a comment, the request is forwarded to
     * the server via `server->FunctionHelp(editor)`. Otherwise, it falls back to the
     * legacy code‑completion system by emitting a `clCodeCompletionEvent` containing
     * the word at the caret, the current cursor position, and the file name.
     *
     * @param editor Pointer to the `IEditor` instance for which the call tip is requested.
     *               Must be non‑null; the function returns immediately if the pointer is invalid.
     */
    void FunctionCalltip(IEditor* editor);

    /**
     * @brief Shows a hover tooltip (type information tip) for the given editor.
     *
     * This function checks the validity of the provided @c IEditor pointer and ensures
     * that the current cursor position is not inside a comment. It then attempts to
     * obtain a code‑completion server associated with the editor. If a server is
     * available, the request is delegated to the server's @c HoverTip method.
     * Otherwise, a @c clCodeCompletionEvent of type @c wxEVT_CC_TYPEINFO_TIP is
     * created and queued, providing basic type‑info tooltip data.
     *
     * @param editor Pointer to the @c IEditor instance for which the hover tip
     *               should be displayed. Must not be @c nullptr.
     *
     * @note The function performs early returns if the editor pointer is invalid
     *       or if the cursor is currently inside a comment block.
     *
     * @return void
     */
    void HoverTip(IEditor* editor);

    /**
     * @brief Requests document symbols from the LSP server for the given editor.
     *
     * This method validates the editor pointer, obtains the associated LSP server,
     * ensures that the server supports the Document Symbols capability, and then
     * sends a DocumentSymbols request for the outline view. A status message is
     * shown while the request is in progress.
     *
     * @param editor Pointer to the editor for which symbols are requested. Must not be `nullptr`.
     * @param cb Callback function that will be invoked with an `LSPEvent` containing the result of the request.
     *
     * @return `true` if the request was successfully dispatched, `false` if any of the
     *         pre‑conditions failed (e.g., `editor` is `nullptr`, no server is
     *         available, or the server does not support document symbols).
     */
    bool RequestSymbolsForEditor(IEditor* editor, std::function<void(const LSPEvent&)> cb);

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
    void OnFileSaved(clCommandEvent& event);
    void OnWorkspaceOpen(clWorkspaceEvent& event);
    void OnCompileCommandsGenerated(clCommandEvent& event);
    void OnSetDiagnostics(LSPEvent& event);
    void OnClearDiagnostics(LSPEvent& event);
    void OnShowQuickOutlineDlg(LSPEvent& event);
    void OnOulineViewSymbols(LSPEvent& event);
    void OnSemanticTokens(LSPEvent& event);
    void OnLogMessage(LSPEvent& event);
    void OnDocumentSymbolsForHighlight(LSPEvent& event);
    void OnBuildEnded(clBuildEvent& event);
    void OnOpenResource(wxCommandEvent& event);
    void OnEditorClosed(clCommandEvent& event);
    void OnActiveEditorChanged(wxCommandEvent& event);
    void OnWorkspaceScanCompleted(clWorkspaceEvent& event);
    void OnMarginClicked(clEditorEvent& event);
    void OnCodeActionAvailable(LSPEvent& event);
    void OnApplyEdits(LSPEvent& event);
    void OnGoinDown(clCommandEvent& event);

    void ShowQuickOutlineDialog(const LSPEvent& event);

    wxString GetEditorFilePath(IEditor* editor) const;
    /**
     * @brief find an editor either by local or remote path
     */
    IEditor* FindEditor(const wxString& path) const;
    IEditor* FindEditor(const LSPEvent& event) const;

    void DiscoverWorkspaceType();
    void UpdateNavigationBar();

    Manager();
    virtual ~Manager();

    struct CrashInfo {
        size_t times{0};
        time_t last_crash{0};
    };

    std::unordered_map<wxString, LanguageServerProtocol::Ptr_t> m_servers;
    std::unordered_map<wxString, CrashInfo> m_restartCounters;
    std::unordered_map<wxString, std::vector<LSP::SymbolInformation>> m_symbols_to_file_cache;
    LSPOutlineViewDlg* m_quick_outline_dlg{nullptr};
    std::unique_ptr<CodeLiteRemoteHelper> m_remoteHelper;
};
} // namespace LSP