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
    static void Generate(Workspace* workspace);


    /**
     * @brief Generate CMakeLists.txt file for given project and
     * configuration.
     *
     * @param project       Project.
     * @param configuration Build configuration.
     * @param compiler      Optional compiler.
     */
    static void Generate(ProjectPtr project,
                         BuildConfigPtr configuration,
                         CompilerPtr compiler = NULL);

};

/* ************************************************************************ */

#endif // CMAKE_GENERATOR_H_
