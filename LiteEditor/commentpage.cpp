#include "editor_config.h"
#include "commentconfigdata.h"
#include "commentpage.h"
#include "free_text_dialog.h"

CommentPage::CommentPage( wxWindow* parent )
		:
		CommentPageBase( parent )
{
	Initialize();
}

void CommentPage::OnClassPattern( wxCommandEvent& event )
{
	FreeTextDialog *dlg = new FreeTextDialog(this, m_textCtrlClassPattern->GetValue(), wxID_ANY, wxT("Class Pattern:"));
	if (dlg->ShowModal() == wxID_OK) {
		m_textCtrlClassPattern->SetValue( dlg->GetValue() );
	}
	dlg->Destroy();
}

void CommentPage::OnFunctionPattern( wxCommandEvent& event )
{
	FreeTextDialog *dlg = new FreeTextDialog(this, m_textCtrlFunctionPattern->GetValue(), wxID_ANY, wxT("Function Pattern:"));
	if (dlg->ShowModal() == wxID_OK) {
		m_textCtrlFunctionPattern->SetValue( dlg->GetValue() );
	}
	dlg->Destroy();
}

void CommentPage::Save()
{
	CommentConfigData data;
	data.SetClassPattern( m_textCtrlClassPattern->GetValue() );
	data.SetFunctionPattern( m_textCtrlFunctionPattern->GetValue());
	data.SetAddStarOnCComment(m_checkBoxContCComment->IsChecked());
	data.SetContinueCppComment(m_checkBoxContinueCppComment->IsChecked());
	data.SetUseShtroodel(m_checkBoxUseShtroodel->IsChecked());
	data.SetUseSlash2Stars(m_checkBoxUseSlash2Starts->IsChecked());

	EditorConfigST::Get()->WriteObject(wxT("CommentConfigData"), &data);
}

void CommentPage::Initialize()
{
	CommentConfigData data;
	EditorConfigST::Get()->ReadObject(wxT("CommentConfigData"), &data);

	m_textCtrlClassPattern->SetValue( data.GetClassPattern() );
	m_textCtrlFunctionPattern->SetValue( data.GetFunctionPattern() );
	m_checkBoxContCComment->SetValue( data.GetAddStarOnCComment() );
	m_checkBoxContinueCppComment->SetValue( data.GetContinueCppComment() );
	m_checkBoxUseShtroodel->SetValue( data.GetUseShtroodel() );
	m_checkBoxUseSlash2Starts->SetValue( data.GetUseSlash2Stars() );
}
