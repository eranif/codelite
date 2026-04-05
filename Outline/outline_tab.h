#ifndef OUTLINETAB_H
#define OUTLINETAB_H

#include "LSP/LSPEvent.h"
#include "LSP/basic_types.h"
#include "wxcrafter.h"

#include <vector>

class OutlineTab : public OutlineTabBaseClass
{
    wxString m_currentSymbolsFileName;
    std::vector<LSP::SymbolInformation> m_symbols;

private:
    void OnOutlineSymbols(LSPEvent& event);
    void OnActiveEditorChanged(wxCommandEvent& event);
    void OnAllEditorsClosed(wxCommandEvent& event);
    void OnSideBarPageChanged(clCommandEvent& event);
    void RenderSymbols(const std::vector<LSP::SymbolInformation>& symbols, const wxString& filename);
    void ClearView();

public:
    explicit OutlineTab(wxWindow* parent);
    ~OutlineTab() override;

protected:
    void OnItemSelected(wxDataViewEvent& event) override;
};
#endif // OUTLINETAB_H
