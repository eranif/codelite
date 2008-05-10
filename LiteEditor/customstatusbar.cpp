#include "progressctrl.h"
#include "customstatusbar.h"

BEGIN_EVENT_TABLE(CustomStatusBar, wxStatusBar)
EVT_SIZE(CustomStatusBar::OnSize)
END_EVENT_TABLE()

CustomStatusBar::CustomStatusBar(wxWindow *parent, wxWindowID id)
: wxStatusBar(parent, id)
{
	SetFieldsCount(5);
	m_gauge = new ProgressCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	m_gauge->SetMaxRange(200);
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
	m_gauge->SetSize(rect.x+1, rect.y+1, rect.width-2, rect.height-2);
	event.Skip();
}

void CustomStatusBar::ResetGauge(int range)
{
	m_gauge->SetMaxRange((size_t)range);
	m_gauge->Update(0, wxEmptyString);
}

void CustomStatusBar::Update(int value, const wxString& message)
{
	m_gauge->Update((size_t)value, message);
}
