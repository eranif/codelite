//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : configuration_manager_dlg.h
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
#ifndef __configuration_manager_dlg__
#define __configuration_manager_dlg__

#include "ConfigManagerBaseDlg.h"
#include "smart_ptr.h"
#include <map>
#include "project_settings.h"
#include "configuration_mapping.h"

class ConfigEntry
{
public:
    ProjectSettingsPtr projectSettings;
    wxString project;
    wxPGProperty* choiceControl;
};

/** Implementing ConfigManagerBaseDlg */
class ConfigurationManagerDlg : public ConfigManagerBaseDlg
{
    bool m_dirty;
    wxString m_currentWorkspaceConfiguration;
    std::map<wxString, ConfigEntry> m_projectPropertiesMap;

protected:
    virtual void OnValueChanged(wxPropertyGridEvent& event);
    void InitDialog();
    ConfigEntry DoCreateChoicesForProject(
        const wxString& projectName, const wxString& selectedConf, wxPGProperty* prop = NULL);
    //----------------------------------
    // Events
    //----------------------------------
    void OnButtonNew(wxCommandEvent& event);
    void OnButtonOK(wxCommandEvent& event);
    void OnWorkspaceConfigSelected(wxCommandEvent& event);
    void OnButtonApply(wxCommandEvent& event);
    void OnButtonApplyUI(wxUpdateUIEvent& event);
    void LoadWorkspaceConfiguration(const wxString& confName);
    void LoadProjectConfiguration(const wxString& projectName);

    void PopulateConfigurations();
    void AddEntry(const wxString& projectName, const wxString& selectedConf);
    WorkspaceConfiguration::ConfigMappingList GetCurrentSettings();
    void SaveCurrentSettings();

public:
    /** Constructor */
    ConfigurationManagerDlg(wxWindow* parent);
    virtual ~ConfigurationManagerDlg();
};

#endif // __configuration_manager_dlg__
