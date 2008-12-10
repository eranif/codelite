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


static wxString GetMakeDirCmd(BuildConfigPtr bldConf)
{
	wxString text;
	if (OS_WINDOWS) {
		text << wxT("@makedir \"") << bldConf->GetIntermediateDirectory() << wxT("\"");
	} else {
		//other OSs
		text << wxT("@test -d ") << bldConf->GetIntermediateDirectory() << wxT(" || mkdir -p ") << bldConf->GetIntermediateDirectory();
	}
	return text;
}

BuilderGnuMake::BuilderGnuMake()
		: Builder(wxT("GNU makefile for g++/gcc"), wxT("make"), wxT("-f"))
{
}

BuilderGnuMake::~BuilderGnuMake()
{
}

bool BuilderGnuMake::Export(const wxString &project, const wxString &confToBuild, bool isProjectOnly, bool force, wxString &errMsg)
{
	TIMER_START();
	PRINT_TIMESTAMP(wxT("Exporting makefile...\n"));
	if (project.IsEmpty()) {
		return false;
	}
	PRINT_TIMESTAMP(wxT("Exporting makefile...done\n"));
	
	ProjectPtr proj = WorkspaceST::Get()->FindProjectByName(project, errMsg);
	if (!proj) {
		errMsg << wxT("Cant open project '") << project << wxT("'");
		return false;
	}

	// get the selected build configuration
	wxString bld_conf_name(confToBuild);

	if (confToBuild.IsEmpty()) {
		BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjBuildConf(project, confToBuild);
		if (!bldConf) {
			errMsg << wxT("Cant find build configuration for project '") << project << wxT("'");
			return false;
		}
		bld_conf_name = bldConf->GetName();
	}

	PRINT_TIMESTAMP(wxT("Reading project dependencies...\n"));
	wxArrayString depsArr = proj->GetDependencies(bld_conf_name);
	PRINT_TIMESTAMP(wxT("Reading project dependencies...done\n"));
	
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
				msg << wxT("CodeLite can not find project '") << depsArr.Item(i) << wxT("' which is required\n");
				msg << wxT("for building project '") << project << wxT("'.\nWould like to remove it from the dependency list?");
				if (wxMessageBox(msg, wxT("CodeLite"), wxYES_NO | wxICON_QUESTION) == wxYES) {
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
		//update the project dependencies a
		bool modified = proj->IsModified();
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
	
	PRINT_TIMESTAMP(wxT("Generating makefile...\n"));
	text << wxT(".PHONY: clean All\n\n");
	text << wxT("All:\n");

	//iterate over the dependencies projects and generate makefile
	wxString buildTool = BuildManagerST::Get()->GetSelectedBuilder()->GetBuildToolCommand(false);
	buildTool = WorkspaceST::Get()->ExpandVariables(buildTool);

	//generate the makefile for the selected workspace configuration
	BuildMatrixPtr matrix = WorkspaceST::Get()->GetBuildMatrix();
	wxString workspaceSelConf = matrix->GetSelectedConfigurationName();

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

			text << wxT("\t@echo ") << BUILD_PROJECT_PREFIX << dependProj->GetName() << wxT(" - ") << projectSelConf << wxT(" ]----------\n");
			// make the paths relative
			wxFileName fn(dependProj->GetFileName());
			fn.MakeRelativeTo(wspfile.GetPath());

			// we handle custom builds and non-custom build separatly:
			if ( isCustom ) {
				
				CreateCustomPreBuildEvents(dependProjbldConf, text);
				text << wxT("\t") << GetCdCmd(wspfile, fn) << dependProjbldConf->GetCustomBuildCmd() << wxT("\n");
				CreateCustomPostBuildEvents(dependProjbldConf, text);
				
			} else {
				PRINT_TIMESTAMP(wxString::Format(wxT("Generating makefile for project %s...\n"), dependProj->GetName().c_str()));
				// generate the dependency project makefile
				GenerateMakefile(dependProj, projectSelConf, confToBuild.IsEmpty() ? force : true);
				text << wxT("\t") << GetCdCmd(wspfile, fn) << buildTool << wxT(" \"") << dependProj->GetName() << wxT(".mk\"\n");
				PRINT_TIMESTAMP(wxString::Format(wxT("Generating makefile for project %s...done\n"), dependProj->GetName().c_str()));
			}
		}
	}

	// Generate makefile for the project itself
	GenerateMakefile(proj, confToBuild, confToBuild.IsEmpty() ? force : true);

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

	text << wxT("\t@echo ") << BUILD_PROJECT_PREFIX << project << wxT(" - ") << projectSelConf << wxT(" ]----------\n");

	//make the paths relative
	wxFileName projectPath(proj->GetFileName());
	projectPath.MakeRelativeTo(wspfile.GetPath());
	text << wxT("\t") << GetCdCmd(wspfile, projectPath) << buildTool << wxT(" \"") << proj->GetName() << wxT(".mk\" \n");

	//create the clean target
	text << wxT("clean:\n");
	if (!isProjectOnly) {
		for (size_t i=0; i<depsArr.GetCount(); i++) {
			bool isCustom(false);
			wxString projectSelConf = matrix->GetProjectSelectedConf(workspaceSelConf, depsArr.Item(i));

			ProjectPtr dependProj = WorkspaceST::Get()->FindProjectByName(depsArr.Item(i), errMsg);
			//Missing dependencies project? just skip it
			if (!dependProj) {
				continue;
			}

			text << wxT("\t@echo ") << CLEAN_PROJECT_PREFIX << dependProj->GetName() << wxT(" - ") << projectSelConf << wxT(" ]----------\n");

			//make the paths relative
			wxFileName fn(dependProj->GetFileName());
			fn.MakeRelativeTo(wspfile.GetPath());

			//if the dependencie project is project of type 'Custom Build' - do the custom build instead
			//of the geenrated makefile
			BuildConfigPtr dependProjbldConf = WorkspaceST::Get()->GetProjBuildConf(dependProj->GetName(), projectSelConf);
			if (dependProjbldConf && dependProjbldConf->IsCustomBuild()) {
				isCustom = true;
			}

			// if we are using custom build command, invoke it instead of calling the generated makefile
			if ( !isCustom ) {
				text << wxT("\t") << GetCdCmd(wspfile, fn) << buildTool << wxT(" \"") << dependProj->GetName() << wxT(".mk\"  clean\n");
			} else {
				text << wxT("\t") << GetCdCmd(wspfile, fn) << dependProjbldConf->GetCustomCleanCmd() << wxT("\n");
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

	text << wxT("\t@echo ") << CLEAN_PROJECT_PREFIX << project << wxT(" - ") << projectSelConf << wxT(" ]----------\n");

	//make the paths relative
	text << wxT("\t") << GetCdCmd(wspfile, projectPath) << buildTool << wxT(" \"") << proj->GetName() << wxT(".mk\" clean\n") ;
	PRINT_TIMESTAMP(wxT("Generating makefile...done\n"));
	
	//dump the content to file
	PRINT_TIMESTAMP(wxT("Writing makefile...\n"));
	wxFileOutputStream output(fn);
	wxStringInputStream content(text);
	output << content;
	PRINT_TIMESTAMP(wxT("Writing makefile...done\n"));
	
	return true;
}

void BuilderGnuMake::GenerateMakefile(ProjectPtr proj, const wxString &confToBuild, bool force)
{
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
	text << wxT("## Auto Generated makefile, please do not edit") << wxT("\n");;
	text << wxT("##") << wxT("\n");

	//----------------------------------------------------------
	//copy environment variables to the makefile
	//----------------------------------------------------------
	EvnVarList vars;
	EnvironmentConfig::Instance()->ReadObject(wxT("Variables"), &vars);
	StringMap varMap = vars.GetVariables();
	StringMap::const_iterator iter = varMap.begin();

	for (; iter != varMap.end(); iter++) {
		wxString name = iter->first;
		wxString value = iter->second;
		text << name << wxT(":=") << value << wxT("") << wxT("\n");
	}

	//create a variable for the project name as well
	text << wxT("ProjectName:=") << proj->GetName() << wxT("\n");
	text << wxT("\n");

	CreateConfigsVariables(proj, bldConf, text);

	// create a list of objects
	CreateObjectList(proj, confToBuild, text);

	//-----------------------------------------------------------
	// create the build targets
	//-----------------------------------------------------------
	text << wxT("##\n");
	text << wxT("## Main Build Tragets \n");
	text << wxT("##\n");

	//incase project is type exe or dll, force link
	//this is to workaround bug in the generated makefiles
	//which causes the makefile to report 'nothing to be done'
	//even when a dependencie was modified
	wxString targetName(bldConf->GetIntermediateDirectory());

	//dont always add the 'PrePreBuild' dependenice, or else it will
	//lead to constant archiving of static libraries, even when there is
	//"Nothing to be done for 'all'" :D
	wxString prePreBuildTarget(wxT("PrePreBuild"));
	if (bldConf->GetPreBuildCustom().IsEmpty()) {
		prePreBuildTarget = wxEmptyString;
	}

	if (settings->GetProjectType(bldConf->GetName()) == Project::EXECUTABLE || settings->GetProjectType(bldConf->GetName()) == Project::DYNAMIC_LIBRARY) {
		text << wxT("all: $(OutputFile)\n\n");
		if ( HasPrebuildCommands(bldConf) ) {
			text << wxT("$(OutputFile): makeDirStep ") << prePreBuildTarget << wxT(" PreBuild $(Objects)\n");
		} else {
			text << wxT("$(OutputFile): makeDirStep ") << prePreBuildTarget << wxT(" $(Objects)\n");
		}
		targetName = wxT("makeDirStep");
	} else {
		text << wxT("all: $(IntermediateDirectory) $(OutputFile)\n\n");
		if (HasPrebuildCommands(bldConf)) {
			text << wxT("$(OutputFile): ") << prePreBuildTarget << wxT(" PreBuild $(Objects)\n");
		} else {
			text << wxT("$(OutputFile): ") << prePreBuildTarget << wxT(" $(Objects)\n");
		}
	}

	if (bldConf->IsLinkerRequired()) {
		CreateTargets(proj->GetSettings()->GetProjectType(bldConf->GetName()), bldConf, text);
	}
	CreatePostBuildEvents(bldConf, text);
	CreateMakeDirsTarget(bldConf, targetName, text);
	CreatePreBuildEvents(bldConf, text);

	//-----------------------------------------------------------
	// Create a list of targets that should be built according to
	// projects' file list
	//-----------------------------------------------------------
	CreateFileTargets(proj, confToBuild, text);

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

void BuilderGnuMake::CreateObjectList(ProjectPtr proj, const wxString &confToBuild, wxString &text)
{
	std::vector<wxFileName> files;
	proj->GetFiles(files);
	text << wxT("Objects=");

	BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjBuildConf(proj->GetName(), confToBuild);
	wxString cmpType = bldConf->GetCompilerType();

	//get the compiler settings
	CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(cmpType);

	int counter = 1;
	Compiler::CmpFileTypeInfo ft;

	for (size_t i=0; i<files.size(); i++) {

		// is this a valid file?
		if (!cmp->GetCmpFileType(files[i].GetExt(), ft))
			continue;

		if ((OS_WINDOWS) && ft.kind == Compiler::CmpFileKindResource && bldConf && !bldConf->IsResCompilerRequired()) {
			// we are on Windows, the file type is Resource and resource compiler is not required
			continue;
		}

		if (ft.kind == Compiler::CmpFileKindResource) {
			// resource files are handled differently
			text << wxT("$(IntermediateDirectory)/") << files[i].GetFullName() << wxT("$(ObjectSuffix) ");
		} else {
			// Compiler::CmpFileKindSource file
			text << wxT("$(IntermediateDirectory)/") << files[i].GetName() << wxT("$(ObjectSuffix) ");
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
	bool generateDependeciesFiles(cmp->GetGenerateDependeciesFile());
	
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
	PRINT_TIMESTAMP(wxT("Looping over the file list...\n"));
	for (size_t i=0; i<abs_files.size(); i++) {
		// is this file interests the compiler?
		if (cmp->GetCmpFileType(abs_files.at(i).GetExt().Lower(), ft) ) {
			wxString absFileName;
			wxFileName fn( abs_files.at(i) );
			
			wxString filenameOnly = fn.GetName();
			wxString fullpathOnly = fn.GetFullPath();
			wxString fullnameOnly = fn.GetFullName();
			
			wxString compilationLine = ft.compilation_line;

			compilationLine.Replace(wxT("$(FileName)"), filenameOnly);
			compilationLine.Replace(wxT("$(FileFullName)"), fullnameOnly);
			compilationLine.Replace(wxT("$(FileFullPath)"), fullpathOnly);

			// use UNIX style slashes
			absFileName = abs_files[i].GetFullPath();
			absFileName.Replace(wxT("\\"), wxT("/"));
			compilationLine.Replace(wxT("\\"), wxT("/"));

			if (ft.kind == Compiler::CmpFileKindSource) {
				wxString objectName;
				wxString dependFile;

				objectName << wxT("$(IntermediateDirectory)/") << filenameOnly << wxT("$(ObjectSuffix)");
				if (generateDependeciesFiles) {
					dependFile << wxT("$(IntermediateDirectory)/") << filenameOnly << wxT("$(ObjectSuffix)") << wxT(".d");
				}

				// set the file rule
				text << objectName << wxT(": ") << rel_paths.at(i).GetFullPath(wxPATH_UNIX) << wxT(" ") << dependFile << wxT("\n");
				text << wxT("\t") << GetMakeDirCmd(bldConf) << wxT("\n");
				text << wxT("\t") << compilationLine << wxT("\n");

				if (generateDependeciesFiles) {
					//add the dependencie rule
					text << dependFile << wxT(": ") << rel_paths.at(i).GetFullPath(wxPATH_UNIX) << wxT("\n");
					text << wxT("\t") << GetMakeDirCmd(bldConf) << wxT("\n");
					text << wxT("\t") << wxT("@$(CompilerName) $(CmpOptions) $(IncludePath) -MT") << objectName <<wxT(" -MF") << dependFile << wxT(" -MM \"") << absFileName << wxT("\"\n\n");
				}

			} else if (ft.kind == Compiler::CmpFileKindResource && bldConf->IsResCompilerRequired() && OS_WINDOWS ) {
				//Windows only
				// we construct an object name which also includes the full name of the reousrce file and appends a .o to the name (to be more
				// precised, $(ObjectSuffix))
				wxString objectName;
				objectName << wxT("$(IntermediateDirectory)/") << fullnameOnly << wxT("$(ObjectSuffix)");

				text << objectName << wxT(": ") << rel_paths.at(i).GetFullPath(wxPATH_UNIX) << wxT("\n");
				text << wxT("\t") << GetMakeDirCmd(bldConf) << wxT("\n");
				text << wxT("\t") << compilationLine << wxT("\n");
			}
		}
	}
	PRINT_TIMESTAMP(wxT("Looping over the file list...done\n"));
	
	//add clean target
	text << wxT("##\n");
	text << wxT("## Clean\n");
	text << wxT("##\n");
	text << wxT("clean:\n");

	if (OS_WINDOWS) {
		//windows clean command
		for (size_t i=0; i<abs_files.size(); i++) {

			Compiler::CmpFileTypeInfo ft;
			if (cmp->GetCmpFileType(abs_files[i].GetExt(), ft)) {

				if (ft.kind == Compiler::CmpFileKindSource) {

					wxString objectName = abs_files[i].GetName() << wxT("$(ObjectSuffix)");
					wxString dependFile = abs_files[i].GetName() << wxT("$(ObjectSuffix)") << wxT(".d");
					text << wxT("\t") << wxT("$(RM) ") << wxT("$(IntermediateDirectory)/") << objectName << wxT("\n");
					text << wxT("\t") << wxT("$(RM) ") << wxT("$(IntermediateDirectory)/") << dependFile << wxT("\n");

				} else if (ft.kind == Compiler::CmpFileKindResource && bldConf->IsResCompilerRequired()) {
					wxString ofile = abs_files[i].GetFullName() << wxT("$(ObjectSuffix)");
					text << wxT("\t") << wxT("$(RM) ") << wxT("$(IntermediateDirectory)/") << ofile << wxT("\n");
				}
			}
		}

		//delete the output file as well
		wxString exeExt(wxEmptyString);
		if (proj->GetSettings()->GetProjectType(bldConf->GetName()) == Project::EXECUTABLE) {
			//under windows, g++ automatically adds the .exe extension to executable
			//make sure we deletes it as well
			exeExt = wxT(".exe");
		}
		text << wxT("\t") << wxT("$(RM) ") << wxT("$(OutputFile)") << wxT("\n");;
		text << wxT("\t") << wxT("$(RM) ") << wxT("$(OutputFile)") << exeExt << wxT("\n");;
	} else {
		//on linux we dont really need resource compiler...
		for (size_t i=0; i<abs_files.size(); i++) {
			Compiler::CmpFileTypeInfo ft;
			if (cmp->GetCmpFileType(abs_files[i].GetExt(), ft) && ft.kind == Compiler::CmpFileKindSource) {

				wxString objectName = abs_files[i].GetName() << wxT("$(ObjectSuffix)");
				wxString dependFile = abs_files[i].GetName() << wxT("$(ObjectSuffix)") << wxT(".d");

				text << wxT("\t") << wxT("$(RM) ") << wxT("$(IntermediateDirectory)/") << objectName << wxT("\n");
				text << wxT("\t") << wxT("$(RM) ") << wxT("$(IntermediateDirectory)/") << dependFile << wxT("\n");
			}
		}

		//delete the output file as well
		text << wxT("\t") << wxT("$(RM) ") << wxT("$(OutputFile)\n");
	}

	if (generateDependeciesFiles) {
		text << wxT("\n-include $(IntermediateDirectory)/*.d\n");
	}
	text << wxT("\n\n");
	PRINT_TIMESTAMP(wxT("Creating file targets...done\n"));
}

void BuilderGnuMake::CreateTargets(const wxString &type, BuildConfigPtr bldConf, wxString &text)
{
	if (OS_WINDOWS) {
		text << wxT("\t") << wxT("@makedir $(@D)\n");
	} else {
		text << wxT("\t") << wxT("@mkdir -p $(@D)\n");
	}

	if (type == Project::STATIC_LIBRARY) {
		//create a static library
		text << wxT("\t") << wxT("$(ArchiveTool) $(ArchiveOutputSwitch)$(OutputFile) $(Objects)\n");
	} else if (type == Project::DYNAMIC_LIBRARY) {
		//create a shared library
		text << wxT("\t") << wxT("$(SharedObjectLinkerName) $(OutputSwitch)$(OutputFile) $(Objects) $(LibPath) $(Libs) $(LinkOptions)\n");
	} else if (type == Project::EXECUTABLE) {
		//create an executable
		text << wxT("\t") << wxT("$(LinkerName) $(OutputSwitch)$(OutputFile) $(Objects) $(LibPath) $(Libs) $(LinkOptions)\n");
	}
}

void BuilderGnuMake::CreatePostBuildEvents(BuildConfigPtr bldConf, wxString &text)
{
	BuildCommandList cmds;
	BuildCommandList::iterator iter;
	wxString name = bldConf->GetName();
	name = NormalizeConfigName(name);

	//generate postbuild commands
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

bool BuilderGnuMake::HasPrebuildCommands(BuildConfigPtr bldConf) const
{
	BuildCommandList cmds;
	BuildCommandList::const_iterator iter;
	bldConf->GetPreBuildCommands(cmds);
	bool first(true);

	if (!cmds.empty()) {
		iter = cmds.begin();
		for (; iter != cmds.end(); iter++) {
			if (iter->GetEnabled()) {
				if (first) {
					first = false;
					break;
				}
			}
		}
	}
	return !first;
}

void BuilderGnuMake::CreatePreBuildEvents(BuildConfigPtr bldConf, wxString &text)
{
	BuildCommandList cmds;
	BuildCommandList::iterator iter;
	wxString name = bldConf->GetName();
	name = NormalizeConfigName(name);

	//add PrePreBuild
	if (!bldConf->GetPreBuildCustom().IsEmpty()) {
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

	text << wxT("## ") << name << wxT("\n");

	// The following two variables are here for compatibility with MSVS
	text << wxT("ConfigurationName :=") << name << wxT("\n");
	text << wxT("IntermediateDirectory :=") << bldConf->GetIntermediateDirectory() << wxT("\n");
	text << wxT("OutDir := $(IntermediateDirectory)\n");
	text << wxT("LinkerName:=") << cmp->GetTool(wxT("LinkerName")) << wxT("\n");
	text << wxT("ArchiveTool :=") << cmp->GetTool(wxT("ArchiveTool")) << wxT("\n");
	text << wxT("SharedObjectLinkerName :=") << cmp->GetTool(wxT("SharedObjectLinkerName")) << wxT("\n");
	text << wxT("ObjectSuffix :=") << cmp->GetObjectSuffix() << wxT("\n");
	text << wxT("DebugSwitch :=") << cmp->GetSwitch(wxT("Debug")) << wxT("\n");
	text << wxT("IncludeSwitch :=") << cmp->GetSwitch(wxT("Include")) << wxT("\n");
	text << wxT("LibrarySwitch :=") << cmp->GetSwitch(wxT("Library")) << wxT("\n");
	text << wxT("OutputSwitch :=") << cmp->GetSwitch(wxT("Output")) << wxT("\n");
	text << wxT("LibraryPathSwitch :=") << cmp->GetSwitch(wxT("LibraryPath")) << wxT("\n");
	text << wxT("PreprocessorSwitch :=") << cmp->GetSwitch(wxT("Preprocessor")) << wxT("\n");
	text << wxT("SourceSwitch :=") << cmp->GetSwitch(wxT("Source")) << wxT("\n");
	text << wxT("CompilerName :=") << cmp->GetTool(wxT("CompilerName")) << wxT("\n");

	text << wxT("OutputFile :=") << bldConf->GetOutputFileName() << wxT("\n");
	text << wxT("Preprocessors :=") << ParsePreprocessor(bldConf->GetPreprocessor()) << wxT("\n");
	text << wxT("ObjectSwitch :=") << cmp->GetSwitch(wxT("Object")) << wxT("\n");
	text << wxT("ArchiveOutputSwitch :=") << cmp->GetSwitch(wxT("ArchiveOutput")) << wxT("\n");

	wxString buildOpts = bldConf->GetCompileOptions();
	buildOpts.Replace(wxT(";"), wxT(" "));
	text << wxT("CmpOptions :=") << buildOpts << wxT(" $(Preprocessors)") << wxT("\n");


	//only if resource compiler required, evaluate the resource variables
	if (bldConf->IsResCompilerRequired()) {
		wxString rcBuildOpts = bldConf->GetResCompileOptions();
		rcBuildOpts.Replace(wxT(";"), wxT(" "));
		text << wxT("RcCmpOptions :=") << rcBuildOpts << wxT("\n");
		text << wxT("RcCompilerName :=") << cmp->GetTool(wxT("ResourceCompiler")) << wxT("\n");
	}

	wxString linkOpt = bldConf->GetLinkOptions();
	linkOpt.Replace(wxT(";"), wxT(" "));

	//link options are kept with semi-colons, strip them
	text << wxT("LinkOptions := ") << linkOpt << wxT("\n");

	// add the global include path followed by the project include path
	text << wxT("IncludePath := ") << ParseIncludePath(cmp->GetGlobalIncludePath(), proj->GetName(), bldConf->GetName()) << wxT(" ") << ParseIncludePath(bldConf->GetIncludePath(), proj->GetName(), bldConf->GetName()) << wxT("\n");

	text << wxT("RcIncludePath :=") << ParseIncludePath(bldConf->GetResCmpIncludePath(), proj->GetName(), bldConf->GetName()) << wxT("\n");
	text << wxT("Libs :=") << ParseLibs(bldConf->GetLibraries()) << wxT("\n");

	// add the global library path followed by the project library path
	text << wxT("LibPath :=") << ParseLibPath(cmp->GetGlobalLibPath(), proj->GetName(), bldConf->GetName()) << wxT(" ") << ParseLibPath(bldConf->GetLibPath(), proj->GetName(), bldConf->GetName()) << wxT("\n");
	text << wxT("\n\n");
}

wxString BuilderGnuMake::ParseIncludePath(const wxString &paths, const wxString &projectName, const wxString &selConf)
{
	//convert semi-colon delimited string into GNU list of
	//include paths:
	wxString incluedPath(wxEmptyString);
	wxStringTokenizer tkz(paths, wxT(";"));
	//prepend each include path with -I
	while (tkz.HasMoreTokens()) {
		wxString path(tkz.NextToken());
		TrimString(path);
		path = ExpandAllVariables(path, WorkspaceST::Get(), projectName, selConf, wxEmptyString);
		path.Replace(wxT("\\"), wxT("/"));
		incluedPath << wxT("\"$(IncludeSwitch)") << path << wxT("\" ");
	}
	return incluedPath;
}

wxString BuilderGnuMake::ParseLibPath(const wxString &paths, const wxString &projectName, const wxString &selConf)
{
	//convert semi-colon delimited string into GNU list of
	//lib path
	wxString libPath(wxEmptyString);
	wxStringTokenizer tkz(paths, wxT(";"));
	//prepend each include path with libpath switch
	while (tkz.HasMoreTokens()) {
		wxString path(tkz.NextToken());
		TrimString(path);
		path = ExpandAllVariables(path, WorkspaceST::Get(), projectName, selConf, wxEmptyString);
		path.Replace(wxT("\\"), wxT("/"));
		libPath << wxT("\"$(LibraryPathSwitch)") << path << wxT("\" ");
	}
	return libPath;
}

wxString BuilderGnuMake::ParsePreprocessor(const wxString &prep)
{
	wxString preprocessor(wxEmptyString);
	wxStringTokenizer tkz(prep, wxT(";"));
	//prepend each include path with libpath switch
	while (tkz.HasMoreTokens()) {
		wxString p(tkz.NextToken());
		TrimString(p);
		preprocessor << wxT("$(PreprocessorSwitch)") << p << wxT(" ");
	}
	return preprocessor;
}

wxString BuilderGnuMake::ParseLibs(const wxString &libs)
{
	//convert semi-colon delimited string into GNU list of
	//libs
	wxString slibs(wxEmptyString);
	wxStringTokenizer tkz(libs, wxT(";"));
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
	buildTool = WorkspaceST::Get()->ExpandVariables(buildTool);

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
	buildTool = WorkspaceST::Get()->ExpandVariables(buildTool);

	BuildMatrixPtr matrix = WorkspaceST::Get()->GetBuildMatrix();
	wxString type = Builder::NormalizeConfigName(matrix->GetSelectedConfigurationName());
	cmd << buildTool << wxT(" \"") << WorkspaceST::Get()->GetName() << wxT("_wsp.mk\" clean");
	return cmd;
}

wxString BuilderGnuMake::GetPOBuildCommand(const wxString &project, const wxString &confToBuild)
{
	wxString errMsg, cmd;
	BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjBuildConf(project, confToBuild);
	if (!bldConf) {
		return wxEmptyString;
	}

	//generate the makefile
	Export(project, confToBuild, true, false, errMsg);

	BuildMatrixPtr matrix = WorkspaceST::Get()->GetBuildMatrix();
	wxString buildTool = BuildManagerST::Get()->GetSelectedBuilder()->GetBuildToolCommand(true);
	buildTool = WorkspaceST::Get()->ExpandVariables(buildTool);

	wxString type = matrix->GetProjectSelectedConf(matrix->GetSelectedConfigurationName(), project);
	if (confToBuild.IsEmpty() == false) {
		// incase we use to generate a 'Project Only' makefile,
		// we allow the caller to override the selected configuration with 'confToBuild' parameter
		type = confToBuild;
	}
	//cd to the project directory
	cmd << buildTool << wxT(" \"") << project << wxT(".mk\"") ;
	return cmd;
}

wxString BuilderGnuMake::GetPOCleanCommand(const wxString &project, const wxString &confToBuild)
{
	wxString errMsg, cmd;
	BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjBuildConf(project, confToBuild);
	if (!bldConf) {
		return wxEmptyString;
	}

	//generate the makefile
	Export(project, confToBuild, true, false, errMsg);

	BuildMatrixPtr matrix = WorkspaceST::Get()->GetBuildMatrix();
	wxString buildTool = BuildManagerST::Get()->GetSelectedBuilder()->GetBuildToolCommand(true);
	buildTool = WorkspaceST::Get()->ExpandVariables(buildTool);

	wxString type = matrix->GetProjectSelectedConf(matrix->GetSelectedConfigurationName(), project);
	if (confToBuild.IsEmpty() == false) {
		// incase we use to generate a 'Project Only' makefile,
		// we allow the caller to override the selected configuration with 'confToBuild' parameter
		type = confToBuild;
	}
	//cd to the project directory
	cmd << buildTool << wxT(" \"") << project << wxT(".mk\" clean");
	return cmd;
}

wxString BuilderGnuMake::GetSingleFileCmd(const wxString &project, const wxString &confToBuild, const wxString &fileName, wxString &errMsg)
{
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

	//create the target
	wxString tareget;
	wxString objSuffix;
	wxFileName fn(fileName);

	wxString cmpType = bldConf->GetCompilerType();
	CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(cmpType);

	tareget << bldConf->GetIntermediateDirectory() << wxT("/") << fn.GetName() << cmp->GetObjectSuffix();
	cmd << buildTool << wxT(" \"") << project << wxT(".mk\" ") << tareget;

	return EnvironmentConfig::Instance()->ExpandVariables(cmd);
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
