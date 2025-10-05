//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : NewPHPProjectWizard.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifndef NEWPHPPROJECTWIZARD_H
#define NEWPHPPROJECTWIZARD_H
#include "php_ui.h"
#include "php_project.h"
#include "php_project_settings_data.h"

class NewPHPProjectWizard : public NewPHPProjectWizardBase
{
    bool m_nameModified;

public:
    NewPHPProjectWizard(wxWindow* parent, bool createProjectFromWorkspaceFolder = false);
    virtual ~NewPHPProjectWizard() = default;

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
