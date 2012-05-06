#if HAS_LIBCLANG

#include "clang_code_completion.h"
#include "pluginmanager.h"
#include "shell_command.h"
#include "compiler_command_line_parser.h"
#include "event_notifier.h"
#include "ctags_manager.h"
#include "tags_options_data.h"
#include "includepathlocator.h"
#include "environmentconfig.h"
#include "file_logger.h"
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
#include "clang_local_paths.h"

ClangCodeCompletion* ClangCodeCompletion::ms_instance = 0;

ClangCodeCompletion::ClangCodeCompletion()
	: m_allEditorsAreClosing(false)
	, m_parseBuildOutput(false)
{
	EventNotifier::Get()->Connect(wxEVT_ACTIVE_EDITOR_CHANGED,         wxCommandEventHandler(ClangCodeCompletion::OnFileLoaded),        NULL, this);
	EventNotifier::Get()->Connect(wxEVT_FILE_SAVED,                    wxCommandEventHandler(ClangCodeCompletion::OnFileSaved),         NULL, this);
	EventNotifier::Get()->Connect(wxEVT_ALL_EDITORS_CLOSING,           wxCommandEventHandler(ClangCodeCompletion::OnAllEditorsClosing), NULL, this);
	EventNotifier::Get()->Connect(wxEVT_ALL_EDITORS_CLOSED,            wxCommandEventHandler(ClangCodeCompletion::OnAllEditorsClosed ), NULL, this);
	EventNotifier::Get()->Connect(wxEVT_SHELL_COMMAND_STARTED,         wxCommandEventHandler(ClangCodeCompletion::OnBuildStarted),      NULL, this);
	EventNotifier::Get()->Connect(wxEVT_SHELL_COMMAND_STARTED_NOCLEAN, wxCommandEventHandler(ClangCodeCompletion::OnBuildStarted),      NULL, this);
	EventNotifier::Get()->Connect(wxEVT_SHELL_COMMAND_PROCESS_ENDED,   wxCommandEventHandler(ClangCodeCompletion::OnBuildEnded),        NULL, this);
	EventNotifier::Get()->Connect(wxEVT_SHELL_COMMAND_ADDLINE,         wxCommandEventHandler(ClangCodeCompletion::OnBuildOutput),       NULL, this);
}

ClangCodeCompletion::~ClangCodeCompletion()
{
	EventNotifier::Get()->Disconnect(wxEVT_ACTIVE_EDITOR_CHANGED,         wxCommandEventHandler(ClangCodeCompletion::OnFileLoaded),        NULL, this);
	EventNotifier::Get()->Disconnect(wxEVT_FILE_SAVED,                    wxCommandEventHandler(ClangCodeCompletion::OnFileSaved),         NULL, this);
	EventNotifier::Get()->Disconnect(wxEVT_ALL_EDITORS_CLOSING,           wxCommandEventHandler(ClangCodeCompletion::OnAllEditorsClosing), NULL, this);
	EventNotifier::Get()->Disconnect(wxEVT_ALL_EDITORS_CLOSED,            wxCommandEventHandler(ClangCodeCompletion::OnAllEditorsClosed ), NULL, this);
	EventNotifier::Get()->Disconnect(wxEVT_SHELL_COMMAND_STARTED,         wxCommandEventHandler(ClangCodeCompletion::OnBuildStarted),      NULL, this);
	EventNotifier::Get()->Disconnect(wxEVT_SHELL_COMMAND_STARTED_NOCLEAN, wxCommandEventHandler(ClangCodeCompletion::OnBuildStarted),      NULL, this);
	EventNotifier::Get()->Disconnect(wxEVT_SHELL_COMMAND_PROCESS_ENDED,   wxCommandEventHandler(ClangCodeCompletion::OnBuildEnded),        NULL, this);
	EventNotifier::Get()->Disconnect(wxEVT_SHELL_COMMAND_ADDLINE,         wxCommandEventHandler(ClangCodeCompletion::OnBuildOutput),       NULL, this);
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

void ClangCodeCompletion::ClearCache()
{
	m_clang.ClearCache();
}

void ClangCodeCompletion::CodeComplete(IEditor* editor)
{
	if(m_clang.IsBusy())
		return;

	m_clang.SetContext(CTX_CodeCompletion);
	m_clang.CodeCompletion(editor);
}

void ClangCodeCompletion::DoCleanUp()
{
	CL_DEBUG(wxT("Aborting PCH caching..."));
	m_clang.Abort();
}

void ClangCodeCompletion::CancelCodeComplete()
{
	if(!(TagsManagerST::Get()->GetCtagsOptions().GetClangOptions() & CC_CLANG_ENABLED))
		return;

	DoCleanUp();
}

void ClangCodeCompletion::Calltip(IEditor* editor)
{
	if(m_clang.IsBusy())
		return;

	m_clang.SetContext(CTX_Calltip);
	m_clang.CodeCompletion(editor);
}

void ClangCodeCompletion::OnFileLoaded(wxCommandEvent& e)
{
	e.Skip();

	// Sanity
	if(!(TagsManagerST::Get()->GetCtagsOptions().GetClangOptions() & CC_CLANG_ENABLED))
		return;

	if(TagsManagerST::Get()->GetCtagsOptions().GetClangCachePolicy() == TagsOptionsData::CLANG_CACHE_ON_FILE_LOAD) {
		CL_DEBUG(wxT("ClangCodeCompletion::OnFileLoaded() START"));
		if(m_clang.IsBusy() || m_allEditorsAreClosing) {
			CL_DEBUG(wxT("ClangCodeCompletion::OnFileLoaded() ENDED"));
			return;
		}
		if(e.GetClientData()) {
			IEditor *editor = (IEditor*)e.GetClientData();
			// sanity
			if(editor->GetProjectName().IsEmpty() || editor->GetFileName().GetFullName().IsEmpty())
				return;
			m_clang.SetContext(CTX_CachePCH);
			m_clang.CodeCompletion(editor);
		}
		CL_DEBUG(wxT("ClangCodeCompletion::OnFileLoaded() ENDED"));
	}
}

void ClangCodeCompletion::OnAllEditorsClosed(wxCommandEvent& e)
{
	e.Skip();
	m_allEditorsAreClosing = false;
}

void ClangCodeCompletion::OnAllEditorsClosing(wxCommandEvent& e)
{
	e.Skip();
	m_allEditorsAreClosing = true;
}

bool ClangCodeCompletion::IsCacheEmpty()
{
	return m_clang.IsCacheEmpty();
}

void ClangCodeCompletion::WordComplete(IEditor* editor)
{
	if(m_clang.IsBusy())
		return;
	m_clang.SetContext(CTX_WordCompletion);
	m_clang.CodeCompletion(editor);
}

void ClangCodeCompletion::ListMacros(IEditor* editor)
{
	m_clang.QueueRequest(editor, CTX_Macros);
}

void ClangCodeCompletion::OnFileSaved(wxCommandEvent& e)
{
	e.Skip();

	if( ! (TagsManagerST::Get()->GetCtagsOptions().GetFlags() & ::CC_DISABLE_AUTO_PARSING) ) {
		CL_DEBUG(wxT("ClangCodeCompletion::OnFileSaved: Auto-parsing of saved files is disabled"));
		return;
	}

	// Incase a file has been saved, we need to reparse its translation unit
	wxString *filename = (wxString*)e.GetClientData();
	if(filename) {
		m_clang.ReparseFile(*filename);
	}
}

void ClangCodeCompletion::OnBuildEnded(wxCommandEvent& e)
{
	e.Skip();

	// Sanity
	if(!(TagsManagerST::Get()->GetCtagsOptions().GetClangOptions() & CC_CLANG_ENABLED)) {
		m_projectCompiled.Clear();
		m_configurationCompiled.Clear();
		return;
	}

	m_parseBuildOutput = false;
	wxString errMsg;

	// In order to update the build configuration, we need the following objects:
	// ProjectPtr, SettingsPtr and BuildConfigPtr
	BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjBuildConf(m_projectCompiled, m_configurationCompiled);
	ProjectPtr     project = WorkspaceST::Get()->FindProjectByName(m_projectCompiled, errMsg);
	if(!bldConf || !project) return;

	// Parse the output
	m_compilerSearchPaths.clear();
	m_compilerMacros.clear();
	DoProcessOutput();

	ClangLocalPaths clangLocalInfo(project->GetFileName());
	if(m_compilerMacros.empty() == false) {
		clangLocalInfo.Options(m_configurationCompiled).SetMacros(m_compilerMacros);
	}
	if(m_compilerSearchPaths.empty() == false) {
		clangLocalInfo.Options(m_configurationCompiled).SetSearchPaths(m_compilerSearchPaths);
	}
	clangLocalInfo.Save();

	m_projectCompiled.Clear();
	m_configurationCompiled.Clear();
	e.Skip();
}

void ClangCodeCompletion::OnBuildStarted(wxCommandEvent& e)
{
	// Sanity
	e.Skip();

	m_projectCompiled.Clear();
	m_configurationCompiled.Clear();
	m_processOutput.Clear();
	if(!(TagsManagerST::Get()->GetCtagsOptions().GetClangOptions() & CC_CLANG_ENABLED)) {
		return;
	}

	bool isClean = false;
	BuildEventDetails *d = dynamic_cast<BuildEventDetails*>(e.GetClientObject());
	if(d) {
		m_projectCompiled       = d->GetProjectName();
		m_configurationCompiled = d->GetConfiguration();
		isClean                 = d->IsClean();
	}

	m_parseBuildOutput = !isClean;
	m_compilerSearchPaths.clear();
	m_compilerMacros.clear();
}

void ClangCodeCompletion::OnBuildOutput(wxCommandEvent& e)
{
	// Sanity
	e.Skip();
	if(!(TagsManagerST::Get()->GetCtagsOptions().GetClangOptions() & CC_CLANG_ENABLED)) {
		return;
	}

	if(m_parseBuildOutput) {
		m_processOutput << e.GetString();
	}
}

wxString ClangCodeCompletion::DoGetCompiledLine() const
{
	wxString s;
	Set_t::const_iterator iter = m_compilerSearchPaths.begin();
	for(; iter != m_compilerSearchPaths.end(); iter++) {
		s << wxT("-I") << *iter << wxT(" ");
	}

	iter = m_compilerMacros.begin();
	for(; iter != m_compilerMacros.end(); iter++) {
		s << wxT("-D") << *iter << wxT(" ");
	}

	s.Trim().Trim(false);
	return s;
}
void ClangCodeCompletion::DoProcessOutput()
{
	Set_t directories;
	
	wxArrayString lines = ::wxStringTokenize(m_processOutput, wxT("\r\n"), wxTOKEN_STRTOK);
	for(size_t i=0; i<lines.GetCount(); i++) {
		CompilerCommandLineParser cmdLineParser(lines.Item(i));
		// Collect the macros / includes
		for(size_t i=0; i<cmdLineParser.GetIncludes().GetCount(); i++) {
			m_compilerSearchPaths.insert(cmdLineParser.GetIncludes().Item(i));
		}
		for(size_t i=0; i<cmdLineParser.GetMacros().GetCount(); i++) {
			m_compilerMacros.insert(cmdLineParser.GetMacros().Item(i));
		}
		
		if(cmdLineParser.GetDiretory().IsEmpty() == false) {
			directories.insert(cmdLineParser.GetDiretory());
		}
	}
	
	// Convert the paths from relative path to full path
	Set_t fullPaths;
	Set_t::const_iterator iter = m_compilerSearchPaths.begin();
	for(; iter != m_compilerSearchPaths.end(); iter++) {
		bool convertedToFullpath = false;
		Set_t::const_iterator dirIter = directories.begin();
		for(; dirIter != directories.end(); dirIter++) {
			wxFileName fn(*iter, wxT(""));
			if(fn.MakeAbsolute(*dirIter) && fn.DirExists()) {
				fullPaths.insert(fn.GetFullPath());
				//wxPrintf(wxT("Adding full path: %s\n"), fn.GetFullPath().c_str());
				convertedToFullpath = true;
				break;
			}
		}
		
		if(!convertedToFullpath) {
			//wxPrintf(wxT("Adding relative path: %s\n"), iter->c_str());
			fullPaths.insert(*iter);
		}
	}
	
	m_compilerSearchPaths = fullPaths;
}

#endif // HAS_LIBCLANG


