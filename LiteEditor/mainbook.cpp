#include "mainbook.h"
#include "wx/choice.h"
#include "wx/xrc/xmlres.h"
#include "wx/log.h"
#include "manager.h"
#include "ctags_manager.h"
#include "editor.h"
#include "macros.h"

MainBook::MainBook(wxChoice* choiceFunc, wxChoice* choiceScope)
: m_choiceFunc(choiceFunc)
, m_choiceScope(choiceScope)
{
	//Connect events
	m_choiceFunc->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(MainBook::OnFuncListMouseDown), NULL, this);
	//m_choiceScope->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(MainBook::OnScopeListMouseDown), NULL, this);
	ConnectChoice(m_choiceFunc, MainBook::OnFunction);
	//ConnectChoice(m_choiceScope, MainBook::OnScope);
}

MainBook::~MainBook()
{
}

void MainBook::OnScope(wxCommandEvent &e)
{
	wxUnusedVar(e);
	int sel = e.GetSelection();
	if(sel != wxNOT_FOUND){
		TagEntry *tag = (TagEntry*) m_choiceScope->GetClientData(sel);
		if(tag){
			ManagerST::Get()->OpenFile(tag->GetFile(), wxEmptyString, tag->GetLine()-1);
		}
	}
}

void MainBook::OnFunction(wxCommandEvent &e)
{
	int sel = e.GetSelection();
	if(sel != wxNOT_FOUND){
		TagEntry *tag = (TagEntry*) m_choiceFunc->GetClientData(sel);
		if(tag){
			ManagerST::Get()->OpenFile(tag->GetFile(), wxEmptyString, tag->GetLine()-1);
		}
	}
}

void MainBook::Clear()
{
	if(!m_choiceFunc ||  !m_choiceScope)
		return;
		
	m_choiceFunc->Freeze();
	m_choiceScope->Freeze();
	m_choiceFunc->Clear();
	m_choiceScope->Clear();
	m_choiceFunc->Thaw();
	m_choiceScope->Thaw();
}

void MainBook::UpdateScope(TagEntryPtr tag)
{
	if(!tag){
		return;
	}

	if(!m_choiceFunc ||  !m_choiceScope)
		return;

	wxString function;
	function << tag->GetName() << tag->GetSignature();
	
	m_choiceFunc->Freeze();
	m_choiceScope->Freeze();
	
	m_choiceFunc->Clear(); 
	m_choiceScope->Clear();
	
	m_choiceFunc->AppendString(function);
	m_choiceScope->AppendString(tag->GetScope());
	
	m_choiceFunc->SetSelection(0);
	m_choiceScope->SetSelection(0);

	m_choiceFunc->Thaw();
	m_choiceScope->Thaw();
}

void MainBook::OnScopeListMouseDown(wxMouseEvent &e)
{
	TagsManager *tagsmgr = TagsManagerST::Get();
	Manager *mgr = ManagerST::Get();
	
	if(mgr->IsWorkspaceOpen()){
		LEditor *editor = mgr->GetActiveEditor();
		if(editor){
			std::vector< TagEntryPtr > tags;
			tagsmgr->GetScopesFromFile(editor->GetFileName(), tags);
			m_choiceScope->Freeze();
			wxString cursel;
			if(m_choiceScope->GetCount() > 0){
				cursel = m_choiceScope->GetStringSelection();
			}
			
			m_choiceScope->Clear();
			
			if(tags.empty()){
				if(cursel.IsEmpty() == false && cursel != wxT("<global>")){
					m_choiceScope->Append(wxT("<global>"), (void*)NULL);
				}
			}
			
			for(size_t i=0; i< tags.size(); i++){
				m_choiceScope->Append(tags.at(i)->GetPath(), new TagEntry(*tags.at(i)));
			}
			
			if(cursel.IsEmpty() == false){
				m_choiceScope->SetStringSelection(cursel);
			}
			m_choiceScope->Thaw();
		}
	}
	
	e.Skip();
}

void MainBook::OnFuncListMouseDown(wxMouseEvent &e)
{
	TagsManager *tagsmgr = TagsManagerST::Get();
	Manager *mgr = ManagerST::Get();
	if(mgr->IsWorkspaceOpen()){
		LEditor *editor = mgr->GetActiveEditor();
		if(editor){
			std::vector< TagEntryPtr > tags;
			tagsmgr->TagsFromFileAndScope(editor->GetFileName(), m_choiceScope->GetStringSelection(), tags);
			m_choiceFunc->Freeze();
			
			wxString cursel;
			if(m_choiceFunc->GetCount() > 0){
				cursel = m_choiceFunc->GetStringSelection();
			}
			
			m_choiceFunc->Clear();
			for(size_t i=0; i< tags.size(); i++){
				m_choiceFunc->Append(tags.at(i)->GetName() + tags.at(i)->GetSignature(), new TagEntry(*tags.at(i)));
			}
			
			if(cursel.IsEmpty() == false){
				m_choiceFunc->SetStringSelection(cursel);
			}
			m_choiceFunc->Thaw();
		}
	}
	e.Skip();
}

