#ifndef LSPOUTLINEVIEWDLG_H
#define LSPOUTLINEVIEWDLG_H

#include "LSPOutlineView.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "UI.h"

using namespace LSP;

class WXDLLIMPEXP_SDK LSPOutlineViewDlg : public LSPOutlineViewDlgBase
{
private:
    LSPOutlineView* m_outlineView = nullptr;
    void OnDismiss(wxCommandEvent& event);
public:
    LSPOutlineViewDlg(wxWindow* parent);
    virtual ~LSPOutlineViewDlg() = default;
    LSPOutlineView* GetOutlineView() { return m_outlineView; }
};
#endif // LSPOUTLINEVIEWDLG_H
