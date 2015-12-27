//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : CMakeGenerator.cpp
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

/* ************************************************************************ */
/* INCLUDES                                                                 */
/* ************************************************************************ */

// Declarations
#include "CMakeGenerator.h"
#include "fileextmanager.h"

// wxWidgets
#include <wx/tokenzr.h>
#include <wx/ffile.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>

// Plugin
#include "CMakePlugin.h"
#include "globals.h"

/* ************************************************************************ */
/* FUNCTIONS                                                                */
/* ************************************************************************ */

/**
 * @brief Check if filename exists and asks if user want it overwrite if
 * exists.
 *
 * @param filename Tested filename.
 *
 * @return If file can be written.
 */
static bool CheckExists(const wxFileName& filename)
{
    // Output file exists, overwrite?
    if (filename.Exists()) {
        wxString msg;
        msg << CMakePlugin::CMAKELISTS_FILE 
            << " exists. Overwrite?\n" 
            << "(" 
            << filename.GetFullPath() 
            << ")";
        wxStandardID answer = ::PromptForYesNoDialogWithCheckbox(   msg, 
                                                                    "CMakePluginOverwriteDlg", 
                                                                    _("Overwrite"), 
                                                                    _("Don't Overwrite"),
                                                                    _("Remember my answer and don't annoy me again"),
                                                                    wxYES_NO | wxCANCEL| wxCENTER | wxICON_QUESTION | wxYES_DEFAULT);
        return (answer == wxID_YES);
    }
    
    // File doesn't exists, write it
    return true;
}

/* ************************************************************************ */

/**
 * @brief Writes content into file.
 *
 * @param filename Output file name.
 * @param content  Written text.
 */
static void WriteContent(const wxFileName& filename, const wxString& content)
{
    // Write result CMakeLists.txt
    wxFFile output(filename.GetFullPath(), "w+b");

    if (output.IsOpened()) {
        output.Write(content);
        output.Close();
    }
}

/* ************************************************************************ */
/* CLASSES                                                                  */
/* ************************************************************************ */

bool
CMakeGenerator::Generate(clCxxWorkspace* workspace)
{
    if ( !workspace )
        return false;
    
    // Export all projects
    wxArrayString projectsArr;
    workspace->GetProjectList( projectsArr );
    
    for(size_t i=0; i<projectsArr.GetCount(); ++i) {
        ProjectPtr pProj = workspace->GetProject(projectsArr.Item(i));
        Generate( pProj, false );
    }
    
    // ====----------------------------
    // Now generate the workspace
    // ====----------------------------
    // Get workspace directory.
    const wxFileName workspaceDir = workspace->GetWorkspaceFileName().
        GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME);

    // Output file name
    const wxFileName filename(workspaceDir.GetPath(), CMakePlugin::CMAKELISTS_FILE);

    if (!CheckExists(filename))
        return false;

    // File content
    wxString content;

    // Print project name
    content << "# Workspace name\n";
    content << "project(" << workspace->GetName() << ")\n\n";

    // Environment variables
    {
        wxString variables = workspace->GetEnvironmentVariabels(); // Nice typo
        variables.Trim().Trim(false);

        if (!variables.IsEmpty()) {
            // Split into a list of pairs
            const wxArrayString list = wxStringTokenize(variables, "\n;");

            for (wxArrayString::const_iterator it = list.begin(),
                ite = list.end(); it != ite; ++it) {
                // Split into name, value pair
                const wxArrayString pair = wxSplit(*it, '=');

                const wxString& name = pair[0];
                const wxString value = (pair.GetCount() >= 2) ? pair[1] : "";

                // Set environment variable
                content << "set(" << name << " \"" << value << "\")\n";
            }

            content << "\n";
        }
    }

    content << "# Projects\n";

    // Get full paths to all projects
    const wxArrayString projects = workspace->GetAllProjectPaths();

    // Foreach projects path
    for (wxArrayString::const_iterator it = projects.begin(),
        ite = projects.end(); it != ite; ++it) {
        wxFileName fullpath = *it;
        fullpath.MakeRelativeTo(workspaceDir.GetPath());

        // Create path to CMakeLists.txt in the project
        const wxFileName cmakelist(fullpath.GetPath(), CMakePlugin::CMAKELISTS_FILE);

        // Skip directories without CMakeLists.txt
        if (!cmakelist.Exists())
            continue;

        // Write
        wxString projectDir = fullpath.GetPath();
        if ( projectDir.IsEmpty() ) projectDir = ".";
        content << "add_subdirectory(" << projectDir << ")\n";
    }

    // Write result
    WriteContent(filename, content);
    return true;
}

/* ************************************************************************ */

bool
CMakeGenerator::Generate(ProjectPtr project, bool topProject)
{
    wxASSERT(project);
    // Get project directory
    wxString projectDir = project->GetFileName().GetPath();
    
    // Get the project build configuration and compiler
    BuildConfigPtr buildConf = project->GetBuildConfiguration();
    
    // Sanity
    if ( !buildConf || buildConf->IsCustomBuild() ) {
        // no build config or custom build?
        return false;
    }
    
    CompilerPtr compiler = buildConf->GetCompiler();
    if ( !compiler ) {
        return false;
    }
    
    wxFileName projectPath = project->GetFileName();
    wxArrayString depsPaths;
    wxArrayString depsProjectNames;
    if ( topProject ) {
        wxArrayString depsProjects = project->GetDependencies( buildConf->GetName() );
        for(size_t i=0; i<depsProjects.GetCount(); ++i) {
            ProjectPtr pProj = clCxxWorkspaceST::Get()->GetProject( depsProjects.Item(i) );
            if ( pProj ) {
                if ( Generate( pProj, false ) ) {
                    wxString depProjFilePath = pProj->GetFileName().GetFullPath();
                    wxFileName fnDepProj( depProjFilePath );
                    fnDepProj.MakeRelativeTo( projectPath.GetPath() );
                    wxString relPath = fnDepProj.GetPath(false, wxPATH_UNIX);
                    
                    // Keep the project path relative to current project
                    depsPaths.Add( relPath );
                    
                    // Keep the project name
                    depsProjectNames.Add( pProj->GetName() );
                }
            }
        }
    }
    
    // Output file name
    wxFileName filename(projectDir, CMakePlugin::CMAKELISTS_FILE);

    if (!CheckExists(filename))
        return false;
    
    // File content
    wxString content;

    content << "cmake_minimum_required(VERSION 2.8.11)\n\n";

    // Print project name
    content << "project(" << project->GetName() << ")\n\n";
    
    // Add the dependencies first
    if ( !depsPaths.IsEmpty() ) {
        content << "# Add the build order directories\n";
    }
    
    for(size_t i=0; i<depsPaths.GetCount(); ++i) {
        content << "add_subdirectory( " << depsPaths.Item(i) << " )\n";
    }
    content << "\n\n";
    
    // Add include directories
    {
        // Get the incldue paths for the project. This also includes any 
        // includes from $(shell ...) commands
        wxArrayString includes = project->GetIncludePaths(true);
        content << "include_directories(\n";
        for(size_t i=0; i<includes.GetCount(); ++i) {
            
            wxString &includePath = includes.Item(i);
            includePath.Replace("\\", "/");
            includePath.Trim(false).Trim();
            if ( includePath.IsEmpty() ) {
                continue;
            }
            content << "    " << includes.Item(i) << "\n";
        }
        content << "\n)\n\n";
    }

    // Add preprocessor definitions
    {
        wxArrayString defines = project->GetPreProcessors(false);
        if ( !defines.IsEmpty() ) {
            content << "add_definitions(\n";
            for(size_t i=0; i<defines.GetCount(); ++i) {
                wxString &pp = defines.Item(i);
                pp.Trim().Trim(false);
                if ( pp.IsEmpty() ) {
                    continue;
                }
                content << "    -D" << pp << "\n";
            }
            content << ")\n\n";
        }
    }

    // Add linker options
    {
        wxString links = buildConf->GetLinkOptions();

        links.Trim().Trim(false);
        links.Replace(";", " ");

        if (!links.IsEmpty()) {
            content << "# Linker options\n";
            content << "set(CMAKE_LDFLAGS \"${CMAKE_LDFLAGS} " << links << "\")\n\n";
        }
    }

    // Add libraries paths
    {
        wxString lib_paths = buildConf->GetLibPath();
        wxString lib_switch = "-L";

        // Get switch from compiler
        if (compiler) {
            lib_switch = compiler->GetSwitch("LibraryPath");

            // Append global library paths
            lib_paths << ";" << compiler->GetGlobalLibPath();
        }

		// to replace all instances of backslashes with forward slashes issue #1073
		lib_paths.Replace("\\", "/");
		lib_paths.Trim(false).Trim();
        // Get list of library paths
        wxArrayString lib_paths_list = wxStringTokenize(lib_paths, ";", wxTOKEN_STRTOK);

        // Clear string
        lib_paths.Clear();

        // Append modified values
        for (size_t i = 0; i < lib_paths_list.GetCount(); ++i) {
            lib_paths << lib_switch << "\\\"" << lib_paths_list.Item(i) << "\\\" ";
        }

        content << "# Library path\n";
        content << "set(CMAKE_LDFLAGS \"${CMAKE_LDFLAGS} " << lib_paths << "\")\n\n";
    }

    wxArrayString cppSources, cSources;
    // Write sources 
    // Get files in the project
    std::vector<wxFileName> files;
    project->GetFiles(files);

    {
        
        for (size_t i = 0; i < files.size(); ++i) {
            
            wxString sourceFile = files.at(i).GetFullPath();
            
            // CMake does not handle backslash properly
            sourceFile.Replace("\\", "/"); 
            if ( FileExtManager::GetType(sourceFile) == FileExtManager::TypeSourceCpp) {
                cppSources.Add( sourceFile );
            } else if ( FileExtManager::GetType(sourceFile) == FileExtManager::TypeSourceC ) {
                cSources.Add( sourceFile );
            }
            
        }
        
        if ( !cSources.IsEmpty() ) {
            content << "# Define the C sources\n";
            content << "set ( C_SRCS\n";
            for(size_t i=0; i<cSources.GetCount(); ++i) {
                content << "    " << cSources.Item(i) << "\n";
            }
            content << ")\n\n";
        }
        
        if ( !cppSources.IsEmpty() ) {
            content << "# Define the CXX sources\n";
            content << "set ( CXX_SRCS\n";
            for(size_t i=0; i<cppSources.GetCount(); ++i) {
                content << "    " << cppSources.Item(i) << "\n";
            }
            content << ")\n\n";
        }
    }
    
    // Add CXX compiler options
    {
        wxArrayString buildOptsArr = project->GetCXXCompilerOptions();
        if ( !buildOptsArr.IsEmpty() && !cppSources.IsEmpty() ) {
            content << "set_source_files_properties(\n    ${CXX_SRCS} PROPERTIES COMPILE_FLAGS \n    \"";
            for(size_t i=0; i<buildOptsArr.GetCount(); ++i) {
                content << " " << buildOptsArr.Item(i) ;
            }
            content << "\")\n\n";
        }
    }
    
    // Add C compiler options
    {
        wxArrayString buildOptsArr = project->GetCCompilerOptions();
        if ( !buildOptsArr.IsEmpty() && !cSources.IsEmpty() ) {
            content << "set_source_files_properties(\n    ${C_SRCS} PROPERTIES COMPILE_FLAGS \n    \"";
            for(size_t i=0; i<buildOptsArr.GetCount(); ++i) {
                content << " " << buildOptsArr.Item(i) ;
            }
            content << "\")\n\n";
        }
    }

    // Get project type
    {
        wxString type = buildConf->GetProjectType();

        if (type == Project::EXECUTABLE) {
            content << "add_executable(" << project->GetName() << " ${CXX_SRCS} ${C_SRCS})\n\n";
            
        } else if (type == Project::DYNAMIC_LIBRARY) {
            content << "add_library(" << project->GetName() << " SHARED ${CXX_SRCS} ${C_SRCS})\n\n";
            
        } else {
            content << "add_library(" << project->GetName() << " ${CXX_SRCS} ${C_SRCS})\n\n";
        }
    }
    
    // Add link libraries
    {
        wxString libs = buildConf->GetLibraries();

        libs.Trim().Trim(false);

        if (!libs.IsEmpty()) {
            libs.Replace(";", "\n    ");
            content << "target_link_libraries(" <<
                project->GetName() << "\n    " <<
                libs << "\n)\n\n";
        }
    }
    
    // And setup the deps
    // Set up a dependecy between the this project and the subproject
    if ( !depsProjectNames.IsEmpty() ) {
        content << "# Setup dependencies\n";
    }
    for(size_t i=0; i<depsProjectNames.GetCount(); ++i) {
        content << "add_dependencies( " << project->GetName() << " " << depsProjectNames.Item(i) << " )\n";
    }
    
    // Write result
    WriteContent(filename, content);
    return true;
}

/* ************************************************************************ */

