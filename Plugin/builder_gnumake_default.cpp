//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : builder_gnumake.cpp
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
#include "builder_gnumake_default.h"

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

BuilderGnuMake::BuilderGnuMake()
    : Builder("CodeLite Makefile Generator")
    , m_objectChunks(1)
    , m_projectFilesMetadata(nullptr)
{
    m_isWindows = wxGetOsVersion() & wxOS_WINDOWS ? true : false;
}

BuilderGnuMake::BuilderGnuMake(const wxString& name, const wxString& buildTool, const wxString& buildToolOptions)
    : Builder(name)
    , m_projectFilesMetadata(nullptr)
{
    m_isWindows = wxGetOsVersion() & wxOS_WINDOWS ? true : false;
}

BuilderGnuMake::~BuilderGnuMake() {}

wxString BuilderGnuMake::MakeDir(const wxString& path)
{
    wxString d;
    wxString fixedPath = path;
    if(fixedPath.StartsWith("$") || fixedPath.Contains(" ") ||
       m_isWindows // HACK: windows mkdir accepts forward-slash if it was double-quoted
    ) {
        fixedPath.Prepend("\"").Append("\"");
    }

#ifdef __WXMSW__
    if(m_isWindows) {
        d << "@if not exist " << fixedPath << " $(MakeDirCommand) " << fixedPath;
    } else {
        fixedPath.Replace("\\", "/");
        d << "@$(MakeDirCommand) " << fixedPath;
    }
#else
    d << "@$(MakeDirCommand) " << fixedPath;
#endif
    return d;
}

wxString BuilderGnuMake::GetIntermediateDirectory(ProjectPtr proj, BuildConfigPtr bldConf) const
{
    wxString workspacePath = clCxxWorkspaceST::Get()->GetWorkspaceFileName().GetPath();
    wxString projectPath = proj->GetFileName().GetPath();
    wxString intermediateDir = bldConf->GetIntermediateDirectory();
    if(intermediateDir.IsEmpty()) {
        wxFileName projName = proj->GetFileName();
        projName.MakeRelativeTo(workspacePath);
        wxString projRel = projName.GetPath(wxPATH_NO_SEPARATOR);
        projRel.Replace(".", "_");
        projRel.Replace(" ", "_");
        intermediateDir << "$(WorkspacePath)/build-$(WorkspaceConfiguration)/" << projRel;
    }
    intermediateDir.Replace("$(WorkspacePath)", workspacePath);
    intermediateDir.Replace("$(ProjectPath)", projectPath);
    wxFileName fnIntermediateDir(intermediateDir, "");
    if(fnIntermediateDir.IsAbsolute()) {
        fnIntermediateDir.MakeRelativeTo(projectPath);
    }
    intermediateDir = fnIntermediateDir.GetPath(wxPATH_NO_SEPARATOR);
    intermediateDir.Replace("\\", "/");
    return intermediateDir;
}

bool BuilderGnuMake::Export(const wxString& project, const wxString& confToBuild, const wxString& arguments,
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

    clDEBUG() << "Generating Makefile..." << endl;
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

    text << ".PHONY: clean All\n\n";
    text << "All:\n";

    // iterate over the dependencies projects and generate makefile
    wxString buildTool = GetBuildToolCommand(project, confToBuild, arguments, false);
    buildTool = EnvironmentConfig::Instance()->ExpandVariables(buildTool, true);

    // fix: replace all Windows like slashes to POSIX
    buildTool.Replace("\\", "/");

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
                wxString dep_file = GetRelinkMarkerForProject(dependProj->GetName());
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

    // dump the content to file
    wxFileOutputStream output(fn);
    wxStringInputStream content(text);
    output << content;

    clDEBUG() << "Generating Makefile...is completed" << endl;
    return true;
}

void BuilderGnuMake::GenerateMakefile(ProjectPtr proj, const wxString& confToBuild, bool force,
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
    m_projectFilesMetadata = &(proj->GetFiles());

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
        text << name << ":=" << value << ""
             << "\n";
    }

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
    CreateMakeDirsTarget(targetName, text);
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

void BuilderGnuMake::CreateMakeDirsTarget(const wxString& targetName, wxString& text)
{
    text << "\n";
    text << "MakeIntermediateDirs:\n";
    text << "\t" << MakeDir("$(IntermediateDirectory)") << "\n";
    text << "\t" << MakeDir("$(OutputDirectory)") << "\n";

    text << "\n";
    text << targetName << ":\n";
    text << "\t" << MakeDir("$(IntermediateDirectory)") << "\n";
}

void BuilderGnuMake::CreateSrcList(ProjectPtr proj, const wxString& confToBuild, wxString& text)
{
    std::vector<wxFileName> files;

    std::for_each(m_projectFilesMetadata->begin(), m_projectFilesMetadata->end(),
                  [&](const Project::FilesMap_t::value_type& vt) {
                      clProjectFile::Ptr_t file = vt.second;
                      // Include only files that don't have the 'exclude from build' flag set
                      if(!file->IsExcludeFromConfiguration(confToBuild)) {
                          files.push_back(wxFileName(file->GetFilenameRelpath()));
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

        if(IsResourceFile(ft) && !HandleResourceFiles()) {
            continue;
        }

        relPath = files.at(i).GetPath(true, wxPATH_UNIX);
        relPath.Trim().Trim(false);
        text << relPath << files[i].GetFullName() << " ";

        if(counter % 10 == 0) {
            text << "\\\n\t";
        }
        counter++;
    }
    text << "\n\n";
}

void BuilderGnuMake::CreateObjectList(ProjectPtr proj, const wxString& confToBuild, wxString& text)
{
    m_objectChunks = 1;
    std::vector<wxFileName> files;

    std::for_each(m_projectFilesMetadata->begin(), m_projectFilesMetadata->end(),
                  [&](const Project::FilesMap_t::value_type& vt) {
                      clProjectFile::Ptr_t file = vt.second;
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

    wxString objectsList;
    size_t objCounter = 0;
    int numOfObjectsInCurrentChunk = 0;
    wxString curChunk;

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

        if(IsResourceFile(ft) && !HandleResourceFiles()) {
            continue;
        }

        wxString objPrefix = DoGetTargetPrefix(files.at(i), projectPath, cmp);
        curChunk << "$(IntermediateDirectory)/" << objPrefix << files[i].GetFullName() << "$(ObjectSuffix) ";

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
    for(size_t i = 0; i < objCounter; ++i)
        text << "$(Objects" << i << ") ";

    text << "\n\n";
    m_objectChunks = objCounter;
}

void BuilderGnuMake::CreateFileTargets(ProjectPtr proj, const wxString& confToBuild, wxString& text)
{
    // get the project specific build configuration for the workspace active
    // configuration
    BuildConfigPtr bldConf = clCxxWorkspaceST::Get()->GetProjBuildConf(proj->GetName(), confToBuild);
    wxString cmpType = bldConf->GetCompilerType();
    // get the compiler settings
    CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(cmpType);
    bool generateDependenciesFiles = cmp->GetGenerateDependeciesFile() && !cmp->GetDependSuffix().IsEmpty();
    bool supportPreprocessOnlyFiles =
        !cmp->GetSwitch("PreprocessOnly").IsEmpty() && !cmp->GetPreprocessSuffix().IsEmpty();

    std::vector<wxFileName> abs_files, rel_paths;

    abs_files.reserve(m_projectFilesMetadata->size());
    rel_paths.reserve(m_projectFilesMetadata->size());

    std::for_each(m_projectFilesMetadata->begin(), m_projectFilesMetadata->end(),
                  [&](const Project::FilesMap_t::value_type& vt) {
                      clProjectFile::Ptr_t file = vt.second;
                      // Include only files that don't have the 'exclude from build' flag set
                      if(!file->IsExcludeFromConfiguration(confToBuild)) {
                          abs_files.push_back(wxFileName(file->GetFilename()));
                          rel_paths.push_back(wxFileName(file->GetFilenameRelpath()));
                      }
                  });

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
            absFileName.Replace("\\", "/");
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
            compilationLine.Replace("\\", "/");

            if(ft.kind == Compiler::CmpFileKindSource) {
                wxString objectName;
                wxString dependFile;
                wxString preprocessedFile;

                bool isCFile = FileExtManager::GetType(rel_paths.at(i).GetFullName()) == FileExtManager::TypeSourceC;

                objectName << "$(IntermediateDirectory)/" << objPrefix << fullnameOnly << "$(ObjectSuffix)";
                if(generateDependenciesFiles) {
                    dependFile << "$(IntermediateDirectory)/" << objPrefix << fullnameOnly << "$(DependSuffix)";
                }
                if(supportPreprocessOnlyFiles) {
                    preprocessedFile << "$(IntermediateDirectory)/" << objPrefix << fullnameOnly
                                     << "$(PreprocessSuffix)";
                }

                if(!isCFile) {
                    // Add the PCH include line
                    compilationLine.Replace("$(CXX)", "$(CXX) $(IncludePCH)");
                }

                // set the file rule
                text << objectName << ": " << rel_paths.at(i).GetFullPath(wxPATH_UNIX) << " " << dependFile << "\n";
                text << "\t" << compilationLine << "\n";

                wxString cmpOptions("$(CXXFLAGS) $(IncludePCH)");
                if(isCFile) {
                    cmpOptions = "$(CFLAGS)";
                }

                // set the source file we want to compile
                wxString source_file_to_compile = rel_paths.at(i).GetFullPath(wxPATH_UNIX);
                ::WrapWithQuotes(source_file_to_compile);

                wxString compilerMacro = DoGetCompilerMacro(rel_paths.at(i).GetFullPath(wxPATH_UNIX));
                if(generateDependenciesFiles) {
                    text << dependFile << ": " << rel_paths.at(i).GetFullPath(wxPATH_UNIX) << "\n";
                    text << "\t"
                         << "@" << compilerMacro << " " << cmpOptions << " $(IncludePath) -MG -MP -MT" << objectName
                         << " -MF" << dependFile << " -MM " << source_file_to_compile << "\n\n";
                }

                if(supportPreprocessOnlyFiles) {
                    text << preprocessedFile << ": " << rel_paths.at(i).GetFullPath(wxPATH_UNIX) << "\n";
                    text << "\t" << compilerMacro << " " << cmpOptions
                         << " $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) " << preprocessedFile << " "
                         << source_file_to_compile << "\n\n";
                }

            } else if(IsResourceFile(ft) && HandleResourceFiles()) {
                // we construct an object name which also includes the full name of the reousrce file and appends a
                // .o to the name (to be more precised, $(ObjectSuffix))
                wxString objectName;
                objectName << "$(IntermediateDirectory)/" << objPrefix << fullnameOnly << "$(ObjectSuffix)";

                text << objectName << ": " << rel_paths.at(i).GetFullPath(wxPATH_UNIX) << "\n";
                text << "\t" << compilationLine << "\n";
            }
        }
    }

    if(generateDependenciesFiles) {
        text << "\n";
        text << "-include $(IntermediateDirectory)/*$(DependSuffix)\n";
    }
}

void BuilderGnuMake::CreateCleanTargets(ProjectPtr proj, const wxString& confToBuild, wxString& text)
{
    // get the project specific build configuration for the workspace active
    // configuration
    BuildConfigPtr bldConf = clCxxWorkspaceST::Get()->GetProjBuildConf(proj->GetName(), confToBuild);
    wxString cmpType = bldConf->GetCompilerType();
    // get the compiler settings
    CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(cmpType);

    // add clean target
    text << "##\n";
    text << "## Clean\n";
    text << "##\n";
    text << "clean:\n";

    // Remove the entire build folder
    text << "\t"
         << "$(RM) -r $(IntermediateDirectory)"
         << "\n";

    // Remove the pre-compiled header
    wxString pchFile = bldConf->GetPrecompiledHeader();
    pchFile.Trim().Trim(false);
    if(pchFile.IsEmpty() == false && (bldConf->GetPCHFlagsPolicy() != BuildConfig::kPCHJustInclude)) {
        text << "\t"
             << "$(RM) " << pchFile << ".gch"
             << "\n";
    }
    text << "\n\n";
}

void BuilderGnuMake::CreateListMacros(ProjectPtr proj, const wxString& confToBuild, wxString& text)
{
    // create a list of Sources
    // CreateSrcList(proj, confToBuild, text);  // Not used/needed for multi-step build
    // create a list of objects
    CreateObjectList(proj, confToBuild, text);
}

void BuilderGnuMake::CreateLinkTargets(const wxString& type, BuildConfigPtr bldConf, wxString& text,
                                       wxString& targetName, const wxString& projName, const wxArrayString& depsProj)
{
    // incase project is type exe or dll, force link
    // this is to workaround bug in the generated makefiles
    // which causes the makefile to report 'nothing to be done'
    // even when a dependency was modified
    text << ".PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs\n";

    wxString extraDeps;
    wxString depsRules;

    wxString errMsg;
    ProjectPtr proj = clCxxWorkspaceST::Get()->FindProjectByName(projName, errMsg);

    for(size_t i = 0; i < depsProj.GetCount(); i++) {
        wxFileName fn(depsProj.Item(i));
        // proj->GetFileName().GetPath().c_str());
        fn.MakeRelativeTo(proj->GetProjectPath());
        extraDeps << "\"" << fn.GetFullPath() << "\" ";

        depsRules << "\"" << fn.GetFullPath() << "\":\n";
        // Make sure the dependecy directory exists
        depsRules << "\t" << MakeDir(fn.GetPath()) << "\n";
        depsRules << "\t@echo stam > "
                  << "\"" << fn.GetFullPath() << "\"\n";
        depsRules << "\n\n";
    }

    if(type == PROJECT_TYPE_EXECUTABLE || type == PROJECT_TYPE_DYNAMIC_LIBRARY) {
        text << "all: MakeIntermediateDirs ";
        text << "$(OutputFile)\n\n";

        text << "$(OutputFile): $(IntermediateDirectory)/.d ";
        if(extraDeps.IsEmpty() == false) {
            text << extraDeps;
        }

        text << "$(Objects) \n";
        targetName = "$(IntermediateDirectory)/.d";

    } else {
        text << "all: MakeIntermediateDirs $(IntermediateDirectory)/"
             << "$(OutputFile)\n\n";
        text << "$(IntermediateDirectory)/"
             << "$(OutputFile): $(Objects)\n";
    }

    if(bldConf->IsLinkerRequired()) {
        text << "\t" << MakeDir("$(IntermediateDirectory)") << "\n";
        text << "\t@echo \"\" > $(IntermediateDirectory)/.d\n";
        CreateTargets(type, bldConf, text, projName);

        if(type == PROJECT_TYPE_EXECUTABLE || type == PROJECT_TYPE_DYNAMIC_LIBRARY) {
            if(depsRules.IsEmpty() == false) {
                text << "\n" << depsRules << "\n";
            }
        }
    }
}

void BuilderGnuMake::CreateTargets(const wxString& type, BuildConfigPtr bldConf, wxString& text,
                                   const wxString& projName)
{
    bool markRebuilt(true);

    CompilerPtr cmp = bldConf->GetCompiler();

    // this is a special target that creates a file with the content of the
    // $(Objects) variable (to be used with the @<file-name> option of the LD
    for(size_t i = 0; i < m_objectChunks; ++i) {
        wxString oper = ">>";
        if(i == 0) {
            oper = " >";
        }
        text << "\t@echo $(Objects" << i << ") " << oper << " $(ObjectsFileList)\n";
    }

    wxString linkerLine = cmp->GetLinkLine(type, cmp->GetReadObjectFilesFromList());
    text << "\t" << linkerLine << "\n";
    markRebuilt = (type != PROJECT_TYPE_EXECUTABLE);

    // If a link occurred, mark this project as "rebuilt" so the parent project will
    // know that a re-link is required
    if(bldConf->IsLinkerRequired() && markRebuilt) {
        text << "\t@echo rebuilt > " << GetRelinkMarkerForProject(projName) << "\n";
    }
}

void BuilderGnuMake::CreatePostBuildEvents(ProjectPtr proj, BuildConfigPtr bldConf, wxString& text)
{
    if(!HasPostbuildCommands(bldConf)) {
        return;
    }

    // generate postbuild commands
    BuildCommandList cmds;
    bldConf->GetPostBuildCommands(cmds);

    // Loop over the commands and replace any macros
    //    std::for_each(cmds.begin(), cmds.end(), [&](BuildCommand& cmd) {
    //        cmd.SetCommand(
    //            MacroManager::Instance()->Expand(cmd.GetCommand(), clGetManager(), proj->GetName(),
    //            bldConf->GetName()));
    //    });

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
            if(m_isWindows && command.StartsWith("copy")) {
                command.Replace("/", "\\");
            }

            if(m_isWindows && command.EndsWith("\\")) {
                command.RemoveLast();
            }

            text << "\t" << iter->GetCommand() << "\n";
        }
    }
    text << "\t@echo Done\n";
}

bool BuilderGnuMake::HasPrebuildCommands(BuildConfigPtr bldConf) const
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

void BuilderGnuMake::CreatePreBuildEvents(ProjectPtr proj, BuildConfigPtr bldConf, wxString& text)
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

void BuilderGnuMake::CreateConfigsVariables(ProjectPtr proj, BuildConfigPtr bldConf, wxString& text)
{
    wxString name = bldConf->GetName();
    name = NormalizeConfigName(name);

    wxString cmpType = bldConf->GetCompilerType();
    CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(cmpType);

    wxString objectsFileName;
    objectsFileName << "$(IntermediateDirectory)/ObjectsList.txt";

    text << "## " << name << "\n";

    wxString outputFile = bldConf->GetOutputFileName();
    if(m_isWindows && (bldConf->GetProjectType() == PROJECT_TYPE_EXECUTABLE || bldConf->GetProjectType().IsEmpty())) {
        outputFile.Trim().Trim(false);
    }

    // Expand the build macros into the generated makefile
    wxString workspacePath = clCxxWorkspaceST::Get()->GetWorkspaceFileName().GetPath();
    wxString projectPath = proj->GetFileName().GetPath();
    wxString startupDir = clCxxWorkspaceST::Get()->GetStartupDir();
    workspacePath.Replace("\\", "/");
    projectPath.Replace("\\", "/");
    startupDir.Replace("\\", "/");

    // We use relative path in $(IntermediateDirectory) and $(OutputFile)
    // to reduce issues with path containing spaces
    wxString intermediateDir = GetIntermediateDirectory(proj, bldConf);

    wxString outputDir = bldConf->GetOutputDirectory();
    if(outputDir.IsEmpty()) {
        outputDir << "$(WorkspacePath)/build-$(WorkspaceConfiguration)/"
                  << (bldConf->GetProjectType() == PROJECT_TYPE_EXECUTABLE ? "bin" : "lib");
    }
    outputDir.Replace("$(WorkspacePath)", workspacePath);
    outputDir.Replace("$(ProjectPath)", projectPath);
    outputDir.Replace("$(IntermediateDirectory)", intermediateDir);
    wxFileName fnOutputFile(outputDir, outputFile.AfterLast('/'));
    if(fnOutputFile.IsAbsolute()) {
        fnOutputFile.MakeRelativeTo(projectPath);
    }
    // $(OutputFile) uses native path separator for backward-compatibility reasons
    // (i.e. it will be backslash on Windows, and forward slash otherwise)
    outputFile = fnOutputFile.GetFullPath(wxPATH_NATIVE);

    wxString mkdirCommand = cmp->GetTool("MakeDirCommand");
    if(mkdirCommand.IsEmpty()) {
        mkdirCommand = m_isWindows ? "mkdir" : "mkdir -p";
    }

    text << "ProjectName            :=" << proj->GetName() << "\n";
    text << "ConfigurationName      :=" << name << "\n";
    text << "WorkspaceConfiguration :=" << clCxxWorkspaceST::Get()->GetSelectedConfig()->GetName() << "\n";
    text << "WorkspacePath          :=" << ::WrapWithQuotes(workspacePath) << "\n";
    text << "ProjectPath            :=" << ::WrapWithQuotes(projectPath) << "\n";
    text << "IntermediateDirectory  :=" << intermediateDir << "\n";
    text << "OutDir                 :=$(IntermediateDirectory)\n";
    text << "CurrentFileName        :=\n";
    text << "CurrentFilePath        :=\n";
    text << "CurrentFileFullPath    :=\n";
    text << "User                   :=" << wxGetUserName() << "\n";
    text << "Date                   :=" << wxDateTime::Now().FormatDate() << "\n";
    text << "CodeLitePath           :=" << ::WrapWithQuotes(startupDir) << "\n";
    text << "MakeDirCommand         :=" << mkdirCommand << "\n";
    text << "LinkerName             :=" << cmp->GetTool("LinkerName") << "\n";
    text << "SharedObjectLinkerName :=" << cmp->GetTool("SharedObjectLinkerName") << "\n";
    text << "ObjectSuffix           :=" << cmp->GetObjectSuffix() << "\n";
    text << "DependSuffix           :=" << cmp->GetDependSuffix() << "\n";
    text << "PreprocessSuffix       :=" << cmp->GetPreprocessSuffix() << "\n";
    text << "DebugSwitch            :=" << cmp->GetSwitch("Debug") << "\n";
    text << "IncludeSwitch          :=" << cmp->GetSwitch("Include") << "\n";
    text << "LibrarySwitch          :=" << cmp->GetSwitch("Library") << "\n";
    text << "OutputSwitch           :=" << cmp->GetSwitch("Output") << "\n";
    text << "LibraryPathSwitch      :=" << cmp->GetSwitch("LibraryPath") << "\n";
    text << "PreprocessorSwitch     :=" << cmp->GetSwitch("Preprocessor") << "\n";
    text << "SourceSwitch           :=" << cmp->GetSwitch("Source") << "\n";
    text << "OutputDirectory        :=" << outputDir << "\n";
    text << "OutputFile             :=" << outputFile << "\n";
    text << "Preprocessors          :=" << ParsePreprocessor(bldConf->GetPreprocessor()) << "\n";
    text << "ObjectSwitch           :=" << cmp->GetSwitch("Object") << "\n";
    text << "ArchiveOutputSwitch    :=" << cmp->GetSwitch("ArchiveOutput") << "\n";
    text << "PreprocessOnlySwitch   :=" << cmp->GetSwitch("PreprocessOnly") << "\n";
    text << "ObjectsFileList        :=" << objectsFileName << "\n";
    text << "PCHCompileFlags        :=" << bldConf->GetPchCompileFlags() << "\n";

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
    if(HandleResourceFiles()) {
        wxString rcBuildOpts = bldConf->GetResCompileOptions();
        rcBuildOpts.Replace(";", " ");
        text << "RcCmpOptions           :=" << rcBuildOpts << "\n";
        text << "RcCompilerName         :=" << cmp->GetTool("ResourceCompiler") << "\n";
    }

    wxString linkOpt = bldConf->GetLinkOptions();
    linkOpt.Replace(";", " ");

    // link options are kept with semi-colons, strip them
    text << "LinkOptions            := " << linkOpt << "\n";

    // add the global include path followed by the project include path
    wxString pchFile;

    // If the PCH is required to be in the command line, add it here
    // otherwise, we just make sure it is generated and the compiler will pick it by itself
    if(bldConf->GetPchInCommandLine()) {
        pchFile = bldConf->GetPrecompiledHeader();
        pchFile.Trim().Trim(false);
        if(pchFile.IsEmpty() == false) {
            pchFile.Prepend(" -include ").Append(" ");
        }
    }

    wxString libraries = bldConf->GetLibraries();
    wxArrayString libsArr = ::wxStringTokenize(libraries, ";", wxTOKEN_STRTOK);
    libraries.Clear();
    libraries << " ";
    for(size_t i = 0; i < libsArr.GetCount(); i++) {
        libsArr.Item(i).Trim().Trim(false);
        libraries << "\"" << libsArr.Item(i) << "\" ";
    }

    text << "IncludePath            := "
         << ParseIncludePath(cmp->GetGlobalIncludePath(), proj->GetName(), bldConf->GetName()) << " "
         << ParseIncludePath(bldConf->GetIncludePath(), proj->GetName(), bldConf->GetName()) << "\n";
    text << "IncludePCH             := " << pchFile << "\n";
    text << "RcIncludePath          := "
         << ParseIncludePath(bldConf->GetResCmpIncludePath(), proj->GetName(), bldConf->GetName()) << "\n";
    text << "Libs                   := " << ParseLibs(bldConf->GetLibraries()) << "\n";
    text << "ArLibs                 := " << libraries << "\n";

    // add the global library path followed by the project library path
    text << "LibPath                :=" << ParseLibPath(cmp->GetGlobalLibPath(), proj->GetName(), bldConf->GetName())
         << " " << ParseLibPath(bldConf->GetLibPath(), proj->GetName(), bldConf->GetName()) << "\n";

    text << "\n";
    text << "##\n";
    text << "## Common variables\n";
    text << "## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overridden using an environment variable\n";
    text << "##\n";
    text << "AR       := " << cmp->GetTool("AR") << "\n";
    text << "CXX      := " << cmp->GetTool("CXX") << "\n";
    text << "CC       := " << cmp->GetTool("CC") << "\n";
    text << "CXXFLAGS := " << buildOpts << " $(Preprocessors)"
         << "\n";
    text << "CFLAGS   := " << cBuildOpts << " $(Preprocessors)"
         << "\n";
    text << "ASFLAGS  := " << asOptions << "\n";
    text << "AS       := " << cmp->GetTool("AS") << "\n";
    text << "\n\n";
}

wxString BuilderGnuMake::ParseIncludePath(const wxString& paths, const wxString& projectName, const wxString& selConf)
{
    // convert semi-colon delimited string into GNU list of
    // include paths:
    wxString incluedPath(wxEmptyString);
    wxStringTokenizer tkz(paths, ";", wxTOKEN_STRTOK);
    while(tkz.HasMoreTokens()) {
        wxString path(tkz.NextToken());
        TrimString(path);
        ::WrapWithQuotes(path);
        incluedPath << "$(IncludeSwitch)" << path << " ";
    }
    return incluedPath;
}

wxString BuilderGnuMake::ParseLibPath(const wxString& paths, const wxString& projectName, const wxString& selConf)
{
    // convert semi-colon delimited string into GNU list of
    // lib path
    wxString libPath(wxEmptyString);
    wxStringTokenizer tkz(paths, ";", wxTOKEN_STRTOK);
    // prepend each include path with libpath switch
    while(tkz.HasMoreTokens()) {
        wxString path(tkz.NextToken());
        path.Trim().Trim(false);
        ::WrapWithQuotes(path);
        libPath << "$(LibraryPathSwitch)" << path << " ";
    }
    return libPath;
}

wxString BuilderGnuMake::ParsePreprocessor(const wxString& prep)
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

wxString BuilderGnuMake::ParseLibs(const wxString& libs)
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

wxString BuilderGnuMake::GetBuildCommand(const wxString& project, const wxString& confToBuild,
                                         const wxString& arguments)
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
    buildTool.Replace("\\", "/");
    cmd << buildTool << " Makefile";
    return cmd;
}

wxString BuilderGnuMake::GetCleanCommand(const wxString& project, const wxString& confToBuild,
                                         const wxString& arguments)
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
    buildTool.Replace("\\", "/");

    BuildMatrixPtr matrix = clCxxWorkspaceST::Get()->GetBuildMatrix();
    wxString type = Builder::NormalizeConfigName(matrix->GetSelectedConfigurationName());
    cmd << buildTool << " Makefile clean";
    return cmd;
}

wxString BuilderGnuMake::GetPOBuildCommand(const wxString& project, const wxString& confToBuild,
                                           const wxString& arguments)
{
    wxString errMsg, cmd;
    ProjectPtr proj = clCxxWorkspaceST::Get()->FindProjectByName(project, errMsg);
    if(!proj) {
        return wxEmptyString;
    }

    // generate the makefile
    Export(project, confToBuild, arguments, true, false, errMsg);
    cmd = GetProjectMakeCommand(proj, confToBuild, "all", kIncludePreBuild | kIncludePostBuild);
    return cmd;
}

wxString BuilderGnuMake::GetPOCleanCommand(const wxString& project, const wxString& confToBuild,
                                           const wxString& arguments)
{
    wxString errMsg, cmd;
    ProjectPtr proj = clCxxWorkspaceST::Get()->FindProjectByName(project, errMsg);
    if(!proj) {
        return wxEmptyString;
    }

    // generate the makefile
    Export(project, confToBuild, arguments, true, false, errMsg);
    cmd = GetProjectMakeCommand(proj, confToBuild, "clean", kCleanOnly | kIncludePreBuild);
    return cmd;
}

wxString BuilderGnuMake::GetSingleFileCmd(const wxString& project, const wxString& confToBuild,
                                          const wxString& arguments, const wxString& fileName)
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

    if(FileExtManager::GetType(fileName) == FileExtManager::TypeHeader) {
        // Attempting to build a header file, try to see if we got an implementation file instead
        // We had the current extension to the array so incase we loop over the entire array
        // we remain with the original file name unmodified
        std::vector<wxString> implExtensions = { "cpp", "cxx", "cc", "c++", "c", fn.GetExt() };
        for(const wxString& ext : implExtensions) {
            fn.SetExt(ext);
            if(fn.FileExists()) {
                break;
            }
        }
    }

    BuildConfigPtr bldConf = clCxxWorkspaceST::Get()->GetProjBuildConf(project, confToBuild);
    if(!bldConf) {
        return wxEmptyString;
    }

    cmpType = bldConf->GetCompilerType();
    CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(cmpType);
    // fn.MakeRelativeTo(proj->GetFileName().GetPath());

    wxString relPath = fn.GetPath(true, wxPATH_UNIX);
    wxString objNamePrefix = DoGetTargetPrefix(fn, proj->GetFileName().GetPath(), cmp);
    target << GetIntermediateDirectory(proj, bldConf) << "/" << objNamePrefix << fn.GetFullName()
           << cmp->GetObjectSuffix();

    target = ExpandAllVariables(target, clCxxWorkspaceST::Get(), proj->GetName(), confToBuild, wxEmptyString);
    cmd = GetProjectMakeCommand(proj, confToBuild, target, kIncludePreBuild);

    return EnvironmentConfig::Instance()->ExpandVariables(cmd, true);
}

wxString BuilderGnuMake::GetPreprocessFileCmd(const wxString& project, const wxString& confToBuild,
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
    buildTool.Replace("\\", "/");

    // create the target
    wxString target;
    wxString objSuffix;
    wxFileName fn(fileName);

    wxString cmpType = bldConf->GetCompilerType();
    CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(cmpType);

    wxString objNamePrefix = DoGetTargetPrefix(fn, proj->GetFileName().GetPath(), cmp);
    target << GetIntermediateDirectory(proj, bldConf) << "/" << objNamePrefix << fn.GetFullName()
           << cmp->GetPreprocessSuffix();

    target = ExpandAllVariables(target, clCxxWorkspaceST::Get(), proj->GetName(), confToBuild, wxEmptyString);
    cmd = GetProjectMakeCommand(proj, confToBuild, target, kIncludePreBuild);
    return EnvironmentConfig::Instance()->ExpandVariables(cmd, true);
}

wxString BuilderGnuMake::GetCdCmd(const wxFileName& path1, const wxFileName& path2)
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

void BuilderGnuMake::CreateCustomPostBuildEvents(BuildConfigPtr bldConf, wxString& text)
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

void BuilderGnuMake::CreateCustomPreBuildEvents(BuildConfigPtr bldConf, wxString& text)
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

wxString BuilderGnuMake::GetProjectMakeCommand(const wxFileName& wspfile, const wxFileName& projectPath,
                                               ProjectPtr proj, const wxString& confToBuild)
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
            makeCommand << basicMakeCommand << " " << precmpheader << ".gch"
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

wxString BuilderGnuMake::GetProjectMakeCommand(ProjectPtr proj, const wxString& confToBuild, const wxString& target,
                                               size_t flags)
{
    bool bCleanOnly = flags & kCleanOnly;
    bool bIncludePreBuild = flags & kIncludePreBuild;
    bool bIncludePostBuild = flags & kIncludePostBuild;
    bool bAddCleanTarget = flags & kAddCleanTarget;

    BuildConfigPtr bldConf = clCxxWorkspaceST::Get()->GetProjBuildConf(proj->GetName(), confToBuild);

    // iterate over the dependencies projects and generate makefile
    wxString makeCommand;
    wxString basicMakeCommand;

    wxString buildTool = GetBuildToolCommand(proj->GetName(), confToBuild, "", true);
    buildTool = EnvironmentConfig::Instance()->ExpandVariables(buildTool, true);
    basicMakeCommand << buildTool << " \"" << proj->GetName() << ".mk\" ";

    if(bAddCleanTarget) {
        makeCommand << basicMakeCommand << " clean && ";
    }

    if(bldConf && !bCleanOnly) {
        wxString preprebuild = bldConf->GetPreBuildCustom();
        wxString precmpheader = bldConf->GetPrecompiledHeader();
        precmpheader.Trim().Trim(false);
        preprebuild.Trim().Trim(false);

        makeCommand << basicMakeCommand << " MakeIntermediateDirs && ";

        if(!preprebuild.IsEmpty()) {
            makeCommand << basicMakeCommand << " PrePreBuild && ";
        }

        if(bIncludePreBuild && HasPrebuildCommands(bldConf)) {
            makeCommand << basicMakeCommand << " PreBuild && ";
        }

        // Run pre-compiled header compilation if any
        if(!precmpheader.IsEmpty() && (bldConf->GetPCHFlagsPolicy() != BuildConfig::kPCHJustInclude)) {
            makeCommand << basicMakeCommand << " " << precmpheader << ".gch"
                        << " && ";
        }
    }

    makeCommand << basicMakeCommand << " " << target;

    // post
    if(bldConf && !bCleanOnly && bIncludePostBuild && HasPostbuildCommands(bldConf)) {
        makeCommand << " && " << basicMakeCommand << " PostBuild";
    }
    return makeCommand;
}

void BuilderGnuMake::CreatePreCompiledHeaderTarget(BuildConfigPtr bldConf, wxString& text)
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
    text << filename << ".gch: " << filename << "\n";
    switch(pchPolicy) {
    case BuildConfig::kPCHPolicyReplace:
        text << "\t" << DoGetCompilerMacro(filename) << " $(SourceSwitch) " << filename << " $(PCHCompileFlags)\n";
        break;
    case BuildConfig::kPCHPolicyAppend:
        text << "\t" << DoGetCompilerMacro(filename) << " $(SourceSwitch) " << filename
             << " $(PCHCompileFlags) $(CXXFLAGS) $(IncludePath)\n";
        break;
    case BuildConfig::kPCHJustInclude:
        // for completeness
        break;
    }
    text << "\n";
}

wxString BuilderGnuMake::GetPORebuildCommand(const wxString& project, const wxString& confToBuild,
                                             const wxString& arguments)
{
    wxString errMsg, cmd;
    ProjectPtr proj = clCxxWorkspaceST::Get()->FindProjectByName(project, errMsg);
    if(!proj) {
        return wxEmptyString;
    }

    // generate the makefile
    Export(project, confToBuild, arguments, true, false, errMsg);
    cmd = GetProjectMakeCommand(proj, confToBuild, "all", kIncludePreBuild | kIncludePostBuild | kAddCleanTarget);
    return cmd;
}

wxString BuilderGnuMake::GetBuildToolCommand(const wxString& project, const wxString& confToBuild,
                                             const wxString& arguments, bool isCommandlineCommand) const
{
    wxString jobsCmd;
    wxString buildTool;

    BuildConfigPtr bldConf = clCxxWorkspaceST::Get()->GetProjBuildConf(project, confToBuild);
    if(!bldConf) {
        return wxEmptyString;
    }

    CompilerPtr compiler = bldConf->GetCompiler();
    if(!compiler) {
        return wxEmptyString;
    }

    if(isCommandlineCommand) {
        buildTool = compiler->GetTool("MAKE");

    } else {
        jobsCmd = wxEmptyString;
        buildTool = "\"$(MAKE)\"";
    }

    if(buildTool.Lower().Contains("make")) {
        if(isCommandlineCommand) {
            return buildTool + " -e -f ";

        } else {
            return buildTool + " -f ";
        }

    } else {
        // Just return the build command as appears in the toolchain
        return buildTool + " ";
    }
}

wxString BuilderGnuMake::DoGetCompilerMacro(const wxString& filename)
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

wxString BuilderGnuMake::DoGetTargetPrefix(const wxFileName& filename, const wxString& cwd, CompilerPtr cmp)
{
    wxString lastDir;
    wxString ret;

    if(cwd == filename.GetPath()) {
        return wxEmptyString;
    }

    if(cmp && cmp->GetObjectNameIdenticalToFileName()) {
        return wxEmptyString;
    }

    if(cwd == filename.GetPath()) {
        return wxEmptyString;
    }

    // remove cwd from filename
    wxFileName relpath = filename;
    relpath.MakeRelativeTo(cwd);

    const wxArrayString& dirs = relpath.GetDirs();
    for(int i = 0; i < (int)dirs.size(); ++i) {
        lastDir = dirs.Item(i);

        // Handle special directory paths
        if(lastDir == "..") {
            lastDir = "up";

        } else if(lastDir == ".") {
            lastDir = "cur";
        }

        if(lastDir.IsEmpty() == false) {
            lastDir << "_";
        }

        ret += lastDir;
    }

    return ret;
}

wxString BuilderGnuMake::GetRelinkMarkerForProject(const wxString& projectName) const
{
    return "$(IntermediateDirectory)/" + projectName + ".relink";
}

bool BuilderGnuMake::HasPostbuildCommands(BuildConfigPtr bldConf) const
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

bool BuilderGnuMake::SendBuildEvent(int eventId, const wxString& projectName, const wxString& configurationName)
{
    clBuildEvent e(eventId);
    e.SetProjectName(projectName);
    e.SetConfigurationName(configurationName);
    return EventNotifier::Get()->ProcessEvent(e);
}

bool BuilderGnuMake::HandleResourceFiles() const { return m_isMSYSEnv || m_isWindows; }

bool BuilderGnuMake::IsResourceFile(const Compiler::CmpFileTypeInfo& file_type) const
{
    return file_type.kind == Compiler::CmpFileKindResource;
}

Builder::OptimalBuildConfig BuilderGnuMake::GetOptimalBuildConfig(const wxString& projectType) const
{
    OptimalBuildConfig conf;
    conf.command = "$(WorkspacePath)/build-$(WorkspaceConfiguration)/bin/$(OutputFile)";
    conf.workingDirectory = "$(WorkspacePath)/build-$(WorkspaceConfiguration)/lib";

    if(projectType == PROJECT_TYPE_STATIC_LIBRARY || projectType == PROJECT_TYPE_DYNAMIC_LIBRARY) {
        conf.outputFile << "lib";
    }
    conf.outputFile << "$(ProjectName)" << GetOutputFileSuffix(projectType);

    return conf;
}
