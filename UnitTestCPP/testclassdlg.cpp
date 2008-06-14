#include "testclassdlg.h"
#include "imanager.h"
#include "ctags_manager.h"

TestClassDlg::TestClassDlg( wxWindow* parent, IManager *mgr )
		: TestClassBaseDlg( parent )
		, m_manager(mgr)
{
	m_manager->GetTagsManager()->GetClasses(m_tags);
	IEditor *editor = m_manager->GetActiveEditor();
	if (editor) {
		m_textCtrlFileName->SetValue(editor->GetFileName().GetFullPath());
		m_textCtrlFileName->Enable(false);
	}
}

void TestClassDlg::OnRefreshFunctions( wxCommandEvent& event )
{
	std::vector<TagEntryPtr> matches;

	// search m_tags for suitable name
	for (size_t i=0; i<m_tags.size(); i++) {
		TagEntryPtr tag = m_tags.at(i);
		if (tag->GetName() == m_textCtrlClassName->GetValue()) {
			matches.push_back(tag);
		}
	}

	if (matches.empty()) {
		return;
	}

	wxString theClass;
	if (matches.size() == 1) {
		//single match we are good
		theClass = matches.at(0)->GetPath();
	} else {
		// suggest the user a multiple choice
		wxArrayString choices;

		for (size_t i=0; i<matches.size(); i++) {
			wxString option;
			TagEntryPtr t = matches.at(i);
			choices.Add(t->GetPath());
		}

		theClass = wxGetSingleChoice(wxT("Select class:"), wxT("Select class:"), choices, this);
	}

	if (theClass.empty()) {
		return;
	}

	// get list of methods for the given path
	matches.clear();
	m_manager->GetTagsManager()->TagsByScope(theClass, wxT("prototype"), matches, false, true);

	// populate the list control
	wxArrayString methods;
	for (size_t i=0; i<matches.size(); i++) {
		TagEntryPtr t = matches.at(i);
		methods.Add(t->GetName() + t->GetSignature());
	}
	m_checkListMethods->Clear();
	m_checkListMethods->Append(methods);

	// check all items
	for (unsigned int idx = 0; idx < m_checkListMethods->GetCount(); idx++) {
		m_checkListMethods->Check(idx, true);
	}
}

void TestClassDlg::OnUseActiveEditor( wxCommandEvent& event )
{
	if (event.IsChecked()) {
		IEditor *editor = m_manager->GetActiveEditor();
		if (editor) {
			m_textCtrlFileName->SetValue(editor->GetFileName().GetFullPath());
		}
		m_textCtrlFileName->Enable(false);
	} else {
		m_textCtrlFileName->Enable(true);
	}
}

void TestClassDlg::OnRefreshButtonUI(wxUpdateUIEvent& e)
{
	e.Enable(m_textCtrlClassName->IsEmpty() ? false : true );
}

void TestClassDlg::OnCheckAll(wxCommandEvent& e)
{
	// check all items
	for (unsigned int idx = 0; idx < m_checkListMethods->GetCount(); idx++) {
		m_checkListMethods->Check(idx, true);
	}
}

void TestClassDlg::OnUnCheckAll(wxCommandEvent& e)
{
	// check all items
	for (unsigned int idx = 0; idx < m_checkListMethods->GetCount(); idx++) {
		m_checkListMethods->Check(idx, false);
	}
}
wxArrayString TestClassDlg::GetTestsList()
{
	wxArrayString results;
	for (unsigned int idx = 0; idx < m_checkListMethods->GetCount(); idx++) {
		if(m_checkListMethods->IsChecked(idx)){
			wxString str = m_checkListMethods->GetString(idx);
			results.Add(str.BeforeFirst(wxT('(')));
		}
	}
	return results;
}

void TestClassDlg::OnUseFixture(wxCommandEvent& e)
{
	m_textCtrlFixtureName->Enable(e.IsChecked());
}
