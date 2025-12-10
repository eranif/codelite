#ifndef LSPOUTLINEVIEW_H
#define LSPOUTLINEVIEW_H

#include <wx/treectrl.h>
#include <wx/dataview.h>
#include <wx/activityindicator.h>
#include <wx/event.h>
#include <wx/aui/auibar.h>
#include <clAuiToolBarArt.h>
#include "LSP/basic_types.h"
#include "LSP/LSPSymbolParser.h"
#include "clTerminalViewCtrl.hpp"
#include "clTreeCtrl.h"
#include "codelite_exports.h"

using namespace LSP;

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_LSP_OUTLINE_SYMBOL_ACTIVATED, wxCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_LSP_OUTLINE_VIEW_DISMISS, wxCommandEvent);

#define kConfigOutlineStartAllCollapsed "OutlineStartAllCollapsed"
#define kConfigOutlineStartEnumsCollapsed "OutlineStartEnumsCollapsed"
#define kConfigOutlineStartClassesCollapsed "OutlineStartClassesCollapsed"
#define kConfigOutlineStartSubclassesCollapsed "OutlineStartSubclassesCollapsed"
#define kConfigOutlineStartFunctionsCollapsed "OutlineStartFunctionsCollapsed"
#define kConfigOutlineStartVariablesCollapsed "OutlineStartVariablesCollapsed"
#define kConfigOutlineStartContainersCollapsed "OutlineStartFunctionsCollapsed"
#define kConfigOutlineKeepNamespacesExpanded "OutlineKeepNamespacesExpanded"
#define kConfigOutlineSortType "OutlineSortType"
#define kConfigOutlineShowSymbolDetails "OutlineShowSymbolDetails"
#define kConfigOutlineShowSymbolKinds "OutlineShowSymbolKinds"
#define kConfigOutlineForceTree "OutlineForceTree"

class WXDLLIMPEXP_SDK LSPOutlineView : public wxPanel
{
public:
    enum class Mode {
        DOCUMENT_SYMBOL,
        SYMBOL_INFORMATION,
        SYMBOL_INFORMATION_PARSED,
    };
    enum Style {
        STYLE_DEFAULT               = 0,
        STYLE_NO_SEARCHBAR          = 1 << 0,
        STYLE_SMALL_FONT            = 1 << 1,
        STYLE_ACTIVATE_ON_SELECTION = 1 << 2
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
    void SortSymbols(LSPSymbolParser::SortType sort);
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
        ID_MENU_KEEP_NAMESPACE_EXPANDED,
        ID_MENU_FORCE_TREE,
        ID_MENU_INITIALISE_ALL_COLLAPSED,
        ID_MENU_INITIALISE_ENUMS_COLLAPSED,
        ID_MENU_INITIALISE_FUNCTIONS_COLLAPSED,
        ID_MENU_INITIALISE_CLASSES_COLLAPSED,
        ID_MENU_INITIALISE_SUBCLASSES_COLLAPSED,
        ID_MENU_INITIALISE_CONTAINERS_COLLAPSED,
        ID_MENU_INITIALISE_VARIABLES_COLLAPSED
    };
    struct TreeState {        
        // Pointer to DocumentSymbol wrapped in wxTreeItemIds
        wxTreeItemId selectedNode;
        bool manuallyCollapsed = false;
        bool manuallyExpanded = false;
        // Pointers to DocumentSymbols wrapped in wxTreeItemIds
        std::vector<wxTreeItemId> expandedNodes;
        // Pointers to DocumentSymbols wrapped in wxTreeItemIds
        std::vector<wxTreeItemId> collapsedNodes;
        bool isBulkChanging = false;
    };
    
    Mode m_mode;
    long m_style = STYLE_DEFAULT;
    bool m_showDetails = true;
    bool m_showSymbolKind = false;
    bool m_initialiseAllCollapsed = false;
    bool m_initialiseEnumsCollapsed = false;
    bool m_initialiseSubclassesCollapsed = false;
    bool m_initialiseFunctionsCollapsed = false;
    bool m_initialiseClassesCollapsed = false;
    bool m_initialiseContainersCollapsed = false;
    bool m_initialiseVariablesCollapsed = false;
    bool m_keepNamespacesExpanded = true;
    bool m_forceTree = true;
    
    TreeState m_treeState;    
    std::vector<wxBitmap> m_bitmaps;
    
    LSPSymbolParser::SortType m_sortType = LSPSymbolParser::SORT_LINE;    
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
     * @brief Recursively Restores expanded/collapsed/selected state after sorting/showing details or kind
     */
    void RestoreTreeStateRec(wxTreeItemId parent);
    /**
     * @brief Initializes the expanded/collapsed state according to initisation/config options 
     */
    void InitExpandedState();
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
    
    void OnTreeItemSelected(wxTreeEvent& event);
    void OnTreeItemActivated(wxTreeEvent& event);
    void OnTreeItemExpanded(wxTreeEvent& event);
    void OnTreeItemCollapsed(wxTreeEvent& event);
    void OnListKeyDown(wxKeyEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnItemActivated(wxDataViewEvent& event);
    void OnEnter(wxCommandEvent& event);
    void OnTextUpdated(wxCommandEvent& event);
    void OnMenu(wxCommandEvent& event);
    /**
     * @brief Handles toolbar actions
     * @param event
     */
    void OnToolBar(wxCommandEvent& event);
    /**
     * @brief Shows the settings menu 
     * @param event
     */
    void OnToolBarDropdown(wxAuiToolBarEvent& event);    
    /**
     * @brief Handles choice events from sorting dropdown
     * @param event
     */
    void OnSortChanged(wxCommandEvent& event);

};
#endif // LSPOUTLINEVIEW_H
