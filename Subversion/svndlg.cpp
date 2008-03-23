#include "svndlg.h"
#include "wx/tokenzr.h"
#include "svncommitmsgsmgr.h"

static void StripComments(wxString &comment)
{
	wxStringTokenizer tok(comment, wxT("\n"), wxTOKEN_STRTOK);
	comment.Clear();
	while(tok.HasMoreTokens()){
		wxString line = tok.GetNextToken();
		line = line.Trim().Trim(false);
		if(!line.StartsWith(wxT("#"))){
			comment << line << wxT("\n");
		}
	}
}

SvnDlg::SvnDlg( wxWindow* parent )
:
SvnBaseDlg( parent )
{
	wxArrayString msgs;
	
	SvnCommitMsgsMgr::Instance()->GetAllMessages( msgs );
	m_comboBoxLastCommitMsgs->Append( msgs );
	
	if( msgs.GetCount() > 0 ) {
		m_comboBoxLastCommitMsgs->SetSelection( msgs.GetCount()-1 );
	}
}

void SvnDlg::OnLastCommitMsgSelected(wxCommandEvent &e)
{
	m_textCtrl->AppendText( e.GetString() );
}

void SvnDlg::OnButtonOK(wxCommandEvent &e)
{
	wxString comment = m_textCtrl->GetValue();
	StripComments( comment );
	
	SvnCommitMsgsMgr::Instance()->AddMessage( comment );
	
	EndModal( wxID_OK );
}
