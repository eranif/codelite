#include "customstatusbar.h"

BEGIN_EVENT_TABLE(CustomStatusBar, wxStatusBar)
EVT_SIZE(CustomStatusBar::OnSize)
END_EVENT_TABLE()

CustomStatusBar::CustomStatusBar(wxWindow *parent, wxWindowID id)
: wxStatusBar(parent, id)
{
	SetFieldsCount(5);
	m_gauge = new wxGauge( this, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL|wxNO_BORDER|wxGA_SMOOTH );
}

CustomStatusBar::~CustomStatusBar()
{
}

void CustomStatusBar::OnSize(wxSizeEvent& event)
{
	if ( !m_gauge )
		return;

	wxRect rect;
	GetFieldRect(4, rect);

	m_gauge->SetSize(rect.x, rect.y, rect.width, rect.height);

	event.Skip();
}

void CustomStatusBar::ResetGauge(int range)
{
	m_gauge->SetValue(0);
	m_gauge->SetRange(range);
}

void CustomStatusBar::Update(int vlaue, const wxString& message)
{
	m_gauge->SetValue(vlaue);
	wxUnusedVar(message);
//	SetStatusText(message);
}
