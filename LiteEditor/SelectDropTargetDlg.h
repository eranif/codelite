#ifndef SELECTDROPTARGETDLG_H
#define SELECTDROPTARGETDLG_H
#include "wxcrafter.h"
#include <map>

class SelectDropTargetDlg : public SelectDropTargetBaseDlg
{
    std::map<wxString, wxWindow*> m_views;
    wxArrayString m_folders;

public:
    SelectDropTargetDlg(wxWindow* parent, const wxArrayString& folders);
    virtual ~SelectDropTargetDlg();

protected:
    void Initialize();
    virtual void OnButtonSelected(wxCommandEvent& event);
};
#endif // SELECTDROPTARGETDLG_H
