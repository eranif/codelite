#include "newinheritancedlg.h"
#include "open_type_dlg.h"
#include "imanager.h"

NewIneritanceDlg::NewIneritanceDlg( wxWindow* parent, IManager *mgr, const wxString &parentName, const wxString &access )
: NewIneritanceBaseDlg( parent, wxID_ANY, wxT("New Inheritance"), wxDefaultPosition, wxSize( 505,196 ), wxDEFAULT_DIALOG_STYLE)
, m_mgr(mgr)
{
	//by default select 0
	m_choiceAccess->Select(0);
	if(access.IsEmpty() == false){
		m_choiceAccess->SetStringSelection(access);
	}
	m_textCtrlInhertiance->SetValue(parentName);
}

void NewIneritanceDlg::OnButtonMore( wxCommandEvent& event )
{
	OpenTypeDlg *dlg = new OpenTypeDlg(this, m_mgr->GetTagsManager());
	if(dlg->ShowModal() == wxID_OK){
		m_textCtrlInhertiance->SetValue(dlg->GetSelectedTag()->GetName());
	}
	dlg->Destroy();
}
