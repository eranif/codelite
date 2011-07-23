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
#include "project.h"
#include "fileextmanager.h"
#include <wx/app.h>
#include <wx/msgdlg.h>

#include "file_logger.h"
#include "editor_config.h"
#include <wx/stopwatch.h>
#include "environmentconfig.h"
#include "evnvarlist.h"
#include "environmentconfig.h"
#include "builder_gnumake.h"
#include "configuration_mapping.h"
#include "dirsaver.h"
#include "wx/tokenzr.h"
#include "macros.h"
#include "build_settings_config.h"
#include "buildmanager.h"
#include "wx/sstream.h"
#include "globals.h"

wxStopWatch g_sw;

static bool OS_WINDOWS = wxGetOsVersion() & wxOS_WINDOWS ? true : false;

#if PERFORMANCE
# define TIMER_START(){\
		g_sw.Start();\
	}

# define PRINT_TIMESTAMP(msg) {\
		wxString log_msg(msg);\
		wxPrintf(wxT("%08d: %s"), g_sw.Time(), log_msg.c_str());\
	}
#else
# define TIMER_START()
# define PRINT_TIMESTAMP(msg)
#endif


static wxString GetMakeDirCmd(BuildConfigPtr bldConf, const wxString &relPath = wxEmptyString)
{
	wxString intermediateDirectory (bldConf->GetIntermediateDirectory());
	wxString relativePath          (relPath);

	intermediateDirectory.Replace(wxT("\\"), wxT("/"));
	intermediateDirectory.Trim().Trim(false);
	if(intermediateDirectory.StartsWith(wxT("./")) && relativePath == wxT("./")) {
		relativePath.Clear();
	}

	if(intermediateDirectory.StartsWith(wxT("./")) && relativePath.IsEmpty() == false) {
		intermediateDirectory = intermediateDirectory.Mid(2);
	}

	wxString text;
	if (OS_WINDOWS) {
		text << wxT("@$(MakeDirCommand) \"") << relativePath << intermediateDirectory << wxT("\"");
	} else {
		//other OSs
		text << wxT("@test -d ") << relativePath << intermediateDirectory << wxT(" || $(MakeDirCommand) ") << relativePath << intermediateDirectory;
	}
	return text;
}

BuilderGnuMake::BuilderGnuMake()
		: Builder(wxT("GNU makefile for g++/gcc"), wxT("make"), wxT("-f"))
{
}

BuilderGnuMake::BuilderGnuMake(const wxString &name, const wxString &buildTool, const wxString &buildToolOptions)
		: Builder(name, buildTool, buildToolOptions)
{
}

BuilderGnuMake::~BuilderGnuMake()
{
}

bool BuilderGnuMake::Export(const wxString &project, const wxString &confToBuild, bool isProjectOnly, bool force, wxString &errMsg)
{
	TIMER_START();
	PRINT_TIMESTAMP(_("Exporting makefile...\n"));
	if (project.IsEmpty()) {
		return false;
	}
	PRINT_TIMESTAMP(_("Exporting makefile...done\n"));

	ProjectPtr proj = WorkspaceST::Get()->FindProjectByName(project, errMsg);
	if (!proj) {
		errMsg << _("Cant open project '") << project << wxT("'");
		return false;
	}

	// get the selected build configuration
	wxString bld_conf_name(confToBuild);

	if (confToBuild.IsEmpty()) {
		BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjBuildConf(project, confToBuild);
		if (!bldConf) {
			errMsg << _("Cant find build configuration for project '") << project << wxT("'");
			return false;
		}
		bld_conf_name = bldConf->GetName();
	}

	PRINT_TIMESTAMP(_("Reading project dependencies...\n"));
	wxArrayString depsArr = proj->GetDependencies(bld_conf_name);
	PRINT_TIMESTAMP(_("Reading project dependencies...done\n"));

	wxArrayString removeList;
	if (!isProjectOnly) {
		//this function assumes that the working directory is located at the workspace path
		//make sure that all dependencies exists
		for (size_t i=0; i<depsArr.GetCount(); i++) {
			ProjectPtr dependProj = WorkspaceST::Get()->FindProjectByName(depsArr.Item(i), errMsg);
			//Missing dependencies project?
			//this can happen if a project was removed from the workspace, but
			//is still on the depdendencie list of this project
			if (!dependProj) {
				wxString msg;
				msg << _("CodeLite can not find project '") << depsArr.Item(i) << _("' which is required\n");
				msg << _("for building project '") << project << _("'.\nWould you like to remove it from the dependency list?");
				if (wxMessageBox(msg, _("CodeLite"), wxYES_NO | wxICON_QUESTION) == wxYES) {
					//remove the project from the dependecie list, and continue
					removeList.Add(depsArr.Item(i));
				}
			}
		}
		bool settingsChanged(false);
		//remove the unfound projects from the dependencies array
		for (size_t i=0; i<removeList.GetCount(); i++) {
			int where = depsArr.Index(removeList.Item(i));
			if (where != wxNOT_FOUND) {
				depsArr.RemoveAt(where);
				settingsChanged = true;
			}
		}
		//update the project dependencies
		bool modified = proj->IsModified();

		// Update the dependencies only if needed
		if(settingsChanged)
			proj->SetDependencies(depsArr, bld_conf_name);

		//the set settings functions marks the project as 'modified' this causes
		//an unneeded makefile generation if the settings was not really modified
		if (!modified && !settingsChanged) {
			proj->SetModified(false);
		}
	}

	wxString fn;
	fn << WorkspaceST::Get()->GetName()  << wxT("_wsp.mk");
	wxString text;

	wxFileName wspfile(WorkspaceST::Get()->GetWorkspaceFileName());

	PRINT_TIMESTAMP(_("Generating makefile...\n"));
	text << wxT(".PHONY: clean All\n\n");
	text << wxT("All:\n");

	//iterate over the dependencies projects and generate makefile
	wxString buildTool = BuildManagerST::Get()->GetSelectedBuilder()->GetBuildToolCommand(false);
	buildTool = EnvironmentConfig::Instance()->ExpandVariables(buildTool, true);

	// fix: replace all Windows like slashes to POSIX
	buildTool.Replace(wxT("\\"), wxT("/"));

	//generate the makefile for the selected workspace configuration
	BuildMatrixPtr matrix = WorkspaceST::Get()->GetBuildMatrix();
	wxString workspaceSelConf = matrix->GetSelectedConfigurationName();
	wxArrayString depsProjs;
	if (!isProjectOnly) {
		for (size_t i=0; i<depsArr.GetCount(); i++) {
			bool isCustom(false);
			ProjectPtr dependProj = WorkspaceST::Get()->FindProjectByName(depsArr.Item(i), errMsg);
			if (!dependProj) {
				continue;
			}

			wxString projectSelConf = matrix->GetProjectSelectedConf(workspaceSelConf, dependProj->GetName());
			BuildConfigPtr dependProjbldConf = WorkspaceST::Get()->GetProjBuildConf(dependProj->GetName(), projectSelConf);
			if (dependProjbldConf && dependProjbldConf->IsCustomBuild()) {
				isCustom = true;
			}

			// incase we manually specified the configuration to be built, set the project
			// as modified, so on next attempt to build it, CodeLite will sync the configuration
			if (confToBuild.IsEmpty() == false) {
				dependProj->SetModified( true );
			}

			text << wxT("\t@echo ") << wxGetTranslation(BUILD_PROJECT_PREFIX) << dependProj->GetName() << wxT(" - ") << projectSelConf << wxT(" ]----------\n");
			// make the paths relative, if it's sensible to do so
			wxFileName fn(dependProj->GetFileName());
			MakeRelativeIfSensible(fn, wspfile.GetPath());

			bool isPluginGeneratedMakefile = SendCmdEvent(wxEVT_GET_IS_PLUGIN_MAKEFILE, (void*)&depsArr.Item(i), projectSelConf);

			// we handle custom builds and non-custom build separatly:
			if ( isPluginGeneratedMakefile ) {

				// this project makefile is generated by a plugin
				// query the plugin about the build command
				wxCommandEvent e(wxEVT_GET_PROJECT_BUILD_CMD);

				e.SetClientData( (void*)&depsArr.Item(i) );
				e.SetString    ( projectSelConf          );
				wxTheApp->ProcessEvent(e);

				// the result is stored in the 'GetString()'
				text << wxT("\t") << e.GetString() << wxT("\n");

			} else if ( isCustom ) {

				CreateCustomPreBuildEvents(dependProjbldConf, text);

				wxString customWd  = dependProjbldConf->GetCustomBuildWorkingDir();
				wxString build_cmd = dependProjbldConf->GetCustomBuildCmd();
				wxString customWdCmd;
				
				customWd  = ExpandAllVariables(customWd,  WorkspaceST::Get(), dependProj->GetName(), dependProjbldConf->GetName(), wxEmptyString);
				build_cmd = ExpandAllVariables(build_cmd, WorkspaceST::Get(), dependProj->GetName(), dependProjbldConf->GetName(), wxEmptyString);
				
				build_cmd.Trim().Trim(false);

				if (build_cmd.empty()) {
					build_cmd << wxT("@echo Project has no custom build command!");
				}

				// if a working directory is provided apply it, otherwise use the project
				// path
				customWd.Trim().Trim(false);
				if (customWd.empty() == false ) {
					customWdCmd << wxT("@cd \"") << ExpandVariables(customWd, dependProj, NULL) << wxT("\" && ");
				} else {
					customWdCmd << GetCdCmd(wspfile, fn);
				}

				text << wxT("\t") << customWdCmd << build_cmd << wxT("\n");
				CreateCustomPostBuildEvents(dependProjbldConf, text);

			} else {
				PRINT_TIMESTAMP(wxString::Format(_("Generating makefile for project %s...\n"), dependProj->GetName().c_str()));
				// generate the dependency project makefile
				wxString dep_file = DoGetMarkerFileDir(wxT(""));
				dep_file << wxFileName::GetPathSeparator() << dependProj->GetName();
				depsProjs.Add(dep_file);
				
				GenerateMakefile(dependProj, projectSelConf, confToBuild.IsEmpty() ? force : true, wxArrayString());
				text << GetProjectMakeCommand(wspfile, fn, dependProj, confToBuild);
				PRINT_TIMESTAMP(wxString::Format(_("Generating makefile for project %s...done\n"), dependProj->GetName().c_str()));
			}
		}
	}

	// Generate makefile for the project itself
	GenerateMakefile(proj, confToBuild, confToBuild.IsEmpty() ? force : true, depsProjs);

	// incase we manually specified the configuration to be built, set the project
	// as modified, so on next attempt to build it, CodeLite will sync the configuration
	if (confToBuild.IsEmpty() == false) {
		proj->SetModified( true );
	}

	wxString projectSelConf = matrix->GetProjectSelectedConf(workspaceSelConf, project);
	if (isProjectOnly && confToBuild.IsEmpty() == false) {
		// incase we use to generate a 'Project Only' makefile,
		// we allow the caller to override the selected configuration with 'confToBuild' parameter
		projectSelConf = confToBuild;
	}

	text << wxT("\t@echo ") << wxGetTranslation(BUILD_PROJECT_PREFIX) << project << wxT(" - ") << projectSelConf << wxT(" ]----------\n");

	//make the paths relative, if it's sensible to do so
	wxFileName projectPath(proj->GetFileName());
	MakeRelativeIfSensible(projectPath, wspfile.GetPath());

	wxString pname( proj->GetName() );

	bool isPluginGeneratedMakefile = SendCmdEvent(wxEVT_GET_IS_PLUGIN_MAKEFILE, (void*)&pname, projectSelConf);
	if ( isPluginGeneratedMakefile ) {

		wxString cmd;
		wxCommandEvent e(wxEVT_GET_PROJECT_BUILD_CMD);
		e.SetClientData((void*)&pname);
		e.SetString(projectSelConf);
		wxTheApp->ProcessEvent(e);

		// the build command is in the 'String' member
		cmd = e.GetString();
		text << wxT("\t") << cmd << wxT("\n");

	} else {

		text << GetProjectMakeCommand(wspfile, projectPath, proj, projectSelConf);

	}

	//create the clean target
	text << wxT("clean:\n");
	if ( !isProjectOnly ) {
		for (size_t i=0; i<depsArr.GetCount(); i++) {
			bool isCustom(false);
			wxString projectSelConf = matrix->GetProjectSelectedConf(workspaceSelConf, depsArr.Item(i));

			ProjectPtr dependProj = WorkspaceST::Get()->FindProjectByName(depsArr.Item(i), errMsg);
			//Missing dependencies project? just skip it
			if (!dependProj) {
				continue;
			}

			text << wxT("\t@echo ") << wxGetTranslation(CLEAN_PROJECT_PREFIX) << dependProj->GetName() << wxT(" - ") << projectSelConf << wxT(" ]----------\n");

			//make the paths relative
			wxFileName fn(dependProj->GetFileName());
			MakeRelativeIfSensible(fn, wspfile.GetPath());

			//if the dependencie project is project of type 'Custom Build' - do the custom build instead
			//of the geenrated makefile
			BuildConfigPtr dependProjbldConf = WorkspaceST::Get()->GetProjBuildConf(dependProj->GetName(), projectSelConf);
			if (dependProjbldConf && dependProjbldConf->IsCustomBuild()) {
				isCustom = true;
			}

			// if we are using custom build command, invoke it instead of calling the generated makefile
			wxString pname = dependProj->GetName();
			bool isPluginGeneratedMakefile = SendCmdEvent(wxEVT_GET_IS_PLUGIN_MAKEFILE, (void*)&pname, projectSelConf);

			// we handle custom builds and non-custom build separately:
			if ( isPluginGeneratedMakefile ) {

				// this project makefile is generated by a plugin
				// query the plugin about the build command
				wxCommandEvent e( wxEVT_GET_PROJECT_CLEAN_CMD );

				e.SetClientData( (void*)&pname  );
				e.SetString    ( projectSelConf );
				wxTheApp->ProcessEvent(e);

				// the result is stored in the 'GetString()'
				text << wxT("\t") << e.GetString() << wxT("\n");

			} else if ( !isCustom ) {

				text << wxT("\t") << GetCdCmd(wspfile, fn) << buildTool << wxT(" \"") << dependProj->GetName() << wxT(".mk\"  clean\n");

			} else {

				wxString customWd  = dependProjbldConf->GetCustomBuildWorkingDir();
				wxString clean_cmd = dependProjbldConf->GetCustomCleanCmd();
				
				customWd  = ExpandAllVariables(customWd,  WorkspaceST::Get(), dependProj->GetName(), dependProjbldConf->GetName(), wxEmptyString);
				clean_cmd = ExpandAllVariables(clean_cmd, WorkspaceST::Get(), dependProj->GetName(), dependProjbldConf->GetName(), wxEmptyString);
				
				wxString customWdCmd;

				clean_cmd.Trim().Trim(false);
				if (clean_cmd.empty()) {
					clean_cmd << wxT("@echo Project has no custom clean command!");
				}

				// if a working directory is provided apply it, otherwise use the project
				// path
				customWd.Trim().Trim(false);
				if (customWd.empty() == false ) {
					customWdCmd << wxT("@cd \"") << ExpandVariables(customWd, dependProj, NULL) << wxT("\" && ");
				} else {
					customWdCmd << GetCdCmd(wspfile, fn);
				}
				text << wxT("\t") << customWdCmd << clean_cmd << wxT("\n");
			}
		}
	}

	//generate makefile for the project itself
	projectSelConf = matrix->GetProjectSelectedConf(workspaceSelConf, project);
	if (isProjectOnly && confToBuild.IsEmpty() == false) {
		// incase we use to generate a 'Project Only' makefile,
		// we allow the caller to override the selected configuration with 'confToBuild' parameter
		projectSelConf = confToBuild;
	}

	text << wxT("\t@echo ") << wxGetTranslation(CLEAN_PROJECT_PREFIX) << project << wxT(" - ") << projectSelConf << wxT(" ]----------\n");
	if ( isPluginGeneratedMakefile ) {

		wxString cmd;
		wxCommandEvent e(wxEVT_GET_PROJECT_CLEAN_CMD);
		e.SetClientData((void*)&pname);
		e.SetString(projectSelConf);
		wxTheApp->ProcessEvent(e);

		// the build command is in the 'String' member
		cmd = e.GetString();
		text << wxT("\t") << cmd << wxT("\n");

	} else {
		text << wxT("\t") << GetCdCmd(wspfile, projectPath) << buildTool << wxT(" \"") << proj->GetName() << wxT(".mk\" clean\n") ;
	}

	PRINT_TIMESTAMP(_("Generating makefile...done\n"));

	//dump the content to file
	PRINT_TIMESTAMP(_("Writing makefile...\n"));
	wxFileOutputStream output(fn);
	wxStringInputStream content(text);
	output << content;
	PRINT_TIMESTAMP(_("Writing makefile...done\n"));

	return true;
}

void BuilderGnuMake::GenerateMakefile(ProjectPtr proj, const wxString &confToBuild, bool force, const wxArrayString &depsProj)
{
	wxString pname (proj->GetName());
	wxString tmpConfigName(confToBuild.c_str());
	if (confToBuild.IsEmpty()) {
		BuildMatrixPtr matrix = WorkspaceST::Get()->GetBuildMatrix();
		tmpConfigName = matrix->GetProjectSelectedConf(matrix->GetSelectedConfigurationName(), proj->GetName());
	}

	bool isPluginGeneratedMakefile = SendCmdEvent(wxEVT_GET_IS_PLUGIN_MAKEFILE, (void*)&pname, tmpConfigName);

	// we handle custom builds and non-custom build separatly:
	if ( isPluginGeneratedMakefile ) {
		if( force ) {
			// Generate the makefile
			SendCmdEvent(wxEVT_PLUGIN_EXPORT_MAKEFILE, (void*)&pname, tmpConfigName);
		}
		return;
	}

	ProjectSettingsPtr settings = proj->GetSettings();
	if (!settings) {
		return;
	}

	//get the selected build configuration for this project
	BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjBuildConf(proj->GetName(), confToBuild);
	if (!bldConf) {
		return;
	}

	wxString path = proj->GetFileName().GetPath();

	DirSaver ds;
	//change directory to the project base dir
	::wxSetWorkingDirectory(path);


	//create new makefile file
	wxString fn(path);
	fn << PATH_SEP << proj->GetName() << wxT(".mk");

	//skip the next test if the makefile does not exist
	if (wxFileName::FileExists( fn )) {
		if (!force) {
			if (proj->IsModified() == false) {
				return;
			}
		}
	}

	//generate the selected configuration for this project
	//wxTextOutputStream text(output);
	wxString text;

	text << wxT("##") << wxT("\n");
	text << wxT("## Auto Generated makefile by CodeLite IDE") << wxT("\n");
	text << wxT("## any manual changes will be erased      ") << wxT("\n");
	text << wxT("##") << wxT("\n");

	// Create the makefile variables
	CreateConfigsVariables(proj, bldConf, text);


	//----------------------------------------------------------
	// copy environment variables to the makefile
	// We put them after the 'hard-coeded' ones
	// so user will be able to override any of the default
	// variables by defining its own
	//----------------------------------------------------------
	EvnVarList vars;
	EnvironmentConfig::Instance()->ReadObject(wxT("Variables"), &vars);
	EnvMap varMap = vars.GetVariables(wxT(""), true, proj->GetName());

	text << wxT("##") << wxT("\n");
	text << wxT("## User defined environment variables") << wxT("\n");
	text << wxT("##") << wxT("\n");

	for (size_t i=0; i<varMap.GetCount(); i++) {
		wxString name, value;
		varMap.Get(i, name, value);
		text << name << wxT(":=") << value << wxT("") << wxT("\n");
	}


	CreateListMacros(proj, confToBuild, text); // list of srcs and list of objects

	//-----------------------------------------------------------
	// create the build targets
	//-----------------------------------------------------------
	text << wxT("##\n");
	text << wxT("## Main Build Targets \n");
	text << wxT("##\n");

	//incase project is type exe or dll, force link
	//this is to workaround bug in the generated makefiles
	//which causes the makefile to report 'nothing to be done'
	//even when a dependency was modified
	wxString targetName(bldConf->GetIntermediateDirectory());
	CreateLinkTargets(proj->GetSettings()->GetProjectType(bldConf->GetName()), bldConf, text, targetName, proj->GetName(), depsProj);
	
	CreatePostBuildEvents        (bldConf, text);
	CreateMakeDirsTarget         (bldConf, targetName, text);
	CreatePreBuildEvents         (bldConf, text);
	CreatePreCompiledHeaderTarget(bldConf, text);

	//-----------------------------------------------------------
	// Create a list of targets that should be built according to
	// projects' file list
	//-----------------------------------------------------------
	CreateFileTargets(proj, confToBuild, text);
	CreateCleanTargets(proj, confToBuild, text);

    //dump the content to a file
	wxFFile output;
	output.Open(fn, wxT("w+"));
	if (output.IsOpened()) {
		output.Write(text);
		output.Close();
	}

	//mark the project as non-modified one
	proj->SetModified(false);
}

void BuilderGnuMake::CreateMakeDirsTarget(BuildConfigPtr bldConf, const wxString &targetName, wxString &text)
{
	text << wxT("\n");
	text << targetName << wxT(":\n");
	text << wxT("\t") << GetMakeDirCmd(bldConf) << wxT("\n");
}

void BuilderGnuMake::CreateSrcList(ProjectPtr proj, const wxString &confToBuild, wxString &text)
{
	std::vector<wxFileName> files;
	proj->GetFiles(files);
	text << wxT("Srcs=");

	BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjBuildConf(proj->GetName(), confToBuild);
	wxString cmpType = bldConf->GetCompilerType();
	wxString relPath;

	//get the compiler settings
	CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(cmpType);

	int counter = 1;
	Compiler::CmpFileTypeInfo ft;

	for (size_t i=0; i<files.size(); i++) {

		// is this a valid file?
		if (!cmp->GetCmpFileType(files[i].GetExt(), ft))
			continue;

		if (ft.kind == Compiler::CmpFileKindResource && bldConf && !bldConf->IsResCompilerRequired()) {
			// we are on Windows, the file type is Resource and resource compiler is not required
			continue;
		}

		relPath = files.at(i).GetPath(true, wxPATH_UNIX);
		relPath.Trim().Trim(false);

		// Compiler::CmpFileKindSource , Compiler::CmpFileKindResource file
		text << relPath <<  files[i].GetFullName() << wxT(" ");

		if (counter % 10 == 0) {
			text << wxT("\\\n\t");
		}
		counter++;
	}
	text << wxT("\n\n");
}

void BuilderGnuMake::CreateObjectList(ProjectPtr proj, const wxString &confToBuild, wxString &text)
{
	std::vector<wxFileName> files;
	proj->GetFiles(files, true);
	text << wxT("Objects=");

	BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjBuildConf(proj->GetName(), confToBuild);
	wxString cmpType = bldConf->GetCompilerType();

	//get the compiler settings
	CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(cmpType);

	int counter = 1;
	Compiler::CmpFileTypeInfo ft;
	wxString cwd = ::wxGetCwd();

	wxString objectsList;
	for (size_t i=0; i<files.size(); i++) {

		// is this a valid file?
		if (!cmp->GetCmpFileType(files[i].GetExt(), ft))
			continue;

		if (ft.kind == Compiler::CmpFileKindResource && bldConf && !bldConf->IsResCompilerRequired()) {
			// we are on Windows, the file type is Resource and resource compiler is not required
			continue;
		}

		wxString objPrefix = DoGetTargetPrefix(files.at(i), cwd);

		if (ft.kind == Compiler::CmpFileKindResource) {
			// resource files are handled differently
			text << wxT("$(IntermediateDirectory)/") << objPrefix << files[i].GetFullName() << wxT("$(ObjectSuffix) ");
		} else {
			// Compiler::CmpFileKindSource file
			text << wxT("$(IntermediateDirectory)/") << objPrefix << files[i].GetName() << wxT("$(ObjectSuffix) ");
		}
		if (counter % 10 == 0) {
			text << wxT("\\\n\t");
		}
		counter++;
	}
	text << wxT("\n\n");
}

void BuilderGnuMake::CreateFileTargets(ProjectPtr proj, const wxString &confToBuild, wxString &text)
{
	PRINT_TIMESTAMP(wxT("Creating file targets...\n"));
	//get the project specific build configuration for the workspace active
	//configuration
	BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjBuildConf(proj->GetName(), confToBuild);
	wxString cmpType = bldConf->GetCompilerType();
	//get the compiler settings
	CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(cmpType);
	bool generateDependenciesFiles = cmp->GetGenerateDependeciesFile() &&
	                                !cmp->GetDependSuffix().IsEmpty();
	bool supportPreprocessOnlyFiles = !cmp->GetSwitch(wxT("PreprocessOnly")).IsEmpty() &&
	                                  !cmp->GetPreprocessSuffix().IsEmpty();

	std::vector<wxFileName> abs_files, rel_paths;

	// support for full path
	PRINT_TIMESTAMP(wxT("Loading file list...\n"));
	proj->GetFiles(rel_paths, abs_files);
	PRINT_TIMESTAMP(wxT("Loading file list...done\n"));

	text << wxT("\n\n");
	// create rule per object
	text << wxT("##\n");
	text << wxT("## Objects\n");
	text << wxT("##\n");

	Compiler::CmpFileTypeInfo ft;

	// Collect all the sub-directories that we generate files for
	wxArrayString subDirs;

	wxString cwd = proj->GetFileName().GetPath();

	PRINT_TIMESTAMP(wxT("Looping over the file list...\n"));
	for (size_t i=0; i<abs_files.size(); i++) {
		// is this file interests the compiler?
		if (cmp->GetCmpFileType(abs_files.at(i).GetExt().Lower(), ft) ) {
			wxString absFileName;
			wxFileName fn( abs_files.at(i) );

			wxString filenameOnly    = fn.GetName();
			wxString fullpathOnly    = fn.GetFullPath();
			wxString fullnameOnly    = fn.GetFullName();
			wxString compilationLine = ft.compilation_line;

			// use UNIX style slashes
			absFileName = abs_files[i].GetFullPath();
			absFileName.Replace(wxT("\\"), wxT("/"));
			wxString relPath;

			relPath = rel_paths.at(i).GetPath(true, wxPATH_UNIX);
			relPath.Trim().Trim(false);

			wxString objPrefix = DoGetTargetPrefix(abs_files.at(i), cwd);

			compilationLine.Replace(wxT("$(FileName)"),     filenameOnly);
			compilationLine.Replace(wxT("$(FileFullName)"), fullnameOnly);
			compilationLine.Replace(wxT("$(FileFullPath)"), fullpathOnly);
			compilationLine.Replace(wxT("$(FilePath)"),     relPath);

			// The object name is handled differently when using resource files
			if(ft.kind == Compiler::CmpFileKindResource)
				compilationLine.Replace(wxT("$(ObjectName)"),   objPrefix + fullnameOnly);
			else
				compilationLine.Replace(wxT("$(ObjectName)"),   objPrefix + filenameOnly);

			compilationLine.Replace(wxT("$(ObjectName)"),   objPrefix + filenameOnly);
			compilationLine.Replace(wxT("\\"),              wxT("/"));

			if (ft.kind == Compiler::CmpFileKindSource) {
				wxString objectName;
				wxString dependFile;
				wxString preprocessedFile;

				bool isCFile = FileExtManager::GetType(rel_paths.at(i).GetFullName()) == FileExtManager::TypeSourceC;
				
				objectName << wxT("$(IntermediateDirectory)/") << objPrefix << filenameOnly << wxT("$(ObjectSuffix)");
				if (generateDependenciesFiles) {
					dependFile << wxT("$(IntermediateDirectory)/") << objPrefix << filenameOnly << wxT("$(DependSuffix)");
				}
				if (supportPreprocessOnlyFiles) {
					preprocessedFile << wxT("$(IntermediateDirectory)/") << objPrefix << filenameOnly << wxT("$(PreprocessSuffix)");
				}

				if(!isCFile) {
					// Add the PCH include line
					compilationLine.Replace(wxT("$(CompilerName)"), wxT("$(CompilerName) $(IncludePCH)"));
				}
				
				// set the file rule
				text << objectName << wxT(": ") << rel_paths.at(i).GetFullPath(wxPATH_UNIX) << wxT(" ") << dependFile << wxT("\n");
				text << wxT("\t") << compilationLine << wxT("\n");
				
				wxString cmpOptions(wxT("$(CmpOptions) $(IncludePCH)"));
				if(isCFile) {
					cmpOptions = wxT("$(C_CmpOptions)");
				}
				
				wxString compilerMacro = DoGetCompilerMacro(rel_paths.at(i).GetFullPath(wxPATH_UNIX));
				if (generateDependenciesFiles) {
					text << dependFile << wxT(": ") << rel_paths.at(i).GetFullPath(wxPATH_UNIX) << wxT("\n");
					text << wxT("\t") << wxT("@") << compilerMacro << wxT(" ") << cmpOptions << wxT(" $(IncludePath) -MG -MP -MT") << objectName <<wxT(" -MF") << dependFile << wxT(" -MM \"") << absFileName << wxT("\"\n\n");
				}

				if (supportPreprocessOnlyFiles) {
					text << preprocessedFile << wxT(": ") << rel_paths.at(i).GetFullPath(wxPATH_UNIX) << wxT("\n");
					text << wxT("\t") << wxT("@") << compilerMacro << wxT(" ") << cmpOptions << wxT(" $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) ") << preprocessedFile << wxT(" \"") << absFileName << wxT("\"\n\n");
				}

			} else if (ft.kind == Compiler::CmpFileKindResource && bldConf->IsResCompilerRequired()) {
				// we construct an object name which also includes the full name of the reousrce file and appends a .o to the name (to be more
				// precised, $(ObjectSuffix))
				wxString objectName;
				objectName << wxT("$(IntermediateDirectory)/") << objPrefix << fullnameOnly << wxT("$(ObjectSuffix)");

				text << objectName << wxT(": ") << rel_paths.at(i).GetFullPath(wxPATH_UNIX) << wxT("\n");
				text << wxT("\t") << compilationLine << wxT("\n");
			}
		}
	}

	if (generateDependenciesFiles) {
		text << wxT("\n");
		text << wxT("-include ") << wxT("$(IntermediateDirectory)/*$(DependSuffix)\n");
	}

	PRINT_TIMESTAMP(_("Looping over the file list...done\n"));
	PRINT_TIMESTAMP(_("Creating file targets...done\n"));
}

void BuilderGnuMake::CreateCleanTargets(ProjectPtr proj, const wxString &confToBuild, wxString &text)
{
	PRINT_TIMESTAMP(_("Creating clean targets...\n"));
	//get the project specific build configuration for the workspace active
	//configuration
	BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjBuildConf(proj->GetName(), confToBuild);
	wxString cmpType = bldConf->GetCompilerType();
	//get the compiler settings
	CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(cmpType);
	std::vector<wxFileName> abs_files, rel_paths;

	// Can we use asterisk in the clean command?
	long asterisk(0);
	EditorConfigST::Get()->GetLongValue(wxT("CleanTragetWithAsterisk"), asterisk);

	wxString imd = bldConf->GetIntermediateDirectory();
	imd.Trim().Trim(false);
	bool useAsterisk = ((imd.IsEmpty() == false) && asterisk);

	// support for full path
	PRINT_TIMESTAMP(_("Loading file list for clean...\n"));
	proj->GetFiles(rel_paths, abs_files);
	PRINT_TIMESTAMP(_("Loading file list...done\n"));

	//add clean target
	text << wxT("##\n");
	text << wxT("## Clean\n");
	text << wxT("##\n");
	text << wxT("clean:\n");

	wxString cwd = proj->GetFileName().GetPath();
	if (OS_WINDOWS) {

		// Windows clean command
		if( !useAsterisk ) {
			for (size_t i=0; i<abs_files.size(); i++) {

				Compiler::CmpFileTypeInfo ft;
				if (cmp->GetCmpFileType(abs_files[i].GetExt(), ft)) {
					wxString objPrefix = DoGetTargetPrefix(abs_files.at(i), cwd);

					if (ft.kind == Compiler::CmpFileKindSource) {

						wxString objectName, dependFile, preprocessFile;
						objectName     << objPrefix << abs_files[i].GetName() << wxT("$(ObjectSuffix)");
						dependFile     << objPrefix << abs_files[i].GetName() << wxT("$(DependSuffix)");
						preprocessFile << objPrefix << abs_files[i].GetName() << wxT("$(PreprocessSuffix)");

						text << wxT("\t") << wxT("$(RM) ") << wxT("$(IntermediateDirectory)/") << objectName     << wxT("\n");
						text << wxT("\t") << wxT("$(RM) ") << wxT("$(IntermediateDirectory)/") << dependFile     << wxT("\n");
						text << wxT("\t") << wxT("$(RM) ") << wxT("$(IntermediateDirectory)/") << preprocessFile << wxT("\n");

					} else if (ft.kind == Compiler::CmpFileKindResource && bldConf->IsResCompilerRequired()) {
						wxString ofile = abs_files[i].GetFullName() + wxT("$(ObjectSuffix)");
						text << wxT("\t") << wxT("$(RM) ") << wxT("$(IntermediateDirectory)/") << ofile << wxT("\n");

					}
				}
			}
		} else {
			// We can use asterisk (intermediat directory is not empty)
			text << wxT("\t") << wxT("$(RM) ") << wxT("$(IntermediateDirectory)/*.*") << wxT("\n");
		}

		//delete the output file as well
		wxString exeExt(wxEmptyString);
		if (proj->GetSettings()->GetProjectType(bldConf->GetName()) == Project::EXECUTABLE) {
			//under windows, g++ automatically adds the .exe extension to executable
			//make sure we delete it as well
			exeExt = wxT(".exe");
		}


		text << wxT("\t") << wxT("$(RM) ") << wxT("$(OutputFile)") << wxT("\n");
		text << wxT("\t") << wxT("$(RM) ") << wxT("$(OutputFile)") << exeExt << wxT("\n");
		text << wxT("\t") << wxT("$(RM) ") << DoGetMarkerFileDir(proj->GetName()) << wxT("\n");
		
		// Remove the pre-compiled header
		wxString pchFile = bldConf->GetPrecompiledHeader();
		pchFile.Trim().Trim(false);

		if(pchFile.IsEmpty() == false) {
			text << wxT("\t") << wxT("$(RM) ") << pchFile << wxT(".gch") << wxT("\n");
		}
	} else {
		//on linux we dont really need resource compiler...
		if(!useAsterisk) {
			for (size_t i=0; i<abs_files.size(); i++) {

				wxString objPrefix = DoGetTargetPrefix( abs_files.at(i), cwd );

				Compiler::CmpFileTypeInfo ft;
				if (cmp->GetCmpFileType(abs_files[i].GetExt(), ft) && ft.kind == Compiler::CmpFileKindSource) {

					wxString relPath;
					relPath = rel_paths.at(i).GetPath(true, wxPATH_UNIX);
					relPath.Trim().Trim(false);

					wxString objectName, dependFile, preprocessFile;

					objectName     << objPrefix << abs_files[i].GetName() << wxT("$(ObjectSuffix)");
					dependFile     << objPrefix << abs_files[i].GetName() << wxT("$(DependSuffix)");
					preprocessFile << objPrefix << abs_files[i].GetName() << wxT("$(PreprocessSuffix)");

					text << wxT("\t") << wxT("$(RM) ") << wxT("$(IntermediateDirectory)/") << objectName     << wxT("\n");
					text << wxT("\t") << wxT("$(RM) ") << wxT("$(IntermediateDirectory)/") << dependFile     << wxT("\n");
					text << wxT("\t") << wxT("$(RM) ") << wxT("$(IntermediateDirectory)/") << preprocessFile << wxT("\n");
				}
			}

		} else {
			text << wxT("\t") << wxT("$(RM) ") << wxT("$(IntermediateDirectory)/*.*") << wxT("\n");
		}

		//delete the output file as well
		text << wxT("\t") << wxT("$(RM) ") << wxT("$(OutputFile)\n");
		text << wxT("\t") << wxT("$(RM) ") << DoGetMarkerFileDir(proj->GetName()) << wxT("\n");
		
		// Remove the pre-compiled header
		wxString pchFile = bldConf->GetPrecompiledHeader();
		pchFile.Trim().Trim(false);

		if(pchFile.IsEmpty() == false) {
			text << wxT("\t") << wxT("$(RM) ") << pchFile << wxT(".gch") << wxT("\n");
		}

	}

	text << wxT("\n\n");
	PRINT_TIMESTAMP(wxT("Creating clean targets...done\n"));
}

void BuilderGnuMake::CreateListMacros(ProjectPtr proj, const wxString &confToBuild, wxString &text)
{
	// create a list of Sources
	// CreateSrcList(proj, confToBuild, text);  // Not used/needed for multi-step build
	// create a list of objects
	CreateObjectList(proj, confToBuild, text);
}

void BuilderGnuMake::CreateLinkTargets(const wxString &type, BuildConfigPtr bldConf, wxString &text, wxString &targetName, const wxString &projName, const wxArrayString &depsProj)
{
	//incase project is type exe or dll, force link
	//this is to workaround bug in the generated makefiles
	//which causes the makefile to report 'nothing to be done'
	//even when a dependency was modified
	
	text << wxT(".PHONY: all clean PreBuild PrePreBuild PostBuild\n");
	
	wxString extraDeps;
	wxString depsRules;
	
	wxString errMsg;
	ProjectPtr proj = WorkspaceST::Get()->FindProjectByName(projName, errMsg);
	
	for(size_t i=0; i<depsProj.GetCount(); i++) {
		wxFileName fn(depsProj.Item(i));
		CL_DEBUG(wxT("making %s relative to %s"), fn.GetFullPath().c_str(), proj->GetFileName().GetPath().c_str());
		fn.MakeRelativeTo(proj->GetFileName().GetPath());
		extraDeps << fn.GetFullPath() << wxT(" ");
		
		depsRules << fn.GetFullPath() << wxT(":\n");
		depsRules << wxT("\t@echo stam > ") << wxT("\"") << fn.GetFullPath() << wxT("\"\n");
		depsRules << wxT("\n\n");
	}
	
	if (type  == Project::EXECUTABLE || type == Project::DYNAMIC_LIBRARY) {
		text << wxT("all: ");
		text << wxT("$(OutputFile)\n\n");
		
		text << wxT("$(OutputFile): $(IntermediateDirectory)/.d ");
		if(extraDeps.IsEmpty() == false)
			text << extraDeps;
			
		text << wxT("$(Objects) \n");
		targetName = wxT("$(IntermediateDirectory)/.d");
		
	} else {
		text << wxT("all: $(IntermediateDirectory) ");
		text << wxT("$(OutputFile)\n\n");
		text << wxT("$(OutputFile): $(Objects)\n");
	}

	if (bldConf->IsLinkerRequired()) {
		CreateTargets(type, bldConf, text, projName);
		
		if(type == Project::EXECUTABLE || type == Project::DYNAMIC_LIBRARY) {
			if(depsRules.IsEmpty() == false) {
				text << wxT("\n") << depsRules << wxT("\n");
			}
		}
	}
}

void BuilderGnuMake::CreateTargets(const wxString &type, BuildConfigPtr bldConf, wxString &text, const wxString &projName)
{
	bool markRebuilt(true);
	text << wxT("\t@$(MakeDirCommand) $(@D)\n");
	text << wxT("\t@echo \"\" > $(IntermediateDirectory)/.d\n");
	
	CompilerPtr cmp = bldConf->GetCompiler();
	
	// this is a special target that creates a file with the content of the 
	// $(Objects) variable (to be used with the @<file-name> option of the LD
	text << wxT("\t@echo $(Objects) > $(ObjectsFileList)\n");
	
	if (type == Project::STATIC_LIBRARY) {
		//create a static library
		// In any case add the 'objects_file' target here
		text << wxT("\t") << wxT("$(ArchiveTool) $(ArchiveOutputSwitch)$(OutputFile)");
		if(cmp && cmp->GetReadObjectFilesFromList()) {
			text << wxT(" @$(ObjectsFileList)\n");
		} else {
			text << wxT(" $(Objects)\n");
		}
		
	} else if (type == Project::DYNAMIC_LIBRARY) {
		//create a shared library
		text << wxT("\t") << wxT("$(SharedObjectLinkerName) $(OutputSwitch)$(OutputFile)");
		if(cmp && cmp->GetReadObjectFilesFromList()) {
			text << wxT(" @$(ObjectsFileList) ");
		} else {
			text << wxT(" $(Objects) ");
		}
		text << wxT("$(LibPath) $(Libs) $(LinkOptions)\n");
		
	} else if (type == Project::EXECUTABLE) {
		//create an executable
		text << wxT("\t") << wxT("$(LinkerName) $(OutputSwitch)$(OutputFile)");
		if(cmp && cmp->GetReadObjectFilesFromList()) {
			text << wxT(" @$(ObjectsFileList) ");
		} else {
			text << wxT(" $(Objects) ");
		}
		text << wxT("$(LibPath) $(Libs) $(LinkOptions)\n");
		markRebuilt = false;
	}
	
	// If a link occured, mark this project as "rebuilt" so the parent project will
	// know that a re-link is required
	if(bldConf->IsLinkerRequired() && markRebuilt) {
		text << wxT("\t@$(MakeDirCommand) \"") 
			 << DoGetMarkerFileDir(wxEmptyString)
			 << wxT("\"\n");
			 
		text << wxT("\t@echo rebuilt > ") 
			 << DoGetMarkerFileDir(projName)
			 << wxT("\n");
	}
}

void BuilderGnuMake::CreatePostBuildEvents(BuildConfigPtr bldConf, wxString &text)
{
	if(!HasPostbuildCommands(bldConf))
		return;
	
	//generate postbuild commands
	BuildCommandList cmds;
	bldConf->GetPostBuildCommands(cmds);
	
	text << wxT("\n");
	text << wxT("PostBuild:\n");
	text << wxT("\t@echo Executing Post Build commands ...\n");
	
	BuildCommandList::const_iterator iter = cmds.begin();
	for (; iter != cmds.end(); iter++) {
		if (iter->GetEnabled()) {
			// HACK:
			// If the command is 'copy' under Windows, make sure that
			// we set all slashes to backward slashes
			wxString command = iter->GetCommand();
			command.Trim().Trim(false);
			if(OS_WINDOWS && command.StartsWith(wxT("copy"))) {
				command.Replace(wxT("/"), wxT("\\"));
			}
			
			if(OS_WINDOWS && command.EndsWith(wxT("\\"))) {
				command.RemoveLast();
			}
			
			text << wxT("\t") << iter->GetCommand() << wxT("\n");
		}
	}
	text << wxT("\t@echo Done\n");
}

bool BuilderGnuMake::HasPrebuildCommands(BuildConfigPtr bldConf) const
{
	BuildCommandList cmds;
	bldConf->GetPreBuildCommands(cmds);
	
	BuildCommandList::const_iterator iter = cmds.begin();
	for (; iter != cmds.end(); iter++) {
		if (iter->GetEnabled()) {
			return true;
		}
	}
	return false;
}

void BuilderGnuMake::CreatePreBuildEvents(BuildConfigPtr bldConf, wxString &text)
{
	BuildCommandList cmds;
	BuildCommandList::iterator iter;
	wxString name = bldConf->GetName();
	name = NormalizeConfigName(name);

	//add PrePreBuild
	wxString preprebuild = bldConf->GetPreBuildCustom();
	preprebuild.Trim().Trim(false);
	if (preprebuild.IsEmpty() == false) {
		text << wxT("PrePreBuild: ");
		text << bldConf->GetPreBuildCustom() << wxT("\n");
	}
	text << wxT("\n");

	cmds.clear();
	bldConf->GetPreBuildCommands(cmds);
	bool first(true);
	text << wxT("PreBuild:\n");
	if (!cmds.empty()) {
		iter = cmds.begin();
		for (; iter != cmds.end(); iter++) {
			if (iter->GetEnabled()) {
				if (first) {
					text << wxT("\t@echo Executing Pre Build commands ...\n");
					first = false;
				}
				text << wxT("\t") << iter->GetCommand() << wxT("\n");
			}
		}
		if (!first) {
			text << wxT("\t@echo Done\n");
		}
	}
}

void BuilderGnuMake::CreateConfigsVariables(ProjectPtr proj, BuildConfigPtr bldConf, wxString &text)
{
	wxString name = bldConf->GetName();
	name = NormalizeConfigName(name);

	wxString cmpType = bldConf->GetCompilerType();
	CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(cmpType);
	
	wxString objectsFileName(proj->GetFileName().GetPath());
	objectsFileName << PATH_SEP << proj->GetName() << wxT(".txt");
	
	text << wxT("## ") << name << wxT("\n");
	
	wxString outputFile = bldConf->GetOutputFileName();
	if(OS_WINDOWS && (bldConf->GetProjectType() == Project::EXECUTABLE || bldConf->GetProjectType().IsEmpty())) {
		outputFile.Trim().Trim(false);
		if(outputFile.EndsWith(wxT(".exe")) == false) {
			outputFile.Append(wxT(".exe"));
		}
	}
	
	// Expand the build macros into the generated makefile 
	text << wxT("ProjectName            :=") << proj->GetName() << wxT("\n");
	text << wxT("ConfigurationName      :=") << name << wxT("\n");
	text << wxT("IntermediateDirectory  :=") << bldConf->GetIntermediateDirectory() << wxT("\n");
	text << wxT("OutDir                 := $(IntermediateDirectory)\n");
	text << wxT("WorkspacePath          := \"") << WorkspaceST::Get()->GetWorkspaceFileName().GetPath() << wxT("\"\n");
	text << wxT("ProjectPath            := \"") << proj->GetFileName().GetPath() << wxT("\"\n");
	text << wxT("CurrentFileName        :=\n"); // TODO:: Need implementation
	text << wxT("CurrentFilePath        :=\n"); // TODO:: Need implementation
	text << wxT("CurrentFileFullPath    :=\n"); // TODO:: Need implementation
	text << wxT("User                   :=") << wxGetUserName() << wxT("\n");
	text << wxT("Date                   :=") << wxDateTime::Now().FormatDate() << wxT("\n");
	text << wxT("CodeLitePath           :=\"") << WorkspaceST::Get()->GetStartupDir() << wxT("\"\n");
	text << wxT("LinkerName             :=") << cmp->GetTool(wxT("LinkerName")) << wxT("\n");
	text << wxT("ArchiveTool            :=") << cmp->GetTool(wxT("ArchiveTool")) << wxT("\n");
	text << wxT("SharedObjectLinkerName :=") << cmp->GetTool(wxT("SharedObjectLinkerName")) << wxT("\n");
	text << wxT("ObjectSuffix           :=") << cmp->GetObjectSuffix() << wxT("\n");
	text << wxT("DependSuffix           :=") << cmp->GetDependSuffix() << wxT("\n");
	text << wxT("PreprocessSuffix       :=") << cmp->GetPreprocessSuffix() << wxT("\n");
	text << wxT("DebugSwitch            :=") << cmp->GetSwitch(wxT("Debug")) << wxT("\n");
	text << wxT("IncludeSwitch          :=") << cmp->GetSwitch(wxT("Include")) << wxT("\n");
	text << wxT("LibrarySwitch          :=") << cmp->GetSwitch(wxT("Library")) << wxT("\n");
	text << wxT("OutputSwitch           :=") << cmp->GetSwitch(wxT("Output")) << wxT("\n");
	text << wxT("LibraryPathSwitch      :=") << cmp->GetSwitch(wxT("LibraryPath")) << wxT("\n");
	text << wxT("PreprocessorSwitch     :=") << cmp->GetSwitch(wxT("Preprocessor")) << wxT("\n");
	text << wxT("SourceSwitch           :=") << cmp->GetSwitch(wxT("Source")) << wxT("\n");
	text << wxT("CompilerName           :=") << cmp->GetTool(wxT("CompilerName")) << wxT("\n");
	text << wxT("C_CompilerName         :=") << cmp->GetTool(wxT("C_CompilerName")) << wxT("\n");
	text << wxT("OutputFile             :=") << outputFile << wxT("\n");
	text << wxT("Preprocessors          :=") << ParsePreprocessor(bldConf->GetPreprocessor()) << wxT("\n");
	text << wxT("ObjectSwitch           :=") << cmp->GetSwitch(wxT("Object")) << wxT("\n");
	text << wxT("ArchiveOutputSwitch    :=") << cmp->GetSwitch(wxT("ArchiveOutput")) << wxT("\n");
	text << wxT("PreprocessOnlySwitch   :=") << cmp->GetSwitch(wxT("PreprocessOnly")) << wxT("\n");
	text << wxT("ObjectsFileList        :=\"") << objectsFileName << wxT("\"\n");
	
	if (OS_WINDOWS) {
		text << wxT("MakeDirCommand         :=") << wxT("makedir") << wxT("\n");
	} else {
		text << wxT("MakeDirCommand         :=") << wxT("mkdir -p") << wxT("\n");
	}

	wxString buildOpts = bldConf->GetCompileOptions();
	buildOpts.Replace(wxT(";"), wxT(" "));
	
	wxString cBuildOpts = bldConf->GetCCompileOptions();
	cBuildOpts.Replace(wxT(";"), wxT(" "));
	
	// Let the plugins add their content here
	wxCommandEvent e(wxEVT_GET_ADDITIONAL_COMPILEFLAGS);
	wxTheApp->ProcessEvent(e);

	wxString additionalCompileFlags = e.GetString();
	if(additionalCompileFlags.IsEmpty() == false) {
		buildOpts << wxT(" ") << additionalCompileFlags;
		cBuildOpts << wxT(" ") << additionalCompileFlags;
	}
	
	text << wxT("CmpOptions             :=") << buildOpts  << wxT(" $(Preprocessors)") << wxT("\n");
	text << wxT("C_CmpOptions           :=") << cBuildOpts << wxT(" $(Preprocessors)") << wxT("\n");

	//only if resource compiler required, evaluate the resource variables
	if (bldConf->IsResCompilerRequired()) {
		wxString rcBuildOpts = bldConf->GetResCompileOptions();
		rcBuildOpts.Replace(wxT(";"), wxT(" "));
		text << wxT("RcCmpOptions           :=") << rcBuildOpts << wxT("\n");
		text << wxT("RcCompilerName         :=") << cmp->GetTool(wxT("ResourceCompiler")) << wxT("\n");
	}

	wxString linkOpt = bldConf->GetLinkOptions();
	linkOpt.Replace(wxT(";"), wxT(" "));

	//link options are kept with semi-colons, strip them
	text << wxT("LinkOptions            := ") << linkOpt << wxT("\n");

	// add the global include path followed by the project include path
	wxString pchFile;

	// If the PCH is required to be in the command line, add it here
	// otherwise, we just make sure it is generated and the compiler will pick it by itself
	if(bldConf->GetPchInCommandLine()) {
		pchFile = bldConf->GetPrecompiledHeader();
		pchFile.Trim().Trim(false);
		if(pchFile.IsEmpty() == false) {
			pchFile.Prepend(wxT(" -include ")).Append(wxT(" "));
		}
	}
	
	text << wxT("IncludePath            := ") << ParseIncludePath(cmp->GetGlobalIncludePath(), proj->GetName(), bldConf->GetName()) << wxT(" ") << ParseIncludePath(bldConf->GetIncludePath(), proj->GetName(), bldConf->GetName()) << wxT("\n");
	text << wxT("IncludePCH             := ") << pchFile << wxT("\n");
	text << wxT("RcIncludePath          := ") << ParseIncludePath(bldConf->GetResCmpIncludePath(), proj->GetName(), bldConf->GetName()) << wxT("\n");
	text << wxT("Libs                   := ") << ParseLibs(bldConf->GetLibraries()) << wxT("\n");

	// add the global library path followed by the project library path
	text << wxT("LibPath                :=") << ParseLibPath(cmp->GetGlobalLibPath(), proj->GetName(), bldConf->GetName()) << wxT(" ") << ParseLibPath(bldConf->GetLibPath(), proj->GetName(), bldConf->GetName()) << wxT("\n");
	text << wxT("\n\n");
}

wxString BuilderGnuMake::ParseIncludePath(const wxString &paths, const wxString &projectName, const wxString &selConf)
{
	//convert semi-colon delimited string into GNU list of
	//include paths:
	wxString incluedPath(wxEmptyString);
	wxStringTokenizer tkz(paths, wxT(";"), wxTOKEN_STRTOK);
	//prepend each include path with -I
	while (tkz.HasMoreTokens()) {
		wxString path(tkz.NextToken());
		TrimString(path);
		path.Replace(wxT("\\"), wxT("/"));
		
		wxString wrapper;
		if(path.Contains(wxT(" ")))
			wrapper = wxT("\"");
		
		incluedPath << wxT("$(IncludeSwitch)") << wrapper << path << wrapper << wxT(" ");
	}
	return incluedPath;
}

wxString BuilderGnuMake::ParseLibPath(const wxString &paths, const wxString &projectName, const wxString &selConf)
{
	//convert semi-colon delimited string into GNU list of
	//lib path
	wxString libPath(wxEmptyString);
	wxStringTokenizer tkz(paths, wxT(";"), wxTOKEN_STRTOK);
	//prepend each include path with libpath switch
	while (tkz.HasMoreTokens()) {
		wxString path(tkz.NextToken());
		path.Trim().Trim(false);
		path.Replace(wxT("\\"), wxT("/"));
		wxString wrapper;
		if(path.Contains(wxT(" ")))
			wrapper = wxT("\"");
			
		libPath << wxT("$(LibraryPathSwitch)") << wrapper << path << wrapper << wxT(" ");
	}
	return libPath;
}

wxString BuilderGnuMake::ParsePreprocessor(const wxString &prep)
{
	wxString preprocessor(wxEmptyString);
	wxStringTokenizer tkz(prep, wxT(";"), wxTOKEN_STRTOK);
	//prepend each include path with libpath switch
	while (tkz.HasMoreTokens()) {
		wxString p(tkz.NextToken());
		p.Trim().Trim(false);
		preprocessor << wxT("$(PreprocessorSwitch)") << p << wxT(" ");
	}

	// if the macro contains # escape it
	// But first remove any manual escaping done by the user
	preprocessor.Replace(wxT("\\#"), wxT("#"));
	preprocessor.Replace(wxT("#"), wxT("\\#"));
	return preprocessor;
}

wxString BuilderGnuMake::ParseLibs(const wxString &libs)
{
	//convert semi-colon delimited string into GNU list of
	//libs
	wxString slibs(wxEmptyString);
	wxStringTokenizer tkz(libs, wxT(";"), wxTOKEN_STRTOK);
	//prepend each include path with -l and strip trailing lib string
	//also, if the file contains an extension (.a, .so, .dynlib) remove them as well
	while (tkz.HasMoreTokens()) {
		wxString lib(tkz.NextToken());
		TrimString(lib);
		//remove lib prefix
		if (lib.StartsWith(wxT("lib"))) {
			lib = lib.Mid(3);
		}

		//remove known suffixes
		if (	lib.EndsWith(wxT(".a")) ||
		        lib.EndsWith(wxT(".so")) ||
		        lib.EndsWith(wxT(".dylib")) ||
		        lib.EndsWith(wxT(".dll"))
		   ) {
			lib = lib.BeforeLast(wxT('.'));
		}

		slibs << wxT("$(LibrarySwitch)") << lib << wxT(" ");
	}
	return slibs;
}

wxString BuilderGnuMake::GetBuildCommand(const wxString &project, const wxString &confToBuild)
{
	wxString errMsg, cmd;
	BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjBuildConf(project, confToBuild);
	if (!bldConf) {
		return wxEmptyString;
	}

	//generate the makefile
	Export(project, confToBuild, false, false, errMsg);

	BuildMatrixPtr matrix = WorkspaceST::Get()->GetBuildMatrix();
	wxString buildTool = BuildManagerST::Get()->GetSelectedBuilder()->GetBuildToolCommand(true);
	buildTool = EnvironmentConfig::Instance()->ExpandVariables(buildTool, true);

	// fix: replace all Windows like slashes to POSIX
	buildTool.Replace(wxT("\\"), wxT("/"));

	wxString type = Builder::NormalizeConfigName(matrix->GetSelectedConfigurationName());
	cmd << buildTool << wxT(" \"") << WorkspaceST::Get()->GetName() << wxT("_wsp.mk\"");
	return cmd;
}

wxString BuilderGnuMake::GetCleanCommand(const wxString &project, const wxString &confToBuild)
{
	wxString errMsg, cmd;
	BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjBuildConf(project, confToBuild);
	if (!bldConf) {
		return wxEmptyString;
	}

	//generate the makefile
	Export(project, confToBuild, false, false, errMsg);

	wxString buildTool = BuildManagerST::Get()->GetSelectedBuilder()->GetBuildToolCommand(true);
	buildTool = EnvironmentConfig::Instance()->ExpandVariables(buildTool, true);

	// fix: replace all Windows like slashes to POSIX
	buildTool.Replace(wxT("\\"), wxT("/"));

	BuildMatrixPtr matrix = WorkspaceST::Get()->GetBuildMatrix();
	wxString type = Builder::NormalizeConfigName(matrix->GetSelectedConfigurationName());
	cmd << buildTool << wxT(" \"") << WorkspaceST::Get()->GetName() << wxT("_wsp.mk\" clean");
	return cmd;
}

wxString BuilderGnuMake::GetPOBuildCommand(const wxString &project, const wxString &confToBuild)
{
	wxString errMsg, cmd;
	ProjectPtr     proj    = WorkspaceST::Get()->FindProjectByName(project, errMsg);
	if (!proj) {
		return wxEmptyString;
	}

	//generate the makefile
	Export(project, confToBuild, true, false, errMsg);
	cmd = GetProjectMakeCommand(proj, confToBuild, wxT("all"), false, false);
	return cmd;
}

wxString BuilderGnuMake::GetPOCleanCommand(const wxString &project, const wxString &confToBuild)
{
	wxString errMsg, cmd;
	ProjectPtr     proj    = WorkspaceST::Get()->FindProjectByName(project, errMsg);
	if (!proj) {
		return wxEmptyString;
	}

	//generate the makefile
	Export(project, confToBuild, true, false, errMsg);
	cmd = GetProjectMakeCommand(proj, confToBuild, wxT("clean"), false, true);
	return cmd;
}

wxString BuilderGnuMake::GetSingleFileCmd(const wxString &project, const wxString &confToBuild, const wxString &fileName)
{
	wxString errMsg, cmd;
	ProjectPtr     proj    = WorkspaceST::Get()->FindProjectByName(project, errMsg);
	if (!proj) {
		return wxEmptyString;
	}

	//generate the makefile
	Export(project, confToBuild, true, false, errMsg);

	// Build the target list
	wxString      target;
	wxString      cmpType;
	wxFileName    fn(fileName);

	BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjBuildConf(project, confToBuild);
	if( !bldConf ) {
		return wxEmptyString;
	}

	cmpType = bldConf->GetCompilerType();
	CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(cmpType);
	//fn.MakeRelativeTo(proj->GetFileName().GetPath());

	wxString relPath = fn.GetPath(true, wxPATH_UNIX);
	wxString objNamePrefix = DoGetTargetPrefix(fn, proj->GetFileName().GetPath());
	target << bldConf->GetIntermediateDirectory() << wxT("/") << objNamePrefix << fn.GetName() << cmp->GetObjectSuffix();

	target = ExpandAllVariables(target, WorkspaceST::Get(), proj->GetName(), confToBuild, wxEmptyString);
	cmd    = GetProjectMakeCommand(proj, confToBuild, target, false, false);

	return EnvironmentConfig::Instance()->ExpandVariables(cmd, true);
}

wxString BuilderGnuMake::GetPreprocessFileCmd(const wxString &project, const wxString &confToBuild, const wxString &fileName, wxString &errMsg)
{
	ProjectPtr     proj    = WorkspaceST::Get()->FindProjectByName(project, errMsg);
	if (!proj) {
		return wxEmptyString;
	}

	wxString cmd;
	BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjBuildConf(project, confToBuild);
	if (!bldConf) {
		return wxEmptyString;
	}

	//generate the makefile
	Export(project, confToBuild, true, false, errMsg);

	BuildMatrixPtr matrix = WorkspaceST::Get()->GetBuildMatrix();
	wxString buildTool = BuildManagerST::Get()->GetSelectedBuilder()->GetBuildToolCommand(true);
	wxString type = matrix->GetProjectSelectedConf(matrix->GetSelectedConfigurationName(), project);

	// fix: replace all Windows like slashes to POSIX
	buildTool.Replace(wxT("\\"), wxT("/"));

	//create the target
	wxString target;
	wxString objSuffix;
	wxFileName fn(fileName);

	wxString cmpType = bldConf->GetCompilerType();
	CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(cmpType);

	wxString objNamePrefix = DoGetTargetPrefix(fn, proj->GetFileName().GetPath());
	target << bldConf->GetIntermediateDirectory() << wxT("/") << objNamePrefix << fn.GetName() << cmp->GetPreprocessSuffix();

	target = ExpandAllVariables(target, WorkspaceST::Get(), proj->GetName(), confToBuild, wxEmptyString);
	cmd    = GetProjectMakeCommand(proj, confToBuild, target, false, false);
	return EnvironmentConfig::Instance()->ExpandVariables(cmd, true);
}

wxString BuilderGnuMake::GetCdCmd(const wxFileName &path1, const wxFileName &path2)
{
	wxString cd_cmd(wxT("@"));
	if (path2.GetPath().IsEmpty()) {
		return cd_cmd;
	}

	if (path1.GetPath() != path2.GetPath()) {
		cd_cmd << wxT("cd \"") << path2.GetPath() << wxT("\" && ");
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
	if (!cmds.empty()) {
		iter = cmds.begin();
		for (; iter != cmds.end(); iter++) {
			if (iter->GetEnabled()) {
				if (first) {
					text << wxT("\t@echo Executing Post Build commands ...\n");
					first = false;
				}
				text << wxT("\t") << iter->GetCommand() << wxT("\n");
			}
		}
		if (!first) {
			text << wxT("\t@echo Done\n");
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
	if (!cmds.empty()) {
		iter = cmds.begin();
		for (; iter != cmds.end(); iter++) {
			if (iter->GetEnabled()) {
				if (first) {
					text << wxT("\t@echo Executing Pre Build commands ...\n");
					first = false;
				}
				text << wxT("\t") << iter->GetCommand() << wxT("\n");
			}
		}
		if (!first) {
			text << wxT("\t@echo Done\n");
		}
	}
}

wxString BuilderGnuMake::GetProjectMakeCommand(const wxFileName& wspfile, const wxFileName& projectPath, ProjectPtr proj, const wxString &confToBuild)
{
	BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjBuildConf(proj->GetName(), confToBuild);

	//iterate over the dependencies projects and generate makefile
	wxString makeCommand;
	wxString basicMakeCommand;

	wxString buildTool = BuildManagerST::Get()->GetSelectedBuilder()->GetBuildToolCommand(false);
	buildTool = EnvironmentConfig::Instance()->ExpandVariables(buildTool, true);
	basicMakeCommand << buildTool << wxT(" \"") << proj->GetName() << wxT(".mk\"");

	makeCommand << wxT("\t") << GetCdCmd(wspfile, projectPath);

	if( bldConf ) {
		wxString preprebuild = bldConf->GetPreBuildCustom();
		wxString precmpheader = bldConf->GetPrecompiledHeader();
		precmpheader.Trim().Trim(false);
		preprebuild.Trim().Trim(false);

		if (preprebuild.IsEmpty() == false) {
			makeCommand << basicMakeCommand << wxT(" PrePreBuild && ");
		}

		if (HasPrebuildCommands(bldConf) ) {
			makeCommand << basicMakeCommand << wxT(" PreBuild && ");
		}

		// Run pre-compiled header compilation if any
		if( precmpheader.IsEmpty() == false) {
			makeCommand << basicMakeCommand << wxT(" ") << precmpheader << wxT(".gch") << wxT(" && ");
		}

	}

	// the main command
	makeCommand << basicMakeCommand;
	
	// post
	if(bldConf && HasPostbuildCommands(bldConf)) {
		makeCommand << wxT(" && ") << basicMakeCommand << wxT(" PostBuild");
	}
	makeCommand << wxT("\n");
	return makeCommand;
}

wxString BuilderGnuMake::GetProjectMakeCommand(ProjectPtr proj, const wxString& confToBuild, const wxString &target, bool addCleanTarget, bool cleanOnly)
{
	BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjBuildConf(proj->GetName(), confToBuild);

	//iterate over the dependencies projects and generate makefile
	wxString makeCommand;
	wxString basicMakeCommand;

	wxString buildTool = BuildManagerST::Get()->GetSelectedBuilder()->GetBuildToolCommand(true);
	buildTool = EnvironmentConfig::Instance()->ExpandVariables(buildTool, true);
	basicMakeCommand << buildTool << wxT(" \"") << proj->GetName() << wxT(".mk\" ");

	if( addCleanTarget ) {
		makeCommand << basicMakeCommand << wxT(" clean && ");
	}

	if( bldConf && !cleanOnly ) {
		wxString preprebuild = bldConf->GetPreBuildCustom();
		wxString precmpheader = bldConf->GetPrecompiledHeader();
		precmpheader.Trim().Trim(false);
		preprebuild.Trim().Trim(false);

		if (preprebuild.IsEmpty() == false) {
			makeCommand << basicMakeCommand << wxT(" PrePreBuild && ");
		}

		if (HasPrebuildCommands(bldConf) ) {
			makeCommand << basicMakeCommand << wxT(" PreBuild && ");
		}

		// Run pre-compiled header compilation if any
		if( precmpheader.IsEmpty() == false) {
			makeCommand << basicMakeCommand << wxT(" ") << precmpheader << wxT(".gch") << wxT(" && ");
		}
	}

	makeCommand << basicMakeCommand << wxT(" ") << target;
	
	// post
	if(bldConf && !cleanOnly && HasPostbuildCommands(bldConf)) {
		makeCommand << wxT(" && ") << basicMakeCommand << wxT(" PostBuild");
	}
	return makeCommand;
}

void BuilderGnuMake::CreatePreCompiledHeaderTarget(BuildConfigPtr bldConf, wxString& text)
{
	wxString filename = bldConf->GetPrecompiledHeader();
	filename.Trim().Trim(false);

	if(filename.IsEmpty()) return;
	
	text << wxT("\n");
	text << wxT("# PreCompiled Header\n");
	text << filename << wxT(".gch: ") << filename << wxT("\n");
	text << wxT("\t") << DoGetCompilerMacro(filename) << wxT(" $(SourceSwitch) ") << filename << wxT(" $(CmpOptions) $(IncludePath)\n");
	text << wxT("\n");
}

wxString BuilderGnuMake::GetPORebuildCommand(const wxString& project, const wxString& confToBuild)
{
	wxString errMsg, cmd;
	ProjectPtr     proj    = WorkspaceST::Get()->FindProjectByName(project, errMsg);
	if (!proj) {
		return wxEmptyString;
	}

	//generate the makefile
	Export(project, confToBuild, true, false, errMsg);
	cmd = GetProjectMakeCommand(proj, confToBuild, wxT("all"), true, false);
	return cmd;
}

wxString BuilderGnuMake::GetBuildToolCommand(bool isCommandlineCommand) const
{
	wxString jobsCmd;
	wxString buildTool;

#if defined (__WXMSW__)
	wxString jobs = GetBuildToolJobs();
	if (jobs == wxT("unlimited"))
		jobsCmd = wxT(" -j ");
	else
		jobsCmd = wxT(" -j ") + jobs + wxT(" ");

	buildTool = GetBuildToolName();
#else
	if (isCommandlineCommand) {
		wxString jobs = GetBuildToolJobs();
		if (jobs == wxT("unlimited"))
			jobsCmd = wxT(" -j ");
		else
			jobsCmd = wxT(" -j ") + jobs + wxT(" ");

		buildTool = GetBuildToolName();
	} else {
		jobsCmd = wxEmptyString;
		buildTool = wxT("$(MAKE)");
	}
#endif
	//enclose the tool path in quatation marks
	return wxT("\"") + buildTool + wxT("\" ") + jobsCmd + GetBuildToolOptions() ;
}

wxString BuilderGnuMake::DoGetCompilerMacro(const wxString& filename)
{
	wxString compilerMacro(wxT("$(CompilerName)"));
	switch(FileExtManager::GetType(filename)) {
	case FileExtManager::TypeSourceC:
		compilerMacro = wxT("$(C_CompilerName)");
		break;
	case FileExtManager::TypeSourceCpp:
	default:
		compilerMacro = wxT("$(CompilerName)");
		break;
	}
	return compilerMacro;
}

wxString BuilderGnuMake::DoGetTargetPrefix(const wxFileName& filename, const wxString &cwd)
{
	size_t        count = filename.GetDirCount();
	wxArrayString dirs  = filename.GetDirs();
	wxString      lastDir;

	if(cwd == filename.GetPath()) {
		return wxEmptyString;
	}

	if(count) {
		lastDir = dirs.Item(count-1);

		// Handle special directory paths
		if(lastDir == wxT("..")) {
			lastDir = wxT("up");

		} else if(lastDir == wxT(".")) {
			lastDir = wxT("cur");
		}

		if(lastDir.IsEmpty() == false) {
			lastDir << wxT("_");

		}
	}

	return lastDir;
}

wxString BuilderGnuMake::DoGetMarkerFileDir(const wxString& projname)
{
	BuildMatrixPtr matrix = WorkspaceST::Get()->GetBuildMatrix();
	wxString workspaceSelConf = matrix->GetSelectedConfigurationName();
	
	workspaceSelConf = NormalizeConfigName(workspaceSelConf);
	workspaceSelConf.MakeLower();
	
	wxString path;
	if(projname.IsEmpty()) {
		path << WorkspaceST::Get()->GetWorkspaceFileName().GetPath()
			 << wxFileName::GetPathSeparator()
			 << wxT(".build-")
			 << workspaceSelConf;

	} else {
		path << wxT("\"")
		     << WorkspaceST::Get()->GetWorkspaceFileName().GetPath()
			 << wxFileName::GetPathSeparator()
			 << wxT(".build-")
			 << workspaceSelConf
			 << wxFileName::GetPathSeparator()
			 << projname
			 << wxT("\"");
	}
	return path;
}

bool BuilderGnuMake::HasPostbuildCommands(BuildConfigPtr bldConf) const
{
	BuildCommandList cmds;
	bldConf->GetPostBuildCommands(cmds);
	
	BuildCommandList::const_iterator iter = cmds.begin();
	for (; iter != cmds.end(); iter++) {
		if (iter->GetEnabled()) {
			return true;
		}
	}
	return false;
}
