#include "ccboxmainpanel.h"
#include <wx/dcbuffer.h>

BEGIN_EVENT_TABLE(CCBoxMainPanel, wxPanel)
EVT_PAINT(CCBoxMainPanel::OnPaint)
EVT_ERASE_BACKGROUND(CCBoxMainPanel::OnErasebg)
END_EVENT_TABLE()

CCBoxMainPanel::CCBoxMainPanel(wxWindow* win)
: wxPanel(win)
{
}

CCBoxMainPanel::~CCBoxMainPanel()
{
}

void CCBoxMainPanel::OnErasebg(wxEraseEvent& e)
{
}

void CCBoxMainPanel::OnPaint(wxPaintEvent& e)
{
	wxBufferedPaintDC dc(this);
	dc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW));
	dc.SetBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
	dc.DrawRectangle(0, 0, this->GetSize().x, this->GetSize().y);
}

