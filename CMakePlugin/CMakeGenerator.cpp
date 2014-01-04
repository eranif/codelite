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

// wxWidgets
#include <wx/tokenzr.h>
#include <wx/ffile.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>

// Plugin
#include "CMakePlugin.h"

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
        int res = wxMessageBox(CMakePlugin::CMAKELISTS_FILE + " exists. Overwrite?\n" +
            "(" + filename.GetFullPath() + ")",
            wxMessageBoxCaptionStr, wxYES | wxNO | wxCENTER | wxICON_QUESTION);

        // Only for YES
        if (res != wxYES)
            return false;
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

void
CMakeGenerator::Generate(Workspace* workspace)
{
    // Get workspace directory.
    const wxFileName workspaceDir = workspace->GetWorkspaceFileName().
        GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME);

    // Output file name
    const wxFileName filename(workspaceDir.GetPath(), CMakePlugin::CMAKELISTS_FILE);

    if (!CheckExists(filename))
        return;

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
        content << "add_subdirectory(" << fullpath.GetPath() << ")\n";
    }

    // Write result
    WriteContent(filename, content);
}

/* ************************************************************************ */

void
CMakeGenerator::Generate(ProjectPtr project, BuildConfigPtr configuration,
    CompilerPtr compiler)
{
    wxASSERT(project);
    wxASSERT(configuration);

    // Get project directory
    const wxFileName projectDir = project->GetFileName().GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME);

    // Output file name
    const wxFileName filename(projectDir.GetPath(), CMakePlugin::CMAKELISTS_FILE);

    if (!CheckExists(filename))
        return;

    // File content
    wxString content;

    // TODO custom version
    content << "cmake_minimum_required(VERSION 2.6.2)\n\n";

    // Print project name
    content << "project(" << project->GetName() << ")\n\n";

    // Add include directories
    {
        wxString includes = configuration->GetIncludePath();

        // Get includes from project
        wxArrayString project_includes = project->GetIncludePaths();

        // Add projects includes
        includes << ";" << wxJoin(project_includes, ';');

        if (compiler) {
            // Append global include paths
            includes << ";" << compiler->GetGlobalIncludePath();
        }

        // Trim all whitespaces
        includes.Trim().Trim(false);

        // Ignore empty include paths
        if (!includes.IsEmpty()) {
            // Separators
            includes.Replace(";", "\n    ");
            // Replace Windows backslashes
            includes.Replace("\\", "/");

            content << "include_directories(\n    " << includes << "\n)\n\n";
        }
    }

    // Add preprocessor definitions
    {
        wxString defines = configuration->GetPreprocessor();

        defines.Trim().Trim(false);
        defines.Replace(";", "\n    -D");

        if (!defines.IsEmpty())
        {
            content << "add_definitions(\n    -D" << defines << "\n)\n\n";
        }
    }

    // Add compiler options
    {
        wxString buildOpts = configuration->GetCompileOptions();

        buildOpts.Trim().Trim(false);
        buildOpts.Replace(";", " ");

        if (!buildOpts.IsEmpty()) {
            content << "set(CMAKE_CXXFLAGS \"${CMAKE_CXXFLAGS} " << buildOpts << "\")\n\n";
        }
    }

    // Add linker options
    {
        wxString links = configuration->GetLinkOptions();

        links.Trim().Trim(false);
        links.Replace(";", " ");

        if (!links.IsEmpty()) {
            content << "# Linker options\n";
            content << "set(CMAKE_LDFLAGS \"${CMAKE_LDFLAGS} " << links << "\")\n\n";
        }
    }

    // Add libraries paths
    {
        wxString lib_paths = configuration->GetLibPath();
        wxString lib_switch = "-L";

        // Get switch from compiler
        if (compiler) {
            lib_switch = compiler->GetSwitch("LibraryPath");

            // Append global library paths
            lib_paths << ";" << compiler->GetGlobalLibPath();
        }

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

    // Write sources
    {
        content << "set(SRCS\n";

        // Get files in the project
        std::vector<wxFileName> files;
        project->GetFiles(files, true);

        for (size_t i = 0; i < files.size(); i++) {
            wxFileName src_filename = files.at(i);
            src_filename.MakeRelativeTo(project->GetFileName().GetPath());

            // Store file name into SRCS
            content << "    " << src_filename.GetFullPath(wxPATH_UNIX) << "\n";
        }

        content << ")\n\n";
    }

    // Get project type
    {
        ProjectSettingsPtr settings = project->GetSettings();

        wxString type = settings->GetProjectType(configuration->GetName());

        if (type == Project::EXECUTABLE) {
            content << "add_executable(" << project->GetName() << " ${SRCS})\n\n";
        } else if (type == Project::DYNAMIC_LIBRARY) {
            content << "add_library(" << project->GetName() << " SHARED ${SRCS})\n\n";
        } else {
            content << "add_library(" << project->GetName() << " ${SRCS})\n\n";
        }
    }

    // Add link libraries
    {
        wxString libs = configuration->GetLibraries();

        libs.Trim().Trim(false);

        if (!libs.IsEmpty()) {
            libs.Replace(";", "\n    ");
            content << "target_link_libraries(" <<
                project->GetName() << "\n    " <<
                libs << "\n)\n\n";
        }
    }

    // Write result
    WriteContent(filename, content);
}

/* ************************************************************************ */

