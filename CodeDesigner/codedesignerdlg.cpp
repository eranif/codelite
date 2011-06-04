#include "codedesignerdlg.h"
#include <wx/filedlg.h>

CodeDesignerDlg::CodeDesignerDlg(wxWindow *parent) : CodeDesignerBaseDlg(parent)
{
}

CodeDesignerDlg::~CodeDesignerDlg()
{
}

void CodeDesignerDlg::OnButtonBrowse(wxCommandEvent& event)
{
	wxUnusedVar(event);
	wxString new_path = wxFileSelector(_("Select CodeDesigner executable:"), m_textCtrlCDPath->GetValue().c_str(), wxT(""), wxT(""), wxFileSelectorDefaultWildcardStr, 0, this);
	if ( !new_path.IsEmpty() )
	{
		m_textCtrlCDPath->SetValue( new_path );
	}
}

void CodeDesignerDlg::OnButtonCancel(wxCommandEvent& event)
{
	EndModal( wxID_CANCEL );
}

void CodeDesignerDlg::OnButtonOK(wxCommandEvent& event)
{
	m_Path = m_textCtrlCDPath->GetValue();
	m_Port = m_textCtrlComPort->GetValue();
	
	EndModal( wxID_OK );
}

void CodeDesignerDlg::OnInit(wxInitDialogEvent& event)
{
	m_textCtrlCDPath->SetValue( m_Path );
	m_textCtrlComPort->SetValue( m_Port );

	event.Skip();
}
