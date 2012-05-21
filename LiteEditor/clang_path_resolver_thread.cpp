#if HAS_LIBCLANG

#include "clang_path_resolver_thread.h"
#include "compiler_command_line_parser.h"
#include <wx/filename.h>
#include "event_notifier.h"
#include <wx/xrc/xmlres.h>

const wxEventType wxEVT_COMMAND_CLANG_PATH_RESOLVED = XRCID("wxEVT_COMMAND_CLANG_PATH_RESOLVED");

ClangPathResolverThread::ClangPathResolverThread(const Set_t& files, const wxString &compilerOutput)
	: wxThread(wxTHREAD_DETACHED)
	, m_compilerOutput(compilerOutput.c_str())
{
	m_workspaceFolders.clear();
	
	Set_t::const_iterator iter = files.begin();
	for(; iter != files.end(); iter++) {
		// Due to bug in wx, we need to force a new copy for each string
		// in the set
		wxFileName fn(iter->c_str());
		m_workspaceFolders.insert(fn.GetPath().c_str());
	}
}

ClangPathResolverThread::~ClangPathResolverThread()
{
}

void* ClangPathResolverThread::Entry()
{
	Set_t directories;
	PathTripplet *tripplet = new PathTripplet;
	
	wxArrayString lines = ::wxStringTokenize(m_compilerOutput, wxT("\r\n"), wxTOKEN_STRTOK);
	for(size_t i=0; i<lines.GetCount(); i++) {
		CompilerCommandLineParser cmdLineParser(lines.Item(i));
		// Collect the macros / includes
		for(size_t i=0; i<cmdLineParser.GetIncludes().GetCount(); i++) {
			tripplet->searchPaths->insert(cmdLineParser.GetIncludes().Item(i).c_str());
		}
		
		for(size_t i=0; i<cmdLineParser.GetMacros().GetCount(); i++) {
			tripplet->macros->insert(cmdLineParser.GetMacros().Item(i).c_str());
		}
		
		for(size_t i=0; i<cmdLineParser.GetFramworks().GetCount(); i++) {
			tripplet->frameworks->insert(cmdLineParser.GetFramworks().Item(i).c_str());
		}
	}
	
	// Convert the paths from relative path to full path
	Set_t *fullPaths = new Set_t();
	Set_t::const_iterator iter = tripplet->searchPaths->begin();
	for(; iter != tripplet->searchPaths->end(); iter++) {
		bool convertedToFullpath = false;
		Set_t::const_iterator dirIter = m_workspaceFolders.begin();
		for(; dirIter != m_workspaceFolders.end(); dirIter++) {
			wxString directory = *dirIter;
			wxFileName fn(*iter, wxT(""));
			if(fn.IsRelative() && fn.MakeAbsolute(directory) && fn.DirExists()) {
				wxString path = fn.GetPath();
				path.Replace(wxT("\\"), wxT("/"));
				fullPaths->insert(path);
				convertedToFullpath = true;
				break;
			}
		}
		
		if(!convertedToFullpath) {
			fullPaths->insert(*iter);
		}
	}
	
	delete tripplet->searchPaths;
	tripplet->searchPaths = fullPaths;
	
	wxCommandEvent evt(wxEVT_COMMAND_CLANG_PATH_RESOLVED);
	evt.SetClientData(tripplet);
	EventNotifier::Get()->AddPendingEvent(evt);
	
	return NULL;
}

void ClangPathResolverThread::Start()
{
	Create();
	Run();
}

#endif // #if HAS_LIBCLANG
