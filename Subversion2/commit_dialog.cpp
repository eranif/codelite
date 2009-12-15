#include "commit_dialog.h"
#include <wx/tokenzr.h>
#include "windowattrmanager.h"
#include "imanager.h"

CommitDialog::CommitDialog( wxWindow* parent, const wxArrayString &paths, IManager *manager )
: CommitDialogBase( parent )
, m_manager(manager)
{
	for(size_t i=0; i<paths.GetCount(); i++) {
		int index = m_checkListFiles->Append(paths.Item(i));
		m_checkListFiles->Check((unsigned int)index);
	}
	m_textCtrlMessage->SetFocus();
	WindowAttrManager::Load(this, wxT("CommitDialog"), m_manager->GetConfigTool());
}

CommitDialog::~CommitDialog()
{
	WindowAttrManager::Save(this, wxT("CommitDialog"), m_manager->GetConfigTool());
}

wxString CommitDialog::GetMesasge()
{
	return NormalizeMessage(m_textCtrlMessage->GetValue());
}

wxString CommitDialog::NormalizeMessage(const wxString& message)
{
	wxString normalizedStr;
	// first remove the comment section of the text
	wxArrayString lines = wxStringTokenize(message, wxT("\r\n"), wxTOKEN_STRTOK);

	for(size_t i=0; i<lines.GetCount(); i++) {
		wxString line = lines.Item(i);
		line = line.Trim().Trim(false);
		if ( !line.StartsWith(wxT("#")) ) {
			normalizedStr << line << wxT("\n");
		}
	}

	normalizedStr.Trim().Trim(false);

	// SVN does not like any quotation marks in the comment -> escape them
	normalizedStr.Replace(wxT("\""), wxT("\\\""));
	return normalizedStr;
}

wxArrayString CommitDialog::GetPaths()
{
	wxArrayString paths;
	for(size_t i=0; i<m_checkListFiles->GetCount(); i++) {
		if(m_checkListFiles->IsChecked(i)) {
			paths.Add( m_checkListFiles->GetString(i) );
		}
	}
	return paths;
}

void CommitDialog::OnChoiceMessage(wxCommandEvent& e)
{
//	m_choiceMessages->GetClientObject()
}
