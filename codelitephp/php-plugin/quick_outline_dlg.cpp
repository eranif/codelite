#include "quick_outline_dlg.h"
#include "globals.h"
#include "precompiled_header.h"

#include <globals.h>
#include <ieditor.h>
#include <imanager.h>
#include <windowattrmanager.h>

PHPQuickOutlineDlg::PHPQuickOutlineDlg(wxWindow* parent, IEditor* editor, IManager* manager)
    : QuickOutlineDlgBase(parent)
    , m_editor(editor)
    , m_mgr(manager)
{
    // Parse the current file
    m_treeCtrlLayout->SetManager(m_mgr);
    m_treeCtrlLayout->SetEditor(m_editor);
    m_treeCtrlLayout->Construct();

    m_treeCtrlLayout->Connect(wxEVT_COMMAND_TREE_ITEM_ACTIVATED,
                              wxTreeEventHandler(PHPQuickOutlineDlg::OnItemActivated), NULL, this);
    m_treeCtrlLayout->SetFocus();
    m_treeCtrlLayout->Bind(wxEVT_KEY_DOWN, &PHPQuickOutlineDlg::OnKeyDown, this);
    SetName("PHPQuickOutlineDlg");
    ::clSetDialogBestSizeAndPosition(this);
}

PHPQuickOutlineDlg::~PHPQuickOutlineDlg()
{
    m_treeCtrlLayout->Unbind(wxEVT_KEY_DOWN, &PHPQuickOutlineDlg::OnKeyDown, this);
}

void PHPQuickOutlineDlg::OnKeyDown(wxKeyEvent& event)
{
    event.Skip();
    if(event.GetKeyCode() == WXK_ESCAPE) {
        event.Skip(false);
        Close();
    }
}

void PHPQuickOutlineDlg::OnItemActivated(wxTreeEvent& event) { DoItemSelected(event.GetItem()); }

void PHPQuickOutlineDlg::DoItemSelected(const wxTreeItemId& item)
{
    if(item.IsOk()) {
        QItemData* data = dynamic_cast<QItemData*>(m_treeCtrlLayout->GetItemData(item));
        if(data && data->m_entry) {
            DoSelectMatch(data->m_entry->GetFilename().GetFullPath(), data->m_entry->GetLine() - 1,
                          data->m_entry->GetShortName());
            Close();
        }
    }
}

void PHPQuickOutlineDlg::DoSelectMatch(const wxString& filename, int line, const wxString& what)
{
    if(m_mgr->OpenFile(filename, wxT(""), line)) {
        IEditor* editor = m_mgr->GetActiveEditor();
        if(editor) {
            m_mgr->FindAndSelect(what, what, editor->PosFromLine(line));
            editor->SetActive();
        }
    }
}
