#include "referenceanalyseroptionsdlg.h"

ReferenceAnalyserOptionsDlg::ReferenceAnalyserOptionsDlg( wxWindow* parent, const AnalyserOptions &options) :
AnalyserOptionsBaseDlg( parent )
{
	m_options = options;
}

void ReferenceAnalyserOptionsDlg::OnButtonOk( wxCommandEvent& event )
{
	wxUnusedVar(event);
	EndModal(wxID_OK);
}
