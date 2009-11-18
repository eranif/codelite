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
#include "editor_config.h"
#include "ctags_manager.h"
#include "settersgetterstreectrl.h"
#include "setters_getters_dlg.h"
#include "macros.h"
#include "language.h"
#include "wx/tokenzr.h"

const wxEventType wxEVT_CMD_UPDATE_PREVIEW = 19343;

//----------------------------------------------------

SettersGettersDlg::SettersGettersDlg(wxWindow* parent)
		: SettersGettersBaseDlg(parent)
		, m_checkForDuplicateEntries(false)
{
	ConnectCheckBox(m_checkStartWithUppercase, SettersGettersDlg::OnCheckStartWithUpperCase);
	m_checkListMembers->Connect(wxEVT_LEFT_DOWN,   wxMouseEventHandler(SettersGettersDlg::OnLeftDown), NULL, this);
	m_checkListMembers->Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(SettersGettersDlg::OnLeftDown), NULL, this);
	Connect ( wxEVT_CMD_UPDATE_PREVIEW,  wxCommandEventHandler ( SettersGettersDlg::OnUpdatePreview ), NULL, this );
}

void SettersGettersDlg::Init(const std::vector<TagEntryPtr> &tags, const wxFileName &file, int lineno)
{
	//convert the tags to string array
	m_file = file;
	m_lineno = lineno;
	m_members = tags;

	BuildTree();

	//set the preview
//	m_textPreview->SetReadOnly(false);
//	m_textPreview->Create(wxEmptyString, m_file);
//	m_textPreview->GotoLine(m_lineno);
//	m_textPreview->EnsureVisible(m_lineno);
//	m_textPreview->SetReadOnly(true);
	UpdatePreview();
}

void SettersGettersDlg::OnCheckStartWithUpperCase(wxCommandEvent &event)
{
	wxUnusedVar(event);
	UpdateTree();
	UpdatePreview();
}

wxString SettersGettersDlg::GenerateFunctions()
{
	wxString code;
	GenerateSetters(code);
	if (code.IsEmpty() == false) {
		code << wxT("\n\n");
	}
	wxString settersCode;
	GenerateGetters(settersCode);
	if (settersCode.IsEmpty() == false) {
		code << settersCode << wxT("\n");
	}
	return code;
}

void SettersGettersDlg::GenerateGetters(wxString &code)
{
	wxTreeItemIdValue cookie;
	wxTreeItemId child = m_checkListMembers->GetFirstChild(m_checkListMembers->GetRootItem(), cookie);
	while (child.IsOk()) {
		if (m_checkListMembers->ItemHasChildren(child)) {

			wxTreeItemIdValue gcookie;
			wxTreeItemId gchild = m_checkListMembers->GetFirstChild(child, gcookie);
			while ( gchild.IsOk() ) {
				SettersGettersTreeData *data = (SettersGettersTreeData *)m_checkListMembers->GetItemData(gchild);
				if ( data->m_kind == SettersGettersTreeData::Kind_Getter && m_checkListMembers->IsChecked(gchild) ) {
					code << GenerateGetter(data->m_tag) << wxT("\n");
					break;
				}
				gchild = m_checkListMembers->GetNextChild(child, gcookie);
			}
		}
		child = m_checkListMembers->GetNextChild(m_checkListMembers->GetRootItem(), cookie);
	}
}

void SettersGettersDlg::GenerateSetters(wxString &code)
{
	wxTreeItemIdValue cookie;
	wxTreeItemId child = m_checkListMembers->GetFirstChild(m_checkListMembers->GetRootItem(), cookie);
	while (child.IsOk()) {
		if (m_checkListMembers->ItemHasChildren(child)) {

			wxTreeItemIdValue gcookie;
			wxTreeItemId gchild = m_checkListMembers->GetFirstChild(child, gcookie);
			while ( gchild.IsOk() ) {
				SettersGettersTreeData *data = (SettersGettersTreeData *)m_checkListMembers->GetItemData(gchild);
				if ( data->m_kind == SettersGettersTreeData::Kind_Setter && m_checkListMembers->IsChecked(gchild)) {
					code << GenerateSetter(data->m_tag) << wxT("\n");
					break;
				}
				gchild = m_checkListMembers->GetNextChild(child, gcookie);
			}
		}
		child = m_checkListMembers->GetNextChild(m_checkListMembers->GetRootItem(), cookie);
	}
}

wxString SettersGettersDlg::GenerateSetter(TagEntryPtr tag, bool &alreadyExist, wxString &displayName)
{
	alreadyExist = false;
	bool startWithUpper  = m_checkStartWithUppercase->IsChecked();

	Variable var;
	wxString method_name, method_signature;

	if (LanguageST::Get()->VariableFromPattern(tag->GetPattern(), tag->GetName(), var)) {
		wxString func;
		wxString scope = _U(var.m_typeScope.c_str());
		func << wxT("void ");

		if (startWithUpper) {
			method_name << wxT("Set");
		} else {
			method_name << wxT("set");
		}

		wxString name = _U(var.m_name.c_str());
		FormatName(name);
		method_name << name;

		// add the method name
		func << method_name;

		// add the signature
		if( var.m_isBasicType ) {
			method_signature << wxT("(");

		} else if (!var.m_isPtr) {
			method_signature << wxT("(const ");

		} else {
			method_signature << wxT("(");

		}

		if (!scope.IsEmpty() && !(scope == wxT("<global>"))) {
			method_signature << scope << wxT("::");
		}

		method_signature << _U(var.m_type.c_str()) << _U(var.m_templateDecl.c_str()) << _U(var.m_starAmp.c_str());
		if(var.m_isBasicType) {
			method_signature << wxT(" ");

		} else if (!var.m_isPtr) {
			method_signature << wxT("& ");

		} else {
			method_signature << wxT(" ");

		}

		wxString tmpName = _U(var.m_name.c_str());
		tmpName.StartsWith(wxT("m_"), &tmpName);

		method_signature << tmpName << wxT(")");
		func << method_signature;

		// at this point, func contains the display_name (i.e. the function without the implementation)
		displayName << func;

		// add the implementation
		func << wxT(" {this->") << _U(var.m_name.c_str()) << wxT(" = ") << tmpName << wxT(";}");

		if ( m_checkForDuplicateEntries ) {
			alreadyExist = DoCheckExistance(tag->GetScope(), method_name, method_signature);
		}

		return func;
	}
	return wxEmptyString;
}

wxString SettersGettersDlg::GenerateGetter(TagEntryPtr tag, bool &alreadyExist, wxString &displayName)
{
	alreadyExist = false;
	bool startWithUpper  = m_checkStartWithUppercase->IsChecked();

	Variable var;
	wxString method_name, method_signature;
	if (LanguageST::Get()->VariableFromPattern(tag->GetPattern(), tag->GetName(), var)) {
		wxString func;
		wxString scope = _U(var.m_typeScope.c_str());

		if (!var.m_isPtr && !var.m_isBasicType) {
			func << wxT("const ");
			if (!scope.IsEmpty() && !(scope == wxT("<global>"))) {
				func << scope
				<< wxT("::");
			}
			func << _U(var.m_type.c_str()) << _U(var.m_templateDecl.c_str()) << _U(var.m_starAmp.c_str()) << wxT("& ");

		} else {
			// generate different code for pointer
			if (!scope.IsEmpty() && !(scope == wxT("<global>"))) {
				func << scope
				<< wxT("::");
			}
			func << _U(var.m_type.c_str()) << _U(var.m_templateDecl.c_str()) << _U(var.m_starAmp.c_str()) << wxT(" ");
		}


		if (startWithUpper) {
			method_name << wxT("Get");
		} else {
			method_name << wxT("get");
		}

		wxString name = _U(var.m_name.c_str());
		FormatName(name);
		method_name << name;

		// add the method name
		func << method_name;
		if (!var.m_isPtr) {
			method_signature << wxT("() const");

		} else {
			method_signature << wxT("()");

		}

		// add the signature
		func << method_signature;

		if (m_checkForDuplicateEntries) {
			alreadyExist = DoCheckExistance(tag->GetScope(), method_name, method_signature);
		}

		displayName << func;

		// add the implementation
		func << wxT(" {return ") << _U(var.m_name.c_str()) << wxT(";}");

		return func;
	}
	return wxEmptyString;
}

void SettersGettersDlg::FormatName(wxString &name)
{
	name.StartsWith(wxT("m_"), &name);
	wxStringTokenizer tkz(name, wxT("_"));
	name.Clear();
	while (tkz.HasMoreTokens()) {
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
//	m_textPreview->SetReadOnly(false);
//	//remove previous preview
//	if (m_textPreview->CanUndo()) m_textPreview->Undo();
//	m_textPreview->BeginUndoAction();
//	m_textPreview->InsertTextWithIndentation(m_code, m_lineno);
//	m_textPreview->EndUndoAction();
//	m_textPreview->SetReadOnly(true);
}

void SettersGettersDlg::OnCheckAll(wxCommandEvent &e)
{
	wxUnusedVar(e);
	wxTreeItemIdValue cookie;
	wxTreeItemId child = m_checkListMembers->GetFirstChild(m_checkListMembers->GetRootItem(), cookie);
	while (child.IsOk()) {
		if (m_checkListMembers->ItemHasChildren(child)) {

			wxTreeItemIdValue gcookie;
			wxTreeItemId gchild = m_checkListMembers->GetFirstChild(child, gcookie);
			while ( gchild.IsOk() ) {
				m_checkListMembers->Check(gchild, true);
				gchild = m_checkListMembers->GetNextChild(child, gcookie);
			}

		}
		child = m_checkListMembers->GetNextChild(m_checkListMembers->GetRootItem(), cookie);
	}
	UpdatePreview();
}

void SettersGettersDlg::OnUncheckAll(wxCommandEvent &e)
{
	wxUnusedVar(e);
	wxTreeItemIdValue cookie;
	wxTreeItemId child = m_checkListMembers->GetFirstChild(m_checkListMembers->GetRootItem(), cookie);
	while (child.IsOk()) {
		if (m_checkListMembers->ItemHasChildren(child)) {

			wxTreeItemIdValue gcookie;
			wxTreeItemId gchild = m_checkListMembers->GetFirstChild(child, gcookie);
			while ( gchild.IsOk() ) {
				m_checkListMembers->Check(gchild, false);
				gchild = m_checkListMembers->GetNextChild(child, gcookie);
			}

		}
		child = m_checkListMembers->GetNextChild(m_checkListMembers->GetRootItem(), cookie);
	}
	UpdatePreview();
}

void SettersGettersDlg::OnLeftDown(wxMouseEvent& event)
{
	int flags;
	wxTreeItemId item = m_checkListMembers->HitTest(event.GetPosition(), flags);
	if (item.IsOk() && flags & wxTREE_HITTEST_ONITEMICON) {
		// Post event to update the preview
		wxCommandEvent event(wxEVT_CMD_UPDATE_PREVIEW);
		AddPendingEvent(event);
	}
	event.Skip();
}

void SettersGettersDlg::OnUpdatePreview(wxCommandEvent& e)
{
	wxUnusedVar(e);
	UpdatePreview();
}


bool SettersGettersDlg::DoCheckExistance(const wxString& scope, const wxString& name, const wxString& method_signature)
{
	std::vector<TagEntryPtr> tmp_tags;
	TagsManagerST::Get()->FindByNameAndScope(name, scope.IsEmpty() ? wxT("<global>") : scope, tmp_tags);
	for ( size_t i=0; i<tmp_tags.size(); i++) {
		TagEntryPtr t = tmp_tags.at(i);
		wxString sig_one = TagsManagerST::Get()->NormalizeFunctionSig(t->GetSignature());
		wxString sig_two = TagsManagerST::Get()->NormalizeFunctionSig( method_signature );
		if ( sig_one == sig_two ) {
			return true;
		}
	}
	return false;
}

wxString SettersGettersDlg::GenerateSetter(TagEntryPtr tag)
{
	bool dummy;
	wxString s_dummy;
	return GenerateSetter(tag, dummy, s_dummy);
}


wxString SettersGettersDlg::GenerateGetter(TagEntryPtr tag)
{
	bool dummy;
	wxString s_dummy;
	return GenerateGetter(tag, dummy, s_dummy);
}


void SettersGettersDlg::BuildTree()
{
	SGDlgData data;
	EditorConfigST::Get()->ReadObject(wxT("SGDlgData"), &data);

	m_checkListMembers->Freeze();
	// Append all members to the check list
	m_checkListMembers->DeleteAllItems();
	wxTreeItemId root = m_checkListMembers->AddRoot(wxT("Root"), false, new SettersGettersTreeData(NULL, SettersGettersTreeData::Kind_Root, false));

	m_checkFormat->SetValue(data.GetFormatSource());
	m_checkStartWithUppercase->SetValue(data.GetUseUpperCase());

	m_checkForDuplicateEntries = true;
	for (size_t i=0; i<m_members.size() ; i++) {
		// add child node for the members
		wxTreeItemId parent = m_checkListMembers->AppendItem(root, m_members.at(i)->GetName(), false, new SettersGettersTreeData(NULL, SettersGettersTreeData::Kind_Parent, false));

		// add two children to generate the name of the next entries
		bool     getter_exist (false);
		bool     setter_exist (false);
		wxString setter_display_name;
		wxString getter_display_name;

		wxString getter = GenerateGetter(m_members.at(i), getter_exist, getter_display_name);
		wxString setter = GenerateSetter(m_members.at(i), setter_exist, setter_display_name);

		wxTreeItemId gitem = m_checkListMembers->AppendItem(parent, getter_display_name, false, new SettersGettersTreeData(m_members.at(i), SettersGettersTreeData::Kind_Getter,
															getter_exist ? true : false));
		if ( getter_exist ) {
			m_checkListMembers->SetItemTextColour(gitem, wxT("GREY"));
		}
		wxTreeItemId sitem = m_checkListMembers->AppendItem(parent, setter_display_name, false, new SettersGettersTreeData(m_members.at(i), SettersGettersTreeData::Kind_Setter,
															setter_exist ? true : false));
		if ( setter_exist ) {
			m_checkListMembers->SetItemTextColour(sitem, wxT("GREY"));
		}
		m_checkListMembers->Expand(parent);
	}

	m_checkForDuplicateEntries = false;

	if (m_members.empty() == false) {
		m_textClassName->SetValue(m_members.at(0)->GetParent());
	}


	m_checkListMembers->Thaw();
}

void SettersGettersDlg::UpdateTree()
{
	wxTreeItemIdValue cookie;
	wxTreeItemId child = m_checkListMembers->GetFirstChild(m_checkListMembers->GetRootItem(), cookie);
	while (child.IsOk()) {
		if (m_checkListMembers->ItemHasChildren(child)) {

			wxTreeItemIdValue gcookie;
			wxTreeItemId gchild = m_checkListMembers->GetFirstChild(child, gcookie);
			while ( gchild.IsOk() ) {
				SettersGettersTreeData *data = (SettersGettersTreeData *)m_checkListMembers->GetItemData(gchild);

				wxString display_name;
				bool dummy;
				if ( data->m_kind == SettersGettersTreeData::Kind_Getter ) {
					GenerateGetter(data->m_tag, dummy, display_name);

				} else if ( data->m_kind == SettersGettersTreeData::Kind_Setter ) {
					GenerateSetter(data->m_tag, dummy, display_name);

				}
				m_checkListMembers->SetItemText(gchild, display_name);
				gchild = m_checkListMembers->GetNextChild(child, gcookie);
			}
		}
		child = m_checkListMembers->GetNextChild(m_checkListMembers->GetRootItem(), cookie);
	}
}

void SettersGettersDlg::OnButtonOk(wxCommandEvent& e)
{
	SGDlgData data;
	data.SetFormatSource( m_checkFormat->IsChecked()  );
	data.SetUseUpperCase( m_checkStartWithUppercase->IsChecked() );
	EditorConfigST::Get()->WriteObject(wxT("SGDlgData"), &data);
	e.Skip();
}

//------------------------------------- Configuration Data -----------------------------------

void SGDlgData::DeSerialize(Archive& arch)
{
	if ( arch.Read(wxT("m_useUpperCase"), m_useUpperCase) == false ) {
		m_useUpperCase = true;
	}

	if ( arch.Read(wxT("m_formatSource"), m_formatSource) == false ) {
		m_formatSource = true;
	}
}

void SGDlgData::Serialize(Archive& arch)
{
	arch.Write(wxT("m_useUpperCase"), m_useUpperCase);
	arch.Write(wxT("m_formatSource"), m_formatSource);
}

