#include "clang_code_completion.h"
#include "environmentconfig.h"
#include "processreaderthread.h"
#include "globals.h"
#include "jobqueue.h"
#include "fileextmanager.h"
#include <wx/tokenzr.h>
#include "ieditor.h"
#include "imanager.h"
#include "workspace.h"
#include "project.h"
#include <wx/filename.h>
#include <wx/dir.h>
#include <wx/arrstr.h>
#include "procutils.h"

static wxStopWatch gStopWatch;

BEGIN_EVENT_TABLE(ClangCodeCompletion, wxEvtHandler)
EVT_COMMAND(wxID_ANY, wxEVT_PROC_DATA_READ,  ClangCodeCompletion::OnProcessOutput)
EVT_COMMAND(wxID_ANY, wxEVT_PROC_TERMINATED, ClangCodeCompletion::OnProcessTerminated)
END_EVENT_TABLE()

ClangCodeCompletion* ClangCodeCompletion::ms_instance = 0;

ClangCodeCompletion::ClangCodeCompletion()
	: m_manager(NULL)
	, m_process(NULL)
{
}

ClangCodeCompletion::~ClangCodeCompletion()
{
}

void ClangCodeCompletion::Initialize(IManager* manager)
{
	m_manager = manager;
}

ClangCodeCompletion* ClangCodeCompletion::Instance()
{
	if(ms_instance == 0) {
		ms_instance = new ClangCodeCompletion();
	}
	return ms_instance;
}

void ClangCodeCompletion::Release()
{
	if(ms_instance) {
		delete ms_instance;
	}
	ms_instance = 0;
}

void ClangCodeCompletion::CodeComplete(IEditor* editor)
{
	// Sanity:
	if(!editor || !m_manager || !m_manager->GetWorkspace() || !m_manager->IsWorkspaceOpen())
		return;

	// First, we need to build the command line
	// try to locate the basic include paths:
	wxArrayString args = GetStandardIncludePathsArgs();

	// Next apppend the user include paths
	wxString errMsg;

	// First, we need to find the currently active workspace configuration
	BuildMatrixPtr matrix = m_manager->GetWorkspace()->GetBuildMatrix();
	if(!matrix) {
		return;
	}

	wxString workspaceSelConf = matrix->GetSelectedConfigurationName();

	// Now that we got the selected workspace configuration, extract the related project configuration
	ProjectPtr proj =  m_manager->GetWorkspace()->FindProjectByName(editor->GetProjectName(), errMsg);
	if(!proj) {
		return;
	}

	wxString currentBuffer = editor->GetTextRange(0, editor->GetCurrentPosition());
	if(currentBuffer.IsEmpty())
		return;

	wxString projectSelConf = matrix->GetProjectSelectedConf(workspaceSelConf, proj->GetName());
	BuildConfigPtr dependProjbldConf = m_manager->GetWorkspace()->GetProjBuildConf(proj->GetName(), projectSelConf);
	if(dependProjbldConf && dependProjbldConf->IsCustomBuild() == false) {
		// Get the include paths and add them
		wxString projectIncludePaths = dependProjbldConf->GetIncludePath();
		wxArrayString projectIncludePathsArr = wxStringTokenize(projectIncludePaths, wxT(";"), wxTOKEN_STRTOK);
		for(size_t i=0; i<projectIncludePathsArr.GetCount(); i++) {
			args.Add( wxString::Format(wxT("-I%s"), projectIncludePathsArr[i].c_str()) );
		}

		// get the compiler options and add them
		wxString projectCompileOptions = dependProjbldConf->GetCompileOptions();
		wxArrayString projectCompileOptionsArr = wxStringTokenize(projectCompileOptions, wxT(";"), wxTOKEN_STRTOK);
		for(size_t i=0; i<projectCompileOptionsArr.GetCount(); i++) {
			wxString cmpOption (projectCompileOptionsArr.Item(i));
			cmpOption.Trim().Trim(false);

			wxString tmp;
			// Expand backticks / $(shell ...) syntax supported by codelite
			if(cmpOption.StartsWith(wxT("$(shell "), &tmp) || cmpOption.StartsWith(wxT("`"), &tmp)) {
				cmpOption = tmp;

				tmp.Clear();
				if(cmpOption.EndsWith(wxT(")"), &tmp) || cmpOption.EndsWith(wxT("`"), &tmp)) {
					cmpOption = tmp;
				}

				if(m_backticks.find(cmpOption) == m_backticks.end()) {

					// Expand the backticks into their value
					wxArrayString outArr;
					// Apply the environment before executing the command
					EnvSetter setter( EnvironmentConfig::Instance() );
					ProcUtils::SafeExecuteCommand(cmpOption, outArr);
					wxString expandedValue;
					for(size_t j=0; j<outArr.size(); j++)
					{
						expandedValue << outArr.Item(j) << wxT(" ");
					}
					m_backticks[cmpOption] = expandedValue;
					cmpOption = expandedValue;
				} else {
					cmpOption = m_backticks.find(cmpOption)->second;
				}
			}
			args.Add( cmpOption );
		}

		// get the compiler preprocessor and add them as well
		wxString projectPreps = dependProjbldConf->GetPreprocessor();
		wxArrayString projectPrepsArr = wxStringTokenize(projectPreps, wxT(";"), wxTOKEN_STRTOK);
		for(size_t i=0; i<projectPrepsArr.GetCount(); i++) {
			args.Add( wxString::Format(wxT("-D%s"), projectPrepsArr[i].c_str()) );
		}

		wxString filterWord;

		// Move backward until we found our -> or :: or .
		for(size_t i=currentBuffer.Length() - 1; i>0; i--) {
			if(currentBuffer.EndsWith(wxT("->")) || currentBuffer.EndsWith(wxT(".")) || currentBuffer.EndsWith(wxT("::"))) {
				break;
			} else {
				filterWord.Prepend(currentBuffer.GetChar(i));
				currentBuffer.Truncate(currentBuffer.Length() - 1);
			}
		}

		// Get the current line's starting pos
		int lineStartPos = editor->PosFromLine( editor->GetCurrentLine() );
		int column       = editor->GetCurrentPosition() - lineStartPos  + 1;
		int line         = editor->GetCurrentLine() + 1;

		column -= (int)filterWord.Length();

		// Create temp file
		wxString tmpfileName;
		tmpfileName << editor->GetFileName().GetPath(wxPATH_GET_SEPARATOR|wxPATH_GET_VOLUME) << editor->GetFileName().GetName() << wxT("_clang_tmp") << wxT(".cpp");

		wxString command;

		command << wxT("clang -w -fsyntax-only -Xclang -code-completion-at=");
		FileExtManager::FileType type = FileExtManager::GetType(editor->GetFileName().GetFullPath());
		if(type == FileExtManager::TypeSourceC || type == FileExtManager::TypeSourceCpp) {
			if(!WriteFileWithBackup(tmpfileName, currentBuffer, false)) {
				wxLogMessage(wxT("Failed to write temp file: %s"), tmpfileName.c_str());
				return;
			}
			wxString completefileName;
			completefileName << editor->GetFileName().GetName() << wxT("_clang_tmp") << wxT(".cpp");
			command << completefileName << wxT(":") << line << wxT(":") << column << wxT(" ") << completefileName;

		} else {
			wxString implFile;
			implFile << wxT("#include <") << editor->GetFileName().GetFullName() << wxT(">\n");
			if(!WriteFileWithBackup(tmpfileName, implFile, false)) {
				wxLogMessage(wxT("Failed to write temp file: %s"), implFile.c_str());
				return;
			}
			wxString completefileName;
			completefileName << editor->GetFileName().GetName() << wxT("_clang_tmp") << wxT(".cpp");
			command << editor->GetFileName().GetFullPath() << wxT(":") << line << wxT(":") << column << wxT(" ") << completefileName;
		}

		// Add the arguments
		wxString argString;
		for(size_t i=0; i<args.size(); i++) {
			argString << wxT(" ") << args.Item(i);
		}

		command << argString;
		if(m_process) {
			delete m_process;
			m_process = NULL;

			m_output.Clear();
		}

		wxLogMessage(wxT("Running: %s"), command.c_str());
		m_process = CreateAsyncProcess(this, command, editor->GetFileName().GetPath(wxPATH_GET_SEPARATOR|wxPATH_GET_VOLUME));
		if(! m_process ) {
			wxLogMessage(wxT("Failed to start process: %s"), command.c_str());
			return;
		}
		gStopWatch.Start();
	}
}

wxArrayString ClangCodeCompletion::GetStandardIncludePathsArgs()
{
	static wxArrayString paths;
#ifdef __WXMSW__
	if(paths.IsEmpty() == false)
		return paths;

	wxString      standardIncludeBase;
	wxGetEnv(wxT("MINGW_INCL_HOME"), &standardIncludeBase);
	if (standardIncludeBase.IsEmpty() == false && wxDir::Exists(standardIncludeBase)) {

		wxString mingwBaseDir (standardIncludeBase );

		// "C:/MinGW-4.4.1/include"
		standardIncludeBase.Prepend(wxT("-I"));
		paths.Add(standardIncludeBase + wxT("\\include"));
		standardIncludeBase << wxT("\\lib\\gcc\\mingw32\\");
		mingwBaseDir        << wxT("\\lib\\gcc\\mingw32\\");

		long          highestVersion(0);
		wxString      sHighestVersion;
		wxArrayString files;

		if (wxDir::Exists( mingwBaseDir ) ) {
			wxDir::GetAllFiles(mingwBaseDir, &files, wxEmptyString, wxDIR_DIRS|wxDIR_FILES);

			//filter out all non-directories
			for (size_t i=0; i<files.GetCount(); i++) {
				wxFileName fn(files.Item(i));

				wxString p = fn.GetPath().Mid( mingwBaseDir.Length() );
				wxString tmp_p(p);
				tmp_p.Replace(wxT("."), wxT(""));
				long number(0);
				tmp_p.ToLong( &number );
				if (number && number > highestVersion) {
					sHighestVersion = p.BeforeFirst(wxFileName::GetPathSeparator());
					highestVersion  = number;
				}
			}

			if (sHighestVersion.IsEmpty() == false) {
				standardIncludeBase << sHighestVersion << wxT("\\include");
				paths.Add( standardIncludeBase );
				paths.Add( standardIncludeBase + wxT("\\c++") );
				paths.Add( standardIncludeBase + wxT("\\c++\\mingw32") );
			}
		}
	}
	return paths;
#else
	return paths;
#endif
}

void ClangCodeCompletion::OnProcessTerminated(wxCommandEvent& e)
{
	//wxLogMessage(wxT("clang process ended after %d ms"), gStopWatch.Time());

	ProcessEventData *ped = (ProcessEventData*) e.GetClientData();
	delete ped;
	delete m_process;
	m_process = NULL;
}

void ClangCodeCompletion::OnProcessOutput(wxCommandEvent& e)
{
	ProcessEventData *ped = (ProcessEventData*) e.GetClientData();
	if(ped) {
		m_output << ped->GetData();
		delete ped;
	}
	e.Skip();
}
