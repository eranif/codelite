#include "LogDialog.h"

LogDialog::LogDialog(wxWindow* parent):_LogDialog(parent)
{
	m_canClose = false;
	
	m_text = _(" --------------- Log starts at: ") + wxNow() + " -------------------\n";
	m_textCtrl11->SetValue(m_text);	
}

LogDialog::~LogDialog()
{
}

void LogDialog::AppendText(const wxString& txt)
{
	m_text.Append(txt + wxT("\n"));
	m_textCtrl11->SetValue(m_text);
}

void LogDialog::Clear()
{
	m_text.clear();
	m_textCtrl11->SetValue(m_text);
}

void LogDialog::OnCloseClick(wxCommandEvent& event)
{
	Clear();
	Destroy();
}
void LogDialog::OnCloseUI(wxUpdateUIEvent& event)
{
	event.Enable(m_canClose);
}

void LogDialog::AppendSeparator()
{
	m_text.Append("*********************************************************\n");
	m_textCtrl11->SetValue(m_text);
}

void LogDialog::AppendComment(const wxString& txt)
{
	m_text.Append(wxNow() + txt + wxT("\n"));
	m_textCtrl11->SetValue(m_text);
}

