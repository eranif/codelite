#include "compilercompileroptionspage.h"
#include "build_settings_config.h"
#include "compiler.h"
#include "globals.h"

CompilerCompilerOptionsPage::CompilerCompilerOptionsPage( wxWindow* parent, const wxString &cmpname )
: CompilerCompilerOptionsBase( parent )
, m_cmpname(cmpname)
, m_selectedCmpOption(wxNOT_FOUND)
{
	m_listCompilerOptions->InsertColumn(0, wxT("Switch"));
	m_listCompilerOptions->InsertColumn(1, wxT("Help"));
	
	CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(m_cmpname);
	const Compiler::CmpCmdLineOptions& cmpOptions = cmp->GetCompilerOptions();
	Compiler::CmpCmdLineOptions::const_iterator itCmpOption = cmpOptions.begin();
	for ( ; itCmpOption != cmpOptions.end(); ++itCmpOption)
	{
		const Compiler::CmpCmdLineOption& cmpOption = itCmpOption->second;
		long idx = m_listCompilerOptions->InsertItem(m_listCompilerOptions->GetItemCount(), cmpOption.name);
		m_listCompilerOptions->SetItem(idx, 1, cmpOption.help);
	}
}

void CompilerCompilerOptionsPage::Save(CompilerPtr cmp)
{
	Compiler::CmpCmdLineOptions cmpOptions;
	for (int idx = 0; idx < m_listCompilerOptions->GetItemCount(); ++idx)
	{
		Compiler::CmpCmdLineOption cmpOption;
		cmpOption.name = m_listCompilerOptions->GetItemText(idx);
		cmpOption.help = GetColumnText(m_listCompilerOptions, idx, 1);
		
		cmpOptions[cmpOption.name] = cmpOption;
	}
	cmp->SetCompilerOptions(cmpOptions);
}

void CompilerCompilerOptionsPage::OnCompilerOptionActivated( wxListEvent& event )
{
	if (m_selectedCmpOption == wxNOT_FOUND) {
		return;
	}
	
	wxString name = m_listCompilerOptions->GetItemText(m_selectedCmpOption);
	wxString help = GetColumnText(m_listCompilerOptions, m_selectedCmpOption, 1);
	CompilerCompilerOptionDialog dlg(this, name, help);
	if (dlg.ShowModal() == wxID_OK)
	{
		SetColumnText(m_listCompilerOptions, m_selectedCmpOption, 0, dlg.m_sName);
		SetColumnText(m_listCompilerOptions, m_selectedCmpOption, 1, dlg.m_sHelp);
	}
}

void CompilerCompilerOptionsPage::OnNewCompilerOption( wxCommandEvent& event )
{
	CompilerCompilerOptionDialog dlg(this, wxEmptyString, wxEmptyString);
	if (dlg.ShowModal() == wxID_OK)
	{
		long idx = m_listCompilerOptions->InsertItem(m_listCompilerOptions->GetItemCount(), dlg.m_sName);
		m_listCompilerOptions->SetItem(idx, 1, dlg.m_sHelp);
	}
}

void CompilerCompilerOptionsPage::OnDeleteCompilerOption( wxCommandEvent& event )
{
	if (m_selectedCmpOption != wxNOT_FOUND) {
		if (wxMessageBox(_("Are you sure you want to delete this compiler option?"), wxT("CodeLite"), wxYES_NO|wxCANCEL) == wxYES) {
			m_listCompilerOptions->DeleteItem(m_selectedCmpOption);
			m_selectedCmpOption = wxNOT_FOUND;
		}
	}
}

void CompilerCompilerOptionsPage::OnCompilerOptionDeSelected(wxListEvent& event)
{
#ifndef __WXMAC__
	m_selectedCmpOption = wxNOT_FOUND;
#endif
	event.Skip();
}

void CompilerCompilerOptionsPage::OnCompilerOptionSelected(wxListEvent& event)
{
	m_selectedCmpOption = event.m_itemIndex;
	event.Skip();
}
