#include "addoptioncheckdlg.h"
#include "windowattrmanager.h"
#include <wx/tokenzr.h>
#include <wx/txtstrm.h>
#include <wx/sstream.h>
#include <list>
#include <algorithm> // std::find

AddOptionCheckDlg::AddOptionCheckDlg(wxWindow* parent, const wxString& title, const Compiler::CmpCmdLineOptions& cmpOptions, const wxString& value)
: AddOptionCheckDialogBase(parent, wxID_ANY, title)
, m_cmpOptions(cmpOptions)
{
	WindowAttrManager::Load(this, wxT("AddOptionCheckDlg"), NULL);

	// Fill the list of available options
	Compiler::CmpCmdLineOptions::const_iterator itOption = m_cmpOptions.begin();
	for ( ; itOption != m_cmpOptions.end(); ++itOption)
	{
		const Compiler::CmpCmdLineOption& cmpOption = itOption->second;
		m_checkListOptions->Append(cmpOption.name);
	}
	
	// Update controls
	SetValue(value);
	UpdateCheckOptions();
}

AddOptionCheckDlg::~AddOptionCheckDlg()
{
	WindowAttrManager::Save(this, wxT("AddOptionCheckDlg"), NULL);
}

void AddOptionCheckDlg::SetValue(const wxString& value)
{
	wxString text;
	wxStringTokenizer tkz(value, wxT(";"));
	while(tkz.HasMoreTokens())
	{
		wxString token = tkz.GetNextToken();
		token = token.Trim().Trim(false);
		if (!token.empty())
		{
			text << token << wxT('\n');
		}
	}
	m_textOptions->ChangeValue(text);
}

wxString AddOptionCheckDlg::GetValue() const
{
	wxStringInputStream input(m_textOptions->GetValue());
	wxTextInputStream text(input);

	wxString value;
	while( !input.Eof() ) {
		// Read the next line
		wxString line = text.ReadLine().Trim().Trim(false);
		if (!line.empty())
		{
			value << line << wxT(';');
		}
	}
	return value.BeforeLast(wxT(';'));
}

void AddOptionCheckDlg::OnOptionToggled( wxCommandEvent& event )
{
	UpdateTextOptions();
}

void AddOptionCheckDlg::OnOptionsText( wxCommandEvent& event )
{
	UpdateCheckOptions();
}

void AddOptionCheckDlg::OnOptionSelected( wxCommandEvent& event )
{
	wxString sOption = m_checkListOptions->GetString(event.GetInt());
	Compiler::CmpCmdLineOptions::const_iterator itOption = m_cmpOptions.find(sOption);
	if (itOption != m_cmpOptions.end())
	{
		wxString sHelp;
		sHelp << sOption << wxT(":\n") << itOption->second.help;
		m_textHelp->ChangeValue(sHelp);
	}
}

void AddOptionCheckDlg::UpdateCheckOptions()
{
	// Remove all check boxes
	m_checkListOptions->Freeze();
	for (unsigned int idx = 0; idx < m_checkListOptions->GetCount(); ++idx)
	{
		m_checkListOptions->Check(idx, false);
	}
	
	// Check all options entered 
	wxStringInputStream input(m_textOptions->GetValue());
	wxTextInputStream text(input);
	while( !input.Eof() )
	{
		// Read the next line
		wxString value = text.ReadLine().Trim().Trim(false);
		if (m_cmpOptions.find(value) != m_cmpOptions.end())
		{
			m_checkListOptions->Check(m_checkListOptions->FindString(value));			
		}
	}
	m_checkListOptions->Thaw();
}

void AddOptionCheckDlg::UpdateTextOptions()
{
	// Store all actual options
	std::list<wxString> options;
	wxStringInputStream input(m_textOptions->GetValue());
	wxTextInputStream text(input);
	while( !input.Eof() ) {
		wxString option = text.ReadLine().Trim().Trim(false);
		if (!option.empty())
		{
			options.push_back(option);
		}
	}
	
	// Read check box options
	for (unsigned int idx = 0; idx < m_checkListOptions->GetCount(); ++idx)
	{
		wxString value = m_checkListOptions->GetString(idx);
		if (m_checkListOptions->IsChecked(idx))
		{
			// If the option doesn't exist actually, add it
			if (std::find(options.begin(), options.end(), value) == options.end())
			{
				options.push_back(value);
			}
		}
		else
		{
			// Remove the unchecked option
			options.remove(value);
		}
	}
	
	// Update the options textctrl
	wxString value;
	std::list<wxString>::const_iterator itOption = options.begin();
	for ( ; itOption != options.end(); ++itOption)
	{
		value << *itOption << wxT('\n');
	}
	m_textOptions->ChangeValue(value);
}
