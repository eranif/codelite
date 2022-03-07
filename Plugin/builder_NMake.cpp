//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : builder_NMake.cpp
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
#include "builder_NMake.h"

#include "StringUtils.h"
#include "build_settings_config.h"
#include "buildmanager.h"
#include "cl_command_event.h"
#include "configuration_mapping.h"
#include "dirsaver.h"
#include "editor_config.h"
#include "environmentconfig.h"
#include "event_notifier.h"
#include "evnvarlist.h"
#include "file_logger.h"
#include "fileextmanager.h"
#include "globals.h"
#include "macromanager.h"
#include "macros.h"
#include "project.h"

#include <algorithm>
#include <wx/app.h>
#include <wx/msgdlg.h>
#include <wx/sstream.h>
#include <wx/stopwatch.h>
#include <wx/tokenzr.h>

static bool OS_WINDOWS = wxGetOsVersion() & wxOS_WINDOWS ? true : false;

static wxString ReplaceBackslashes(const wxString& instr)
{
    wxString tmpstr = instr;
    tmpstr.Trim().Trim(false);

    if(tmpstr.StartsWith("\"")) {
        tmpstr = tmpstr.Mid(1);
        wxString strBefore = tmpstr.BeforeFirst('"');
        wxString strAfter = tmpstr.AfterFirst('"');
        strBefore.Prepend("\"").Append("\""); // restore the " that we removed
        strBefore.Replace("/", "\\");
        strBefore << strAfter;
        tmpstr.swap(strBefore);
    } else {
        wxString strBefore = tmpstr.BeforeFirst(' ');
        wxString strAfter = tmpstr.AfterFirst(' ');
        strAfter.Prepend(" ");
        strBefore.Replace("/", "\\");
        strBefore << strAfter;
        tmpstr.swap(strBefore);
    }
    return tmpstr;
}

BuilderNMake::BuilderNMake()
    : Builder("NMakefile for MSVC toolset")
{
}

BuilderNMake::BuilderNMake(const wxString& name, const wxString& buildTool, const wxString& buildToolOptions)
    : Builder(name)
{
}

BuilderNMake::~BuilderNMake() {}

wxString BuilderNMake::GetIntermediateDirectory(ProjectPtr proj, BuildConfigPtr bldConf) const
{
    wxString workspacePath = clCxxWorkspaceST::Get()->GetWorkspaceFileName().GetPath();
    wxString projectPath = proj->GetFileName().GetPath();
    wxString intermediateDir = bldConf->GetIntermediateDirectory();
    intermediateDir.Replace("$(WorkspacePath)", workspacePath);
    intermediateDir.Replace("$(ProjectPath)", projectPath);
    wxFileName fnIntermediateDir(intermediateDir, "");
    if(fnIntermediateDir.IsAbsolute()) {
        fnIntermediateDir.MakeRelativeTo(projectPath);
    }
    intermediateDir = fnIntermediateDir.GetPath(wxPATH_NO_SEPARATOR);
    intermediateDir.Replace("/", "\\");
    return intermediateDir;
}

bool BuilderNMake::Export(const wxString& project, const wxString& confToBuild, const wxString& arguments,
                          bool isProjectOnly, bool force, wxString& errMsg)
{
    if(project.IsEmpty()) {
        return false;
    }
    ProjectPtr proj = clCxxWorkspaceST::Get()->FindProjectByName(project, errMsg);
    if(!proj) {
        errMsg << _("Cant open project '") << project << "'";
        return false;
    }

    // get the selected build configuration
    wxString bld_conf_name(confToBuild);

    if(confToBuild.IsEmpty()) {
        BuildConfigPtr bldConf = clCxxWorkspaceST::Get()->GetProjBuildConf(project, confToBuild);
        if(!bldConf) {
            errMsg << _("Cant find build configuration for project '") << project << "'";
            return false;
        }
        bld_conf_name = bldConf->GetName();
    }

    BuildConfigPtr bldConf = clCxxWorkspaceST::Get()->GetProjBuildConf(project, bld_conf_name);
    if(!bldConf) {
        errMsg << _("Cant find build configuration for project '") << project << "'";
        return false;
    }
    if(!bldConf->GetCompiler()) {
        errMsg << _("Cant find proper compiler for project '") << project << "'";
        return false;
    }

    wxArrayString depsArr = proj->GetDependencies(bld_conf_name);

    CL_DEBUG("Generating Makefile...");
    // Filter all disabled projects from the dependencies array
    wxArrayString updatedDepsArr;
    for(size_t i = 0; i < depsArr.GetCount(); ++i) {
        wxString errmsg;
        ProjectPtr dependProj = clCxxWorkspaceST::Get()->FindProjectByName(depsArr.Item(i), errmsg);
        if(!dependProj) {
            continue;
        }

        wxString projectSelConf = clCxxWorkspaceST::Get()->GetBuildMatrix()->GetProjectSelectedConf(
            clCxxWorkspaceST::Get()->GetBuildMatrix()->GetSelectedConfigurationName(), dependProj->GetName());
        BuildConfigPtr dependProjbldConf =
            clCxxWorkspaceST::Get()->GetProjBuildConf(dependProj->GetName(), projectSelConf);
        if(dependProjbldConf && dependProjbldConf->IsProjectEnabled()) {
            updatedDepsArr.Add(depsArr.Item(i));
        }
    }

    // Update the dependencies with the correct list
    depsArr.swap(updatedDepsArr);

    wxArrayString removeList;
    if(!isProjectOnly) {
        // this function assumes that the working directory is located at the workspace path
        // make sure that all dependencies exists
        for(size_t i = 0; i < depsArr.GetCount(); i++) {
            ProjectPtr dependProj = clCxxWorkspaceST::Get()->FindProjectByName(depsArr.Item(i), errMsg);
            // Missing dependencies project?
            // this can happen if a project was removed from the workspace, but
            // is still on the depdendencie list of this project
            if(!dependProj) {
                wxString msg;
                msg << _("CodeLite can not find project '") << depsArr.Item(i) << _("' which is required\n");
                msg << _("for building project '") << project
                    << _("'.\nWould you like to remove it from the dependency list?");
                if(wxMessageBox(msg, _("CodeLite"), wxYES_NO | wxICON_QUESTION) == wxYES) {
                    // remove the project from the dependecie list, and continue
                    removeList.Add(depsArr.Item(i));
                }
            }
        }
        bool settingsChanged(false);
        // remove the unfound projects from the dependencies array
        for(size_t i = 0; i < removeList.GetCount(); i++) {
            int where = depsArr.Index(removeList.Item(i));
            if(where != wxNOT_FOUND) {
                depsArr.RemoveAt(where);
                settingsChanged = true;
            }
        }
        // update the project dependencies
        bool modified = proj->IsModified();

        // Update the dependencies only if needed
        if(settingsChanged) {
            proj->SetDependencies(depsArr, bld_conf_name);
        }

        // the set settings functions marks the project as 'modified' this causes
        // an unneeded makefile generation if the settings was not really modified
        if(!modified && !settingsChanged) {
            proj->SetModified(false);
        }
    }

    wxString fn;
    fn << "Makefile";
    wxString text;

    wxFileName wspfile(clCxxWorkspaceST::Get()->GetWorkspaceFileName());

    text << "All:\n";

    // iterate over the dependencies projects and generate makefile
    wxString buildTool = GetBuildToolCommand(project, confToBuild, "", false);
    buildTool = EnvironmentConfig::Instance()->ExpandVariables(buildTool, true);

    // fix: replace all Windows like slashes to POSIX
    // buildTool.Replace("\\", "/");

    // generate the makefile for the selected workspace configuration
    BuildMatrixPtr matrix = clCxxWorkspaceST::Get()->GetBuildMatrix();
    wxString workspaceSelConf = matrix->GetSelectedConfigurationName();
    wxArrayString depsProjs;
    if(!isProjectOnly) {
        for(size_t i = 0; i < depsArr.GetCount(); ++i) {
            bool isCustom(false);
            ProjectPtr dependProj = clCxxWorkspaceST::Get()->FindProjectByName(depsArr.Item(i), errMsg);
            if(!dependProj) {
                continue;
            }

            wxString projectSelConf = matrix->GetProjectSelectedConf(workspaceSelConf, dependProj->GetName());
            BuildConfigPtr dependProjbldConf =
                clCxxWorkspaceST::Get()->GetProjBuildConf(dependProj->GetName(), projectSelConf);
            if(dependProjbldConf && dependProjbldConf->IsCustomBuild()) {
                isCustom = true;
            }

            // incase we manually specified the configuration to be built, set the project
            // as modified, so on next attempt to build it, CodeLite will sync the configuration
            if(confToBuild.IsEmpty() == false) {
                dependProj->SetModified(true);
            }

            if(!dependProjbldConf->IsProjectEnabled()) {
                // Ignore disabled projects
                continue;
            }

            text << "\t@echo \"" << BUILD_PROJECT_PREFIX << dependProj->GetName() << " - " << projectSelConf
                 << " ]----------\"\n";
            // make the paths relative, if it's sensible to do so
            wxFileName fn(dependProj->GetFileName());
            MakeRelativeIfSensible(fn, wspfile.GetPath());

            bool isPluginGeneratedMakefile =
                SendBuildEvent(wxEVT_GET_IS_PLUGIN_MAKEFILE, depsArr.Item(i), projectSelConf);

            // we handle custom builds and non-custom build separatly:
            if(isPluginGeneratedMakefile) {

                // this project makefile is generated by a plugin
                // query the plugin about the build command
                clBuildEvent e(wxEVT_GET_PROJECT_BUILD_CMD);
                e.SetProjectName(depsArr.Item(i));
                e.SetConfigurationName(projectSelConf);
                e.SetProjectOnly(false);
                EventNotifier::Get()->ProcessEvent(e);
                text << "\t" << e.GetCommand() << "\n";

            } else if(isCustom) {

                CreateCustomPreBuildEvents(dependProjbldConf, text);

                wxString customWd = dependProjbldConf->GetCustomBuildWorkingDir();
                wxString build_cmd = dependProjbldConf->GetCustomBuildCmd();
                wxString customWdCmd;

                customWd = ExpandAllVariables(customWd, clCxxWorkspaceST::Get(), dependProj->GetName(),
                                              dependProjbldConf->GetName(), wxEmptyString);
                build_cmd = ExpandAllVariables(build_cmd, clCxxWorkspaceST::Get(), dependProj->GetName(),
                                               dependProjbldConf->GetName(), wxEmptyString);

                build_cmd.Trim().Trim(false);

                if(build_cmd.empty()) {
                    build_cmd << "@echo Project has no custom build command!";
                }

                // if a working directory is provided apply it, otherwise use the project
                // path
                customWd.Trim().Trim(false);
                if(customWd.empty() == false) {
                    customWdCmd << "@cd \"" << ExpandVariables(customWd, dependProj, NULL) << "\" && ";
                } else {
                    customWdCmd << GetCdCmd(wspfile, fn);
                }

                text << "\t" << customWdCmd << build_cmd << "\n";
                CreateCustomPostBuildEvents(dependProjbldConf, text);

            } else {
                // generate the dependency project makefile
                wxString dep_file = DoGetMarkerFileDir("");
                dep_file << wxFileName::GetPathSeparator() << dependProj->GetName();
                depsProjs.Add(dep_file);

                GenerateMakefile(dependProj, projectSelConf, confToBuild.IsEmpty() ? force : true, wxArrayString());
                text << GetProjectMakeCommand(wspfile, fn, dependProj, projectSelConf);
            }
        }
    }

    // Generate makefile for the project itself
    GenerateMakefile(proj, confToBuild, confToBuild.IsEmpty() ? force : true, depsProjs);

    // incase we manually specified the configuration to be built, set the project
    // as modified, so on next attempt to build it, CodeLite will sync the configuration
    if(confToBuild.IsEmpty() == false) {
        proj->SetModified(true);
    }

    wxString projectSelConf = matrix->GetProjectSelectedConf(workspaceSelConf, project);
    if(isProjectOnly && confToBuild.IsEmpty() == false) {
        // incase we use to generate a 'Project Only' makefile,
        // we allow the caller to override the selected configuration with 'confToBuild' parameter
        projectSelConf = confToBuild;
    }

    text << "\t@echo \"" << BUILD_PROJECT_PREFIX << project << " - " << projectSelConf << " ]----------\"\n";

    // make the paths relative, if it's sensible to do so
    wxFileName projectPath(proj->GetFileName());
    MakeRelativeIfSensible(projectPath, wspfile.GetPath());

    wxString pname(proj->GetName());

    bool isPluginGeneratedMakefile = SendBuildEvent(wxEVT_GET_IS_PLUGIN_MAKEFILE, pname, projectSelConf);
    if(isPluginGeneratedMakefile) {

        wxString cmd;
        clBuildEvent e(wxEVT_GET_PROJECT_BUILD_CMD);
        e.SetProjectName(pname);
        e.SetConfigurationName(projectSelConf);
        e.SetProjectOnly(isProjectOnly);
        EventNotifier::Get()->ProcessEvent(e);

        cmd = e.GetCommand();
        text << "\t" << cmd << "\n";

    } else {
        text << GetProjectMakeCommand(wspfile, projectPath, proj, projectSelConf);
    }

    // create the clean target
    text << "clean:\n";
    if(!isProjectOnly) {
        for(size_t i = 0; i < depsArr.GetCount(); i++) {
            bool isCustom(false);
            wxString projectSelConf = matrix->GetProjectSelectedConf(workspaceSelConf, depsArr.Item(i));

            ProjectPtr dependProj = clCxxWorkspaceST::Get()->FindProjectByName(depsArr.Item(i), errMsg);
            // Missing dependencies project? just skip it
            if(!dependProj) {
                continue;
            }

            text << "\t@echo \"" << CLEAN_PROJECT_PREFIX << dependProj->GetName() << " - " << projectSelConf
                 << " ]----------\"\n";

            // make the paths relative
            wxFileName fn(dependProj->GetFileName());
            MakeRelativeIfSensible(fn, wspfile.GetPath());

            // if the dependencie project is project of type 'Custom Build' - do the custom build instead
            // of the geenrated makefile
            BuildConfigPtr dependProjbldConf =
                clCxxWorkspaceST::Get()->GetProjBuildConf(dependProj->GetName(), projectSelConf);
            if(dependProjbldConf && dependProjbldConf->IsCustomBuild()) {
                isCustom = true;
            }

            // if we are using custom build command, invoke it instead of calling the generated makefile
            wxString pname = dependProj->GetName();
            bool isPluginGeneratedMakefile = SendBuildEvent(wxEVT_GET_IS_PLUGIN_MAKEFILE, pname, projectSelConf);

            // we handle custom builds and non-custom build separately:
            if(isPluginGeneratedMakefile) {

                // this project makefile is generated by a plugin
                // query the plugin about the build command
                clBuildEvent e(wxEVT_GET_PROJECT_CLEAN_CMD);
                e.SetProjectOnly(false);
                e.SetProjectName(pname);
                e.SetConfigurationName(projectSelConf);
                EventNotifier::Get()->ProcessEvent(e);
                text << "\t" << e.GetCommand() << "\n";

            } else if(!isCustom) {

                text << "\t" << GetCdCmd(wspfile, fn) << buildTool << " \"" << dependProj->GetName()
                     << ".mk\"  clean\n";

            } else {

                wxString customWd = dependProjbldConf->GetCustomBuildWorkingDir();
                wxString clean_cmd = dependProjbldConf->GetCustomCleanCmd();

                customWd = ExpandAllVariables(customWd, clCxxWorkspaceST::Get(), dependProj->GetName(),
                                              dependProjbldConf->GetName(), wxEmptyString);
                clean_cmd = ExpandAllVariables(clean_cmd, clCxxWorkspaceST::Get(), dependProj->GetName(),
                                               dependProjbldConf->GetName(), wxEmptyString);

                wxString customWdCmd;

                clean_cmd.Trim().Trim(false);
                if(clean_cmd.empty()) {
                    clean_cmd << "@echo Project has no custom clean command!";
                }

                // if a working directory is provided apply it, otherwise use the project
                // path
                customWd.Trim().Trim(false);
                if(customWd.empty() == false) {
                    customWdCmd << "@cd \"" << ExpandVariables(customWd, dependProj, NULL) << "\" && ";
                } else {
                    customWdCmd << GetCdCmd(wspfile, fn);
                }
                text << "\t" << customWdCmd << clean_cmd << "\n";
            }
        }
    }

    // generate makefile for the project itself
    projectSelConf = matrix->GetProjectSelectedConf(workspaceSelConf, project);
    if(isProjectOnly && confToBuild.IsEmpty() == false) {
        // incase we use to generate a 'Project Only' makefile,
        // we allow the caller to override the selected configuration with 'confToBuild' parameter
        projectSelConf = confToBuild;
    }

    text << "\t@echo \"" << CLEAN_PROJECT_PREFIX << project << " - " << projectSelConf << " ]----------\"\n";
    if(isPluginGeneratedMakefile) {

        wxString cmd;

        clBuildEvent e(wxEVT_GET_PROJECT_CLEAN_CMD);
        e.SetProjectName(pname);
        e.SetConfigurationName(projectSelConf);
        e.SetProjectOnly(isProjectOnly);
        EventNotifier::Get()->ProcessEvent(e);
        cmd = e.GetCommand();
        text << "\t" << cmd << "\n";

    } else {
        text << "\t" << GetCdCmd(wspfile, projectPath) << buildTool << " \"" << proj->GetName() << ".mk\" clean\n";
    }

    text << ".PHONY: clean All\n\n";
    // dump the content to file
    wxFileOutputStream output(fn);
    wxStringInputStream content(text);
    output << content;

    CL_DEBUG("Generating Makefile...is completed");
    return true;
}

void BuilderNMake::GenerateMakefile(ProjectPtr proj, const wxString& confToBuild, bool force,
                                    const wxArrayString& depsProj)
{
    wxString pname(proj->GetName());
    wxString tmpConfigName(confToBuild.c_str());
    if(confToBuild.IsEmpty()) {
        BuildMatrixPtr matrix = clCxxWorkspaceST::Get()->GetBuildMatrix();
        tmpConfigName = matrix->GetProjectSelectedConf(matrix->GetSelectedConfigurationName(), proj->GetName());
    }

    bool isPluginGeneratedMakefile = SendBuildEvent(wxEVT_GET_IS_PLUGIN_MAKEFILE, pname, tmpConfigName);

    // we handle custom builds and non-custom build separatly:
    if(isPluginGeneratedMakefile) {
        if(force) {
            // Generate the makefile
            SendBuildEvent(wxEVT_PLUGIN_EXPORT_MAKEFILE, pname, tmpConfigName);
        }
        return;
    }

    ProjectSettingsPtr settings = proj->GetSettings();
    if(!settings) {
        return;
    }

    // get the selected build configuration for this project
    BuildConfigPtr bldConf = clCxxWorkspaceST::Get()->GetProjBuildConf(proj->GetName(), confToBuild);
    if(!bldConf) {
        return;
    }

    wxString path = proj->GetFileName().GetPath();

    DirSaver ds;
    // change directory to the project base dir
    ::wxSetWorkingDirectory(path);

    // create new makefile file
    wxString fn(path);
    fn << PATH_SEP << proj->GetName() << ".mk";

    // skip the next test if the makefile does not exist
    if(wxFileName::FileExists(fn)) {
        if(!force) {
            if(proj->IsModified() == false) {
                return;
            }
        }
    }

    // Load the current project files
    m_allFiles.clear();
    proj->GetFilesAsVector(m_allFiles);

    // generate the selected configuration for this project
    // wxTextOutputStream text(output);
    wxString text;

    text << "##"
         << "\n";
    text << "## Auto Generated makefile by CodeLite IDE"
         << "\n";
    text << "## any manual changes will be erased      "
         << "\n";
    text << "##"
         << "\n";

    // Create the makefile variables
    CreateConfigsVariables(proj, bldConf, text);

    //----------------------------------------------------------
    // copy environment variables to the makefile
    // We put them after the 'hard-coeded' ones
    // so user will be able to override any of the default
    // variables by defining its own
    //----------------------------------------------------------
    EvnVarList vars;
    EnvironmentConfig::Instance()->ReadObject("Variables", &vars);
    EnvMap varMap = vars.GetVariables("", true, proj->GetName(), bldConf->GetName());

    text << "##"
         << "\n";
    text << "## User defined environment variables"
         << "\n";
    text << "##"
         << "\n";

    for(size_t i = 0; i < varMap.GetCount(); i++) {
        wxString name, value;
        varMap.Get(i, name, value);
        text << name << "=" << value << ""
             << "\n";
    }

    text << "\n\n";

    CreateListMacros(proj, confToBuild, text); // list of srcs and list of objects

    //-----------------------------------------------------------
    // create the build targets
    //-----------------------------------------------------------
    text << "##\n";
    text << "## Main Build Targets \n";
    text << "##\n";

    // incase project is type exe or dll, force link
    // this is to workaround bug in the generated makefiles
    // which causes the makefile to report 'nothing to be done'
    // even when a dependency was modified
    wxString targetName("$(IntermediateDirectory)");
    CreateLinkTargets(proj->GetSettings()->GetProjectType(bldConf->GetName()), bldConf, text, targetName,
                      proj->GetName(), depsProj);

    CreatePostBuildEvents(proj, bldConf, text);
    CreateMakeDirsTarget(proj, bldConf, targetName, text);
    CreatePreBuildEvents(proj, bldConf, text);
    CreatePreCompiledHeaderTarget(bldConf, text);

    //-----------------------------------------------------------
    // Create a list of targets that should be built according to
    // projects' file list
    //-----------------------------------------------------------
    CreateFileTargets(proj, confToBuild, text);
    CreateCleanTargets(proj, confToBuild, text);

    // dump the content to a file
    wxFFile output;
    output.Open(fn, "w+");
    if(output.IsOpened()) {
        output.Write(text);
        output.Close();
    }

    // mark the project as non-modified one
    proj->SetModified(false);
}

void BuilderNMake::CreateMakeDirsTarget(ProjectPtr proj, BuildConfigPtr bldConf, const wxString& targetName,
                                        wxString& text)
{
    text << "\n";
    text << "MakeIntermediateDirs:\n";
    text << "\t"
         << "@$(MakeDirCommand) \"$(IntermediateDirectory)\""
         << "\n";
    text << "\t"
         << "@$(MakeDirCommand) \"$(OutputDirectory)\""
         << "\n\n";

    text << "\n";
    text << targetName << ":\n";
    text << "\t"
         << "@echo Creating Intermediate Directory\n";
    text << "\t"
         << "@$(MakeDirCommand) \"$(IntermediateDirectory)\""
         << "\n";
    text << "\t"
         << "@echo Intermediate directories created";
}

void BuilderNMake::CreateSrcList(ProjectPtr proj, const wxString& confToBuild, wxString& text)
{
    std::vector<wxFileName> files;
    const Project::FilesMap_t& all_files = proj->GetFiles();

    // Remove excluded files
    std::for_each(all_files.begin(), all_files.end(), [&](const Project::FilesMap_t::value_type& vt) {
        clProjectFile::Ptr_t f = vt.second;
        if(!f->IsExcludeFromConfiguration(confToBuild)) {
            files.push_back(wxFileName(f->GetFilenameRelpath()));
        }
    });

    text << "Srcs=";

    BuildConfigPtr bldConf = clCxxWorkspaceST::Get()->GetProjBuildConf(proj->GetName(), confToBuild);
    wxString cmpType = bldConf->GetCompilerType();
    wxString relPath;

    // get the compiler settings
    CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(cmpType);

    int counter = 1;
    Compiler::CmpFileTypeInfo ft;

    for(size_t i = 0; i < files.size(); i++) {

        // is this a valid file?
        if(!cmp->GetCmpFileType(files[i].GetExt(), ft)) {
            continue;
        }

        if(ft.kind == Compiler::CmpFileKindResource) {
#ifndef __WXMSW__
            // Resource compiler "windres" is not supported under
            // *nix OS
            continue;
#endif
        }

        relPath = files.at(i).GetPath(true, wxPATH_UNIX);
        relPath.Trim().Trim(false);

        // Compiler::CmpFileKindSource , Compiler::CmpFileKindResource file
        text << relPath << files[i].GetFullName() << " ";

        if(counter % 10 == 0) {
            text << "\\\n\t";
        }
        counter++;
    }
    text << "\n\n";
}

void BuilderNMake::CreateObjectList(ProjectPtr proj, const wxString& confToBuild, wxString& text)
{
    m_objectChunks = 1;
    std::vector<wxFileName> files;

    std::for_each(m_allFiles.begin(), m_allFiles.end(), [&](clProjectFile::Ptr_t file) {
        // Include only files that don't have the 'exclude from build' flag set
        if(!file->IsExcludeFromConfiguration(confToBuild)) {
            files.push_back(wxFileName(file->GetFilename()));
        }
    });

    BuildConfigPtr bldConf = clCxxWorkspaceST::Get()->GetProjBuildConf(proj->GetName(), confToBuild);
    wxString cmpType = bldConf->GetCompilerType();

    // get the compiler settings
    CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(cmpType);

    int counter = 1;
    Compiler::CmpFileTypeInfo ft;
    wxString projectPath = proj->GetFileName().GetPath();

    size_t objCounter = 0;
    int numOfObjectsInCurrentChunk = 0;
    wxString curChunk;

    text << "##\n";
    text << "## Object Targets Lists \n";
    text << "##\n";

    // PCH object file needs to be linked as part of them
    m_hasObjectPCH = false;
    wxString pchFile = bldConf->GetPrecompiledHeader();
    pchFile.Trim().Trim(false);
    if(pchFile.IsEmpty() == false && (bldConf->GetPCHFlagsPolicy() != BuildConfig::kPCHJustInclude)) {
        text << "ObjectPCH=" << pchFile << "$(ObjectSuffix)\n\n";
        m_hasObjectPCH = true;
    }

    // We break the list of files into a seriese of objects variables
    // each variable contains up to 100 files.
    // This is needed because on MSW, the ECHO command can not handle over 8K bytes
    // and codelite uses the ECHO command to create a file with all the objects
    // so we can pass it to the liker
    for(size_t i = 0; i < files.size(); ++i) {

        // Did we collect 100 objects yet?
        if(i && ((i % 100) == 0)) {

            // only if this chunk contains objects (even one), add it to the makefile
            // otherwise, clear it and continue collecting
            if(numOfObjectsInCurrentChunk) {
                curChunk.Prepend(wxString() << "Objects" << objCounter << "=");
                curChunk << "\n\n";
                text << curChunk;
                objCounter++;
            }

            // start a new chunk
            curChunk.clear();
            numOfObjectsInCurrentChunk = 0;
        }

        // is this a valid file?
        if(!cmp->GetCmpFileType(files[i].GetExt(), ft)) {
            continue;
        }

        if(ft.kind == Compiler::CmpFileKindResource && !OS_WINDOWS) {
            continue;
        }

        wxString objPrefix = DoGetTargetPrefix(files.at(i), projectPath, cmp);
        curChunk << "$(IntermediateDirectory)\\" << objPrefix << files[i].GetFullName() << "$(ObjectSuffix) ";

        // for readability, break every 10 objects and start a new line
        if(counter % 10 == 0) {
            curChunk << "\\\n\t";
        }
        counter++;
        numOfObjectsInCurrentChunk++;
    }

    // Add any leftovers...
    if(numOfObjectsInCurrentChunk) {
        curChunk.Prepend(wxString() << "Objects" << objCounter << "=");
        curChunk << "\n\n";
        text << curChunk;
        objCounter++;
    }

    text << "\n\nObjects=";
    if(m_hasObjectPCH) {
        text << "$(ObjectPCH) ";
    }
    for(size_t i = 0; i < objCounter; ++i)
        text << "$(Objects" << i << ") ";

    text << "\n\n";
    m_objectChunks = objCounter;
}

void BuilderNMake::CreateFileTargets(ProjectPtr proj, const wxString& confToBuild, wxString& text)
{
    // get the project specific build configuration for the workspace active
    // configuration
    BuildConfigPtr bldConf = clCxxWorkspaceST::Get()->GetProjBuildConf(proj->GetName(), confToBuild);
    wxString cmpType = bldConf->GetCompilerType();
    // get the compiler settings
    CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(cmpType);
    bool supportPreprocessOnlyFiles =
        !cmp->GetSwitch("PreprocessOnly").IsEmpty() && !cmp->GetPreprocessSuffix().IsEmpty();

    std::vector<wxFileName> abs_files, rel_paths;

    abs_files.reserve(m_allFiles.size());
    rel_paths.reserve(m_allFiles.size());

    clProjectFile::Vec_t::const_iterator iterFile = m_allFiles.begin();
    for(; iterFile != m_allFiles.end(); ++iterFile) {
        // Include only files that don't have the 'exclude from build' flag set
        if(!(*iterFile)->IsExcludeFromConfiguration(confToBuild)) {
            abs_files.push_back(wxFileName((*iterFile)->GetFilename()));
            rel_paths.push_back(wxFileName((*iterFile)->GetFilenameRelpath()));
        }
    }

    text << "\n\n";
    // create rule per object
    text << "##\n";
    text << "## Objects\n";
    text << "##\n";

    Compiler::CmpFileTypeInfo ft;

    // Collect all the sub-directories that we generate files for
    wxArrayString subDirs;

    wxString cwd = proj->GetFileName().GetPath();

    for(size_t i = 0; i < abs_files.size(); i++) {
        // is this file interests the compiler?
        if(cmp->GetCmpFileType(abs_files.at(i).GetExt().Lower(), ft)) {
            wxString absFileName;
            wxFileName fn(abs_files.at(i));

            wxString filenameOnly = fn.GetName();
            wxString fullpathOnly = fn.GetFullPath();
            wxString fullnameOnly = fn.GetFullName();
            wxString compilationLine = ft.compilation_line;

            // use UNIX style slashes
            absFileName = abs_files[i].GetFullPath();
            // absFileName.Replace("\\", "/");
            wxString relPath;

            relPath = rel_paths.at(i).GetPath(true, wxPATH_UNIX);
            relPath.Trim().Trim(false);

            wxString objPrefix = DoGetTargetPrefix(abs_files.at(i), cwd, cmp);

            compilationLine.Replace("$(FileName)", filenameOnly);
            compilationLine.Replace("$(FileFullName)", fullnameOnly);
            compilationLine.Replace("$(FileFullPath)", fullpathOnly);
            compilationLine.Replace("$(FilePath)", relPath);

            // The object name is handled differently when using resource files
            compilationLine.Replace("$(ObjectName)", objPrefix + fullnameOnly);
            // compilationLine.Replace("\\", "/");

            if(ft.kind == Compiler::CmpFileKindSource) {
                wxString objectName;
                wxString dependFile;
                wxString preprocessedFile;

                bool isCFile = FileExtManager::GetType(rel_paths.at(i).GetFullName()) == FileExtManager::TypeSourceC;

                objectName << "$(IntermediateDirectory)\\" << objPrefix << fullnameOnly << "$(ObjectSuffix)";
                if(supportPreprocessOnlyFiles) {
                    preprocessedFile << "$(IntermediateDirectory)\\" << objPrefix << fullnameOnly
                                     << "$(PreprocessSuffix)";
                }

                if(!isCFile) {
                    // Add the PCH include line
                    compilationLine.Replace("$(CXX)", "$(CXX) $(PCHUseFlags)");
                }

                // set the file rule
                text << objectName << ": " << rel_paths.at(i).GetFullPath(wxPATH_WIN) << " " << dependFile << "\n";
                text << "\t" << compilationLine << "\n";

                wxString cmpOptions("$(CXXFLAGS) $(PCHUseFlags)");
                if(isCFile) {
                    cmpOptions = "$(CFLAGS)";
                }

                // set the source file we want to compile
                wxString source_file_to_compile = rel_paths.at(i).GetFullPath(wxPATH_WIN);

                wxString compilerMacro = DoGetCompilerMacro(rel_paths.at(i).GetFullPath(wxPATH_WIN));
                if(supportPreprocessOnlyFiles) {
                    text << preprocessedFile << ": " << rel_paths.at(i).GetFullPath(wxPATH_UNIX) << "\n";
                    text << "\t" << compilerMacro << " " << cmpOptions << " $(IncludePath) $(PreprocessOnlySwitch) "
                         << preprocessedFile << " \"" << source_file_to_compile << "\"\n\n";
                }

            } else if(ft.kind == Compiler::CmpFileKindResource && OS_WINDOWS) {
                // we construct an object name which also includes the full name of the reousrce file and appends a .o
                // to the name (to be more
                // precised, $(ObjectSuffix))
                wxString objectName;
                objectName << "$(IntermediateDirectory)\\" << objPrefix << fullnameOnly << "$(ObjectSuffix)";

                text << objectName << ": " << rel_paths.at(i).GetFullPath(wxPATH_WIN) << "\n";
                text << "\t" << compilationLine << "\n";
            }
        }
    }
}

void BuilderNMake::CreateCleanTargets(ProjectPtr proj, const wxString& confToBuild, wxString& text)
{
    // get the project specific build configuration for the workspace active
    // configuration
    BuildConfigPtr bldConf = clCxxWorkspaceST::Get()->GetProjBuildConf(proj->GetName(), confToBuild);
    wxString cmpType = bldConf->GetCompilerType();
    // get the compiler settings
    CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(cmpType);

    // Can we use asterisk in the clean command?
    wxString imd = GetIntermediateDirectory(proj, bldConf);

    // add clean target
    text << "##\n";
    text << "## Clean\n";
    text << "##\n";
    text << "clean:\n";

    wxString cwd = proj->GetFileName().GetPath();
    if(!imd.IsEmpty()) {
        // Remove the entire build folder
        text << "\t"
             << "@if exist \"$(IntermediateDirectory)\" rmdir /S /Q \"$(IntermediateDirectory)\""
             << "\n";

        // Remove the pre-compiled header
        wxString pchFile = bldConf->GetPrecompiledHeader();
        pchFile.Trim().Trim(false);

        if(pchFile.IsEmpty() == false && (bldConf->GetPCHFlagsPolicy() != BuildConfig::kPCHJustInclude)) {
            text << "\t"
                 << "@del /Q " << pchFile << ".pch"
                 << "\n";
            text << "\t"
                 << "@del /Q " << pchFile << "$(ObjectSuffix)"
                 << "\n";
        }

    } else if(OS_WINDOWS) {
        text << "\t"
             << "@del /Q *$(ObjectSuffix)"
             << "\n";
        text << "\t"
             << "@del /Q *$(DependSuffix)"
             << "\n";
        // delete the output file as well
        text << "\t"
             << "@del /Q "
             << "$(OutputFile)"
             << "\n";
        text << "\t"
             << "@del /Q " << DoGetMarkerFileDir(proj->GetName(), proj->GetFileName().GetPath()) << "\n";

        // Remove the pre-compiled header
        wxString pchFile = bldConf->GetPrecompiledHeader();
        pchFile.Trim().Trim(false);

        if(pchFile.IsEmpty() == false && (bldConf->GetPCHFlagsPolicy() != BuildConfig::kPCHJustInclude)) {
            text << "\t"
                 << "@del /Q " << pchFile << ".pch"
                 << "\n";
            text << "\t"
                 << "@del /Q " << pchFile << "$(ObjectSuffix)"
                 << "\n";
        }
    }
    text << "\n\n";
}

void BuilderNMake::CreateListMacros(ProjectPtr proj, const wxString& confToBuild, wxString& text)
{
    // create a list of objects
    CreateObjectList(proj, confToBuild, text);
}

void BuilderNMake::CreateLinkTargets(const wxString& type, BuildConfigPtr bldConf, wxString& text, wxString& targetName,
                                     const wxString& projName, const wxArrayString& depsProj)
{

    wxString extraDeps;
    wxString depsRules;

    wxString errMsg;
    ProjectPtr proj = clCxxWorkspaceST::Get()->FindProjectByName(projName, errMsg);

    for(size_t i = 0; i < depsProj.GetCount(); i++) {
        wxFileName fn(depsProj.Item(i));
        fn.MakeRelativeTo(proj->GetProjectPath());
        extraDeps << "\"" << fn.GetFullPath() << "\" ";

        depsRules << "\"" << fn.GetFullPath() << "\":\n";
        // Make sure the dependecy directory exists
        depsRules << "\t@$(MakeDirCommand) \"" << fn.GetPath() << "\"\n";
        depsRules << "\t@echo stam > "
                  << "\"" << fn.GetFullPath() << "\"\n";
        depsRules << "\n\n";
    }

    if(type == PROJECT_TYPE_EXECUTABLE || type == PROJECT_TYPE_DYNAMIC_LIBRARY) {
        text << "all: ";
        text << "$(OutputFile)\n\n";

        text << "$(OutputFile): $(IntermediateDirectory)\\.d ";
        if(extraDeps.IsEmpty() == false) {
            text << extraDeps;
        }
        text << "$(Objects) \n";
        targetName = "$(IntermediateDirectory)\\.d";

    } else {
        text << "all: $(IntermediateDirectory) ";
        text << "$(OutputFile)\n\n";
        text << "$(OutputFile): $(Objects)\n";
    }

    if(bldConf->IsLinkerRequired()) {
        CreateTargets(type, bldConf, text, projName);

        if(type == PROJECT_TYPE_EXECUTABLE || type == PROJECT_TYPE_DYNAMIC_LIBRARY) {
            if(depsRules.IsEmpty() == false) {
                text << "\n" << depsRules << "\n";
            }
        }
    }
    // incase project is type exe or dll, force link
    // this is to workaround bug in the generated makefiles
    // which causes the makefile to report 'nothing to be done'
    // even when a dependency was modified
    text << ".PHONY: clean PrePreBuild PreBuild MakeIntermediateDirs all PostBuild \n";
}

void BuilderNMake::CreateTargets(const wxString& type, BuildConfigPtr bldConf, wxString& text, const wxString& projName)
{
    bool markRebuilt(true);
    text << "\t@$(MakeDirCommand) $(@D)\n";
    text << "\t@echo \"\" > $(IntermediateDirectory)\\.d\n";

    CompilerPtr cmp = bldConf->GetCompiler();

    // this is a special target that creates a file with the content of the
    // $(Objects) variable (to be used with the @<file-name> option of the LINK)
    if(m_hasObjectPCH) {
        text << "\t@echo $(ObjectPCH) > $(ObjectsFileList)\n";
    }
    for(size_t i = 0; i < m_objectChunks; ++i) {
        wxString oper = ">>";
        if(i == 0 && !m_hasObjectPCH) {
            oper = " >";
        }
        text << "\t@echo $(Objects" << i << ") " << oper << " $(ObjectsFileList)\n";
    }

    if(type == PROJECT_TYPE_STATIC_LIBRARY) {
        // create a static library
        // In any case add the 'objects_file' target here
        text << "\t"
             << "$(AR) $(ArchiveOutputSwitch)$(OutputFile)";
        if(cmp && cmp->GetReadObjectFilesFromList()) {
            text << " @$(ObjectsFileList) $(ArLibs)\n";
        } else {
            text << " $(Objects) $(ArLibs)\n";
        }

    } else if(type == PROJECT_TYPE_DYNAMIC_LIBRARY) {
        // create a shared library
        text << "\t"
             << "$(SharedObjectLinkerName) $(OutputSwitch)$(OutputFile)";
        if(cmp && cmp->GetReadObjectFilesFromList()) {
            text << " @$(ObjectsFileList) ";
        } else {
            text << " $(Objects) ";
        }
        text << "$(LibPath) $(Libs) $(LinkOptions)\n";

    } else if(type == PROJECT_TYPE_EXECUTABLE) {
        // create an executable
        text << "\t"
             << "$(LinkerName) $(OutputSwitch)$(OutputFile)";
        if(cmp && cmp->GetReadObjectFilesFromList()) {
            text << " @$(ObjectsFileList) ";
        } else {
            text << " $(Objects) ";
        }
        text << "$(LibPath) $(Libs) $(LinkOptions)\n";
        markRebuilt = false;
    }

    // If a link occurred, mark this project as "rebuilt" so the parent project will
    // know that a re-link is required
    if(bldConf->IsLinkerRequired() && markRebuilt) {
        text << "\t@$(MakeDirCommand) \"" << DoGetMarkerFileDir(wxEmptyString) << "\"\n";

        text << "\t@echo rebuilt > " << DoGetMarkerFileDir(projName) << "\n";
    }
}

void BuilderNMake::CreatePostBuildEvents(ProjectPtr proj, BuildConfigPtr bldConf, wxString& text)
{
    if(!HasPostbuildCommands(bldConf)) {
        return;
    }

    // generate postbuild commands
    BuildCommandList cmds;
    bldConf->GetPostBuildCommands(cmds);

    // Loop over the commands and replace any macros
    std::for_each(cmds.begin(), cmds.end(), [&](BuildCommand& cmd) {
        cmd.SetCommand(
            MacroManager::Instance()->Expand(cmd.GetCommand(), clGetManager(), proj->GetName(), bldConf->GetName()));
    });

    text << "\n";
    text << "PostBuild:\n";
    text << "\t@echo Executing Post Build commands ...\n";

    BuildCommandList::const_iterator iter = cmds.begin();
    for(; iter != cmds.end(); iter++) {
        if(iter->GetEnabled()) {
            // HACK:
            // If the command is 'copy' under Windows, make sure that
            // we set all slashes to backward slashes
            wxString command = iter->GetCommand();
            command.Trim().Trim(false);
            if(OS_WINDOWS && command.StartsWith("copy")) {
                command.Replace("/", "\\");
            }

            if(OS_WINDOWS && command.EndsWith("\\")) {
                command.RemoveLast();
            }

            text << "\t" << command << "\n";
        }
    }
    text << "\t@echo Done\n";
}

bool BuilderNMake::HasPrebuildCommands(BuildConfigPtr bldConf) const
{
    BuildCommandList cmds;
    bldConf->GetPreBuildCommands(cmds);

    BuildCommandList::const_iterator iter = cmds.begin();
    for(; iter != cmds.end(); iter++) {
        if(iter->GetEnabled()) {
            return true;
        }
    }
    return false;
}

void BuilderNMake::CreatePreBuildEvents(ProjectPtr proj, BuildConfigPtr bldConf, wxString& text)
{
    BuildCommandList cmds;
    BuildCommandList::iterator iter;
    wxString name = bldConf->GetName();
    name = NormalizeConfigName(name);

    // add PrePreBuild
    wxString preprebuild = bldConf->GetPreBuildCustom();
    preprebuild.Trim().Trim(false);
    if(preprebuild.IsEmpty() == false) {
        text << "PrePreBuild: ";
        text << bldConf->GetPreBuildCustom() << "\n";
    }
    text << "\n";
    bldConf->GetPreBuildCommands(cmds);

    // Loop over the commands and replace any macros
    std::for_each(cmds.begin(), cmds.end(), [&](BuildCommand& cmd) {
        cmd.SetCommand(
            MacroManager::Instance()->Expand(cmd.GetCommand(), clGetManager(), proj->GetName(), bldConf->GetName()));
    });

    bool first(true);
    text << "PreBuild:\n";
    if(!cmds.empty()) {
        iter = cmds.begin();
        for(; iter != cmds.end(); iter++) {
            if(iter->GetEnabled()) {
                if(first) {
                    text << "\t@echo Executing Pre Build commands ...\n";
                    first = false;
                }
                text << "\t" << iter->GetCommand() << "\n";
            }
        }
        if(!first) {
            text << "\t@echo Done\n";
        }
    }
}

void BuilderNMake::CreateConfigsVariables(ProjectPtr proj, BuildConfigPtr bldConf, wxString& text)
{
    wxString name = bldConf->GetName();
    name = NormalizeConfigName(name);

    wxString cmpType = bldConf->GetCompilerType();
    CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(cmpType);

    wxString objectsFileName(proj->GetFileName().GetPath());
    objectsFileName << PATH_SEP << proj->GetName() << ".txt";

    text << "## " << name << "\n";

    wxString outputFile = bldConf->GetOutputFileName();
    if(OS_WINDOWS && (bldConf->GetProjectType() == PROJECT_TYPE_EXECUTABLE || bldConf->GetProjectType().IsEmpty())) {
        outputFile.Trim().Trim(false);
    }

    // Expand the build macros into the generated makefile
    wxString workspacePath = clCxxWorkspaceST::Get()->GetWorkspaceFileName().GetPath();
    wxString projectPath = proj->GetFileName().GetPath();
    wxString startupDir = clCxxWorkspaceST::Get()->GetStartupDir();
    workspacePath.Replace("/", "\\");
    projectPath.Replace("/", "\\");
    startupDir.Replace("/", "\\");

    // We use relative path in $(IntermediateDirectory) and $(OutputFile)
    // to reduce issues with path containing spaces
    wxString intermediateDir = GetIntermediateDirectory(proj, bldConf);

    wxString outputDir = bldConf->GetOutputDirectory();
    outputDir.Replace("$(WorkspacePath)", workspacePath);
    outputDir.Replace("$(ProjectPath)", projectPath);
    outputDir.Replace("$(IntermediateDirectory)", intermediateDir);
    outputDir.Replace("/", "\\");
    wxFileName fnOutputFile(outputDir, outputFile.AfterLast('/'));
    if(fnOutputFile.IsAbsolute()) {
        fnOutputFile.MakeRelativeTo(projectPath);
    }
    outputFile = fnOutputFile.GetFullPath();
    outputFile.Replace("/", "\\");

    text << "ProjectName            =" << proj->GetName() << "\n";
    text << "ConfigurationName      =" << name << "\n";
    text << "WorkspaceConfiguration =" << clCxxWorkspaceST::Get()->GetSelectedConfig()->GetName() << "\n";
    text << "WorkspacePath          =" << ::WrapWithQuotes(workspacePath) << "\n";
    text << "ProjectPath            =" << ::WrapWithQuotes(projectPath) << "\n";
    text << "IntermediateDirectory  =" << intermediateDir << "\n";
    text << "OutDir                 =$(IntermediateDirectory)\n";
    text << "CurrentFileName        =\n"; // TODO:: Need implementation
    text << "CurrentFilePath        =\n"; // TODO:: Need implementation
    text << "CurrentFileFullPath    =\n"; // TODO:: Need implementation
    text << "User                   =" << wxGetUserName() << "\n";
    text << "Date                   =" << wxDateTime::Now().FormatDate() << "\n";
    text << "CodeLitePath           =" << ::WrapWithQuotes(startupDir) << "\n";
    // replace all occurence of forward slash with double backslash
    wxString linkerStr(cmp->GetTool("LinkerName"));
    if(OS_WINDOWS) {
        linkerStr = ReplaceBackslashes(linkerStr);
    }
    text << "LinkerName             =" << linkerStr << "\n";
    wxString shObjlinkerStr(cmp->GetTool("SharedObjectLinkerName"));
    if(OS_WINDOWS) {
        shObjlinkerStr = ReplaceBackslashes(shObjlinkerStr);
    }

    text << "SharedObjectLinkerName =" << shObjlinkerStr << "\n";
    text << "ObjectSuffix           =" << cmp->GetObjectSuffix() << "\n";
    text << "DependSuffix           =" << cmp->GetDependSuffix() << "\n";
    text << "PreprocessSuffix       =" << cmp->GetPreprocessSuffix() << "\n";
    text << "DebugSwitch            =" << cmp->GetSwitch("Debug") << "\n";
    text << "IncludeSwitch          =" << cmp->GetSwitch("Include") << "\n";
    text << "LibrarySwitch          =" << cmp->GetSwitch("Library") << "\n";
    text << "OutputSwitch           =" << cmp->GetSwitch("Output") << "\n";
    text << "LibraryPathSwitch      =" << cmp->GetSwitch("LibraryPath") << "\n";
    text << "PreprocessorSwitch     =" << cmp->GetSwitch("Preprocessor") << "\n";
    text << "SourceSwitch           =" << cmp->GetSwitch("Source") << "\n";
    text << "OutputDirectory        =" << outputDir << "\n";
    text << "OutputFile             =" << outputFile << "\n";
    text << "Preprocessors          =" << ParsePreprocessor(bldConf->GetPreprocessor()) << "\n";
    text << "ObjectSwitch           =" << cmp->GetSwitch("Object") << "\n";
    text << "ArchiveOutputSwitch    =" << cmp->GetSwitch("ArchiveOutput") << "\n";
    text << "PreprocessOnlySwitch   =" << cmp->GetSwitch("PreprocessOnly") << "\n";

    wxFileName fnObjectsFileName(objectsFileName);
    fnObjectsFileName.MakeRelativeTo(proj->GetFileName().GetPath());

    text << "ObjectsFileList        =\"" << fnObjectsFileName.GetFullPath() << "\"\n";

    wxString mkdirCommand = cmp->GetTool("MakeDirCommand");
    if(!mkdirCommand.IsEmpty()) {
        // use the compiler defined one
        if(OS_WINDOWS) {
            mkdirCommand = ReplaceBackslashes(mkdirCommand);
        }
        text << "MakeDirCommand         =" << mkdirCommand << "\n";
    } else {
        text << "MakeDirCommand         ="
             << "makedir"
             << "\n";
    }

    wxString buildOpts = bldConf->GetCompileOptions();
    buildOpts.Replace(";", " ");

    wxString cBuildOpts = bldConf->GetCCompileOptions();
    cBuildOpts.Replace(";", " ");

    wxString asOptions = bldConf->GetAssmeblerOptions();
    asOptions.Replace(";", " ");

    // Let the plugins add their content here
    clBuildEvent e(wxEVT_GET_ADDITIONAL_COMPILEFLAGS);
    e.SetProjectName(proj->GetName());
    e.SetConfigurationName(bldConf->GetName());
    EventNotifier::Get()->ProcessEvent(e);

    wxString additionalCompileFlags = e.GetCommand();
    if(additionalCompileFlags.IsEmpty() == false) {
        buildOpts << " " << additionalCompileFlags;
        cBuildOpts << " " << additionalCompileFlags;
    }

    // only if resource compiler required, evaluate the resource variables
    if(OS_WINDOWS) {
        wxString rcBuildOpts = bldConf->GetResCompileOptions();
        rcBuildOpts.Replace(";", " ");
        text << "RcCmpOptions           =" << rcBuildOpts << "\n";
        wxString rcCompilerstr(cmp->GetTool("ResourceCompiler"));
        rcCompilerstr.Replace("/", "\\");
        text << "RcCompilerName         =" << rcCompilerstr << "\n";
    }

    wxString linkOpt = bldConf->GetLinkOptions();
    linkOpt.Replace(";", " ");

    // link options are kept with semi-colons, strip them
    text << "LinkOptions            = " << linkOpt << "\n";

    // Precompiled header flags (compile, use and implicit include)
    wxString pchFile = bldConf->GetPrecompiledHeader();
    wxString pchCreateFlags, pchUseFlags;
    pchFile.Trim().Trim(false);
    if(pchFile.IsEmpty() == false) {
        if(bldConf->GetPCHFlagsPolicy() != BuildConfig::kPCHJustInclude) {
            pchCreateFlags << "/Yc ";
            pchCreateFlags << "/Fp" << pchFile << ".pch ";
            pchCreateFlags << "$(ObjectSwitch)" << pchFile << "$(ObjectSuffix)";
            pchUseFlags << "/Yu" << pchFile << " ";
            pchUseFlags << "/Fp" << pchFile << ".pch";
            // Append PCH compile flags, if any
            if(!bldConf->GetPchCompileFlags().IsEmpty()) {
                pchCreateFlags << " " << bldConf->GetPchCompileFlags();
            }
        }
        // If the PCH is required to be in the command line, add it here
        // otherwise, we just make sure it is generated and the compiler will pick it by itself
        if(bldConf->GetPchInCommandLine()) {
            pchUseFlags << " /FI" << pchFile;
        }
    }

    text << "PCHCreateFlags         =" << pchCreateFlags << "\n";
    text << "PCHUseFlags            =" << pchUseFlags << "\n";

    // add the global include path followed by the project include path
    wxString libraries = bldConf->GetLibraries();
    wxArrayString libsArr = ::wxStringTokenize(libraries, ";", wxTOKEN_STRTOK);
    libraries.Clear();
    libraries << " ";
    for(size_t i = 0; i < libsArr.GetCount(); i++) {
        libsArr.Item(i).Trim().Trim(false);
        libraries << "\"" << libsArr.Item(i) << "\" ";
    }
    libraries.Replace("/", "\\");

    text << "IncludePath            = "
         << ParseIncludePath(cmp->GetGlobalIncludePath(), proj->GetName(), bldConf->GetName()) << " "
         << ParseIncludePath(bldConf->GetIncludePath(), proj->GetName(), bldConf->GetName()) << "\n";
    text << "RcIncludePath          = "
         << ParseIncludePath(bldConf->GetResCmpIncludePath(), proj->GetName(), bldConf->GetName()) << "\n";
    text << "Libs                   = " << ParseLibs(bldConf->GetLibraries()) << "\n";
    text << "ArLibs                 = " << libraries << "\n";

    // add the global library path followed by the project library path
    text << "LibPath                =" << ParseLibPath(cmp->GetGlobalLibPath(), proj->GetName(), bldConf->GetName())
         << " " << ParseLibPath(bldConf->GetLibPath(), proj->GetName(), bldConf->GetName()) << "\n";

    text << "\n";
    text << "##\n";
    text << "## Common variables\n";
    text << "## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overridden using an environment variable\n";
    text << "##\n";
    wxString arStr(cmp->GetTool("AR"));
    wxString cxxStr(cmp->GetTool("CXX"));
    wxString ccStr(cmp->GetTool("CC"));
    wxString asStr(cmp->GetTool("AS"));

    if(OS_WINDOWS) {
        arStr = ReplaceBackslashes(arStr);
        cxxStr = ReplaceBackslashes(cxxStr);
        ccStr = ReplaceBackslashes(ccStr);
        asStr = ReplaceBackslashes(asStr);
    }

    text << "AR       = " << arStr << "\n";
    text << "CXX      = " << cxxStr << "\n";
    text << "CC       = " << ccStr << "\n";
    text << "CXXFLAGS = " << buildOpts << " $(Preprocessors)"
         << "\n";
    text << "CFLAGS   = " << cBuildOpts << " $(Preprocessors)"
         << "\n";
    text << "ASFLAGS  = " << asOptions << "\n";
    text << "AS       = " << asStr << "\n";
    text << "\n\n";
}

wxString BuilderNMake::ParseIncludePath(const wxString& paths, const wxString& projectName, const wxString& selConf)
{
    // convert semi-colon delimited string into GNU list of
    // include paths:
    wxString incluedPath(wxEmptyString);
    wxStringTokenizer tkz(paths, ";", wxTOKEN_STRTOK);
    // prepend each include path with -I
    while(tkz.HasMoreTokens()) {
        wxString path(tkz.NextToken());
        TrimString(path);
        if(path.EndsWith("/") || path.EndsWith("\\")) {
            path.RemoveLast();
        }
        // path.Replace("\\", "/");

        wxString wrapper;
        if(path.Contains(" ")) {
            wrapper = "\"";
        }
        incluedPath << "$(IncludeSwitch)" << wrapper << path << wrapper << " ";
    }
    return incluedPath;
}

wxString BuilderNMake::ParseLibPath(const wxString& paths, const wxString& projectName, const wxString& selConf)
{
    // convert semi-colon delimited string into GNU list of
    // lib path
    wxString libPath(wxEmptyString);
    wxStringTokenizer tkz(paths, ";", wxTOKEN_STRTOK);
    // prepend each include path with libpath switch
    while(tkz.HasMoreTokens()) {
        wxString path(tkz.NextToken());
        path.Trim().Trim(false);
        if(path.EndsWith("/") || path.EndsWith("\\")) {
            path.RemoveLast();
        }
        // path.Replace("\\", "/");
        wxString wrapper;
        if(path.Contains(" ")) {
            wrapper = "\"";
        }
        libPath << "$(LibraryPathSwitch)" << wrapper << path << wrapper << " ";
    }
    return libPath;
}

wxString BuilderNMake::ParsePreprocessor(const wxString& prep)
{
    wxString preprocessor(wxEmptyString);
    wxArrayString tokens = StringUtils::BuildArgv(prep);
    // prepend each include path with libpath switch
    for(wxString& p : tokens) {
        p.Trim().Trim(false);
        preprocessor << "$(PreprocessorSwitch)" << p << " ";
    }

    // if the macro contains # escape it
    // But first remove any manual escaping done by the user
    preprocessor.Replace("\\#", "#");
    preprocessor.Replace("#", "\\#");
    return preprocessor;
}

wxString BuilderNMake::ParseLibs(const wxString& libs)
{
    // convert semi-colon delimited string into GNU list of
    // libs
    wxString slibs(wxEmptyString);
    wxStringTokenizer tkz(libs, ";", wxTOKEN_STRTOK);
    // prepend each include path with -l and strip trailing lib string
    // also, if the file contains an extension (.a, .so, .dynlib) remove them as well
    while(tkz.HasMoreTokens()) {
        wxString lib(tkz.NextToken());
        TrimString(lib);
        // remove lib prefix
        if(lib.StartsWith("lib")) {
            lib = lib.Mid(3);
        }

        // remove known suffixes
        if(lib.EndsWith(".a") || lib.EndsWith(".so") || lib.EndsWith(".dylib") || lib.EndsWith(".dll")) {
            lib = lib.BeforeLast('.');
        }

        slibs << "$(LibrarySwitch)" << lib << " ";
    }
    return slibs;
}

wxString BuilderNMake::GetBuildCommand(const wxString& project, const wxString& confToBuild, const wxString& arguments)
{
    wxString errMsg, cmd;
    BuildConfigPtr bldConf = clCxxWorkspaceST::Get()->GetProjBuildConf(project, confToBuild);
    if(!bldConf) {
        return wxEmptyString;
    }

    // generate the makefile
    Export(project, confToBuild, arguments, false, false, errMsg);

    wxString buildTool = GetBuildToolCommand(project, confToBuild, arguments, true);
    buildTool = EnvironmentConfig::Instance()->ExpandVariables(buildTool, true);

    // fix: replace all Windows like slashes to POSIX
    // buildTool.Replace("\\", "/");
    cmd << buildTool << " Makefile";
    return cmd;
}

wxString BuilderNMake::GetCleanCommand(const wxString& project, const wxString& confToBuild, const wxString& arguments)
{
    wxString errMsg, cmd;
    BuildConfigPtr bldConf = clCxxWorkspaceST::Get()->GetProjBuildConf(project, confToBuild);
    if(!bldConf) {
        return wxEmptyString;
    }

    // generate the makefile
    Export(project, confToBuild, arguments, false, false, errMsg);

    wxString buildTool = GetBuildToolCommand(project, confToBuild, arguments, true);
    buildTool = EnvironmentConfig::Instance()->ExpandVariables(buildTool, true);

    // fix: replace all Windows like slashes to POSIX
    // buildTool.Replace("\\", "/");

    BuildMatrixPtr matrix = clCxxWorkspaceST::Get()->GetBuildMatrix();
    wxString type = Builder::NormalizeConfigName(matrix->GetSelectedConfigurationName());
    cmd << buildTool << " Makefile clean";
    return cmd;
}

wxString BuilderNMake::GetPOBuildCommand(const wxString& project, const wxString& confToBuild,
                                         const wxString& arguments)
{
    wxString errMsg, cmd;
    ProjectPtr proj = clCxxWorkspaceST::Get()->FindProjectByName(project, errMsg);
    if(!proj) {
        return wxEmptyString;
    }

    // generate the makefile
    Export(project, confToBuild, arguments, true, false, errMsg);
    cmd = GetProjectMakeCommand(proj, confToBuild, "all", false, false);
    return cmd;
}

wxString BuilderNMake::GetPOCleanCommand(const wxString& project, const wxString& confToBuild,
                                         const wxString& arguments)
{
    wxString errMsg, cmd;
    ProjectPtr proj = clCxxWorkspaceST::Get()->FindProjectByName(project, errMsg);
    if(!proj) {
        return wxEmptyString;
    }

    // generate the makefile
    Export(project, confToBuild, arguments, true, false, errMsg);
    cmd = GetProjectMakeCommand(proj, confToBuild, "clean", false, true);
    return cmd;
}

wxString BuilderNMake::GetSingleFileCmd(const wxString& project, const wxString& confToBuild, const wxString& arguments,
                                        const wxString& fileName)
{
    wxString errMsg, cmd;
    ProjectPtr proj = clCxxWorkspaceST::Get()->FindProjectByName(project, errMsg);
    if(!proj) {
        return wxEmptyString;
    }

    // generate the makefile
    Export(project, confToBuild, arguments, true, false, errMsg);

    // Build the target list
    wxString target;
    wxString cmpType;
    wxFileName fn(fileName);

    BuildConfigPtr bldConf = clCxxWorkspaceST::Get()->GetProjBuildConf(project, confToBuild);
    if(!bldConf) {
        return wxEmptyString;
    }

    cmpType = bldConf->GetCompilerType();
    CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(cmpType);
    // fn.MakeRelativeTo(proj->GetFileName().GetPath());

    wxString relPath = fn.GetPath(true, wxPATH_UNIX);
    wxString objNamePrefix = DoGetTargetPrefix(fn, proj->GetFileName().GetPath(), cmp);
    target << GetIntermediateDirectory(proj, bldConf) << "\\" << objNamePrefix << fn.GetFullName()
           << cmp->GetObjectSuffix();

    target = ExpandAllVariables(target, clCxxWorkspaceST::Get(), proj->GetName(), confToBuild, wxEmptyString);
    target.Replace("/", "\\");
    cmd = GetProjectMakeCommand(proj, confToBuild, target, false, false);

    return EnvironmentConfig::Instance()->ExpandVariables(cmd, true);
}

wxString BuilderNMake::GetPreprocessFileCmd(const wxString& project, const wxString& confToBuild,
                                            const wxString& arguments, const wxString& fileName, wxString& errMsg)
{
    ProjectPtr proj = clCxxWorkspaceST::Get()->FindProjectByName(project, errMsg);
    if(!proj) {
        return wxEmptyString;
    }

    wxString cmd;
    BuildConfigPtr bldConf = clCxxWorkspaceST::Get()->GetProjBuildConf(project, confToBuild);
    if(!bldConf) {
        return wxEmptyString;
    }

    // generate the makefile
    Export(project, confToBuild, arguments, true, false, errMsg);

    BuildMatrixPtr matrix = clCxxWorkspaceST::Get()->GetBuildMatrix();
    wxString buildTool = GetBuildToolCommand(project, confToBuild, arguments, true);
    wxString type = matrix->GetProjectSelectedConf(matrix->GetSelectedConfigurationName(), project);

    // fix: replace all Windows like slashes to POSIX
    // buildTool.Replace("\\", "/");

    // create the target
    wxString target;
    wxString objSuffix;
    wxFileName fn(fileName);

    wxString cmpType = bldConf->GetCompilerType();
    CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(cmpType);

    wxString objNamePrefix = DoGetTargetPrefix(fn, proj->GetFileName().GetPath(), cmp);
    target << GetIntermediateDirectory(proj, bldConf) << "\\" << objNamePrefix << fn.GetFullName()
           << cmp->GetPreprocessSuffix();

    target = ExpandAllVariables(target, clCxxWorkspaceST::Get(), proj->GetName(), confToBuild, wxEmptyString);
    target.Replace("/", "\\");
    cmd = GetProjectMakeCommand(proj, confToBuild, target, false, false);
    return EnvironmentConfig::Instance()->ExpandVariables(cmd, true);
}

wxString BuilderNMake::GetCdCmd(const wxFileName& path1, const wxFileName& path2)
{
    wxString cd_cmd("@");
    if(path2.GetPath().IsEmpty()) {
        return cd_cmd;
    }

    if(path1.GetPath() != path2.GetPath()) {
        cd_cmd << "cd \"" << path2.GetPath() << "\" && ";
    }
    return cd_cmd;
}

void BuilderNMake::CreateCustomPostBuildEvents(BuildConfigPtr bldConf, wxString& text)
{
    BuildCommandList cmds;
    BuildCommandList::iterator iter;

    cmds.clear();
    bldConf->GetPostBuildCommands(cmds);
    bool first(true);
    if(!cmds.empty()) {
        iter = cmds.begin();
        for(; iter != cmds.end(); iter++) {
            if(iter->GetEnabled()) {
                if(first) {
                    text << "\t@echo Executing Post Build commands ...\n";
                    first = false;
                }
                text << "\t" << iter->GetCommand() << "\n";
            }
        }
        if(!first) {
            text << "\t@echo Done\n";
        }
    }
}

void BuilderNMake::CreateCustomPreBuildEvents(BuildConfigPtr bldConf, wxString& text)
{
    BuildCommandList cmds;
    BuildCommandList::iterator iter;

    cmds.clear();
    bldConf->GetPreBuildCommands(cmds);
    bool first(true);
    if(!cmds.empty()) {
        iter = cmds.begin();
        for(; iter != cmds.end(); iter++) {
            if(iter->GetEnabled()) {
                if(first) {
                    text << "\t@echo Executing Pre Build commands ...\n";
                    first = false;
                }
                text << "\t" << iter->GetCommand() << "\n";
            }
        }
        if(!first) {
            text << "\t@echo Done\n";
        }
    }
}

wxString BuilderNMake::GetProjectMakeCommand(const wxFileName& wspfile, const wxFileName& projectPath, ProjectPtr proj,
                                             const wxString& confToBuild)
{
    BuildConfigPtr bldConf = clCxxWorkspaceST::Get()->GetProjBuildConf(proj->GetName(), confToBuild);

    // iterate over the dependencies projects and generate makefile
    wxString makeCommand;
    wxString basicMakeCommand;

    wxString buildTool = GetBuildToolCommand(proj->GetName(), confToBuild, "", false);
    buildTool = EnvironmentConfig::Instance()->ExpandVariables(buildTool, true);
    basicMakeCommand << buildTool << " \"" << proj->GetName() << ".mk\"";

    makeCommand << "\t" << GetCdCmd(wspfile, projectPath);

    if(bldConf) {
        wxString preprebuild = bldConf->GetPreBuildCustom();
        wxString precmpheader = bldConf->GetPrecompiledHeader();
        precmpheader.Trim().Trim(false);
        preprebuild.Trim().Trim(false);

        if(preprebuild.IsEmpty() == false) {
            makeCommand << basicMakeCommand << " PrePreBuild && ";
        }

        if(HasPrebuildCommands(bldConf)) {
            makeCommand << basicMakeCommand << " PreBuild && ";
        }

        // Run pre-compiled header compilation if any
        if(precmpheader.IsEmpty() == false && (bldConf->GetPCHFlagsPolicy() != BuildConfig::kPCHJustInclude)) {
            makeCommand << basicMakeCommand << " " << precmpheader << ".pch"
                        << " && ";
        }
    }

    // the main command
    makeCommand << basicMakeCommand;

    // post
    if(bldConf && HasPostbuildCommands(bldConf)) {
        makeCommand << " && " << basicMakeCommand << " PostBuild";
    }
    makeCommand << "\n";
    return makeCommand;
}

wxString BuilderNMake::GetProjectMakeCommand(ProjectPtr proj, const wxString& confToBuild, const wxString& target,
                                             bool addCleanTarget, bool cleanOnly)
{
    BuildConfigPtr bldConf = clCxxWorkspaceST::Get()->GetProjBuildConf(proj->GetName(), confToBuild);

    // iterate over the dependencies projects and generate makefile
    wxString makeCommand;
    wxString basicMakeCommand;

    wxString buildTool = GetBuildToolCommand(proj->GetName(), confToBuild, "", true);
    buildTool = EnvironmentConfig::Instance()->ExpandVariables(buildTool, true);
    basicMakeCommand << buildTool << " \"" << proj->GetName() << ".mk\" ";

    if(addCleanTarget) {
        makeCommand << basicMakeCommand << " clean && ";
    }

    if(bldConf && !cleanOnly) {
        wxString preprebuild = bldConf->GetPreBuildCustom();
        wxString precmpheader = bldConf->GetPrecompiledHeader();
        precmpheader.Trim().Trim(false);
        preprebuild.Trim().Trim(false);

        makeCommand << basicMakeCommand << " MakeIntermediateDirs && ";

        if(preprebuild.IsEmpty() == false) {
            makeCommand << basicMakeCommand << " PrePreBuild && ";
        }

        if(HasPrebuildCommands(bldConf)) {
            makeCommand << basicMakeCommand << " PreBuild && ";
        }

        // Run pre-compiled header compilation if any
        if(precmpheader.IsEmpty() == false && (bldConf->GetPCHFlagsPolicy() != BuildConfig::kPCHJustInclude)) {
            makeCommand << basicMakeCommand << " " << precmpheader << ".pch"
                        << " && ";
        }
    }

    makeCommand << basicMakeCommand << " " << target;

    // post
    if(bldConf && !cleanOnly && HasPostbuildCommands(bldConf)) {
        makeCommand << " && " << basicMakeCommand << " PostBuild";
    }
    return makeCommand;
}

void BuilderNMake::CreatePreCompiledHeaderTarget(BuildConfigPtr bldConf, wxString& text)
{
    wxString filename = bldConf->GetPrecompiledHeader();
    filename.Trim().Trim(false);

    if(filename.IsEmpty()) {
        return;
    }

    auto pchPolicy = bldConf->GetPCHFlagsPolicy();
    if(pchPolicy == BuildConfig::kPCHJustInclude) {
        // no need to add rule here
        return;
    }

    text << "\n";
    text << "# PreCompiled Header\n";
    text << filename << ".pch: " << filename << "\n";
    switch(pchPolicy) {
    case BuildConfig::kPCHPolicyReplace:
        text << "\t" << DoGetCompilerMacro(filename) << " $(SourceSwitch) " << filename << " $(PCHCreateFlags)\n";
        break;
    case BuildConfig::kPCHPolicyAppend:
        text << "\t" << DoGetCompilerMacro(filename) << " $(SourceSwitch) " << filename
             << " $(PCHCreateFlags) $(CXXFLAGS) $(IncludePath)\n";
        break;
    case BuildConfig::kPCHJustInclude:
        // for completeness
        break;
    }
    text << "\n";
}

wxString BuilderNMake::GetPORebuildCommand(const wxString& project, const wxString& confToBuild,
                                           const wxString& arguments)
{
    wxString errMsg, cmd;
    ProjectPtr proj = clCxxWorkspaceST::Get()->FindProjectByName(project, errMsg);
    if(!proj) {
        return wxEmptyString;
    }

    // generate the makefile
    Export(project, confToBuild, arguments, true, false, errMsg);
    cmd = GetProjectMakeCommand(proj, confToBuild, "all", true, false);
    return cmd;
}

wxString BuilderNMake::GetBuildToolCommand(const wxString& project, const wxString& confToBuild,
                                           const wxString& arguments, bool isCommandlineCommand) const
{
    BuildConfigPtr bldConf = clCxxWorkspaceST::Get()->GetProjBuildConf(project, confToBuild);
    if(!bldConf) {
        return wxEmptyString;
    }

    CompilerPtr compiler = bldConf->GetCompiler();
    if(!compiler) {
        return wxEmptyString;
    }

    if(isCommandlineCommand) {
        return compiler->GetTool("MAKE") + " /e /f ";

    } else {
        // $(MAKE) is already quoted by NMake
        return "$(MAKE) /nologo /f ";
    }
}

wxString BuilderNMake::DoGetCompilerMacro(const wxString& filename)
{
    wxString compilerMacro("$(CXX)");
    switch(FileExtManager::GetType(filename)) {
    case FileExtManager::TypeSourceC:
        compilerMacro = "$(CC)";
        break;
    case FileExtManager::TypeSourceCpp:
    default:
        compilerMacro = "$(CXX)";
        break;
    }
    return compilerMacro;
}

wxString BuilderNMake::DoGetTargetPrefix(const wxFileName& filename, const wxString& cwd, CompilerPtr cmp)
{
    size_t count = filename.GetDirCount();
    const wxArrayString& dirs = filename.GetDirs();
    wxString lastDir;

    if(cwd == filename.GetPath()) {
        return wxEmptyString;
    }

    if(cmp && cmp->GetObjectNameIdenticalToFileName()) {
        return wxEmptyString;
    }

    if(cwd == filename.GetPath()) {
        return wxEmptyString;
    }

    if(count) {
        lastDir = dirs.Item(count - 1);

        // Handle special directory paths
        if(lastDir == "..") {
            lastDir = "up";

        } else if(lastDir == ".") {
            lastDir = "cur";
        }

        if(lastDir.IsEmpty() == false) {
            lastDir << "_";
        }
    }

    return lastDir;
}

wxString BuilderNMake::DoGetMarkerFileDir(const wxString& projname, const wxString& projectPath)
{
    BuildMatrixPtr matrix = clCxxWorkspaceST::Get()->GetBuildMatrix();
    wxString workspaceSelConf = matrix->GetSelectedConfigurationName();

    workspaceSelConf = NormalizeConfigName(workspaceSelConf);
    workspaceSelConf.MakeLower();

    wxString path;
    if(projname.IsEmpty()) {
        path << clCxxWorkspaceST::Get()->GetWorkspaceFileName().GetPath() << "/"
             << ".build-" << workspaceSelConf;

    } else {
        path << clCxxWorkspaceST::Get()->GetWorkspaceFileName().GetPath() << "/"
             << ".build-" << workspaceSelConf << "/" << projname;
    }

    if(projectPath.IsEmpty() == false) {
        wxFileName fn(path);
        fn.MakeRelativeTo(projectPath);
        path = fn.GetFullPath(wxPATH_WIN);
    }

    if(!projname.IsEmpty()) {
        return "\"" + path + "\"";
    } else {
        return path;
    }
}

bool BuilderNMake::HasPostbuildCommands(BuildConfigPtr bldConf) const
{
    BuildCommandList cmds;
    bldConf->GetPostBuildCommands(cmds);

    BuildCommandList::const_iterator iter = cmds.begin();
    for(; iter != cmds.end(); iter++) {
        if(iter->GetEnabled()) {
            return true;
        }
    }
    return false;
}

bool BuilderNMake::SendBuildEvent(int eventId, const wxString& projectName, const wxString& configurationName)
{
    clBuildEvent e(eventId);
    e.SetProjectName(projectName);
    e.SetConfigurationName(configurationName);
    return EventNotifier::Get()->ProcessEvent(e);
}

Builder::OptimalBuildConfig BuilderNMake::GetOptimalBuildConfig(const wxString& projectType) const
{
    OptimalBuildConfig conf;
    conf.intermediateDirectory = "$(ProjectPath)/$(ConfigurationName)";
    conf.outputFile << "$(WorkspacePath)/$(WorkspaceConfiguration)/$(ProjectName)" << GetOutputFileSuffix(projectType);
    conf.command = "$(OutputFile)";
    return conf;
}
