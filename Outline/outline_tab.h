#ifndef OUTLINETAB_H
#define OUTLINETAB_H

#include "LSP/LSPEvent.h"
#include "LSP/LSPOutlineView.h"

class OutlineTab : public LSPOutlineView
{
private:
    void OnOutlineSymbols(LSPEvent& event);
    void OnActiveEditorChanged(wxCommandEvent& event);
    void OnAllEditorsClosed(wxCommandEvent& event);
    void OnSideBarPageChanged(clCommandEvent& event);

public:
    OutlineTab(wxWindow* parent);
    virtual ~OutlineTab();
};
#endif // OUTLINETAB_H
