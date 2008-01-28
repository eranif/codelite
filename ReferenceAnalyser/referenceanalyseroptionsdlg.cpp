#include "referenceanalyseroptionsdlg.h"

ReferenceAnalyserOptionsDlg::ReferenceAnalyserOptionsDlg( wxWindow* parent, const AnalyserOptions &options) :
AnalyserOptionsBaseDlg( parent )
{
	m_options = options;
	m_containerName->SetValue(m_options.getContainerName());
	m_containerOperator->SetValue(m_options.getContainerOperator());
}

void ReferenceAnalyserOptionsDlg::OnButtonOk( wxCommandEvent& event )
{
	wxUnusedVar(event);
	
	m_options.setContainerName(m_containerName->GetValue());
	m_options.setContainerOperator(m_containerOperator->GetValue());
	
	EndModal(wxID_OK);
}
