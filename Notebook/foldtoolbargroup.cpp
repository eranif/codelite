#include "wx/image.h"
#include "wx/settings.h"
#include "wx/dcbuffer.h"
#include "foldtoolbargroup.h"
#include "foldtoolbar.h"
//#include "drawingutils.h"

BEGIN_EVENT_TABLE(FoldToolbarGroup, wxPanel)
	EVT_PAINT(FoldToolbarGroup::OnPaint)
	EVT_SIZE(FoldToolbarGroup::OnSize)
	EVT_ERASE_BACKGROUND(FoldToolbarGroup::OnEraseBg)
	EVT_ENTER_WINDOW(FoldToolbarGroup::OnEnterWindow)
END_EVENT_TABLE()

FoldToolbarGroup::FoldToolbarGroup(FoldToolBar *parent, const wxString &caption)
: wxPanel(parent)
, m_caption(caption)
, m_hovered(false)
{
	Initialize();
}

FoldToolbarGroup::~FoldToolbarGroup()
{
}

void FoldToolbarGroup::OnPaint(wxPaintEvent &e)
{
	wxUnusedVar(e);
	wxBufferedPaintDC dc(this);
	
	wxRect rr(GetClientSize());
	
	dc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
	dc.SetBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
	dc.DrawRectangle(rr);
	
	if(m_hovered) {
		//draw a simple raisde border
		
		dc.SetPen(*wxWHITE_PEN);
		dc.DrawLine(rr.x, rr.y, rr.x+rr.width-1, rr.y);
		dc.DrawLine(rr.x, rr.y, rr.x, rr.y+rr.height-1);
		
		dc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW));
		dc.DrawLine(rr.x+rr.width-1, rr.y, rr.x+rr.width-1, rr.y+rr.height-1);
		dc.DrawLine(rr.x, rr.y+rr.height-1, rr.x+rr.width, rr.y+rr.height-1);
	}
}

void FoldToolbarGroup::OnEraseBg(wxEraseEvent &e)
{
	wxUnusedVar(e);
}

void FoldToolbarGroup::OnSize(wxSizeEvent &e)
{
	Refresh();
	e.Skip();
}

void FoldToolbarGroup::Initialize()
{
	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	SetSizer(sizer);
}

void FoldToolbarGroup::Add(wxWindow* item, int proportion)
{
	wxSizer *sz = GetSizer();
	sz->Add(item, proportion, wxALL|wxEXPAND, 3);
}

void FoldToolbarGroup::OnEnterWindow(wxMouseEvent &e)
{
	m_hovered = true;
	Refresh();
	e.Skip();
}

void FoldToolbarGroup::SetHovered(const bool& hovered)
{
	this->m_hovered = hovered;
	Refresh();
}

