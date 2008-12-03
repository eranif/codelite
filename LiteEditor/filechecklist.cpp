#include "filechecklist.h"

FileCheckList::FileCheckList(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size)
    : FileCheckListBase( parent, id, title, pos, size )
    , m_baseDir(wxGetCwd())
{
}

void FileCheckList::OnCheckAll( wxCommandEvent& event )
{
    wxUnusedVar(event);
    for (size_t i = 0; i < m_files.size(); i++) {
        m_files[i].second = true;
        m_fileCheckList->Check(i, true);
    }
}

void FileCheckList::OnClearAll( wxCommandEvent& event )
{
    wxUnusedVar(event);
    for (size_t i = 0; i < m_files.size(); i++) {
        m_files[i].second = false;
        m_fileCheckList->Check(i, false);
    }
}

void FileCheckList::OnFileSelected( wxCommandEvent& event )
{
    wxUnusedVar(event);
    ShowFilePath(event.GetSelection());
}

void FileCheckList::OnFileCheckChanged(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_files[event.GetInt()].second = m_fileCheckList->IsChecked(event.GetInt());
}

void FileCheckList::ShowFilePath(size_t n)
{
    wxFileName file = m_files[n].first;
    file.MakeRelativeTo(m_baseDir.GetFullPath());
    m_selectedFilePath->SetValue(file.GetFullPath());
}

void FileCheckList::SetCaption(const wxString &caption)
{
    m_caption->SetLabel(caption);
}

void FileCheckList::SetBaseDir(const wxFileName& dir)
{
    m_baseDir = dir;
    m_baseDir.MakeAbsolute();
}

void FileCheckList::SetFiles(const std::vector<std::pair<wxFileName, bool> > &files)
{
    m_files = files;
    m_fileCheckList->Clear();
    for (size_t i = 0; i < m_files.size(); i++) {
        m_fileCheckList->Append(m_files[i].first.GetFullName());
        m_fileCheckList->Check(i, m_files[i].second);
    }
    m_selectedFilePath->Clear();
    if (!m_files.empty()) {
        m_fileCheckList->Select(0);
        ShowFilePath(0);
    }
}

