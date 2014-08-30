#include "precompiled_header.h"
#include "quick_outline_dlg.h"
#include <globals.h>
#include "php_parser_api.h"
#include <imanager.h>
#include <ieditor.h>
#include <windowattrmanager.h>



PHPQuickOutlineDlg::PHPQuickOutlineDlg( wxWindow* parent, IEditor *editor, IManager *manager )
    : QuickOutlineDlgBase( parent )
    , m_editor(editor)
    , m_mgr(manager)
{
    // Parse the current file
    m_treeCtrlLayout->SetManager(m_mgr);
    m_treeCtrlLayout->SetEditor(m_editor);
    m_treeCtrlLayout->Construct();

    m_treeCtrlLayout->Connect(wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler(PHPQuickOutlineDlg::OnItemActivated), NULL, this);
    m_textCtrl->SetFocus();
    WindowAttrManager::Load(this, "PHPQuickOutlineDlg", NULL);
}

PHPQuickOutlineDlg::~PHPQuickOutlineDlg()
{
    WindowAttrManager::Save(this, "PHPQuickOutlineDlg", NULL);
}

void PHPQuickOutlineDlg::OnKeyDown( wxKeyEvent& event )
{
    if(event.GetKeyCode() == WXK_ESCAPE)
        Close();

    else if(event.GetKeyCode() == WXK_DOWN ) {
        m_treeCtrlLayout->AdvanceSelection( true );

    } else if ( event.GetKeyCode() == WXK_UP ) {
        m_treeCtrlLayout->AdvanceSelection( false );

    } else {
        event.Skip();
    }
}

void PHPQuickOutlineDlg::OnTextEntered( wxCommandEvent& event )
{
    event.Skip();
    wxString filter = m_textCtrl->GetValue();
    m_treeCtrlLayout->FindWord(filter);
}

void PHPQuickOutlineDlg::OnItemActivated(wxTreeEvent& event)
{
    DoItemSelected(event.GetItem());
}

void PHPQuickOutlineDlg::OnEnter(wxCommandEvent& event)
{
    wxUnusedVar(event);
    DoItemSelected(m_treeCtrlLayout->GetSelection());
}

void PHPQuickOutlineDlg::DoItemSelected(const wxTreeItemId& item)
{
    if(item.IsOk()) {
        QItemData *data = dynamic_cast<QItemData*>(m_treeCtrlLayout->GetItemData(item));
        if(data && data->m_entry) {
            DoSelectMatch(data->m_entry->getFileName(), data->m_entry->getLineNumber() -1, data->m_entry->getName());
            Close();
        }
    }
}

void PHPQuickOutlineDlg::DoSelectMatch(const wxString& filename, int line, const wxString &what)
{
    if(m_mgr->OpenFile(filename, wxT(""), line)) {
        IEditor *editor = m_mgr->GetActiveEditor();
        if(editor) {
            m_mgr->FindAndSelect(what, what, editor->GetCurrentPosition());
        }
    }
}
