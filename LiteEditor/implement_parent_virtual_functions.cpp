#include "implement_parent_virtual_functions.h"
#include "editor_config.h"
#include "implparentvirtualfunctionsdata.h"
#include <wx/tokenzr.h>
#include "ctags_manager.h"
#include "cpp_comment_creator.h"
#include "windowattrmanager.h"
#include "context_cpp.h"

ImplementParentVirtualFunctionsDialog::ImplementParentVirtualFunctionsDialog( wxWindow* parent, const wxString &scopeName, const std::vector<TagEntryPtr> &tags, wxChar doxyPrefix, ContextCpp *contextCpp )
		: ImplementParentVirtualFunctionsBase(parent    )
		, m_tags                             (tags      )
		, m_doxyPrefix                       (doxyPrefix)
		, m_contextCpp                       (contextCpp)
		, m_scope                            (scopeName )
{
	WindowAttrManager::Load(this, wxT("ImplementParentVirtualFunctionsDialog"), NULL);
	ImplParentVirtualFunctionsData data;
	EditorConfigST::Get()->ReadObject(wxT("ImplParentVirtualFunctionsData"), &data);

	m_checkBoxFormat->SetValue(data.GetFlags() & ImplParentVirtualFunctionsData::FormatText);
	m_checkBoxAddDoxy->SetValue(data.GetFlags() & ImplParentVirtualFunctionsData::InsertDoxygenComment);
	DoInitialize(false);
}

ImplementParentVirtualFunctionsDialog::~ImplementParentVirtualFunctionsDialog()
{
	ImplParentVirtualFunctionsData data;

	size_t flags(0);
	if(m_checkBoxAddDoxy->IsChecked())
		flags |= ImplParentVirtualFunctionsData::InsertDoxygenComment;

	if(m_checkBoxFormat->IsChecked())
		flags |= ImplParentVirtualFunctionsData::FormatText;

	data.SetFlags(flags);
	EditorConfigST::Get()->WriteObject(wxT("ImplParentVirtualFunctionsData"), &data);
	WindowAttrManager::Save(this, wxT("ImplementParentVirtualFunctionsDialog"), NULL);
}

void ImplementParentVirtualFunctionsDialog::OnAddDoxy(wxCommandEvent& event)
{
	wxUnusedVar(event);
	DoInitialize(true);
}

void ImplementParentVirtualFunctionsDialog::DoInitialize(bool updateDoxyOnly)
{
	m_textCtrlProtos->Clear();

	wxString decl;
	bool addComments = m_checkBoxAddDoxy->IsChecked();

	// Add declration
	//////////////////////////////////////////////////////
	for (size_t i=0; i<m_tags.size(); i++) {
		TagEntryPtr tag = m_tags.at(i);
		if (addComments) {
			// Add doxygen comment
			CppCommentCreator commentCreator(tag, m_doxyPrefix);
			DoxygenComment dc;
			dc.comment = commentCreator.CreateComment();
			dc.name    = tag->GetName();
			m_contextCpp->DoMakeDoxyCommentString( dc );

			// Format the comment
			wxString textComment = dc.comment;
			textComment.Replace(wxT("\r"), wxT("\n"));
			wxArrayString lines = wxStringTokenize(textComment, wxT("\n"), wxTOKEN_STRTOK);
			textComment.Clear();

			for (size_t i=0; i<lines.GetCount(); i++)
				textComment << lines.Item(i) << wxT("\n");

			if(decl.EndsWith(wxT("\n")) == false)
				decl << wxT("\n");

			decl << textComment;
		}

		tag->SetScope(m_scope);
		decl << TagsManagerST::Get()->FormatFunction(tag, false) << wxT("\n");
	}

	m_textCtrlProtos->SetValue(decl);

	if ( !updateDoxyOnly ) {
		// Add Implementations
		//////////////////////////////////////////////////////
		wxString impl;
		m_textCtrlImpl->Clear();
		for (size_t i=0; i<m_tags.size(); i++) {
			TagEntryPtr tag = m_tags.at(i);
			tag->SetScope(m_scope);
			impl << TagsManagerST::Get()->FormatFunction(tag, true) << wxT("\n");
		}
		m_textCtrlImpl->SetValue(impl);

	}
}

wxString ImplementParentVirtualFunctionsDialog::GetDecl()
{
	wxString decl ( m_textCtrlProtos->GetValue() );
	decl.Trim().Trim();
	decl.Prepend(wxT("\n"));
	decl.Append(wxT("\n"));
	return decl;
}

wxString ImplementParentVirtualFunctionsDialog::GetImpl()
{
	return m_textCtrlImpl->GetValue();
}
