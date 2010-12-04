#include "cc_advance_page.h"

CCAdvancePage::CCAdvancePage( wxWindow* parent, const TagsOptionsData& data, TagsOptionsDlg *pd )
	: CCAdvancedBasePage( parent )
	, m_dlg(pd)
{
	m_textPrep->SetValue         (data.GetTokens());
	m_textTypes->SetValue        (data.GetTypes());
	m_textCtrlFilesList->SetValue(data.GetMacrosFiles());
	m_textFileSpec->SetValue     (data.GetFileSpec());
}

void CCAdvancePage::OnParse( wxCommandEvent& event )
{
	m_dlg->Parse();
}

void CCAdvancePage::OnFileSelectedUI( wxUpdateUIEvent& event )
{
	event.Enable( m_textCtrlFilesList->GetValue().IsEmpty() == false);
}

void CCAdvancePage::Save(TagsOptionsData& data)
{
	data.SetFileSpec(m_textFileSpec->GetValue());
	data.SetTokens(m_textPrep->GetValue());
	data.SetTypes(m_textTypes->GetValue());
	data.SetMacrosFiles( m_textCtrlFilesList->GetValue() );
}

wxString CCAdvancePage::GetFiles() const
{
	return m_textCtrlFilesList->GetValue();
}
