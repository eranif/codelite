#ifndef LSPOUTLINEVIEWDLG_H
#define LSPOUTLINEVIEWDLG_H

#include "LSP/basic_types.h"
#include "LSP/LSPSymbolSorter.h"
#include "clAuiToolBarArt.h"
#include "clToolBar.h"
#include "UI.h"
#include "codelite_exports.h"

#include <vector>

using namespace LSP;

class WXDLLIMPEXP_SDK LSPOutlineViewDlg : public LSPOutlineViewDlgBase
{
private:
    std::vector<DocumentSymbol> m_documentSymbols;
    std::vector<SymbolInformation> m_symbolsInformation;
    enum class Mode {
        DOCUMENT_SYMBOL,
        SYMBOL_INFORMATION
    };
    enum ID {
        ID_SHOW_DETAILS,
        ID_SHOW_KIND,
        ID_SORT_KIND_NAME,
        ID_SORT_KIND_NAME_CASE_SENSIIVE,
        ID_SORT_LINE,
        ID_TOOL_SORT,
        ID_TOOL_DETAILS,
        ID_TOOL_KIND
    };
    
    Mode m_mode;
    bool m_showDetails = true;
    bool m_showSymbolKind = false;
    LSPSymbolSorter::SortType m_sortType = LSPSymbolSorter::SORT_LINE;    
    wxAuiToolBar* m_toolbar = nullptr;
    wxChoice* m_sortOptions = nullptr;
    wxTextCtrl* m_textCtrlFilter = nullptr;
    
    void CreateToolbar();
    void DoActivate(const wxDataViewItem& item);
    void DoActivate(const wxTreeItemId& item);
    void DoFindNext();
    void DoFindPrev();
    void DoInitialiseSymbolInformation();    
    void DoInitialiseDocumentSymbol();
    void DoInitialiseEmpty();
    void AddDocumentSymbolRec(wxTreeItemId parent, const LSP::DocumentSymbol& symbol, LexerConf* lexer);    

public:
    LSPOutlineViewDlg(wxWindow* parent);
    virtual ~LSPOutlineViewDlg() = default;

    void SetSymbols(const std::vector<SymbolInformation>& symbols);
    void SetSymbols(const std::vector<DocumentSymbol>& symbols);
    void SetEmptySymbols();
    /**
     * @brief Sorts the current DocumentSymbol. Does not refresh the tree - use DoInitialiseDocumentSymbol after!
     * @param sort
     */
    void SortSymbols(LSPSymbolSorter::SortType sort);

protected:
    void OnTreeItemActivated(wxTreeEvent& event);
    virtual void OnListKeyDown(wxKeyEvent& event) override;
    virtual void OnKeyDown(wxKeyEvent& event);
    virtual void OnItemActivated(wxDataViewEvent& event) override;
    virtual void OnEnter(wxCommandEvent& event);
    virtual void OnTextUpdated(wxCommandEvent& event);
};
#endif // LSPOUTLINEVIEWDLG_H
