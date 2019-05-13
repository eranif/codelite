#ifndef EOSPROJECTWIZARD_H
#define EOSPROJECTWIZARD_H
#include "eosUI.h"
#include "EOSProjectData.h"

class EOSProjectWizard : public EOSProjectWizardBase
{
public:
    EOSProjectWizard(wxWindow* parent);
    virtual ~EOSProjectWizard();

    wxString GetName() const;
    wxString GetPath() const;
    wxString GetProjectFile() const;
    static wxString GetToolchainPath();
    EOSProjectData GetData() const;

protected:
    virtual void OnCreateInSeparateDir(wxCommandEvent& event);
    virtual void OnNameUpdated(wxCommandEvent& event);
    virtual void OnPathChanged(wxFileDirPickerEvent& event);
    virtual void OnOKUI(wxUpdateUIEvent& event);
};
#endif // EOSPROJECTWIZARD_H
