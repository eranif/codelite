#ifndef PROJECTCOLOURSDLG_H
#define PROJECTCOLOURSDLG_H
#include "wxcrafter.h"
#include "tweaks_settings.h"

class ProjectColoursDlg : public ProjectColoursDlgBase
{
public:
    ProjectColoursDlg(wxWindow* parent, const wxString& project = "");
    virtual ~ProjectColoursDlg();
    ProjectTweaks GetSelection() const;
protected:
    virtual void OnOK(wxCommandEvent& event);
    virtual void OnOKUI(wxUpdateUIEvent& event);
    virtual void OnProjectChanged(wxCommandEvent& event);

};
#endif // PROJECTCOLOURSDLG_H
