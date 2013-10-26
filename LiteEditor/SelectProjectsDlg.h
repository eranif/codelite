#ifndef SELECTPROJECTSDLG_H
#define SELECTPROJECTSDLG_H
#include "rename_symbool_dlg.h"

class SelectProjectsDlg : public SelectProjectsDlgBase
{
public:
    SelectProjectsDlg(wxWindow* parent);
    virtual ~SelectProjectsDlg();
    wxArrayString GetProjects() const;
    
protected:
    virtual void OnSelectAll(wxCommandEvent& event);
    virtual void OnUnSelectAll(wxCommandEvent& event);
};
#endif // SELECTPROJECTSDLG_H
