#include "progress_dialog.h"

#ifdef __WXMSW__

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
	
	if(value % 20 == 0) {
		wxSafeYield(this, true);
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

#else // UNIX

clProgressDlg::clProgressDlg(wxWindow* parent, const wxString &title, const wxString& msg, int maxValue)
	: wxProgressDialog(title, msg, maxValue, NULL, wxPD_APP_MODAL | wxPD_SMOOTH | wxPD_AUTO_HIDE )
{
	SetSizeHints(400, -1);
	GetSizer()->Fit(this);
	CenterOnScreen();
}

clProgressDlg::~clProgressDlg()
{
}

bool clProgressDlg::Update(int value, const wxString &msg)
{
	return wxProgressDialog::Update(value, msg, NULL);
}

bool clProgressDlg::Pulse(const wxString &msg)
{
	return wxProgressDialog::Pulse(msg, NULL);
}

#endif
