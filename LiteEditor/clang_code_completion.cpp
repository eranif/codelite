#if HAS_LIBCLANG

#include "clang_code_completion.h"
#include "pluginmanager.h"
#include "shell_command.h"
#include "compiler_command_line_parser.h"
#include "event_notifier.h"
#include "clang_path_resolver_thread.h"
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
#include "manager.h"
#include <memory>

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
	EventNotifier::Get()->Connect(wxEVT_COMMAND_CLANG_PATH_RESOLVED,   wxCommandEventHandler(ClangCodeCompletion::OnClangPathResolved), NULL, this);
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
	EventNotifier::Get()->Disconnect(wxEVT_COMMAND_CLANG_PATH_RESOLVED,   wxCommandEventHandler(ClangCodeCompletion::OnClangPathResolved), NULL, this);
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
			
			if(!TagsManagerST::Get()->IsValidCtagsFile(editor->GetFileName()))
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
	m_clang.GetMacros(editor);
}

void ClangCodeCompletion::OnFileSaved(wxCommandEvent& e)
{
    e.Skip();
    if(!(TagsManagerST::Get()->GetCtagsOptions().GetClangOptions() & CC_CLANG_ENABLED))
		return;
    
	if( TagsManagerST::Get()->GetCtagsOptions().GetFlags() & ::CC_DISABLE_AUTO_PARSING) {
		CL_DEBUG(wxT("ClangCodeCompletion::OnFileSaved: Auto-parsing of saved files is disabled"));
		return;
	}

	// Incase a file has been saved, we need to reparse its translation unit
	wxString *filename = (wxString*)e.GetClientData();
	if(filename) {
		
		wxFileName fn(*filename);
		if(!TagsManagerST::Get()->IsValidCtagsFile(fn))
			return;
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

	ProjectPtr project = WorkspaceST::Get()->FindProjectByName(m_projectCompiled, errMsg);
	if(!project) return;
	
	std::set<wxString> workspaceFiles;
	ManagerST::Get()->GetWorkspaceFiles(workspaceFiles);
	
	// Construct a thread to process the output 
	ClangPathResolverThread *thr = new ClangPathResolverThread(workspaceFiles, m_processOutput);
	thr->Start();
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

void ClangCodeCompletion::OnClangPathResolved(wxCommandEvent& e)
{
	PathTripplet *data = (PathTripplet *)e.GetClientData();
	if(data) {
		
		wxString errMsg;
		ProjectPtr project = WorkspaceST::Get()->FindProjectByName(m_projectCompiled, errMsg);
		std::auto_ptr<PathTripplet> spData( data  );
		if(!project) {
			// Release all allocated data
			return;
		}
		
		ClangLocalPaths clangLocalInfo(project->GetFileName());
		bool saveRequired = false;
		if(data->macros->empty() == false) {
			clangLocalInfo.Options(m_configurationCompiled).UpdateMacros(*data->macros);
			saveRequired = true;
		}
		
		if(data->searchPaths->empty() == false) {
			clangLocalInfo.Options(m_configurationCompiled).UpdateSearchPaths(*data->searchPaths);
			saveRequired = true;
		}
		
		if(data->frameworks->empty() == false) {
			clangLocalInfo.Options(m_configurationCompiled).UpdateFrameworks(*data->frameworks);
			saveRequired = true;
		}
		
		if(saveRequired) {
			clangLocalInfo.Save();
			m_clang.ClearCache();
		}
	}
	
	m_projectCompiled.Clear();
	m_configurationCompiled.Clear();
}

void ClangCodeCompletion::GotoDeclaration(IEditor* editor)
{
	if(m_clang.IsBusy())
		return;

	m_clang.SetContext(CTX_GotoDecl);
	m_clang.CodeCompletion(editor);	
}

void ClangCodeCompletion::GotoImplementation(IEditor* editor)
{
	if(m_clang.IsBusy())
		return;

	m_clang.SetContext(CTX_GotoImpl);
	m_clang.CodeCompletion(editor);	
	
}


#endif // HAS_LIBCLANG


