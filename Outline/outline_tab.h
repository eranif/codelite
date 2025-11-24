#ifndef OUTLINETAB_H
#define OUTLINETAB_H

#include "LSP/LSPEvent.h"
#include "LSP/basic_types.h"
#include "LSP/LSPSymbolStyle.h"
#include "wxcrafter.h"
#include <wx/treectrl.h>

#include <vector>

class OutlineTab : public OutlineTabBaseClass
{
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
    wxString m_currentSymbolsFileName;
    std::vector<LSP::DocumentSymbol> m_documentSymbols;
    std::vector<LSP::SymbolInformation> m_symbolsInformation;
    Mode m_mode;
    bool m_showDetails = true;
    bool m_showSymbolKind = false;
    
private:
    void OnOutlineSymbols(LSPEvent& event);
    void OnActiveEditorChanged(wxCommandEvent& event);
    void OnAllEditorsClosed(wxCommandEvent& event);
    void OnSideBarPageChanged(clCommandEvent& event);
    /**
     * @brief Checks if filename matches the current editor and 
     *          requests new symbols if not
     * @param filename
     * @return true if filename matches, false if it doesn't match and 
     *          symbols have been requested
     */
    bool CheckAndRequest(const wxString& filename);
    /**
     * @brief Build and show a symbol tree
     * @param symbols
     * @param filename
     */
    void RenderSymbols(const std::vector<LSP::DocumentSymbol>& symbols, const wxString& filename);
    /**
     * @brief Recursively add a DocumentSymbol to a clTreeCtrl
     * @param parent
     * @param symbol
     * @param lexer raw pointer to the lexer used for colors
     */
    void AddDocumentSymbolRec(wxTreeItemId parent, const LSP::DocumentSymbol& symbol, LexerConf* lexer); 
    /**
     * @brief build and show symbol list in case LS does not support hierarchies 
     * @param symbols
     * @param filename
     */
    void RenderSymbols(const std::vector<LSP::SymbolInformation>& symbols, const wxString& filename);
    /**
     * @brief Shows a message panel with an optional loading indicator
     * @param message
     * @param showLoadingIndicator
     */
    void ShowMessage(const wxString& message, bool showLoadingIndicator);
    void HideMessage();
    /**
     * @brief deletes all symbols and empties the tree/terminal view
     */
    void ClearView();
    

public:
    OutlineTab(wxWindow* parent);
    virtual ~OutlineTab();

protected:
    void OnOptionsButton(wxCommandEvent& event) override;
    void OnSortButton(wxCommandEvent& event) override;
    void OnMenu(wxCommandEvent& event);
    virtual void OnItemSelected(wxDataViewEvent& event) override;
    virtual void OnTreeItemSelected(wxTreeEvent& event);
};
#endif // OUTLINETAB_H
