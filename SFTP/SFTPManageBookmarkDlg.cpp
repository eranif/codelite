#include "SFTPManageBookmarkDlg.h"
#include "windowattrmanager.h"

SFTPManageBookmarkDlg::SFTPManageBookmarkDlg(wxWindow* parent, const wxArrayString& bookmarks)
    : SFTPManageBookmarkDlgBase(parent)
{
    m_listBoxBookmarks->Append( bookmarks );
    WindowAttrManager::Load(this, "SFTPManageBookmarkDlg");
}

SFTPManageBookmarkDlg::~SFTPManageBookmarkDlg()
{
    WindowAttrManager::Save(this, "SFTPManageBookmarkDlg");
}

void SFTPManageBookmarkDlg::OnDelete(wxCommandEvent& event)
{
    int sel = m_listBoxBookmarks->GetSelection();
    if ( sel == wxNOT_FOUND )
        return;
    
    m_listBoxBookmarks->Delete( sel );
}

void SFTPManageBookmarkDlg::OnDeleteUI(wxUpdateUIEvent& event)
{
    event.Enable( m_listBoxBookmarks->GetSelection() != wxNOT_FOUND );
}

wxArrayString SFTPManageBookmarkDlg::GetBookmarks() const
{
    wxArrayString bookmarks;
    for(unsigned int i=0; i<m_listBoxBookmarks->GetCount(); ++i) {
        bookmarks.Add( m_listBoxBookmarks->GetString( i )) ;
    }
    return bookmarks;
}
