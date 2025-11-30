#ifndef LSPOUTLINEVIEW_H
#define LSPOUTLINEVIEW_H

#include <wx/treectrl.h>
#include <wx/dataview.h>
#include <wx/activityindicator.h>
#include <wx/event.h>
#include <wx/aui/auibar.h>
#include <clAuiToolBarArt.h>
#include "LSP/basic_types.h"
#include "LSP/LSPSymbolSorter.h"
#include "clTerminalViewCtrl.hpp"
#include "clTreeCtrl.h"
#include "codelite_exports.h"

using namespace LSP;

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_LSP_OUTLINE_SYMBOL_ACTIVATED, wxCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_LSP_OUTLINE_VIEW_DISMISS, wxCommandEvent);

class WXDLLIMPEXP_SDK LSPOutlineView : public wxPanel
{
public:
    enum class Mode {
        DOCUMENT_SYMBOL,
        SYMBOL_INFORMATION
    };
    enum Style {
        STYLE_DEFAULT       = 0,
        STYLE_NO_SEARCHBAR  = 1 << 0,
        STYLE_SMALL_FONT    = 1 << 1
    };
    LSPOutlineView(wxWindow* parent, wxWindowID id = wxID_ANY, long style = LSPOutlineView::STYLE_DEFAULT);
    virtual ~LSPOutlineView() = default;

    void SetSymbols(const std::vector<SymbolInformation>& symbols, const wxString& filename);
    void SetSymbols(const std::vector<DocumentSymbol>& symbols, const wxString& filename);
    void SetEmptySymbols();
    void SetLoadingMessage(const wxString& message) { m_loadingMessage = message; }
    /**
     * @brief Sorts the current DocumentSymbol. Does not refresh the tree - use DoInitialiseDocumentSymbol after!
     * @param sort
     */
    void SortSymbols(LSPSymbolSorter::SortType sort);
    void CollapseTree();
    void ExpandTree();
    
protected:
    wxString m_currentSymbolsFileName;
    std::vector<DocumentSymbol> m_documentSymbols;
    std::vector<SymbolInformation> m_symbolsInformation;
    enum ID {
        ID_SHOW_DETAILS,
        ID_SHOW_KIND,
        ID_SORT_KIND_NAME,
        ID_SORT_KIND_NAME_CASE_SENSIIVE,
        ID_SORT_LINE,
        ID_TOOL_SORT,
        ID_TOOL_DETAILS,
        ID_TOOL_KIND,
        ID_TOOL_COLLAPSE_ALL,
        ID_TOOL_EXPAND_ALL,
        ID_TOOL_MENU,
        ID_MENU_INITIALISE_EXPANDED,
        ID_MENU_KEEP_NAMESPACE_EXPANDED
    };
    
    Mode m_mode;
    long m_style = STYLE_DEFAULT;
    bool m_showDetails = true;
    bool m_showSymbolKind = false;
    bool m_initialiseExpanded = false;
    bool m_keepNamespacesExpanded = true;
    LSPSymbolSorter::SortType m_sortType = LSPSymbolSorter::SORT_LINE;    
    wxAuiToolBar* m_toolbar = nullptr;
    wxChoice* m_sortOptions = nullptr;
    wxTextCtrl* m_textCtrlFilter = nullptr;
    clTreeCtrl* m_treeCtrl = nullptr;
    clTerminalViewCtrl* m_terminalViewCtrl = nullptr;
    wxPanel* m_msgPanel = nullptr;
    wxActivityIndicator* m_msgIndicator = nullptr;
    wxStaticText* m_msgText = nullptr;
    wxString m_loadingMessage = _("Waiting for response from Language Server...");
    
    void CreateUI();
    void CreateToolbar();
    void DoActivate(const wxDataViewItem& item);
    void DoActivate(const wxTreeItemId& item);
    void DoFindNext();
    void DoFindPrev();
    void DoInitialiseSymbolInformation();    
    void DoInitialiseDocumentSymbol();
    void DoInitialiseEmpty();
    void AddDocumentSymbolRec(wxTreeItemId parent, const LSP::DocumentSymbol& symbol, LexerConf* lexer);    
    
    /**
     * @brief deletes all symbols and empties the tree/terminal view
     */
    void ClearView();
    /**
     * @brief Shows a message panel with an optional loading indicator
     * @param message
     * @param showLoadingIndicator
     */
    void ShowMessage(const wxString& message, bool showLoadingIndicator);
    void HideMessage();
    
    /**
     * @brief Checks if filename matches the current editor and 
     *          requests new symbols if not
     * @param filename
     * @return true if filename matches, false if it doesn't match and 
     *          symbols have been requested
     */
    bool CheckAndRequest(const wxString& filename);
    
    virtual void OnTreeItemActivated(wxTreeEvent& event);
    virtual void OnListKeyDown(wxKeyEvent& event);
    virtual void OnKeyDown(wxKeyEvent& event);
    virtual void OnItemActivated(wxDataViewEvent& event);
    virtual void OnEnter(wxCommandEvent& event);
    virtual void OnTextUpdated(wxCommandEvent& event);
    void OnMenu(wxCommandEvent& event);

};
#endif // LSPOUTLINEVIEW_H
