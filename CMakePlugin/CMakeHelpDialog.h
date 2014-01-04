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

#ifndef CMAKE_HELP_DIALOG_H_
#define CMAKE_HELP_DIALOG_H_

/* ************************************************************************ */
/* INCLUDES                                                                 */
/* ************************************************************************ */

// UI
#include "CMakeHelpDialogBase.h"

/* ************************************************************************ */
/* FORWARD DECLARATIONS                                                     */
/* ************************************************************************ */

class CMake;

/* ************************************************************************ */
/* CLASSES                                                                  */
/* ************************************************************************ */

/**
 * @brief Help dialog.
 */
class CMakeHelpDialog : public CMakeHelpDialogBase
{

// Public Ctors & Dtors
public:


    /**
     * @brief Create a CMake settings dialog
     *
     * @param parent Pointer to parent window.
     * @param cmake  CMake pointer.
     */
    explicit CMakeHelpDialog(wxWindow* parent, CMake* cmake);


    /**
     * @brief Destructor
     */
    ~CMakeHelpDialog();


// Private Data Members
private:

    /// A pointer to cmake
    CMake* const m_cmake;

};

/* ************************************************************************ */

#endif // CMAKE_HELP_DIALOG_H_
