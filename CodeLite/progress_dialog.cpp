#include "progress_dialog.h"

clProgressDlg::clProgressDlg( wxWindow* parent, const wxString &title, const wxString& msg, int maxValue )
	: clProgressDlgBase( parent )
{
	SetSizeHints(400, -1);
	SetTitle(title);
	m_staticLabel->SetLabel(msg);
	m_gauge->SetRange(maxValue);
	GetSizer()->Fit(this);
	Show();
	wxYieldIfNeeded();
}

bool clProgressDlg::Update(int value, const wxString& msg)
{
	m_staticLabel->SetLabel(msg);
	m_gauge->SetValue(value);
	
	if(value % 10 == 0) {
		wxYieldIfNeeded();
		this->ProcessPendingEvents();
	}
	return true;
}

bool clProgressDlg::Pulse(const wxString& msg)
{
	int curval = m_gauge->GetValue();
	m_gauge->SetValue(curval+1);
	if(!msg.IsEmpty())
		m_staticLabel->SetLabel(msg);
	return true;
}
