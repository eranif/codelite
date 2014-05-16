#ifndef SFTPMANAGEBOOKMARKDLG_H
#define SFTPMANAGEBOOKMARKDLG_H
#include "UI.h"

class SFTPManageBookmarkDlg : public SFTPManageBookmarkDlgBase
{
public:
    SFTPManageBookmarkDlg(wxWindow* parent, const wxArrayString& bookmarks);
    virtual ~SFTPManageBookmarkDlg();
    wxArrayString GetBookmarks() const;
protected:
    virtual void OnDelete(wxCommandEvent& event);
    virtual void OnDeleteUI(wxUpdateUIEvent& event);
};
#endif // SFTPMANAGEBOOKMARKDLG_H
