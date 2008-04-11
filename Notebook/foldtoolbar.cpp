#include "wx/settings.h"
#include "wx/dcbuffer.h"
#include "foldtoolbar.h"
#include "foldtoolbargroup.h"

#define BUTTON_WIDTH 	6
#define BUTTON_MARGIN 	1

BEGIN_EVENT_TABLE(FoldToolBar, wxPanel)
	EVT_PAINT(FoldToolBar::OnPaint)
	EVT_SIZE(FoldToolBar::OnSize)
	EVT_ERASE_BACKGROUND(FoldToolBar::OnEraseBg)
	EVT_ENTER_WINDOW(FoldToolBar::OnEnterWindow)
	EVT_LEAVE_WINDOW(FoldToolBar::OnLeaveWindow)
	EVT_MOTION(FoldToolBar::OnMouseMove)
	EVT_LEFT_DOWN(FoldToolBar::OnLeftDown)
	EVT_LEFT_UP(FoldToolBar::OnLeftUp)
END_EVENT_TABLE()

FoldToolBar::FoldToolBar(wxWindow *parent, long style)
		: wxPanel(parent)
		, m_style(style)
		, m_state(FB_BTN_NONE)
{
	wxBoxSizer *sz = new wxBoxSizer(wxHORIZONTAL);
	SetSizer( sz );

	m_mainSizer = new wxBoxSizer(wxHORIZONTAL);
	sz->Add(m_mainSizer, 1, wxEXPAND|wxRIGHT, BUTTON_WIDTH+BUTTON_MARGIN);
}

FoldToolBar::~FoldToolBar()
{
}

void FoldToolBar::Initialize()
{
}

void FoldToolBar::OnPaint(wxPaintEvent &e)
{
	wxUnusedVar(e);
	wxBufferedPaintDC dc(this);
	wxRect rr(GetClientSize());

	dc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
	dc.SetBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));

	//paint the background in 3DFACE
	dc.DrawRectangle(rr);

	wxPoint leftUp		(rr.x + rr.width - BUTTON_WIDTH - BUTTON_MARGIN, rr.y);
	wxPoint leftBottom	(rr.x + rr.width - BUTTON_WIDTH - BUTTON_MARGIN, rr.y + rr.height);
	wxPoint rightUp		(rr.x + rr.width - BUTTON_MARGIN, rr.y);
	wxPoint rightBottom	(rr.x + rr.width - BUTTON_MARGIN, rr.y + rr.height);

	switch (m_state) {
	case FB_BTN_HOVERED:
		dc.SetPen(*wxWHITE_PEN);
		dc.DrawLine(leftUp, rightUp);
		dc.DrawLine(leftUp, leftBottom);

		dc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW));
		dc.DrawLine(rightUp, rightBottom);
		dc.DrawLine(rightBottom, leftBottom);
		break;
	case FB_BTN_PUSHED:
		dc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW));
		dc.DrawLine(leftUp, rightUp);
		dc.DrawLine(leftUp, leftBottom);

		dc.SetPen(*wxWHITE_PEN);
		dc.DrawLine(rightUp, rightBottom);
		dc.DrawLine(rightBottom, leftBottom);
		break;
	case FB_BTN_NONE:
	default:
		break;
	}
}

void FoldToolBar::OnEraseBg(wxEraseEvent &e)
{
	wxUnusedVar(e);
}

void FoldToolBar::OnSize(wxSizeEvent &e)
{
	Refresh();
	e.Skip();
}

void FoldToolBar::AddGroup(FoldToolbarGroup *group, int proportion, int border)
{
	m_mainSizer->Add(group, proportion, wxALL|wxEXPAND, border);
	m_mainSizer->Layout();
}

void FoldToolBar::Realize()
{
	m_mainSizer->Fit(this);
	m_mainSizer->Layout();
}

void FoldToolBar::OnEnterWindow(wxMouseEvent &e)
{
	wxSizerItemList items = m_mainSizer->GetChildren();
	wxSizerItemList::iterator iter = items.begin();
	for (; iter != items.end(); iter++) {
		wxSizerItem *item = *iter;
		wxWindow *win = item->GetWindow();
		if (win) {
			FoldToolbarGroup *group = (FoldToolbarGroup*)win;
			group->SetHovered(false);
		}
	}
}

void FoldToolBar::OnLeftDown(wxMouseEvent &e)
{
	if (m_state == FB_BTN_HOVERED) {
		//we are on top of the button area
		m_state = FB_BTN_PUSHED;
		Refresh();
	}
	e.Skip();
}

void FoldToolBar::OnMouseMove(wxMouseEvent &e)
{
	if(m_state == FB_BTN_PUSHED) {
		e.Skip();
		return;
	}
	
	FbButtonState curState = m_state;

	wxRect rr = GetClientSize();
	wxRect buttonRect(	rr.x + rr.width - BUTTON_WIDTH - BUTTON_MARGIN,
	                   rr.y,
	                   BUTTON_WIDTH,
	                   rr.height);

	if (buttonRect.Contains(e.GetPosition())) {
		m_state = FB_BTN_HOVERED;
	} else {
		m_state = FB_BTN_NONE;
	}

	//update only if needed
	if (curState != m_state) {
		Refresh();
	}

	e.Skip();
}

void FoldToolBar::OnLeaveWindow(wxMouseEvent &e)
{
	m_state = FB_BTN_NONE;
	Refresh();
	e.Skip();
}

void FoldToolBar::OnLeftUp(wxMouseEvent &e)
{
	wxRect rr = GetClientSize();
	wxRect buttonRect(	rr.x + rr.width - BUTTON_WIDTH - BUTTON_MARGIN,
	                   rr.y,
	                   BUTTON_WIDTH,
	                   rr.height);

	if (buttonRect.Contains(e.GetPosition())) {
		m_state = FB_BTN_HOVERED;
	} else {
		m_state = FB_BTN_NONE;
	}

	Refresh();
	e.Skip();
}
