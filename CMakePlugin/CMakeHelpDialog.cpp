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

/* ************************************************************************ */
/* INCLUDES                                                                 */
/* ************************************************************************ */

// Declaration
#include "CMakeHelpDialog.h"

// Codelite
#include "windowattrmanager.h"

// CMakePlugin
#include "CMake.h"
#include "CMakeHelpPanel.h"

/* ************************************************************************ */
/* CLASSES                                                                  */
/* ************************************************************************ */

CMakeHelpDialog::CMakeHelpDialog(wxWindow* parent, CMake* cmake)
    : CMakeHelpDialogBase(parent)
    , m_cmake(cmake)
{
    wxASSERT(cmake);

    // Set CMake version
    m_staticTextVersionValue->SetLabel(cmake->GetVersion());

    // Set data
    m_panelModules->SetData(&cmake->GetModules());
    m_panelCommands->SetData(&cmake->GetCommands());
    m_panelProperties->SetData(&cmake->GetProperties());
    m_panelVariables->SetData(&cmake->GetVariables());

    // Load window layout
    WindowAttrManager::Load(this, "CMakeHelpDialog", NULL);
}

/* ************************************************************************ */

CMakeHelpDialog::~CMakeHelpDialog()
{
    // Save window layout
    WindowAttrManager::Save(this, "CMakeHelpDialog", NULL);
}

/* ************************************************************************ */
