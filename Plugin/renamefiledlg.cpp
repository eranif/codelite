#include "renamefiledlg.h"
#include "windowattrmanager.h"

RenameFileDlg::RenameFileDlg( wxWindow* parent, const wxString &replaceWith, std::vector<IncludeStatement> &matches  )
		: RenameFileBaseDlg( parent )
{
	m_textCtrlReplaceWith->SetValue(replaceWith);

	for (size_t i=0; i<matches.size(); i++) {
		wxString         displayString;
		IncludeStatement is = matches.at(i);


		displayString << wxString(is.includedFrom.c_str(), wxConvUTF8)
		<< wxT(":")
		<< is.line;
		int idx = m_checkListMatches->Append(displayString);

		// Keep the information about this entry
		IncludeStatementEntry ise;
		ise.checked = true;
		ise.statement = is;
		m_entries[idx] = ise;
		m_checkListMatches->Check(idx);
	}

	if ( m_checkListMatches->GetCount() ) {
		m_checkListMatches->Select(0);
		DoSelectItem(0, RFD_Select);
	}
	WindowAttrManager::Load(this, wxT("RenameFileDlg"), NULL);
}

void RenameFileDlg::OnFileSelected( wxCommandEvent& event )
{
	DoSelectItem(event.GetSelection(), RFD_None);
}

void RenameFileDlg::OnFileToggeled( wxCommandEvent& event )
{
	DoSelectItem(event.GetSelection(), event.IsChecked() ? RFD_Select : RFD_UnSelect);
}

RenameFileDlg::~RenameFileDlg()
{
	WindowAttrManager::Save(this, wxT("RenameFileDlg"), NULL);
}

void RenameFileDlg::DoSelectItem(int idx, int check)
{
	std::map<int, IncludeStatementEntry>::iterator iter = m_entries.find(idx);
	if ( iter != m_entries.end() ) {
		IncludeStatementEntry ise = iter->second;
		wxString line;
		line << ise.statement.line;
		m_staticTextFoundInLine->SetLabel( line );

		m_staticTextIncludedInFile->SetLabel(wxString(ise.statement.includedFrom.c_str(), wxConvUTF8));
		m_staticTextPattern->SetLabel(wxString::Format(wxT("#include %s"), wxString(ise.statement.pattern.c_str(), wxConvUTF8).c_str()));
		if(check != RFD_None) {
			check == RFD_Select ? ise.checked = true : ise.checked = false;
		}

		// Update the entry
		m_entries[idx] = ise;
	}
}

std::vector<IncludeStatement> RenameFileDlg::GetMatches() const
{
	std::vector<IncludeStatement> matches;
	std::map<int, IncludeStatementEntry>::const_iterator iter = m_entries.begin();
	for(; iter != m_entries.end(); iter++){
		if(iter->second.checked) {
			matches.push_back(iter->second.statement);
		}
	}
	return matches;
}

wxString RenameFileDlg::GetReplaceWith() const
{
	return m_textCtrlReplaceWith->GetValue();
}
