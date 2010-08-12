#include "commit_dialog.h"
#include <wx/tokenzr.h>
#include "windowattrmanager.h"
#include "imanager.h"
#include "subversion2.h"
#include "svn_local_properties.h"

class CommitMessageStringData : public wxClientData {
	wxString m_data;
public:
	CommitMessageStringData(const wxString &data) : m_data(data.c_str()){}
	virtual ~CommitMessageStringData(){}

	const wxString &GetData() const {return m_data;}
};

CommitDialog::CommitDialog(wxWindow* parent, Subversion2* plugin)
: CommitDialogBase( parent )
, m_plugin(plugin)
{
	m_checkListFiles->Clear();

	// Hide the bug tracker ID
	m_textCtrlBugID->Clear();
	m_textCtrlBugID->Hide();
	m_staticTextBugID->Hide();

	m_checkListFiles->Disable();
	m_panel1->Disable();
	wxArrayString lastMessages, previews;
	m_plugin->GetCommitMessagesCache().GetMessages(lastMessages, previews);

	for(size_t i=0; i<previews.GetCount(); i++) {
		m_choiceMessages->Append(previews.Item(i), new CommitMessageStringData(lastMessages.Item(i)));
	}

	m_textCtrlMessage->SetFocus();
	WindowAttrManager::Load(this, wxT("CommitDialog"), m_plugin->GetManager()->GetConfigTool());
}

CommitDialog::CommitDialog( wxWindow* parent, const wxArrayString &paths, const wxString &url, Subversion2 *plugin)
		: CommitDialogBase( parent )
		, m_plugin(plugin)
{
	for (size_t i=0; i<paths.GetCount(); i++) {
		int index = m_checkListFiles->Append(paths.Item(i));
		m_checkListFiles->Check((unsigned int)index);
	}

	SubversionLocalProperties props(url);
	m_bugTrackerMessage = props.ReadProperty(SubversionLocalProperties::BUG_TRACKER_MESSAGE);

	if(m_bugTrackerMessage.IsEmpty()) {
		// Hide the bug tracker ID
		m_textCtrlBugID->Clear();
		m_textCtrlBugID->Hide();
		m_staticTextBugID->Hide();
	}

	wxArrayString lastMessages, previews;
	m_plugin->GetCommitMessagesCache().GetMessages(lastMessages, previews);

	for(size_t i=0; i<previews.GetCount(); i++) {
		m_choiceMessages->Append(previews.Item(i), new CommitMessageStringData(lastMessages.Item(i)));
	}

	m_textCtrlMessage->SetFocus();
	WindowAttrManager::Load(this, wxT("CommitDialog"), m_plugin->GetManager()->GetConfigTool());
}

CommitDialog::~CommitDialog()
{
	wxString message = m_textCtrlMessage->GetValue();
	m_plugin->GetCommitMessagesCache().AddMessage(message);
	WindowAttrManager::Save(this, wxT("CommitDialog"), m_plugin->GetManager()->GetConfigTool());
}

wxString CommitDialog::GetMesasge()
{
	wxString msg = NormalizeMessage(m_textCtrlMessage->GetValue());
	if(m_textCtrlBugID->IsShown()) {
		wxString bugTrackerMsg = m_bugTrackerMessage;
		wxString bugId         = m_textCtrlBugID->GetValue();

		bugId.Trim().Trim(false);
		if(bugId.IsEmpty() == false) {
			bugTrackerMsg.Trim().Trim(false);
			bugTrackerMsg.Prepend(wxT("\n"));
			bugTrackerMsg.Replace(wxT("$(BUGID)"), bugId);
			msg << bugTrackerMsg;
		}
	}
	return msg;
}

wxString CommitDialog::NormalizeMessage(const wxString& message)
{
	wxString normalizedStr;
	// first remove the comment section of the text
	wxArrayString lines = wxStringTokenize(message, wxT("\r\n"), wxTOKEN_STRTOK);

	for (size_t i=0; i<lines.GetCount(); i++) {
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
	for (size_t i=0; i<m_checkListFiles->GetCount(); i++) {
		if (m_checkListFiles->IsChecked(i)) {
			paths.Add( m_checkListFiles->GetString(i) );
		}
	}
	return paths;
}

void CommitDialog::OnChoiceMessage(wxCommandEvent& e)
{
	int idx = e.GetSelection();
	if(idx == wxNOT_FOUND)
		return;

	CommitMessageStringData* data = (CommitMessageStringData*)m_choiceMessages->GetClientObject(idx);
	if(data) {
		m_textCtrlMessage->SetValue(data->GetData());
	}
}

