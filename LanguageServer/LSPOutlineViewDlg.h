#ifndef LSPOUTLINEVIEWDLG_H
#define LSPOUTLINEVIEWDLG_H

#include "LSP/basic_types.h"
#include "UI.h"

#include <vector>

using namespace std;
using namespace LSP;

class LSPOutlineViewDlg : public LSPOutlineViewDlgBase
{
    vector<SymbolInformation> m_symbols;

private:
    void DoSelectionActivate();

public:
    LSPOutlineViewDlg(wxWindow* parent, const vector<SymbolInformation>& symbols);
    virtual ~LSPOutlineViewDlg();

protected:
    virtual void OnKeyDown(wxKeyEvent& event);
    virtual void OnItemActivated(wxDataViewEvent& event);
    virtual void OnEnter(wxCommandEvent& event);
    virtual void OnTextUpdated(wxCommandEvent& event);
};
#endif // LSPOUTLINEVIEWDLG_H
