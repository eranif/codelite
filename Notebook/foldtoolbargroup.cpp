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

void FoldToolbarGroup::Add(wxWindow* item, int proportion, int border)
{
	wxSizer *sz = GetSizer();
	sz->Add(item, proportion, wxALL|wxEXPAND, border );
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

