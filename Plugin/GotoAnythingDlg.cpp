#include "GotoAnythingDlg.h"
#include "windowattrmanager.h"
#include "clKeyboardManager.h"
#include "event_notifier.h"
#include <wx/app.h>
#include <algorithm>
#include "file_logger.h"

GotoAnythingDlg::GotoAnythingDlg(wxWindow* parent)
    : GotoAnythingBaseDlg(parent)
{
    DoPopulate(m_allEntries);
    WindowAttrManager::Load(this);
}

GotoAnythingDlg::~GotoAnythingDlg()
{
    std::for_each(m_allEntries.begin(), m_allEntries.end(), [&](const std::pair<wxString, GotoAnythingItemData*>& p) {
        delete p.second;
    });
    m_allEntries.clear();
}

void GotoAnythingDlg::OnKeyDown(wxKeyEvent& event)
{
    event.Skip();
    if(event.GetUnicodeKey() == WXK_ESCAPE) {
        EndModal(wxID_CANCEL);
    }
}

void GotoAnythingDlg::OnEnter(wxCommandEvent& event)
{
    wxUnusedVar(event);
    DoExecuteActionAndClose();
}

GotoAnythingItemData* GotoAnythingDlg::GetSelectedItemData()
{
    wxDataViewItem item = m_dvListCtrl->GetSelection();
    CHECK_ITEM_RET_NULL(item);
    return reinterpret_cast<GotoAnythingItemData*>(m_dvListCtrl->GetItemData(item));
}

void GotoAnythingDlg::DoPopulate(std::vector<std::pair<wxString, GotoAnythingItemData*> >& entries)
{
    m_dvListCtrl->DeleteAllItems();
    std::for_each(entries.begin(), entries.end(), [&](const std::pair<wxString, GotoAnythingItemData*>& p) {
        wxVector<wxVariant> cols;
        cols.push_back(p.first);
        m_dvListCtrl->AppendItem(cols, (wxUIntPtr)p.second);
    });

    if(!entries.empty()) {
        m_dvListCtrl->SelectRow(0);
    }
}

void GotoAnythingDlg::DoExecuteActionAndClose()
{
    GotoAnythingItemData* itemData = GetSelectedItemData();
    CHECK_PTR_RET(itemData);

    // Execute the action
    clDEBUG() << "GotoAnythingDlg: action selected:" << itemData->m_desc << clEndl;

    // TODO:: execute the action here
    EndModal(wxID_OK);
}

void GotoAnythingDlg::OnIdle(wxIdleEvent& e)
{
    e.Skip();
    // Create a list the matches the typed text
    wxString filter = m_textCtrl8->GetValue();
    if(m_currentFilter == filter) return;

    // Update the last applied filter
    m_currentFilter = filter;
    if(filter.IsEmpty()) {
        DoPopulate(m_allEntries);
    } else {

        // Filter the list
        std::vector<std::pair<wxString, GotoAnythingItemData*> > matchedEntries;
        std::for_each(
            m_allEntries.begin(), m_allEntries.end(), [&](const std::pair<wxString, GotoAnythingItemData*>& p) {
                if(p.first.Contains(filter)) {
                    matchedEntries.push_back(p);
                }
            });

        // And populate the list
        DoPopulate(matchedEntries);
    }
}
