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

#include "CMakeBuilder.h"
#include "fileextmanager.h"
#include "fileutils.h"

#include <algorithm>

// wxWidgets
#include <wx/ffile.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <wx/tokenzr.h>

// Plugin
#include "CMakePlugin.h"
#include "globals.h"

#define CODELITE_CMAKE_PREFIX "# -*- CMakeLists.txt generated by CodeLite IDE. Do not edit by hand -*-"
#define CMAKELISTS_USER_CODE_01_PREFIX "#{{{{ User Code 01"
#define CMAKELISTS_USER_CODE_02_PREFIX "#{{{{ User Code 02"
#define CMAKELISTS_USER_CODE_1_PREFIX "#{{{{ User Code 1"
#define CMAKELISTS_USER_CODE_2_PREFIX "#{{{{ User Code 2"
#define CMAKELISTS_USER_CODE_3_PREFIX "#{{{{ User Code 3"
#define CMAKELISTS_USER_CODE_END "#}}}}"

namespace
{
wxArrayString wxArrayUniqueMerge(const wxArrayString& arr1, const wxArrayString& arr2)
{
    wxArrayString outArr;
    for(size_t i = 0; i < arr1.size(); ++i) {
        if(outArr.Index(arr1.Item(i)) == wxNOT_FOUND) {
            outArr.Add(arr1.Item(i));
        }
    }

    for(size_t i = 0; i < arr2.size(); ++i) {
        if(outArr.Index(arr2.Item(i)) == wxNOT_FOUND) {
            outArr.Add(arr2.Item(i));
        }
    }
    return outArr;
}
} // namespace

CMakeGenerator::CMakeGenerator()
    : m_counter(0)
{
}

CMakeGenerator::~CMakeGenerator() {}

bool CMakeGenerator::CheckExists(const wxFileName& filename)
{
    // Output file exists, overwrite?
    if(filename.Exists()) {
        // Check to see if this file was generated by CodeLite
        wxString content;
        FileUtils::ReadFileContent(filename, content);
        if(!content.StartsWith(CODELITE_CMAKE_PREFIX)) {
            wxString msg;
            msg << _("A custom ") << CMakePlugin::CMAKELISTS_FILE << _(" exists.\nWould you like to overwrite it?\n")
                << "( " << filename.GetFullPath() << " )";
            wxStandardID answer =
                ::PromptForYesNoDialogWithCheckbox(msg, "CMakePluginOverwriteDlg", _("Overwrite"), _("Don't Overwrite"),
                                                   _("Remember my answer and don't annoy me again"),
                                                   wxYES_NO | wxCANCEL | wxCENTER | wxICON_QUESTION | wxYES_DEFAULT);
            return (answer == wxID_YES);
        } else {
            // A CodeLite generated CMakeLists.txt file
            // Read any user custom code before we overwrite the content
            ReadUserCode(content);
            return true;
        }
    }

    // File doesn't exists, write it
    return true;
}

/**
 * @brief The Generator main entry point
 */
bool CMakeGenerator::Generate(ProjectPtr p)
{
    m_counter = 0;
    CHECK_COND_RET_FALSE(p);

    BuildConfigPtr buildConf = p->GetBuildConfiguration();
    CHECK_COND_RET_FALSE(buildConf);

    clCxxWorkspace* workspace = clCxxWorkspaceST::Get();

    // Generate CMakeLists.txt file for each dependency project
    wxArrayString projectsArr = p->GetDependencies(buildConf->GetName());
    for(size_t i = 0; i < projectsArr.GetCount(); ++i) {
        ProjectPtr pProj = workspace->GetProject(projectsArr.Item(i));
        CMakeGenerator generator;
        wxString cmakeFileContent = generator.GenerateProject(pProj, false);
        if(!cmakeFileContent.IsEmpty()) {
            // Write the content to the file system
            wxFileName cmakelists(pProj->GetFileName());
            cmakelists.SetFullName(CMakePlugin::CMAKELISTS_FILE);
            FileUtils::WriteFileContent(cmakelists, cmakeFileContent);
        }
    }

    // Generate the top level CMakeLists.txt
    wxString topProjectContent = GenerateProject(p, true);

    // ====--------------------------------
    // Now generate the top level project
    // ====--------------------------------

    // Output file name
    const wxFileName filename(p->GetFileName().GetPath(), CMakePlugin::CMAKELISTS_FILE);

    // Check for file existance and read any user code from the current CMakeLists.txt file
    if(!CheckExists(filename))
        return false;

    // File content
    wxString mainProjectContent;

    // Print project name
    mainProjectContent << CODELITE_CMAKE_PREFIX << "\n\n";
    mainProjectContent << "cmake_minimum_required(VERSION 3.5)\n";
    AddUserCodeSection(mainProjectContent, CMAKELISTS_USER_CODE_01_PREFIX, m_userBlock01);
    mainProjectContent << "enable_language(CXX C ASM)\n";
    mainProjectContent << "# Project name\n";
    mainProjectContent << "project(" << p->GetName() << ")\n\n";
    AddUserCodeSection(mainProjectContent, CMAKELISTS_USER_CODE_02_PREFIX, m_userBlock02);

    mainProjectContent << "# This setting is useful for providing JSON file used by CodeLite for code completion\n";
    mainProjectContent << "set(CMAKE_EXPORT_COMPILE_COMMANDS 1)\n\n";

    // Export common variables into CMake variables
    {
        mainProjectContent << "set(CONFIGURATION_NAME \"" << workspace->GetBuildMatrix()->GetSelectedConfigurationName()
                           << "\")\n";
        mainProjectContent << "\n";
    }

    wxFileName fn = workspace->GetFileName();
    fn.MakeRelativeTo(p->GetFileName().GetPath());

    wxString workspacePath = fn.GetPath(false, wxPATH_UNIX);
    if(workspacePath.IsEmpty()) {
        workspacePath = ".";
    }
    mainProjectContent << "set(CL_WORKSPACE_DIRECTORY " << workspacePath << ")\n";

    // Set the output directory. We want a common location for all projects
    // so we set it under cmake-build-Debug/bin
    wxString outputDir;
    outputDir << "${CMAKE_CURRENT_LIST_DIR}/${CL_WORKSPACE_DIRECTORY}/"
              << "cmake-build-${CONFIGURATION_NAME}/output";

    mainProjectContent << "# Set default locations\n";
    mainProjectContent << "set(CL_OUTPUT_DIRECTORY " << outputDir << ")\n";
    mainProjectContent << "set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CL_OUTPUT_DIRECTORY})\n";
    mainProjectContent << "set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CL_OUTPUT_DIRECTORY})\n";
    mainProjectContent << "set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CL_OUTPUT_DIRECTORY})\n";
    mainProjectContent << "\n";

    // Environment variables
    {
        wxString variables = workspace->GetEnvironmentVariables();
        variables.Trim().Trim(false);

        if(!variables.IsEmpty()) {
            // Split into a list of pairs
            for (const auto& line : wxStringTokenize(variables, "\n;")) {
                // Split into name, value pair
                const wxArrayString pair = wxSplit(line, '=');
                if(pair.IsEmpty()) {
                    continue;
                }
                const wxString& name = pair[0];
                const wxString value = (pair.GetCount() >= 2) ? pair[1] : "";

                // Set environment variable
                mainProjectContent << "set(" << name << " \"" << value << "\")\n";
            }

            mainProjectContent << "\n";
        }
    }

    mainProjectContent << "# Projects\n";

    // the workspace and the active project "sits" on the same folder
    // merge the two cmakelists.txt files
    mainProjectContent << "\n\n";
    mainProjectContent << "# Top project\n";
    mainProjectContent << topProjectContent;

    // Write the workspace
    wxFileName fnMainCMakeLists(p->GetFileName());
    fnMainCMakeLists.SetFullName(CMakePlugin::CMAKELISTS_FILE);
    FileUtils::WriteFileContent(fnMainCMakeLists, mainProjectContent);
    return true;
}

/* ************************************************************************ */

wxString CMakeGenerator::GenerateProject(ProjectPtr project, bool topProject, const wxString& configName)
{
    wxASSERT(project);
    wxString projectPathVariable;      // PROJECT_<name>_PATH
    wxString projectPathVariableValue; // ${PROJECT_<name>_PATH}
    wxString workspacePath;

    projectPathVariable << "PROJECT_" << project->GetName() << "_PATH";
    projectPathVariableValue << "${PROJECT_" << project->GetName() << "_PATH}";

    wxFileName fnWorkspace = clCxxWorkspaceST::Get()->GetFileName();
    fnWorkspace.MakeRelativeTo(project->GetFileName().GetPath());
    workspacePath << "${CMAKE_CURRENT_LIST_DIR}/" << fnWorkspace.GetPath(wxPATH_NO_SEPARATOR, wxPATH_UNIX);

    // Sources are kept relative in the project
    // we need to fix this
    const Project::FilesMap_t& allFiles = project->GetFiles();

    // Get the project build configuration and compiler
    BuildConfigPtr buildConf = project->GetBuildConfiguration(configName);
    if(!buildConf)
        return "";

    wxArrayString cppSources, cSources, resourceFiles, asmSources;
    cppSources.reserve(allFiles.size());
    cSources.reserve(allFiles.size());
    resourceFiles.reserve(allFiles.size());
    asmSources.reserve(allFiles.size());

    for (const auto& [_, file] : allFiles) {
        if(!file->IsExcludeFromConfiguration(buildConf->GetName())) {
            wxFileName fn(file->GetFilename());
            // Append each file with the "${CMAKE_CURRENT_LIST_DIR}/" prefix
            wxString file_name;
            file_name << "${CMAKE_CURRENT_LIST_DIR}/" << file->GetFilenameRelpath();
            file_name.Replace("\\", "/");
            wxString fullname = fn.GetFullName();
            if(FileExtManager::GetType(fullname) == FileExtManager::TypeSourceC) {
                cSources.Add(file_name);
            } else if(FileExtManager::GetType(fullname) == FileExtManager::TypeSourceCpp) {
                cppSources.Add(file_name);
            } else if(FileExtManager::GetType(fullname) == FileExtManager::TypeResource) {
                resourceFiles.Add(file_name);
            } else if(FileExtManager::GetType(fullname) == FileExtManager::TypeAsm) {
                asmSources.Add(file_name);
            }
        }
    }

    // Get project directory
    wxString projectDir = project->GetFileName().GetPath();

    // Sanity
    if(!buildConf || buildConf->IsCustomBuild()) {
        // no build config or custom build?
        return "";
    }

    CompilerPtr compiler = buildConf->GetCompiler();
    if(!compiler) {
        return "";
    }

    wxFileName projectPath = project->GetFileName();

    // Output file name
    wxFileName filename(projectDir, CMakePlugin::CMAKELISTS_FILE);

    if(!CheckExists(filename))
        return "";

    // File content
    wxString content;
    if(!topProject) {
        content << Prefix(project);
    }

    if(topProject) {
        // Add the dependencies
        wxArrayString projectsArr = project->GetDependencies(buildConf->GetName());
        if(!projectsArr.IsEmpty()) {
            content << "## Add the dependencies\n";
        }

        for(size_t i = 0; i < projectsArr.GetCount(); ++i) {
            ProjectPtr pProj = clCxxWorkspaceST::Get()->GetProject(projectsArr.Item(i));
            wxFileName fnProject = pProj->GetFileName();
            fnProject.MakeRelativeTo(project->GetFileName().GetPath());
            wxString path = fnProject.GetPath();
            path.Prepend("${CMAKE_SOURCE_DIR}/");
            path.Replace("\\", "/");
            content << "add_subdirectory(" << path << " ${CMAKE_CURRENT_BINARY_DIR}/" << pProj->GetName()
                    << "-build)\n";
        }
    }

    content << "# Define some variables\n";
    content << "set(" << projectPathVariable << " \"${CMAKE_CURRENT_LIST_DIR}\")\n";
    content << "set(WORKSPACE_PATH \"" << workspacePath << "\")\n";
    content << "\n";

    // Add the first hook here
    AddUserCodeSection(content, CMAKELISTS_USER_CODE_1_PREFIX, m_userBlock1);

    // Add include directories
    {
        wxArrayString includes = ::wxStringTokenize(buildConf->GetIncludePath(), ";", wxTOKEN_STRTOK);
        content << "include_directories(\n";
        for(size_t i = 0; i < includes.GetCount(); ++i) {

            wxString& includePath = includes.Item(i);
            includePath.Replace("\\", "/");
            includePath.Trim(false).Trim();
            // Replace standard macros with CMake variables
            includePath.Replace("$(WorkspacePath)", "${WORKSPACE_PATH}");
            includePath.Replace("$(WorkspaceConfiguration)", "${CONFIGURATION_NAME}");
            includePath.Replace("$(ProjectPath)", projectPathVariableValue);
            if(includePath.IsEmpty()) {
                continue;
            }
            content << "    " << includes.Item(i) << "\n";
        }
        content << "\n)\n\n";
    }

    // Add custom command from the compiler options
    {
        wxArrayString c_optsNames, c_opts, cxx_optNames, cxx_opts;
        content << "\n# Compiler options\n";

        wxString cxxCmpOptions = buildConf->GetCompileOptions();
        ExpandOptions(cxxCmpOptions, content, cxx_optNames, cxx_opts);

        wxString cCmpOptions = buildConf->GetCCompileOptions();
        ExpandOptions(cCmpOptions, content, c_optsNames, c_opts);

        // Merge the two
        wxArrayString opts = wxArrayUniqueMerge(cxx_opts, c_opts);
        wxArrayString optsNames = wxArrayUniqueMerge(cxx_optNames, c_optsNames);

        for(size_t i = 0; i < optsNames.size(); ++i) {
            content << "add_definitions(${" << optsNames.Item(i) << "})\n";
        }
        for(size_t i = 0; i < opts.size(); ++i) {
            content << "add_definitions(" << opts.Item(i) << ")\n";
        }
    }

    // Add preprocessor definitions
    {
        wxArrayString defines = project->GetPreProcessors(false);
        if(!defines.IsEmpty()) {
            content << "add_definitions(\n";
            for(size_t i = 0; i < defines.GetCount(); ++i) {
                wxString& pp = defines.Item(i);
                pp.Trim().Trim(false);
                if(pp.IsEmpty()) {
                    continue;
                }
                content << "    -D" << pp << "\n";
            }
            content << ")\n\n";
        }
    }

    // Add linker options
    {
        wxArrayString optsNames, opts;
        wxString linkOptions = buildConf->GetLinkOptions();
        content << "\n# Linker options\n";
        ExpandOptions(linkOptions, content, optsNames, opts);
        bool first = true;
        for(size_t i = 0; i < optsNames.size(); ++i) {
            content << "set(LINK_OPTIONS " << (first ? "" : "${LINK_OPTIONS} ") << "${" << optsNames.Item(i) << "})\n";
            first = false;
        }

        for(size_t i = 0; i < opts.size(); ++i) {
            content << "set(LINK_OPTIONS " << (first ? "" : "${LINK_OPTIONS} ") << opts.Item(i) << ")\n";
            first = false;
        }
        content << "\n";
    }

    {
        wxArrayString optsNames, opts;
        wxString rcOptions = buildConf->GetResCompileOptions();
        content << "\nif(WIN32)\n";
        content << "    # Resource options\n";
        ExpandOptions(rcOptions, content, optsNames, opts, "    ");
        bool first = true;
        for(size_t i = 0; i < optsNames.size(); ++i) {
            content << "    set(RC_OPTIONS " << (first ? "" : "${RC_OPTIONS} ") << "${" << optsNames.Item(i) << "})\n";
            first = false;
        }

        for(size_t i = 0; i < opts.size(); ++i) {
            content << "    set(RC_OPTIONS " << (first ? "" : "${RC_OPTIONS} ") << opts.Item(i) << ")\n";
            first = false;
        }
        content << "endif(WIN32)\n";
        content << "\n";

        // Resource options
    }

    // Add libraries paths
    {
        wxString lib_paths = buildConf->GetLibPath();
        // Get switch from compiler
        if(compiler) {
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
        for(size_t i = 0; i < lib_paths_list.GetCount(); ++i) {
            wxString libPath = lib_paths_list.Item(i);
            // Replace standard macros with CMake variables
            libPath.Replace("$(WorkspacePath)", "${WORKSPACE_PATH}");
            libPath.Replace("$(WorkspaceConfiguration)", "${CONFIGURATION_NAME}");
            libPath.Replace("$(ProjectPath)", projectPathVariableValue);
            ::WrapWithQuotes(libPath);
            lib_paths << "    " << libPath << "\n";
        }

        content << "# Library path\n";
        content << "link_directories(\n" << lib_paths << ")\n\n";
    }

    // Write sources
    {
        if(!cSources.IsEmpty()) {
            content << "# Define the C sources\n";
            content << "set ( C_SRCS\n";
            for(size_t i = 0; i < cSources.GetCount(); ++i) {
                content << "    " << cSources.Item(i) << "\n";
            }
            content << ")\n\n";
        }

        if(!cppSources.IsEmpty()) {
            content << "# Define the CXX sources\n";
            content << "set ( CXX_SRCS\n";
            for(size_t i = 0; i < cppSources.GetCount(); ++i) {
                content << "    " << cppSources.Item(i) << "\n";
            }
            content << ")\n\n";
        }

        if(!resourceFiles.IsEmpty()) {
            content << "if(WIN32)\n";
            content << "    # Define the resource files\n";
            content << "    set ( RC_SRCS\n";
            for(size_t i = 0; i < resourceFiles.GetCount(); ++i) {
                content << "        " << resourceFiles.Item(i) << "\n";
            }
            content << "    )\n";
            content << "endif(WIN32)\n\n";
        }

        if(!asmSources.IsEmpty()) {
            content << "# Define the ASM sources\n";
            content << "set ( ASM_SRCS\n";
            for(size_t i = 0; i < asmSources.GetCount(); ++i) {
                content << "    " << asmSources.Item(i) << "\n";
            }
            content << ")\n\n";
        }
    }

    // Add CXX compiler options
    {
        wxArrayString buildOptsArr = project->GetCXXCompilerOptions();
        if(!buildOptsArr.IsEmpty() && !cppSources.IsEmpty()) {
            content << "set_source_files_properties(\n    ${CXX_SRCS} PROPERTIES COMPILE_FLAGS \n    \"";
            for(size_t i = 0; i < buildOptsArr.GetCount(); ++i) {
                content << " " << buildOptsArr.Item(i);
            }
            content << "\")\n\n";
        }
    }

    // Add C compiler options
    {
        wxArrayString buildOptsArr = project->GetCCompilerOptions();
        if(!buildOptsArr.IsEmpty() && !cSources.IsEmpty()) {
            content << "set_source_files_properties(\n    ${C_SRCS} PROPERTIES COMPILE_FLAGS \n    \"";
            for(size_t i = 0; i < buildOptsArr.GetCount(); ++i) {
                content << " " << buildOptsArr.Item(i);
            }
            content << "\")\n\n";
        }
    }

    {
        content << "if(WIN32)\n"
                << "    enable_language(RC)\n"
                << "    set(CMAKE_RC_COMPILE_OBJECT\n"
                << "        \"<CMAKE_RC_COMPILER> ${RC_OPTIONS} -O coff -i <SOURCE> -o <OBJECT>\")\n"
                << "endif(WIN32)\n\n";
    }

    // Add the second hook here
    AddUserCodeSection(content, CMAKELISTS_USER_CODE_2_PREFIX, m_userBlock2);

    // Get project type
    {
        wxString type = buildConf->GetProjectType();
        if(type == PROJECT_TYPE_EXECUTABLE) {
            content << "add_executable(" << project->GetName() << " ${RC_SRCS} ${CXX_SRCS} ${C_SRCS} ${ASM_SRCS})\n";
            content << "target_link_libraries(" << project->GetName() << " ${LINK_OPTIONS})\n\n";

        } else if(type == PROJECT_TYPE_DYNAMIC_LIBRARY) {
            content << "add_library(" << project->GetName()
                    << " SHARED ${RC_SRCS} ${CXX_SRCS} ${C_SRCS} ${ASM_SRCS})\n";
            content << "target_link_libraries(" << project->GetName() << " ${LINK_OPTIONS})\n\n";

        } else {
            content << "add_library(" << project->GetName() << " ${RC_SRCS} ${CXX_SRCS} ${C_SRCS} ${ASM_SRCS})\n\n";
        }
    }

    // Add link libraries
    {
        wxString libs = buildConf->GetLibraries();

        libs.Trim().Trim(false);

        if(!libs.IsEmpty()) {
            libs.Replace(";", "\n    ");
            content << "target_link_libraries(" << project->GetName() << "\n    " << libs << "\n)\n\n";
        }
    }

    // set the dependecies for the top level project
    if(topProject) {
        wxArrayString projects = project->GetDependencies(buildConf->GetName());
        if(!projects.IsEmpty()) {
            content << "\n";
            content << "# Adding dependencies\n";
            for(size_t i = 0; i < projects.GetCount(); ++i) {
                content << "add_dependencies(" << project->GetName() << " " << projects.Item(i) << ")\n";
            }
        }
    }

    // Add pre|post build commands
    {
        BuildCommandList commands;
        buildConf->GetPreBuildCommands(commands);
        AddBuildCommands("PRE_BUILD", commands, project, content);
    }

    {
        BuildCommandList commands;
        buildConf->GetPostBuildCommands(commands);
        AddBuildCommands("POST_BUILD", commands, project, content);
    }

    // Add the last hook here
    AddUserCodeSection(content, CMAKELISTS_USER_CODE_3_PREFIX, m_userBlock3);

    // Write result
    return content;
}

void CMakeGenerator::AddBuildCommands(const wxString& buildType, const BuildCommandList& commands, ProjectPtr project,
                                      wxString& content)
{
    if(!commands.empty()) {
        wxString projectPathVariableValue;
        projectPathVariableValue << "${PROJECT_" << project->GetName() << "_PATH}";
        content << "\n# Adding " << buildType << " commands\n";
        for (const BuildCommand& buildCommand : commands) {
            if(buildCommand.GetEnabled()) {
                wxString cmd = buildCommand.GetCommand();
                cmd.Replace("$(WorkspacePath)", "${WORKSPACE_PATH}");
                cmd.Replace("$(ProjectPath)", projectPathVariableValue);
                content << "add_custom_command(\n"
                        << "    TARGET " << project->GetName() << "\n"
                        << "    " << buildType << "\n"
                        << "    COMMAND " << cmd << ")\n";
            }
        }
        content << "\n";
    }
}

bool CMakeGenerator::CanGenerate(ProjectPtr project)
{
    clCxxWorkspace* workspace = clCxxWorkspaceST::Get();
    wxFileName workspaceFile = workspace->GetFileName();
    std::vector<wxFileName> cmakelistsArr;
    cmakelistsArr.push_back(wxFileName(workspaceFile.GetPath(), CMakePlugin::CMAKELISTS_FILE));
    cmakelistsArr.push_back(wxFileName(project->GetFileName().GetPath(), CMakePlugin::CMAKELISTS_FILE));

    BuildConfigPtr buildConf = project->GetBuildConfiguration();
    CHECK_COND_RET_FALSE(buildConf);

    // Add the projects
    wxArrayString projects = project->GetDependencies(buildConf->GetName());
    for(size_t i = 0; i < projects.size(); ++i) {
        ProjectPtr p = workspace->GetProject(projects.Item(i));
        if(p) {
            cmakelistsArr.push_back(wxFileName(p->GetFileName().GetPath(), CMakePlugin::CMAKELISTS_FILE));
        }
    }

    // Read each of the CMakeLists.txt file in the array, and check if we can find our top level comment
    for(size_t i = 0; i < cmakelistsArr.size(); ++i) {
        const wxFileName& fn = cmakelistsArr.at(i);
        wxString content;
        if(fn.Exists() && FileUtils::ReadFileContent(fn, content)) {
            if(!content.StartsWith(CODELITE_CMAKE_PREFIX)) {
                return false;
            }
        }
    }
    return true;
}

void CMakeGenerator::ExpandOptions(const wxString& options, wxString& content, wxArrayString& arrVars,
                                   wxArrayString& arrOut, const wxString& indent)
{
    arrVars.clear();
    arrOut.clear();

    wxArrayString arrOptions = ::wxStringTokenize(options, ";", wxTOKEN_STRTOK);
    for(size_t i = 0; i < arrOptions.size(); ++i) {
        wxString cmpOption(arrOptions.Item(i));
        cmpOption.Trim().Trim(false);
        wxString tmp;
        // Expand backticks / $(shell ...) syntax supported by CodeLite
        if(cmpOption.StartsWith(wxT("$(shell "), &tmp) || cmpOption.StartsWith(wxT("`"), &tmp)) {
            cmpOption.swap(tmp);
            tmp.Clear();
            if(cmpOption.EndsWith(wxT(")"), &tmp) || cmpOption.EndsWith(wxT("`"), &tmp)) {
                cmpOption.swap(tmp);
            }

            wxString varname;
            varname << "CL_VAR_" << (++m_counter);
            // Create a CMake command that executes this command
            content << indent << "execute_process(COMMAND \n"
                    << indent << "    " << cmpOption << "\n"
                    << indent << "    "
                    << "OUTPUT_VARIABLE\n"
                    << indent << "    "
                    << "CL_TMP_VAR\n"
                    << indent << "    "
                    << "OUTPUT_STRIP_TRAILING_WHITESPACE)\n";
            content << indent << "string(STRIP ${CL_TMP_VAR} " << varname << ")\n";
            arrVars.Add(varname);
        } else {
            arrOut.Add(cmpOption); // Keep as is
        }
    }
}

wxString CMakeGenerator::Prefix(ProjectPtr project)
{
    // File content
    wxString content;

    content << CODELITE_CMAKE_PREFIX << "\n\n";
    content << "cmake_minimum_required(VERSION 3.5)\n";
    
    AddUserCodeSection(content, CMAKELISTS_USER_CODE_01_PREFIX, m_userBlock01);
    content << "enable_language(CXX C ASM)\n\n";

    // Print project name
    content << "project(" << project->GetName() << ")\n\n";
    AddUserCodeSection(content, CMAKELISTS_USER_CODE_02_PREFIX, m_userBlock02);
    return content;
}

bool CMakeGenerator::IsCustomCMakeLists(const wxFileName& fn)
{
    if(fn.FileExists()) {
        wxFileName fn;
        wxString content;
        if(FileUtils::ReadFileContent(fn, content)) {
            return !content.StartsWith(CODELITE_CMAKE_PREFIX);
        }
    }
    return false;
}

void CMakeGenerator::ReadUserCode(const wxString& content)
{
    m_userBlock01.clear();
    m_userBlock02.clear();
    m_userBlock1.clear();
    m_userBlock2.clear();
    m_userBlock3.clear();

    wxArrayString lines = ::wxStringTokenize(content, "\n", wxTOKEN_RET_DELIMS);
    while(!lines.IsEmpty()) {
        const wxString& curline = lines.Item(0);
        lines.RemoveAt(0);
        if(curline.StartsWith(CMAKELISTS_USER_CODE_1_PREFIX)) {
            ReadUntilEndOfUserBlock(lines, m_userBlock1);
        } else if(curline.StartsWith(CMAKELISTS_USER_CODE_2_PREFIX)) {
            ReadUntilEndOfUserBlock(lines, m_userBlock2);
        } else if(curline.StartsWith(CMAKELISTS_USER_CODE_3_PREFIX)) {
            ReadUntilEndOfUserBlock(lines, m_userBlock3);
        } else if(curline.StartsWith(CMAKELISTS_USER_CODE_01_PREFIX)) {
            ReadUntilEndOfUserBlock(lines, m_userBlock01);
        } else if(curline.StartsWith(CMAKELISTS_USER_CODE_02_PREFIX)) {
            ReadUntilEndOfUserBlock(lines, m_userBlock02);
        }
    }
}

void CMakeGenerator::ReadUntilEndOfUserBlock(wxArrayString& lines, wxString& content)
{
    bool removeLf = false;
    while(!lines.IsEmpty()) {
        const wxString& curline = lines.Item(0);
        lines.RemoveAt(0);
        if(curline.StartsWith(CMAKELISTS_USER_CODE_END)) {
            return;
        } else {
            content << curline;
            // removeLf = true;
        }
    }
    if(removeLf) {
        content.RemoveLast();
    }
}

void CMakeGenerator::AddUserCodeSection(wxString& content, const wxString& sectionPrefix, const wxString& sectionCode)
{
    content << "\n";
    content << "\n";
    content << sectionPrefix << "\n";
    if(sectionCode.IsEmpty()) {
        content << "# Place your code here"
                << "\n";
    } else {
        content << sectionCode;
    }
    content << CMAKELISTS_USER_CODE_END;
    content << "\n\n";
}
