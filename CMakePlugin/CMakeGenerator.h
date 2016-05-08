//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : CMakeGenerator.h
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

#ifndef CMAKE_GENERATOR_H_
#define CMAKE_GENERATOR_H_

/* ************************************************************************ */
/* INCLUDES                                                                 */
/* ************************************************************************ */

// CodeLite
#include "workspace.h"
#include "project.h"
#include "build_config.h"
#include "compiler.h"

/* ************************************************************************ */
/* CLASSES                                                                  */
/* ************************************************************************ */

/**
 * @brief The CMakeLists.txt generator.
 */
class CMakeGenerator
{

    // Public Operations
public:
    /**
     * @brief Generate CMakeLists.txt for workspace.
     *
     * Creates a base CMakeLists.txt file that adds only projects
     * (like subdirectories) that have CMakeLists.txt. Others are
     * ignored.
     *
     * @param workspace Exported workspace.
     */
    static bool Generate(clCxxWorkspace* workspace);

    /**
     * @brief Generate CMakeLists.txt file for given project
     * This function always generte it for the build configuration that matches
     * to the workspace configuration. 
     * 
     * 
     * @param project       Project.
     * @param topProject    When set to true, generator will generate CMakeLists.txt file
     * for all dependencies as well
     */
    static bool Generate(ProjectPtr project, bool topProject, const wxString& configName = wxEmptyString);
    
    /**
     * @brief check if a we can generate a CMakeLists.txt for the workspace.
     * Return false if the CMakeLists.txt was hand edited by the user
     */
    static bool CanGenerate(clCxxWorkspace* workspace);
};

/* ************************************************************************ */

#endif // CMAKE_GENERATOR_H_
