#ifndef PHPPROJECTSETUPDLG_H
#define PHPPROJECTSETUPDLG_H

#include "php_ui.h"
#include "php_project_settings_data.h"

class PHPProjectSetupDlg : public PHPProjectSetupDlgBase
{
public:
    PHPProjectSetupDlg(wxWindow* parent);
    virtual ~PHPProjectSetupDlg();
    
    bool IsImportFiles() const {
        return m_checkBoxImportFiles->IsChecked();
    }
    
    wxString GetPhpExecutable() const {
        return m_filePickerPhpExe->GetPath();
    }
    
    int GetProjectType() const {
        if(m_choiceProjectType->GetStringSelection() == "Run project as command line") {
            return PHPProjectSettingsData::kRunAsCLI;
        } else {
            return PHPProjectSettingsData::kRunAsWebsite;
        }
    }
};
#endif // PHPPROJECTSETUPDLG_H
