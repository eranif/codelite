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
	: m_manager         (NULL)
	, m_process         (NULL)
	, m_activationPos   (wxNOT_FOUND)
	, m_activationEditor(NULL)
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
	// TODO :: this code works but for now we disable it until we
	// add an option in the UI for it
#if 0
	// Sanity:
	if(!editor || !m_manager || !m_manager->GetWorkspace() || !m_manager->IsWorkspaceOpen())
		return;
	
	// clang process is already running
	if(m_process)
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
		m_tmpfile.clear();
		m_tmpfile << editor->GetFileName().GetPath(wxPATH_GET_SEPARATOR|wxPATH_GET_VOLUME) << editor->GetFileName().GetName() << wxT("_clang_tmp") << wxT(".cpp");

		wxString command;

		command << wxT("clang -w -fsyntax-only -Xclang -code-completion-at=");
		FileExtManager::FileType type = FileExtManager::GetType(editor->GetFileName().GetFullPath());
		if(type == FileExtManager::TypeSourceC || type == FileExtManager::TypeSourceCpp) {
			if(!WriteFileWithBackup(m_tmpfile, currentBuffer, false)) {
				wxLogMessage(wxT("Failed to write temp file: %s"), m_tmpfile.c_str());
				return;
			}
			wxString completefileName;
			completefileName << editor->GetFileName().GetName() << wxT("_clang_tmp") << wxT(".cpp");
			command << completefileName << wxT(":") << line << wxT(":") << column << wxT(" ") << completefileName;

		} else {
			wxString implFile;
			implFile << wxT("#include <") << editor->GetFileName().GetFullName() << wxT(">\n");
			if(!WriteFileWithBackup(m_tmpfile, implFile, false)) {
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
			DoCleanUp();
		}
		
		command.Replace(wxT("\n"), wxT(" "));
		command.Replace(wxT("\r"), wxT(" "));
		
		m_process = CreateAsyncProcess(this, command, editor->GetFileName().GetPath(wxPATH_GET_SEPARATOR|wxPATH_GET_VOLUME));
		if(! m_process ) {
			wxLogMessage(wxT("Failed to start process: %s"), command.c_str());
			return;
		}
		
		m_activationEditor = editor;
		m_activationPos    = lineStartPos + column - 1;
		
		gStopWatch.Start();
	}
#endif
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
	ProcessEventData *ped = (ProcessEventData*) e.GetClientData();
	delete ped;
	
	//wxLogMessage(wxT("clang process ended after %d ms"), gStopWatch.Time());
	
	// Parse the output
	if(m_output.IsEmpty() == false) {
		DoParseOutput();
	}
	DoCleanUp();
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

void ClangCodeCompletion::DoParseOutput()
{
	// Sanity
	if(m_output.IsEmpty() || !m_activationEditor || m_activationPos == wxNOT_FOUND)
		return;
		
	wxArrayString entries = wxStringTokenize(m_output, wxT("\n\r"), wxTOKEN_STRTOK);
	std::vector<TagEntryPtr> tags;
	tags.reserve( entries.size() );
	
	for(size_t i=0; i<entries.GetCount(); i++) {
		entries.Item(i).Trim().Trim(false);
		if(entries.Item(i).IsEmpty())
			continue;
		
		TagEntryPtr tag = ClangEntryToTagEntry( entries.Item(i) );
		if(tag) {
			tags.push_back( tag );
		}
		
	}
	
	if(tags.empty() == false) {
		int curline = m_activationEditor->GetCurrentLine();
		int actline = m_activationEditor->LineFromPos(m_activationPos);
		if(curline != actline)
			return;
		// we are still on the same line
		
		// We need to make sure that the caret is still infront of the completion char
		if(m_activationEditor->GetCurrentPosition() < m_activationPos)
			return;
		
		// Get the text between the current position and the activation pos and filter all results
		// that dont match
		wxString filter = m_activationEditor->GetTextRange(m_activationPos, m_activationEditor->GetCurrentPosition());
		if(filter.IsEmpty() == false) {
			filter.MakeLower();
			
			std::vector<TagEntryPtr> filteredTags;
			filteredTags.reserve( tags.size() );
			for(size_t i=0; i<tags.size(); i++) {
				wxString n = tags.at(i)->GetName();
				n.MakeLower();
				
				if(n.StartsWith(filter)) {
					filteredTags.push_back(tags.at(i));
				}
			}
			
			if(filteredTags.empty() == false) {
				m_activationEditor->ShowCompletionBox(filteredTags, filter, NULL);
			}
		} else {
			m_activationEditor->ShowCompletionBox(tags, wxEmptyString, NULL);
		}
	}
}

TagEntryPtr ClangCodeCompletion::ClangEntryToTagEntry(const wxString& line)
{
	// an example of line:
	// COMPLETION: OpenFile : [#bool#]OpenFile(<#class wxString const &fileName#>{#, <#class wxString const &projectName#>{#, <#int lineno#>#}#})
	wxString tmp;
	if(line.StartsWith(wxT("COMPLETION: "), &tmp) == false)
		return NULL;
	
	if(line.Contains(wxT("(Hidden)")))
		return NULL;
	
	// Next comes the name of the entry
	wxString name = tmp.BeforeFirst(wxT(':'));
	name.Trim().Trim(false);
	
	if(name.IsEmpty())
		return NULL;
	
	TagEntry *t = new TagEntry();
	TagEntryPtr tag(t);
	tag->SetName( name );
	
	tmp = tmp.AfterFirst(wxT(':'));
	tmp.Trim().Trim(false);
	
	// determine the kind
	tag->SetKind(wxT("prototype")); // default
	
	if(tmp == name) {
		// this a type (enum/typedef/internal class)
		tag->SetKind(wxT("class"));
	} else if(tmp.Contains(wxT("("))) {
		// definitly a method, do nothing
	} else {
		tag->SetKind(wxT("member"));
	}
	
	return tag;
}

void ClangCodeCompletion::DoCleanUp()
{
	delete m_process;
	m_process = NULL;
	
	// remove the temporary file
	if(m_tmpfile.IsEmpty() == false) {
#ifndef __WXMSW__
		::unlink( m_tmpfile.mb_str(wxConvUTF8).data() );
#endif
		wxRemoveFile( m_tmpfile );
		m_tmpfile.Clear();
	}
	
	m_output.Clear();
	m_activationEditor = NULL;
	m_activationPos    = wxNOT_FOUND;
}

void ClangCodeCompletion::CancelCodeComplete()
{
	DoCleanUp();
}
