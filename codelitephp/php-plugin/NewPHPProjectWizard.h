#ifndef NEWPHPPROJECTWIZARD_H
#define NEWPHPPROJECTWIZARD_H
#include "php_ui.h"
#include "php_project.h"
#include "php_project_settings_data.h"

class NewPHPProjectWizard : public NewPHPProjectWizardBase
{
    bool m_nameModified;

public:
    NewPHPProjectWizard(wxWindow* parent);
    virtual ~NewPHPProjectWizard();

    PHPProject::CreateData GetCreateData();

    int GetProjectType() const
    {
        if(m_choiceProjectType->GetStringSelection() == "Run project as command line") {
            return PHPProjectSettingsData::kRunAsCLI;
        } else {
            return PHPProjectSettingsData::kRunAsWebsite;
        }
    }

protected:
    virtual void OnBrowseForCCFolder(wxCommandEvent& event);
    virtual void OnCheckSeparateFolder(wxCommandEvent& event);
    virtual void OnDirSelected(wxFileDirPickerEvent& event);
    virtual void OnNameUpdated(wxCommandEvent& event);
    virtual void OnPageChanging(wxWizardEvent& event);
    virtual void OnFinish(wxWizardEvent& event);

protected:
    void DoUpdateProjectFolder();
};
#endif // NEWPHPPROJECTWIZARD_H
