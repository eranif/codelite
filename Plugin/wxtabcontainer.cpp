#include "wxtabcontainer.h"
#include "drawingutils.h"
#include "wx/settings.h"
#include "wx/sizer.h"
#include "wxverticaltab.h"
#include "wxverticalbook.h"
#include "wx/dcbuffer.h"

BEGIN_EVENT_TABLE(wxTabContainer, wxPanel)
EVT_PAINT(wxTabContainer::OnPaint)
EVT_ERASE_BACKGROUND(wxTabContainer::OnEraseBg)
EVT_SIZE(wxTabContainer::OnSizeEvent)
END_EVENT_TABLE()
wxTabContainer::wxTabContainer(wxWindow *win, wxWindowID id, int orientation)
		: wxPanel(win, id)
		, m_orientation(orientation)
{
	Initialize();
}

wxTabContainer::~wxTabContainer()
{
}

void wxTabContainer::Initialize()
{
	wxBoxSizer *sz = new wxBoxSizer(wxVERTICAL);
	SetSizer(sz);
	
	m_tabsSizer = new wxBoxSizer(wxVERTICAL);
	sz->AddSpacer(3);
	sz->Add(m_tabsSizer, 1, wxEXPAND);
	sz->Layout();
}

void wxTabContainer::AddTab(wxVerticalTab *tab)
{
	Freeze();
	m_tabsSizer->Add(tab, 0, wxLEFT | wxRIGHT, 3);

	if (tab->GetSelected()) {
		//find the old selection and unselect it
		wxVerticalTab *selectedTab = GetSelection();
		if (selectedTab) {
			selectedTab->SetSelected( false );
		}
	}

	//if there are no selection, and this tab is not selected as well,
	//force selection
	if (GetSelection() == NULL && tab->GetSelected() == false) {
		tab->SetSelected(true);
	}
	
	Thaw();
	m_tabsSizer->Layout();
	
}

wxVerticalTab* wxTabContainer::GetSelection()
{
	//iteratre over the items in the sizer, and search for the selected one
	wxSizer *sz = m_tabsSizer;
	wxSizerItemList items = sz->GetChildren();

	wxSizerItemList::iterator iter = items.begin();
	for (; iter != items.end(); iter++) {
		wxSizerItem *item = *iter;
		wxWindow *win = item->GetWindow();
		if (win) {
			wxVerticalTab *tab = dynamic_cast<wxVerticalTab*>(win);
			if (tab && tab->GetSelected()) {
				return tab;
			}
		}
	}
	return NULL;
}

void wxTabContainer::SetSelection(wxVerticalTab *tab, bool notify)
{
	//iteratre over the items in the sizer, and search for the selected one
	if (!tab) {
		return;
	}
	
	size_t oldSel((size_t)-1);;
	if(notify) {
		//send event to noitfy that the page is changing
		oldSel = TabToIndex( GetSelection() );

		wxVerticalBookEvent event(wxEVT_COMMAND_VERTICALBOOK_PAGE_CHANGING, GetId());
		event.SetSelection( TabToIndex( tab ) );
		event.SetOldSelection( oldSel );
		event.SetEventObject( this );
		GetEventHandler()->ProcessEvent(event);

		if( !event.IsAllowed() ){
			return;
		}
	}
	
	//let the notebook process this first
	wxVerticalBook *book = dynamic_cast<wxVerticalBook *>( GetParent() );
	if(book) {
		book->SetSelection(tab);
	}
	
	//find the old selection
	wxVerticalTab *oldSelection = GetSelection();
	if (oldSelection) {
		oldSelection->SetSelected(false);
		oldSelection->Refresh();
	}

	tab->SetSelected(true);
	tab->Refresh();
	
	if(notify) {
		//send event to noitfy that the page has changed
		wxVerticalBookEvent event(wxEVT_COMMAND_VERTICALBOOK_PAGE_CHANGED, GetId());
		event.SetSelection( TabToIndex( tab ) );
		event.SetOldSelection( oldSel );
		event.SetEventObject( this );
		GetEventHandler()->ProcessEvent(event);
	}
}

wxVerticalTab* wxTabContainer::IndexToTab(size_t page)
{
	//return the tab of given index
	wxSizer *sz = m_tabsSizer;
	
	//check limit
	if(page >= sz->GetChildren().GetCount()){
		return NULL;
	}
	
	wxSizerItem *szItem = sz->GetItem(page);
	if(szItem) {
		return dynamic_cast< wxVerticalTab* >(szItem->GetWindow());
	}
	return NULL;
}

size_t wxTabContainer::TabToIndex(wxVerticalTab *tab)
{
	//iteratre over the items in the sizer, and search for the selected one
	if(!tab) {
		return static_cast<size_t>(-1);
	}
	
	wxSizer *sz = m_tabsSizer;
	wxSizerItemList items = sz->GetChildren();

	wxSizerItemList::iterator iter = items.begin();
	for (size_t i=0; iter != items.end(); iter++, i++) {
		wxSizerItem *item = *iter;
		wxWindow *win = item->GetWindow();
		if (win) {
			wxVerticalTab *curtab = dynamic_cast<wxVerticalTab*>(win);
			if (curtab == tab) {
				return i;
			}
		}
	}
	return static_cast<size_t>(-1);
}

size_t wxTabContainer::GetTabsCount()
{
	wxSizer *sz = m_tabsSizer;
	wxSizerItemList items = sz->GetChildren();
	return items.GetCount();
}

void wxTabContainer::OnPaint(wxPaintEvent &e)
{
	wxUnusedVar(e);
	wxBufferedPaintDC dc(this);
	
	dc.SetPen(wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW)));
	dc.SetBrush(*wxTRANSPARENT_BRUSH);
	
	wxRect rr = GetClientSize();
	wxColour endColour = DrawingUtils::LightColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE), 30);
	DrawingUtils::PaintStraightGradientBox(dc, rr, wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE), endColour, false);
	
	if(m_orientation == wxRIGHT) {
		rr.x += 3;
		rr.width -= 3;
	}else{
		rr.width -= 3;
	}
	dc.DrawRectangle(rr);
}

void wxTabContainer::OnEraseBg(wxEraseEvent &)
{
	//do nothing
}

void wxTabContainer::OnSizeEvent(wxSizeEvent &e)
{
	wxUnusedVar(e);
	Refresh();
}

void wxTabContainer::SetOrientation(const int& orientation)
{
	this->m_orientation = orientation;
	
	wxSizer *sz = m_tabsSizer;
	wxSizerItemList items = sz->GetChildren();

	wxSizerItemList::iterator iter = items.begin();
	for (; iter != items.end(); iter++) {
		wxSizerItem *item = *iter;
		wxWindow *win = item->GetWindow();
		if (win) {
			wxVerticalTab *curtab = dynamic_cast<wxVerticalTab*>(win);
			if (curtab) {
				curtab->SetOrientation(m_orientation);
			}
		}
	}
	GetSizer()->Layout();
}
