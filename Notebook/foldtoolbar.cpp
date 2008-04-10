#include "wx/settings.h"
#include "wx/dcbuffer.h"
#include "foldtoolbar.h"
#include "foldtoolbargroup.h"

BEGIN_EVENT_TABLE(FoldToolBar, wxPanel)
	EVT_PAINT(FoldToolBar::OnPaint)
	EVT_SIZE(FoldToolBar::OnSize)
	EVT_ERASE_BACKGROUND(FoldToolBar::OnEraseBg)
	EVT_ENTER_WINDOW(FoldToolBar::OnDeselectAll)
//	EVT_LEAVE_WINDOW(FoldToolBar::OnDeselectAll)
END_EVENT_TABLE()

FoldToolBar::FoldToolBar(wxWindow *parent, long style)
: wxPanel(parent)
, m_style(style)
{
	wxBoxSizer *sz = new wxBoxSizer(wxHORIZONTAL);
	SetSizer( sz );
	
	m_mainSizer = new wxBoxSizer(wxHORIZONTAL);
	sz->Add(m_mainSizer, 1, wxEXPAND|wxALL, 2);
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

void FoldToolBar::AddGroup(FoldToolbarGroup *group, int proportion)
{
	m_mainSizer->Add(group, proportion, wxALL|wxEXPAND, 2);
	m_mainSizer->Layout();
}

void FoldToolBar::Realize()
{
	m_mainSizer->Fit(this);
	m_mainSizer->Layout();
}

void FoldToolBar::OnDeselectAll(wxMouseEvent &e)
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
