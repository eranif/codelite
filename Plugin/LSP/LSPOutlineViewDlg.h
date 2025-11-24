#ifndef LSPOUTLINEVIEWDLG_H
#define LSPOUTLINEVIEWDLG_H

#include "LSP/basic_types.h"
#include "UI.h"
#include "codelite_exports.h"

#include <vector>

using namespace LSP;

class WXDLLIMPEXP_SDK LSPOutlineViewDlg : public LSPOutlineViewDlgBase
{
    std::vector<DocumentSymbol> m_documentSymbols;
    std::vector<SymbolInformation> m_symbolsInformation;

private:
    enum class Mode {
        DOCUMENT_SYMBOL,
        SYMBOL_INFORMATION
    };
    enum ID {
        ID_SHOW_DETAILS,
        ID_SHOW_KIND,
        ID_SORT_ALPHABETICALLY,
        ID_SORT_LINE
    };
    
    Mode m_mode;
    bool m_showDetails = true;
    bool m_showSymbolKind = false;
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

protected:
    void OnOptionsButton(wxCommandEvent& event) override;
    void OnSortButton(wxCommandEvent& event) override; 
    void OnMenu(wxCommandEvent& event);
    void OnTreeItemActivated(wxTreeEvent& event);
    virtual void OnListKeyDown(wxKeyEvent& event) override;
    virtual void OnKeyDown(wxKeyEvent& event) override;
    virtual void OnItemActivated(wxDataViewEvent& event) override;
    virtual void OnEnter(wxCommandEvent& event) override;
    virtual void OnTextUpdated(wxCommandEvent& event) override;
};
#endif // LSPOUTLINEVIEWDLG_H
