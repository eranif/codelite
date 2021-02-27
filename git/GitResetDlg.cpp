#include "GitResetDlg.h"
#include "bitmap_loader.h"
#include "globals.h"
#include "imanager.h"
#include "windowattrmanager.h"

GitResetDlg::GitResetDlg(wxWindow* parent, const wxArrayString& filesToRevert, const wxArrayString& filesToRemove)
    : GitResetDlgBase(parent)
    , m_toggleReverts(false)
    , m_toggleRemoves(false)
{
    for(size_t i = 0; i < filesToRevert.GetCount(); ++i) {
        m_checkListBoxChanged->Append(filesToRevert.Item(i));
        m_checkListBoxChanged->Check(i, true);
    }
    for(size_t i = 0; i < filesToRemove.GetCount(); ++i) {
        m_checkListBoxNew->Append(filesToRemove.Item(i));
        m_checkListBoxNew->Check(i, true);
    }

    auto images = m_clToolbarAltered->GetBitmapsCreateIfNeeded();
    m_clToolbarAltered->AddTool(XRCID("toggle-all-altered"), _("Toggle All"), images->Add("check-all"));
    m_clToolbarAltered->Bind(wxEVT_TOOL, &GitResetDlg::OnToggleAllRevert, this, XRCID("toggle-all-altered"));
    m_clToolbarAltered->Bind(wxEVT_UPDATE_UI, &GitResetDlg::OnToggleAllRevertUI, this, XRCID("toggle-all-altered"));
    m_clToolbarAltered->Realize();
    
    images = m_clToolbarAdded->GetBitmapsCreateIfNeeded();
    m_clToolbarAdded->AddTool(XRCID("toggle-all-added"), _("Toggle All"), images->Add("check-all"));
    m_clToolbarAdded->Bind(wxEVT_TOOL, &GitResetDlg::OnToggleAllRemove, this, XRCID("toggle-all-added"));
    m_clToolbarAdded->Bind(wxEVT_UPDATE_UI, &GitResetDlg::OnToggleAllRemoveUI, this, XRCID("toggle-all-added"));
    m_clToolbarAdded->Realize();
    ::clSetSmallDialogBestSizeAndPosition(this);
}

GitResetDlg::~GitResetDlg() {}

wxArrayString GitResetDlg::GetItemsToRevert() const
{
    wxArrayString toRevert;
    for(size_t i = 0; i < m_checkListBoxChanged->GetCount(); ++i) {
        if(m_checkListBoxChanged->IsChecked(i)) {
            toRevert.Add(m_checkListBoxChanged->GetString(i));
        }
    }

    return toRevert;
}
wxArrayString GitResetDlg::GetItemsToRemove() const
{
    wxArrayString toRemove;
    for(size_t i = 0; i < m_checkListBoxNew->GetCount(); ++i) {
        if(m_checkListBoxNew->IsChecked(i)) {
            toRemove.Add(m_checkListBoxNew->GetString(i));
        }
    }

    return toRemove;
}

void GitResetDlg::OnToggleAllRevert(wxCommandEvent& event)
{
    for(size_t i = 0; i < m_checkListBoxChanged->GetCount(); ++i) {
        m_checkListBoxChanged->Check(i, m_toggleReverts);
    }
    m_toggleReverts = !m_toggleReverts;
}

void GitResetDlg::OnToggleAllRemove(wxCommandEvent& event)
{
    for(size_t i = 0; i < m_checkListBoxNew->GetCount(); ++i) {
        m_checkListBoxNew->Check(i, m_toggleRemoves);
    }
    m_toggleRemoves = !m_toggleRemoves;
}

void GitResetDlg::OnToggleAllRevertUI(wxUpdateUIEvent& event) { event.Enable(m_checkListBoxChanged->GetCount()); }

void GitResetDlg::OnToggleAllRemoveUI(wxUpdateUIEvent& event) { event.Enable(m_checkListBoxNew->GetCount()); }
