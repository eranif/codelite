#include "LSPOutlineViewDlg.h"
#include "globals.h"
#include <wx/dataview.h>
#include "globals.h"
#include <wx/dataview.h>


LSPOutlineViewDlg::LSPOutlineViewDlg(wxWindow* parent)
    : LSPOutlineViewDlgBase(parent)
{
    m_outlineView = new LSPOutlineView(this, wxID_ANY, LSPOutlineView::STYLE_DEFAULT);
    m_outlineView->SetLoadingMessage(_("Waiting for response from Language Server...\nHit ESCAPE to dismiss!"));
    m_outlineView->SetEmptyMessage(_("No symbols found.\nHit ESCAPE to dismiss!"));
    GetSizer()->Add(m_outlineView, 1, wxEXPAND);
    clSetDialogBestSizeAndPosition(this);  

    m_outlineView->Bind(wxEVT_LSP_OUTLINE_SYMBOL_ACTIVATED, &LSPOutlineViewDlg::OnDismiss, this);
    m_outlineView->Bind(wxEVT_LSP_OUTLINE_VIEW_DISMISS, &LSPOutlineViewDlg::OnDismiss, this);
    
    EventNotifier::Get()->Bind(wxEVT_ACTIVE_EDITOR_CHANGED, &LSPOutlineViewDlg::OnDismiss, this);
    EventNotifier::Get()->Bind(wxEVT_ALL_EDITORS_CLOSED, &LSPOutlineViewDlg::OnDismiss, this);
}

void LSPOutlineViewDlg::OnDismiss(wxCommandEvent& event) 
{
    Hide();
    event.Skip();
}
    
