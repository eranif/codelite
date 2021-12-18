#ifndef OUTLINETAB_H
#define OUTLINETAB_H

#include "LSP/LSPEvent.h"
#include "LSP/basic_types.h"
#include "wxcrafter.h"

#include <vector>

using namespace std;
class OutlineTab : public OutlineTabBaseClass
{
    wxString m_currentSymbolsFileName;
    vector<LSP::SymbolInformation> m_symbols;

private:
    void OnOutlineSymbols(LSPEvent& event);
    void OnActiveEditorChanged(wxCommandEvent& event);
    void OnAllEditorsClosed(wxCommandEvent& event);
    void RenderSymbols(const vector<LSP::SymbolInformation>& symbols, const wxString& filename);
    void ClearView();

public:
    OutlineTab(wxWindow* parent);
    virtual ~OutlineTab();

protected:
    virtual void OnItemSelected(wxDataViewEvent& event);
};
#endif // OUTLINETAB_H
