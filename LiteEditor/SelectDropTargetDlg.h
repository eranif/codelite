#ifndef SELECTDROPTARGETDLG_H
#define SELECTDROPTARGETDLG_H
#include "wxcrafter.h"
#include <map>

class SelectDropTargetDlg : public SelectDropTargetBaseDlg
{
    std::map<wxString, wxWindow*> m_views;
    wxArrayString m_folders;
    wxWindow* m_selectedView;
    
public:
    SelectDropTargetDlg(wxWindow* parent, const wxArrayString& folders);
    virtual ~SelectDropTargetDlg();

protected:
    virtual void OnSelectionActivated(wxDataViewEvent& event);
    virtual void OnOK(wxCommandEvent& event);
    virtual void OnOKUI(wxUpdateUIEvent& event);
    void Initialize();
    void ActivateSelection();
};
#endif // SELECTDROPTARGETDLG_H
