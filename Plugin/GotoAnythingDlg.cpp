#include "GotoAnythingDlg.h"

#include "bitmap_loader.h"
#include "clAnagram.h"
#include "clKeyboardManager.h"
#include "cl_config.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "globals.h"
#include "imanager.h"
#include "macros.h"
#include "windowattrmanager.h"

#include <algorithm>
#include <wx/app.h>

GotoAnythingDlg::GotoAnythingDlg(wxWindow* parent, const std::vector<clGotoEntry>& entries)
    : GotoAnythingBaseDlg(parent)
    , m_allEntries(entries)
{
    DoPopulate(m_allEntries);
    CallAfter(&GotoAnythingDlg::UpdateLastSearch);

    m_bitmaps.push_back(clGetManager()->GetStdIcons()->LoadBitmap("placeholder"));
    m_dvListCtrl->SetBitmaps(&m_bitmaps);
    ::clSetDialogBestSizeAndPosition(this);
}

GotoAnythingDlg::~GotoAnythingDlg()
{
    // clConfig::Get().Write("GotoAnything/LastSearch", m_textCtrlSearch->GetValue());
}

void GotoAnythingDlg::OnKeyDown(wxKeyEvent& event)
{
    event.Skip();
    if(event.GetKeyCode() == WXK_ESCAPE) {
        event.Skip(false);
        EndModal(wxID_CANCEL);
    } else if(event.GetKeyCode() == WXK_DOWN) {
        event.Skip(false);
        int row = m_dvListCtrl->GetSelectedRow();
        if((size_t)(row + 1) < m_dvListCtrl->GetItemCount()) {
            row++;
            DoSelectItem(m_dvListCtrl->RowToItem(row));
        }
    } else if(event.GetKeyCode() == WXK_UP) {
        event.Skip(false);
        int row = m_dvListCtrl->GetSelectedRow();
        if((row - 1) >= 0) {
            row--;
            DoSelectItem(m_dvListCtrl->RowToItem(row));
        }
    }
}

void GotoAnythingDlg::OnEnter(wxCommandEvent& event)
{
    wxUnusedVar(event);
    DoExecuteActionAndClose();
}

void GotoAnythingDlg::DoPopulate(const std::vector<clGotoEntry>& entries, const std::vector<int>& indexes)
{
    m_dvListCtrl->DeleteAllItems();
    m_dvListCtrl->Begin();
    for(size_t i = 0; i < entries.size(); ++i) {
        const clGotoEntry& entry = entries[i];
        wxVector<wxVariant> cols;
        cols.push_back(::MakeBitmapIndexText(entry.GetDesc(), 0));
        cols.push_back(entry.GetKeyboardShortcut());
        m_dvListCtrl->AppendItem(cols, indexes.empty() ? i : indexes[i]);
    }
    m_dvListCtrl->Commit();
    if(!entries.empty()) {
        m_dvListCtrl->SelectRow(0);
    }
}

void GotoAnythingDlg::DoExecuteActionAndClose()
{
    int row = m_dvListCtrl->GetSelectedRow();
    if(row == wxNOT_FOUND)
        return;

    // Execute the action
    int index = m_dvListCtrl->GetItemData(m_dvListCtrl->RowToItem(row));
    const clGotoEntry& entry = m_allEntries[index];
    clDEBUG() << "GotoAnythingDlg: action selected:" << entry.GetDesc() << clEndl;

    clGotoEvent evtAction(wxEVT_GOTO_ANYTHING_SELECTED);
    evtAction.SetEntry(entry);
    EventNotifier::Get()->AddPendingEvent(evtAction);
    EndModal(wxID_OK);
}

void GotoAnythingDlg::OnIdle(wxIdleEvent& e)
{
    e.Skip();
    ApplyFilter();
}

void GotoAnythingDlg::UpdateLastSearch() {}

void GotoAnythingDlg::ApplyFilter()
{
    // Create a list the matches the typed text
    wxString filter = m_textCtrlSearch->GetValue();
    if(m_currentFilter == filter)
        return;

    // Update the last applied filter
    m_currentFilter = filter;
    if(filter.IsEmpty()) {
        DoPopulate(m_allEntries);
    } else {

        // Filter the list
        clAnagram anagram(filter);
        std::vector<clGotoEntry> matchedEntries;
        std::vector<int> matchedEntriesIndex;
        for(size_t i = 0; i < m_allEntries.size(); ++i) {
            const clGotoEntry& entry = m_allEntries[i];
            if(anagram.MatchesInOrder(entry.GetDesc())) {
                matchedEntries.push_back(entry);
                matchedEntriesIndex.push_back(i);
            }
        }

        // And populate the list
        DoPopulate(matchedEntries, matchedEntriesIndex);
    }
}

void GotoAnythingDlg::OnItemActivated(wxDataViewEvent& event)
{
    wxUnusedVar(event);
    DoExecuteActionAndClose();
}

void GotoAnythingDlg::DoSelectItem(const wxDataViewItem& item)
{
    CHECK_ITEM_RET(item);
    m_dvListCtrl->UnselectAll();
    m_dvListCtrl->Select(item);
    m_dvListCtrl->EnsureVisible(item);
}
