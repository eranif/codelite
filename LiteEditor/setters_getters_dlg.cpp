//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : setters_getters_dlg.cpp              
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
 #include "precompiled_header.h"
#include "setters_getters_dlg.h"
#include "macros.h"
#include "language.h"
#include "wx/tokenzr.h"

SettersGettersDlg::SettersGettersDlg(wxWindow* parent)
: SettersGettersBaseDlg(parent)
{
	ConnectCheckBox(m_checkStartWithUppercase, SettersGettersDlg::OnCheckStartWithUpperCase);
	ConnectCheckList(m_checkListMembers, SettersGettersDlg::OnCheckStartWithUpperCase);
} 

void SettersGettersDlg::Init(const std::vector<TagEntryPtr> &tags, const wxFileName &file, int lineno)
{
	//convert the tags to string array
	m_file = file;
	m_lineno = lineno;
	m_members = tags;

	wxArrayString members;
	for(size_t i=0; i<tags.size() ; i++){
		members.Add(tags.at(i)->GetName() + wxT(" : [Getter]"));
		members.Add(tags.at(i)->GetName() + wxT(" : [Setter]"));

		m_tagsMap[tags.at(i)->GetName() + wxT(" : [Getter]")] = tags.at(i);
		m_tagsMap[tags.at(i)->GetName() + wxT(" : [Setter]")] = tags.at(i);
	}
	
	//append all members to the check list
	m_checkListMembers->Clear();
	m_checkListMembers->Append(members);
	
	//uncheck all items
	for(unsigned int i=0; i<m_checkListMembers->GetCount(); i++)
	{
		m_checkListMembers->Check(i, false);
	}
	
	if(tags.empty() == false){
		m_textClassName->SetValue(tags.at(0)->GetParent());
	}

	//set the preview
	m_textPreview->SetReadOnly(false);
	m_textPreview->Create(wxEmptyString, m_file);
	m_textPreview->GotoLine(m_lineno);
	m_textPreview->SetReadOnly(true);
	UpdatePreview();
}

void SettersGettersDlg::OnCheckStartWithUpperCase(wxCommandEvent &event)
{
	wxUnusedVar(event);
	UpdatePreview();
}

wxString SettersGettersDlg::GenerateFunctions()
{
	wxString code;
	GenerateSetters(code);
	if(code.IsEmpty() == false){
		code.Prepend(wxT("//Setters\n"));
		code << wxT("\n\n");
	}
	wxString settersCode;
	GenerateGetters(settersCode);
	if(settersCode.IsEmpty() == false){
		code << wxT("//Getters\n");
		code << settersCode << wxT("\n");
	}
	return code;
}

void SettersGettersDlg::GenerateGetters(wxString &code)
{
	for(size_t i=0; i<m_checkListMembers->GetCount(); i++)
	{
		TagEntryPtr tag;
		if(m_checkListMembers->IsChecked((unsigned int)i)){
			wxString item = m_checkListMembers->GetString((unsigned int)i);
			//get the tag for this
			std::map<wxString, TagEntryPtr>::iterator iter = m_tagsMap.find(item);
			if(iter != m_tagsMap.end()){
				tag = iter->second;
				//geenerate function for this tag
				if(item.EndsWith(wxT("[Getter]"))){
					code << GenerateGetter(tag) << wxT("\n");
				}
			}
		}
	}
}

void SettersGettersDlg::GenerateSetters(wxString &code)
{
	for(size_t i=0; i<m_checkListMembers->GetCount(); i++)
	{
		TagEntryPtr tag;
		if(m_checkListMembers->IsChecked((unsigned int)i)){
			wxString item = m_checkListMembers->GetString((unsigned int)i);
			//get the tag for this
			std::map<wxString, TagEntryPtr>::iterator iter = m_tagsMap.find(item);
			if(iter != m_tagsMap.end()){
				tag = iter->second;
				//geenerate function for this tag
				if(item.EndsWith(wxT("[Setter]"))){
					code << GenerateSetter(tag) << wxT("\n");;
				}
			}
		}
	}
}

wxString SettersGettersDlg::GenerateSetter(TagEntryPtr tag)
{
	bool startWithUpper  = m_checkStartWithUppercase->IsChecked();

	Variable var;
	if(LanguageST::Get()->VariableFromPattern(tag->GetPattern(), var))
	{
		wxString func;
		wxString scope = _U(var.m_typeScope.c_str());
		func << wxT("void ");
		if(startWithUpper){
			func << wxT("Set");
		}else{
			func << wxT("set");
		}
		wxString name = _U(var.m_name.c_str());
		FormatName(name);
		func << name << wxT("(const ");

		if(!scope.IsEmpty() && !(scope == wxT("<global>"))){
			func << scope << wxT("::");
		}

		func << _U(var.m_type.c_str()) 
			 << _U(var.m_templateDecl.c_str()) 
			 << _U(var.m_starAmp.c_str())
			 << wxT("& ");
		
		wxString tmpName = _U(var.m_name.c_str());
		tmpName.StartsWith(wxT("m_"), &tmpName);

		func << tmpName << wxT(") {this->") << _U(var.m_name.c_str()) << wxT(" = ") << tmpName << wxT(";}");
		return func;			 
	}
	return wxEmptyString;
}

wxString SettersGettersDlg::GenerateGetter(TagEntryPtr tag)
{
	bool startWithUpper  = m_checkStartWithUppercase->IsChecked();

	Variable var;
	if(LanguageST::Get()->VariableFromPattern(tag->GetPattern(), var))
	{
		wxString func;
		wxString scope = _U(var.m_typeScope.c_str());
		func << wxT("const ");
		if(!scope.IsEmpty() && !(scope == wxT("<global>"))){
			func << scope 
				 << wxT("::");
		}
		func << _U(var.m_type.c_str()) 
			 << _U(var.m_templateDecl.c_str()) 
			 << _U(var.m_starAmp.c_str())
			 << wxT("& ");
		if(startWithUpper){
			func << wxT("Get");
		}else{
			func << wxT("get");
		}

		wxString name = _U(var.m_name.c_str());
		FormatName(name);
		func << name << wxT("() const {return ") << _U(var.m_name.c_str()) << wxT(";}");
		return func;			 
	}
	return wxEmptyString;
}

void SettersGettersDlg::FormatName(wxString &name)
{
	name.StartsWith(wxT("m_"), &name);
	wxStringTokenizer tkz(name, wxT("_"));
	name.Clear();
	while(tkz.HasMoreTokens()){
		wxString token = tkz.NextToken();
		wxString pre = token.Mid(0, 1);
		token.Remove(0, 1);
		pre.MakeUpper();
		token.Prepend(pre);
		name << token;
	}

}

void SettersGettersDlg::UpdatePreview()
{
	m_code.Clear();
	m_code = GenerateFunctions();
	m_textPreview->SetReadOnly(false);
	//remove previous preview
	if(m_textPreview->CanUndo()) m_textPreview->Undo();
	m_textPreview->BeginUndoAction();
	m_textPreview->InsertTextWithIndentation(m_code, m_lineno);
	m_textPreview->EndUndoAction();
	m_textPreview->SetReadOnly(true);
}

void SettersGettersDlg::OnCheckAll(wxCommandEvent &e)
{
	wxUnusedVar(e);
	for(unsigned int i=0; i< (unsigned int)m_checkListMembers->GetCount(); i++){
		m_checkListMembers->Check(i);
	}
	UpdatePreview();
}

void SettersGettersDlg::OnUncheckAll(wxCommandEvent &e)
{
	wxUnusedVar(e);
	for(unsigned int i=0; i< (unsigned int)m_checkListMembers->GetCount(); i++){
		m_checkListMembers->Check(i, false);
	}
	UpdatePreview();
}
