#ifndef LSPOUTLINEVIEWDLG_H
#define LSPOUTLINEVIEWDLG_H

#include "LSPOutlineView.h"
#include "UI.h"

using namespace LSP;

class WXDLLIMPEXP_SDK LSPOutlineViewDlg : public LSPOutlineViewDlgBase
{
private:
    LSPOutlineView* m_outlineView = nullptr;
public:
    LSPOutlineViewDlg(wxWindow* parent);
    virtual ~LSPOutlineViewDlg() = default;
    LSPOutlineView* GetOutlineView() { return m_outlineView; }
    
protected:
};
#endif // LSPOUTLINEVIEWDLG_H
