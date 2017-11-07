#include "GotoAnythingDlg.h"
#include "clAnagram.h"
#include "clKeyboardManager.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "windowattrmanager.h"
#include <algorithm>
#include <wx/app.h>

GotoAnythingDlg::GotoAnythingDlg(wxWindow* parent)
    : GotoAnythingBaseDlg(parent)
{
    m_allEntries = clGotoAnythingManager::Get().GetActions();
    DoPopulate(m_allEntries);
    WindowAttrManager::Load(this);
}

GotoAnythingDlg::~GotoAnythingDlg() { m_allEntries.clear(); }

void GotoAnythingDlg::OnKeyDown(wxKeyEvent& event)
{
    event.Skip();
    if(event.GetKeyCode() == WXK_ESCAPE) {
        event.Skip(false);
        EndModal(wxID_CANCEL);
    } else if(event.GetKeyCode() == WXK_DOWN) {
        event.Skip(false);
        int row = m_dvListCtrl->GetSelectedRow();
        if((row + 1) < m_dvListCtrl->GetItemCount()) {
            row++;
            m_dvListCtrl->SelectRow(row);
        }
    } else if(event.GetKeyCode() == WXK_UP) {
        event.Skip(false);
        int row = m_dvListCtrl->GetSelectedRow();
        if((row - 1) >= 0) {
            row--;
            m_dvListCtrl->SelectRow(row);
        }
    }
}

void GotoAnythingDlg::OnEnter(wxCommandEvent& event)
{
    wxUnusedVar(event);
    DoExecuteActionAndClose();
}

void GotoAnythingDlg::DoPopulate(const std::vector<clGotoEntry>& entries)
{
    m_dvListCtrl->DeleteAllItems();
    std::for_each(entries.begin(), entries.end(), [&](const clGotoEntry& entry) {
        wxVector<wxVariant> cols;
        cols.push_back(entry.GetDesc());
        cols.push_back(entry.GetKeyboardShortcut());
        m_dvListCtrl->AppendItem(cols);
    });

    if(!entries.empty()) { m_dvListCtrl->SelectRow(0); }
}

void GotoAnythingDlg::DoExecuteActionAndClose()
{
    int row = m_dvListCtrl->GetSelectedRow();
    if(row == wxNOT_FOUND) return;

    // Execute the action
    wxVariant v;
    m_dvListCtrl->GetValue(v, row, 0);
    clDEBUG() << "GotoAnythingDlg: action selected:" << v.GetString() << clEndl;

    clCommandEvent evtAction(wxEVT_GOTO_ANYTHING_SELECTED);
    evtAction.SetString(v.GetString());
    EventNotifier::Get()->AddPendingEvent(evtAction);
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
        clAnagram anagram(filter);
        std::vector<clGotoEntry> matchedEntries;
        std::for_each(m_allEntries.begin(), m_allEntries.end(), [&](const clGotoEntry& entry) {
            if(anagram.MatchesInOrder(entry.GetDesc())) { matchedEntries.push_back(entry); }
        });

        // And populate the list
        DoPopulate(matchedEntries);
    }
}
