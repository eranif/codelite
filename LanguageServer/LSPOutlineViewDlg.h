#ifndef LSPOUTLINEVIEWDLG_H
#define LSPOUTLINEVIEWDLG_H

#include "LSP/basic_types.h"
#include "UI.h"

#include <vector>

using namespace LSP;

class LSPOutlineViewDlg : public LSPOutlineViewDlgBase
{
    std::vector<SymbolInformation> m_symbols;

private:
    void DoSelectionActivate();
    void DoFindNext();
    void DoFindPrev();
    void DoInitialise();

public:
    LSPOutlineViewDlg(wxWindow* parent);
    virtual ~LSPOutlineViewDlg();

    void SetSymbols(const std::vector<SymbolInformation>& symbols);

protected:
    virtual void OnListKeyDown(wxKeyEvent& event);
    virtual void OnKeyDown(wxKeyEvent& event);
    virtual void OnItemActivated(wxDataViewEvent& event);
    virtual void OnEnter(wxCommandEvent& event);
    virtual void OnTextUpdated(wxCommandEvent& event);
};
#endif // LSPOUTLINEVIEWDLG_H
