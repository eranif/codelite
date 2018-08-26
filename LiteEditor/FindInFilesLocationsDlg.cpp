#include "FindInFilesLocationsDlg.h"
#include "macros.h"
#include <wx/dirdlg.h>

FindInFilesLocationsDlg::FindInFilesLocationsDlg(wxWindow* parent, const wxArrayString& locations)
    : FindInFilesLocationsDlgBase(parent)
    , m_initialLocations(locations)
{
    // Add the default locations
    // Populate the
    DoAppendItem(SEARCH_IN_WORKSPACE_FOLDER);
    m_initialLocations.Remove(SEARCH_IN_WORKSPACE_FOLDER);
    DoAppendItem(SEARCH_IN_WORKSPACE);
    m_initialLocations.Remove(SEARCH_IN_WORKSPACE);
    DoAppendItem(SEARCH_IN_PROJECT);
    m_initialLocations.Remove(SEARCH_IN_PROJECT);
    DoAppendItem(SEARCH_IN_CURR_FILE_PROJECT);
    m_initialLocations.Remove(SEARCH_IN_CURR_FILE_PROJECT);
    DoAppendItem(SEARCH_IN_CURRENT_FILE);
    m_initialLocations.Remove(SEARCH_IN_CURRENT_FILE);
    DoAppendItem(SEARCH_IN_OPEN_FILES);
    m_initialLocations.Remove(SEARCH_IN_OPEN_FILES);

    for(size_t i = 0; i < m_initialLocations.size(); ++i) {
        DoAppendItem(m_initialLocations.Item(i));
    }
    m_initialLocations.clear();
}

FindInFilesLocationsDlg::~FindInFilesLocationsDlg() {}

void FindInFilesLocationsDlg::DoAppendItem(const wxString& str)
{
    int pos = m_checkListBoxLocations->Append(str);
    m_checkListBoxLocations->Check(pos, (m_initialLocations.Index(str) != wxNOT_FOUND));
}

wxArrayString FindInFilesLocationsDlg::GetLocations() const
{
    wxArrayString locs;
    for(size_t i = 0; i < m_checkListBoxLocations->GetCount(); ++i) {
        if(m_checkListBoxLocations->IsChecked(i)) {
            locs.Add(m_checkListBoxLocations->GetString(i));
        }
    }
    return locs;
}
void FindInFilesLocationsDlg::OnAddPath(wxCommandEvent& event)
{
    wxString path = ::wxDirSelector();
    if(path.IsEmpty()) return;
    DoAppendItem(path, true);
}

void FindInFilesLocationsDlg::OnDeletePath(wxCommandEvent& event)
{
    int sel = m_checkListBoxLocations->GetSelection();
    if(sel == wxNOT_FOUND) return;
    m_checkListBoxLocations->Delete(sel);
}

void FindInFilesLocationsDlg::OnDeletePathUI(wxUpdateUIEvent& event)
{
    wxString selectedStr = m_checkListBoxLocations->GetStringSelection();
    event.Enable(!selectedStr.IsEmpty() && !selectedStr.StartsWith("<"));
}

void FindInFilesLocationsDlg::DoAppendItem(const wxString& str, bool check)
{
    int pos = m_checkListBoxLocations->Append(str);
    m_checkListBoxLocations->Check(pos, true);
}
