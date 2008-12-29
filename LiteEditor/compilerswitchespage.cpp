#include "compilerswitchespage.h"
#include "compiler.h"
#include "build_settings_config.h"
#include "globals.h"
#include <wx/textdlg.h>

CompilerSwitchesPage::CompilerSwitchesPage( wxWindow* parent, const wxString &cmpname )
		: CompilerSwitchesBase( parent )
		, m_cmpname(cmpname)
{
	InitSwitches();
}

void CompilerSwitchesPage::AddSwitch(const wxString &name, const wxString &value, bool choose)
{
	long item = AppendListCtrlRow(m_listSwitches);
	SetColumnText(m_listSwitches, item, 0, name);
	SetColumnText(m_listSwitches, item, 1, value);

	if (choose) {
		m_selSwitchName = name;
		m_selSwitchValue = value;
	}
}

void CompilerSwitchesPage::EditSwitch()
{
	wxString message;
	message << m_selSwitchName << wxT(" switch:");
	wxTextEntryDialog dlg(this, message, wxT("Edit"), m_selSwitchValue);
	if (dlg.ShowModal() == wxID_OK) {
		wxString newVal = dlg.GetValue();
		CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(m_cmpname);
		cmp->SetSwitch(m_selSwitchName, dlg.GetValue());
		BuildSettingsConfigST::Get()->SetCompiler(cmp);
		InitSwitches();
	}
}

void CompilerSwitchesPage::OnItemActivated( wxListEvent& event )
{
		//get the var name & value
	wxListItem info;
	info.m_itemId = event.m_itemIndex;
	info.m_col = 0;	//name
	info.m_mask = wxLIST_MASK_TEXT;

	if ( m_listSwitches->GetItem(info) ) {
		m_selSwitchName = info.m_text;
	}

	info.m_col = 1;//value
	if ( m_listSwitches->GetItem(info)) {
		m_selSwitchValue = info.m_text;
	}
	EditSwitch();
}

void CompilerSwitchesPage::OnItemSelected( wxListEvent& event )
{
	//get the var name & value
	wxListItem info;
	info.m_itemId = event.m_itemIndex;
	info.m_col = 0;	//name
	info.m_mask = wxLIST_MASK_TEXT;

	if ( m_listSwitches->GetItem(info) ) {
		m_selSwitchName = info.m_text;
	}

	info.m_col = 1;//value
	if ( m_listSwitches->GetItem(info)) {
		m_selSwitchValue = info.m_text;
	}
}

void CompilerSwitchesPage::InitSwitches()
{
	//insert columns:
	m_listSwitches->Freeze();
	m_listSwitches->ClearAll();
	m_listSwitches->InsertColumn(0, wxT("Name"));
	m_listSwitches->InsertColumn(1, wxT("Value"));

	//populate the list control
	CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(m_cmpname);
	Compiler::ConstIterator iter = cmp->SwitchesBegin();
	for (; iter != cmp->SwitchesEnd(); iter++) {
		AddSwitch(iter->first, iter->second, iter == cmp->SwitchesBegin());
	}
	m_listSwitches->Thaw();
}

void CompilerSwitchesPage::Save(CompilerPtr cmp)
{
	wxUnusedVar(cmp);
}
