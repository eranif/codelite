#include "clang_code_completion.h"
#include "pluginmanager.h"
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

ClangCodeCompletion* ClangCodeCompletion::ms_instance = 0;

ClangCodeCompletion::ClangCodeCompletion()
	: m_activationPos       (wxNOT_FOUND)
	, m_activationEditor    (NULL)
	, m_allEditorsAreClosing(false)
{
	wxTheApp->Connect(wxEVT_ACTIVE_EDITOR_CHANGED, wxCommandEventHandler(ClangCodeCompletion::OnFileLoaded),        NULL, this);
	wxTheApp->Connect(wxEVT_ALL_EDITORS_CLOSING,   wxCommandEventHandler(ClangCodeCompletion::OnAllEditorsClosing), NULL, this);
	wxTheApp->Connect(wxEVT_ALL_EDITORS_CLOSED,    wxCommandEventHandler(ClangCodeCompletion::OnAllEditorsClosed ), NULL, this);
	
	// make sure that clang's PCH cache directory exists
	wxString cacheDir = ClangPCHCache::GetCacheDirectory();
	if(! wxDir::Exists(cacheDir) ) {
		wxMkdir(cacheDir);
	}
}

ClangCodeCompletion::~ClangCodeCompletion()
{
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
		
	m_activationEditor = editor;
	m_clang.SetContext(ClangDriver::CTX_CodeCompletion);
	m_clang.CodeCompletion(editor);
}

void ClangCodeCompletion::DoParseOutput(const wxString &output)
{
	// Sanity
	m_activationPos = m_clang.GetActivationPos();
	if(output.IsEmpty() || !m_activationEditor || m_activationPos == wxNOT_FOUND)
		return;
	
	wxArrayString entries = wxStringTokenize(output, wxT("\n\r"), wxTOKEN_STRTOK);
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
		
		if(m_clang.GetContext() == ClangDriver::CTX_Calltip && filter.EndsWith(wxT("("))) {
			filter.RemoveLast();
		}
		
		std::vector<TagEntryPtr> *tagsToShow = &tags;
		std::vector<TagEntryPtr> filteredTags;
		wxString tmpfilter = filter;
		
		if(tmpfilter.IsEmpty() == false) {
			tmpfilter.MakeLower();
			
			filteredTags.reserve( tags.size() );
			for(size_t i=0; i<tags.size(); i++) {
				wxString n = tags.at(i)->GetName();
				n.MakeLower();
				
				if(n.StartsWith(tmpfilter)) {
					filteredTags.push_back(tags.at(i));
				}
			}
			
			tagsToShow = &filteredTags;
		}
		
		if(m_clang.GetContext() == ClangDriver::CTX_Calltip) {
			std::vector<TagEntryPtr> tips;
			TagsManagerST::Get()->GetFunctionTipFromTags(*tagsToShow, filter, tips);
			m_activationEditor->ShowCalltip(new clCallTip(tips));
			
		} else {
			m_activationEditor->ShowCompletionBox(*tagsToShow, filter, NULL);
			
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
	tag->SetIsClangTag(true);
	
	tmp.Replace(wxT("[#"), wxT(""));
	tmp.Replace(wxT("<#"), wxT(""));
	tmp.Replace(wxT("{#"), wxT(""));
	tmp.Replace(wxT("#]"), wxT(" "));
	tmp.Replace(wxT("#>"), wxT(" "));
	tmp.Replace(wxT("#}"), wxT(" "));
	
	wxString pattern;
	// Simulate a valid ctags pattern...
	pattern << wxT("/^ ") << tmp << wxT(" $/");
	
	if(tmp == name) {
		// this a type (enum/typedef/internal class)
		tag->SetKind(wxT("class"));
		
	} else if(tmp.Contains(wxT("(")) || tmp.EndsWith(wxT("::"))) {
		wxString signature = tmp.AfterFirst(wxT('('));
		signature = signature.BeforeLast(wxT(')'));
		
		tag->SetSignature(wxT("(") + signature + wxT(")"));
		
	} else {
		tag->SetKind(wxT("member"));
		
	}
	tag->SetPattern(pattern);
	return tag;
}

void ClangCodeCompletion::DoCleanUp()
{
	CL_DEBUG(wxT("Aborting PCH caching..."));
	m_clang.Abort();
	m_activationEditor = NULL;
	m_activationPos    = wxNOT_FOUND;
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
		
	m_activationEditor = editor;
	m_clang.SetContext(ClangDriver::CTX_Calltip);
	m_clang.CodeCompletion(editor);
}

void ClangCodeCompletion::OnFileLoaded(wxCommandEvent& e)
{
	e.Skip();
	
	// Sanity
	if(!(TagsManagerST::Get()->GetCtagsOptions().GetClangOptions() & CC_CLANG_ENABLED)) 
		return;

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
		
		m_activationEditor = editor;
		m_clang.SetContext(ClangDriver::CTX_CachePCH);
		m_clang.CodeCompletion(m_activationEditor);
	
	}
	CL_DEBUG(wxT("ClangCodeCompletion::OnFileLoaded() ENDED"));
}

void ClangCodeCompletion::OnAllEditorsClosed(wxCommandEvent& e)
{
	wxUnusedVar(e);
	m_allEditorsAreClosing = false;
}

void ClangCodeCompletion::OnAllEditorsClosing(wxCommandEvent& e)
{
	wxUnusedVar(e);
	m_allEditorsAreClosing = true;
}

bool ClangCodeCompletion::IsCacheEmpty()
{
	return m_clang.IsCacheEmpty();
}
