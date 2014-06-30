//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : CMakeSettingsDialog.h
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

#ifndef CMAKE_SETTINGS_DIALOG_H_
#define CMAKE_SETTINGS_DIALOG_H_

/* ************************************************************************ */
/* INCLUDES                                                                 */
/* ************************************************************************ */

// UI
#include "CMakePluginUi.h"

/* ************************************************************************ */
/* FORWARD DECLARATIONS                                                     */
/* ************************************************************************ */

class CMakePlugin;

/* ************************************************************************ */
/* CLASSES                                                                  */
/* ************************************************************************ */

/**
 * @brief Dialog for global CMake settings.
 */
class CMakeSettingsDialog : public CMakeSettingsDialogBase
{

// Public Ctors & Dtors
public:


    /**
     * @brief Create a CMake settings dialog
     *
     * @param parent Pointer to parent window.
     * @param plugin CMakePlugin pointer.
     */
    explicit CMakeSettingsDialog(wxWindow* parent, CMakePlugin* plugin);


    /**
     * @brief Destructor.
     */
    virtual ~CMakeSettingsDialog();


// Public Accessors
public:


    /**
     * @brief Returns path to cmake program.
     *
     * @return
     */
    wxString GetCMakePath() const {
        return m_filePickerProgram->GetPath();
    }


    /**
     * @brief Returns selected default generator.
     *
     * @return
     */
    wxString GetDefaultGenerator() const {
        return m_choiceDefaultGenerator->GetStringSelection();
    }


// Public Mutators
public:


    /**
     * @brief Set path to CMake progam.
     *
     * @param path
     */
    void SetCMakePath(const wxString& path) {
        m_filePickerProgram->SetPath(path);
    }


    /**
     * @brief Change default generator.
     *
     * @param generator New default generator.
     */
    void SetDefaultGenerator(const wxString& generator) {
        m_choiceDefaultGenerator->SetStringSelection(generator);
    }


// Private Data Members
private:

    /// Pointer to CMakePlugin.
    CMakePlugin* const m_plugin;

};

/* ************************************************************************ */

#endif // CMAKE_SETTINGS_DIALOG_H_
