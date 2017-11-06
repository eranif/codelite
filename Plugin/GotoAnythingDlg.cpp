#include "GotoAnythingDlg.h"
#include "clGotoAnythingManager.h"
#include "clKeyboardManager.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "windowattrmanager.h"
#include <algorithm>
#include <wx/app.h>
#include "clAnagram.h"

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
    if(event.GetUnicodeKey() == WXK_ESCAPE) {
        event.Skip(false);
        EndModal(wxID_CANCEL);
    }
}

void GotoAnythingDlg::OnEnter(wxCommandEvent& event)
{
    wxUnusedVar(event);
    DoExecuteActionAndClose();
}

void GotoAnythingDlg::DoPopulate(const std::vector<wxString>& entries)
{
    m_dvListCtrl->DeleteAllItems();
    std::for_each(entries.begin(), entries.end(), [&](const wxString& action) {
        wxVector<wxVariant> cols;
        cols.push_back(action);
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
        std::vector<wxString> matchedEntries;
        std::for_each(m_allEntries.begin(), m_allEntries.end(), [&](const wxString& str) {
            if(anagram.Matches(str)) { matchedEntries.push_back(str); }
        });

        // And populate the list
        DoPopulate(matchedEntries);
    }
}
