//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : CMakeProjectSettingsPanel.h
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

/* ************************************************************************ */
/*                                                                          */
/* CMakePlugin for Codelite                                                 */
/* Copyright (C) 2013 Jiří Fatka <ntsfka@gmail.com>                         */
/*                                                                          */
/* This program is free software: you can redistribute it and/or modify     */
/* it under the terms of the GNU General Public License as published by     */
/* the Free Software Foundation, either version 3 of the License, or        */
/* (at your option) any later version.                                      */
/*                                                                          */
/* This program is distributed in the hope that it will be useful,          */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of           */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             */
/* GNU General Public License for more details.                             */
/*                                                                          */
/* You should have received a copy of the GNU General Public License        */
/* along with this program. If not, see <http://www.gnu.org/licenses/>.     */
/*                                                                          */
/* ************************************************************************ */

#ifndef CMAKE_PROJECT_SETTINGS_PANEL_H_
#define CMAKE_PROJECT_SETTINGS_PANEL_H_

/* ************************************************************************ */
/* INCLUDES                                                                 */
/* ************************************************************************ */

// Codelite
#include "project.h"

// UI
#include "CMakePluginUi.h"

// CMakePlugin
#include "CMakeProjectSettings.h"

/* ************************************************************************ */
/* FORWARD DECLARATIONS                                                     */
/* ************************************************************************ */

class CMakePlugin;

/* ************************************************************************ */
/* CLASSES                                                                  */
/* ************************************************************************ */

/**
 * @brief Tab for project settings for CMake.
 */
class CMakeProjectSettingsPanel : public CMakeProjectSettingsPanelBase
{

// Public Ctors & Dtors
public:


    /**
     * @brief Create a CMake output tab.
     *
     * @param parent  Pointer to parent window.
     * @param project Project pointer.
     */
    explicit CMakeProjectSettingsPanel(wxWindow* parent, CMakePlugin* plugin);


// Public Accessors
public:


    /**
     * @brief Returns if custom CMakeLists.txt is enabled.
     *
     * @return
     */
    bool IsCMakeEnabled() const {
        return m_checkBoxEnable->IsChecked();
    }


    /**
     * @brief Returns name of parent project.
     *
     * @return Parent project name or empty string.
     */
    wxString GetParentProject() const {
        return m_choiceParent->GetStringSelection();
    }


    /**
     * @brief Returns source directory.
     *
     * @return
     */
    wxString GetSourceDirectory() const {
        return m_dirPickerSourceDir->GetPath();
    }


    /**
     * @brief Returns build directory.
     *
     * @return
     */
    wxString GetBuildDirectory() const {
        return m_dirPickerBuildDir->GetPath();
    }


    /**
     * @brief Returns generator name.
     *
     * @return
     */
    wxString GetGenerator() const {
        return m_choiceGenerator->GetStringSelection();
    }


    /**
     * @brief Returns build type.
     *
     * @return
     */
    wxString GetBuildType() const {
        return m_comboBoxBuildType->GetStringSelection();
    }


    /**
     * @brief Returns cmake arguments.
     *
     * @return
     */
    wxArrayString GetArguments() const {
        return wxSplit(m_textCtrlArguments->GetValue(), '\n');
    }


    /**
     * @brief Returns a pointer to project settings.
     *
     * @return
     */
    CMakeProjectSettings* GetSettings() const {
        return m_settings;
    }


// Public Mutators
public:


    /**
     * @brief Set if custom CMakeLists.txt is enabled.
     *
     * @param value
     */
    void SetCMakeEnabled(bool value) const {
        m_checkBoxEnable->SetValue(value);
    }


    /**
     * @brief Set name of the parent project.
     *
     * @param project
     */
    void SetParentProject(const wxString& project) {
        m_choiceParent->SetStringSelection(project);
    }


    /**
     * @brief Change source directory.
     *
     * @param dir Directory.
     */
    void SetSourceDirectory(const wxString& dir) {
        m_dirPickerSourceDir->SetPath(dir);
    }


    /**
     * @brief Change build directory.
     *
     * @param dir Directory.
     */
    void SetBuildDirectory(const wxString& dir) {
        m_dirPickerBuildDir->SetPath(dir);
    }


    /**
     * @brief Changes CMake generator.
     *
     * @param generator
     */
    void SetGenerator(const wxString& generator) {
        m_choiceGenerator->SetStringSelection(generator);
    }


    /**
     * @brief Changes CMake build type.
     *
     * @param buildType
     */
    void SetBuildType(const wxString& buildType) {
        m_comboBoxBuildType->SetStringSelection(buildType);
    }


    /**
     * @brief Set cmake arguments.
     *
     * @param arguments
     */
    void SetArguments(const wxArrayString& arguments) {
        m_textCtrlArguments->SetValue(wxJoin(arguments, '\n'));
    }


    /**
     * @brief Set project setting pointer.
     *
     * @param settings
     * @param project
     * @param config
     */
    void SetSettings(CMakeProjectSettings* settings, const wxString& project,
                     const wxString& config);


// Public Events
public:


    /**
     * @brief On enable or disable.
     *
     * @param event
     */
    void OnCheck(wxUpdateUIEvent& event) {
        event.Enable(m_checkBoxEnable->IsChecked());
    }


    /**
     * @brief On enable or disable.
     *
     * @param event
     */
    void OnCheck2(wxUpdateUIEvent& event) {
        event.Enable(m_checkBoxEnable->IsChecked() &&
                     GetParentProject().empty());
    }


// Public Operations
public:


    /**
     * @brief Loads from settings pointer to GUI widgets.
     */
    void LoadSettings();


    /**
     * @brief Stores settings from GUI widgets into settings pointer.
     */
    void StoreSettings();


    /**
     * @brief Clear settings from GUI widgets into settings pointer.
     */
    void ClearSettings();


// Private Data Members
private:


    /// A pointer to plugin.
    CMakePlugin* const m_plugin;

    /// Pointer to settings.
    CMakeProjectSettings* m_settings;

};

/* ************************************************************************ */

#endif // CMAKE_PROJECT_SETTINGS_PANEL_H_
