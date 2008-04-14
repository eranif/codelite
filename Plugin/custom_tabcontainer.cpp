#include "dropbutton.h"
#include "custom_tabcontainer.h"
#include "drawingutils.h"
#include "wx/settings.h"
#include "wx/sizer.h"
#include "custom_tab.h"
#include "custom_notebook.h"
#include "wx/dcbuffer.h"
#include "wx/menu.h"

#include <wx/arrimpl.cpp> // this is a magic incantation which must be done!

BEGIN_EVENT_TABLE(wxTabContainer, wxPanel)
	EVT_PAINT(wxTabContainer::OnPaint)
	EVT_ERASE_BACKGROUND(wxTabContainer::OnEraseBg)
	EVT_SIZE(wxTabContainer::OnSizeEvent)
END_EVENT_TABLE()

wxTabContainer::wxTabContainer(wxWindow *win, wxWindowID id, int orientation, long style)
		: wxPanel(win, id)
		, m_orientation(orientation)
		, m_draggedTab(NULL)
		, m_rightClickMenu(NULL)
{
	Initialize();
}

wxTabContainer::~wxTabContainer()
{
	if (m_rightClickMenu) {
		delete m_rightClickMenu;
		m_rightClickMenu = NULL;
	}
}

void wxTabContainer::Initialize()
{
	wxOrientation sizerOri( wxHORIZONTAL );
	if (m_orientation == wxLEFT || m_orientation == wxRIGHT) {
		sizerOri = wxVERTICAL;
	}

	wxBoxSizer *sz = new wxBoxSizer(sizerOri);
	SetSizer(sz);

	m_tabsSizer = new wxBoxSizer(sizerOri);

	DropButton *btn = new DropButton(this, this);

	int flag(wxALIGN_CENTER_VERTICAL);

	if (sizerOri == wxVERTICAL) {
		flag = wxALIGN_CENTER_HORIZONTAL;
	}

	sz->Add(btn, 0, flag|wxALL, 2);
	sz->Add(m_tabsSizer, 1, wxEXPAND);
	sz->Layout();
}

void wxTabContainer::AddTab(CustomTab *tab)
{
	size_t oldSel(0);

	if (tab->GetSelected() == false && GetTabsCount() == 0) {
		tab->SetSelected(true);
		PushPageHistory(tab);
	}

	if (m_orientation == wxLEFT || m_orientation == wxRIGHT) {
		m_tabsSizer->Add(tab, 0, wxLEFT | wxRIGHT, 3);
	} else {
		m_tabsSizer->Add(tab, 0, wxTOP | wxBOTTOM, 3);
	}

	if (tab->GetSelected()) {
		//find the old selection and unselect it
		CustomTab *selectedTab = GetSelection();

		if (selectedTab && selectedTab != tab) {
			selectedTab->SetSelected( false );
			oldSel = TabToIndex( selectedTab );
		}
	}

	m_tabsSizer->Layout();
	if (tab->GetSelected()) {

		EnsureVisible(tab);
		PushPageHistory(tab);

		//fire page changed event
		NotebookEvent event(wxEVT_COMMAND_BOOK_PAGE_CHANGED, GetId());
		event.SetSelection( TabToIndex(tab) );
		event.SetOldSelection( oldSel );
		event.SetEventObject( this );
		GetEventHandler()->ProcessEvent(event);
	}
}

CustomTab* wxTabContainer::GetSelection()
{
	//iteratre over the items in the sizer, and search for the selected one
	wxSizer *sz = m_tabsSizer;
	wxSizerItemList items = sz->GetChildren();

	wxSizerItemList::iterator iter = items.begin();
	for (; iter != items.end(); iter++) {
		wxSizerItem *item = *iter;
		wxWindow *win = item->GetWindow();
		if (win) {
			CustomTab *tab = (CustomTab*)win;
			if (tab && tab->GetSelected()) {
				return tab;
			}
		}
	}
	return NULL;
}

void wxTabContainer::SetSelection(CustomTab *tab, bool notify)
{
	//iteratre over the items in the sizer, and search for the selected one
	if (!tab) {
		return;
	}

	tab->GetWindow()->SetFocus();

	size_t oldSel((size_t)-1);
	if (notify) {
		//send event to noitfy that the page is changing
		oldSel = TabToIndex( GetSelection() );

		NotebookEvent event(wxEVT_COMMAND_BOOK_PAGE_CHANGING, GetId());
		event.SetSelection( TabToIndex( tab ) );
		event.SetOldSelection( oldSel );
		event.SetEventObject( this );
		GetEventHandler()->ProcessEvent(event);

		if ( !event.IsAllowed() ) {
			return;
		}
	}

	//let the notebook process this first
	Notebook *book = (Notebook *)GetParent();
	if (book) {
		book->SetSelection(tab);
	}

	//find the old selection
	CustomTab *oldSelection = GetSelection();
	if (oldSelection) {
		oldSelection->SetSelected(false);
		oldSelection->Refresh();
	}

	tab->SetSelected(true);
	EnsureVisible(tab);

	tab->Refresh();
	tab->GetWindow()->SetFocus();

	//add this page to the history
	PushPageHistory(tab);

	if (notify) {
		//send event to noitfy that the page has changed
		NotebookEvent event(wxEVT_COMMAND_BOOK_PAGE_CHANGED, GetId());
		event.SetSelection( TabToIndex( tab ) );
		event.SetOldSelection( oldSel );
		event.SetEventObject( this );
		GetEventHandler()->ProcessEvent(event);
	}
}

CustomTab* wxTabContainer::IndexToTab(size_t page)
{
	//return the tab of given index
	wxSizer *sz = m_tabsSizer;

	//check limit
	if (page >= sz->GetChildren().GetCount()) {
		return NULL;
	}

	wxSizerItem *szItem = sz->GetItem(page);
	if (szItem) {
		return (CustomTab*)szItem->GetWindow();
	}
	return NULL;
}

size_t wxTabContainer::TabToIndex(CustomTab *tab)
{
	//iteratre over the items in the sizer, and search for the selected one
	if (!tab) {
		return static_cast<size_t>(-1);
	}

	wxSizer *sz = m_tabsSizer;
	wxSizerItemList items = sz->GetChildren();

	wxSizerItemList::iterator iter = items.begin();
	for (size_t i=0; iter != items.end(); iter++, i++) {
		wxSizerItem *item = *iter;
		wxWindow *win = item->GetWindow();
		if (win == tab) {
			return i;
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
	wxBufferedPaintDC dc(this);
	Notebook *book = (Notebook *)GetParent();

	wxRect rr = GetClientSize();

	if (GetTabsCount() == 0) {
		dc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
		dc.SetBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
		dc.DrawRectangle(rr);
		return;
	}

	wxColour col1 = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
	wxColour col2 = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);

	if ( book->m_style & wxVB_BG_GRADIENT ) {
		col1 = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
		col2 = DrawingUtils::LightColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW), 30);
	}

	//paint gradient background
	switch (m_orientation) {
	case wxLEFT:
		DrawingUtils::PaintStraightGradientBox(dc, rr, col1, col2, false);
		break;
	case wxRIGHT:
		DrawingUtils::PaintStraightGradientBox(dc, rr, col2, col1, false);
		break;
	case wxBOTTOM:
		DrawingUtils::PaintStraightGradientBox(dc, rr, col2, col1, true);
		break;
	case wxTOP:
	default:
		DrawingUtils::PaintStraightGradientBox(dc, rr, col1, col2, true);
		break;
	}

	//draw border around the tab area
	if (book->m_style & wxVB_BORDER) {
		dc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW));
		dc.SetBrush(*wxTRANSPARENT_BRUSH);
		dc.DrawRectangle(rr);
	}

	dc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
	for (int i=0; i<3; i++) {
		switch (m_orientation) {
		case wxRIGHT:
			dc.DrawLine(rr.x+i, rr.y, rr.x+i, rr.y+rr.height);
			break;
		case wxTOP:
			dc.DrawLine(rr.x, rr.height-i-1, rr.x+rr.width, rr.height-i-1);
			break;
		case wxLEFT:
			dc.DrawLine(rr.x+rr.width-i, rr.y, rr.x+rr.width-i, rr.y+rr.height);
			break;
		default:
			dc.DrawLine(rr.x, rr.y+i, rr.x + rr.width, rr.y+i);
			break;
		}
	}

	dc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW));
	switch (m_orientation) {
	case wxRIGHT:
		dc.DrawLine(rr.x+3, rr.y, rr.x+3, rr.y+rr.height);
		break;
	case wxTOP:
		dc.DrawLine(rr.x, rr.height-4, rr.x+rr.width, rr.height-4);
		break;
	case wxLEFT:
		dc.DrawLine(rr.x+rr.width-4, rr.y, rr.x+rr.width-4, rr.y+rr.height);
		break;
	default:
		dc.DrawLine(rr.x, rr.y+3, rr.x + rr.width, rr.y+3);
		break;
	}
}

void wxTabContainer::OnEraseBg(wxEraseEvent &)
{
	//do nothing
}

void wxTabContainer::OnSizeEvent(wxSizeEvent &e)
{
	Refresh();
	e.Skip();
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
			CustomTab *curtab = (CustomTab*)win;
			curtab->SetOrientation(m_orientation);
		}
	}
	GetSizer()->Layout();
}

void wxTabContainer::SetDraggedTab(CustomTab *tab)
{
	m_draggedTab = tab;
}

void wxTabContainer::SwapTabs(CustomTab *tab)
{
	if (m_draggedTab == tab) {
		return;
	}
	if (m_draggedTab == NULL) {
		return;
	}

	int orientation(wxBOTTOM);

	size_t index = TabToIndex(tab);
	if (index == static_cast<size_t>(-1)) {
		return;
	}

	size_t index2 = TabToIndex(m_draggedTab);
	if (index2 == static_cast<size_t>(-1)) {
		return;
	}

	orientation = index2 > index ? wxTOP : wxBOTTOM;

	Freeze();
	//detach the dragged tab from the sizer
	m_tabsSizer->Detach(m_draggedTab);

	int flags(0);
	if (m_orientation == wxLEFT || m_orientation == wxRIGHT) {
		flags = wxLEFT | wxRIGHT;
	} else {
		flags = wxTOP | wxBOTTOM;
	}

	index = TabToIndex(tab);
	if (orientation == wxBOTTOM) {
		//tab is being dragged bottom
		if (index == GetTabsCount()-1) {
			//last tab
			m_tabsSizer->Add(m_draggedTab, 0, flags, 3);
		} else {
			m_tabsSizer->Insert(index+1, m_draggedTab, 0, flags, 3);
		}
	} else {
		//dragg is going up
		m_tabsSizer->Insert(index, m_draggedTab, 0, flags, 3);
	}
	Thaw();
	m_tabsSizer->Layout();

}

void wxTabContainer::OnLeaveWindow(wxMouseEvent &e)
{
	wxUnusedVar(e);
	m_draggedTab = NULL;
}

void wxTabContainer::OnLeftUp(wxMouseEvent &e)
{
	wxUnusedVar(e);
	m_draggedTab = NULL;
}

void wxTabContainer::PushPageHistory(CustomTab *page)
{
	if (page == NULL)
		return;

	int where = m_history.Index(page);
	//remove old entry of this page and re-insert it as first
	if (where != wxNOT_FOUND) {
		m_history.Remove(page);
	}
	m_history.Insert(page, 0);
}

void wxTabContainer::PopPageHistory(CustomTab *page)
{
	int where = m_history.Index(page);
	while (where != wxNOT_FOUND) {
		CustomTab *tab = static_cast<CustomTab *>(m_history.Item(where));
		m_history.Remove(tab);

		//remove all appearances of this page
		where = m_history.Index(page);
	}
}

CustomTab* wxTabContainer::GetPreviousSelection()
{
	if (m_history.empty()) {
		return NULL;
	}
	//return the top of the heap
	return static_cast<CustomTab*>( m_history.Item(0));
}

void wxTabContainer::DeletePage(CustomTab *deleteTab, bool notify)
{
	DoRemoveTab(deleteTab, true, notify);
}

void wxTabContainer::RemovePage(CustomTab *removePage, bool notify)
{
	DoRemoveTab(removePage, false, notify);
}

void wxTabContainer::DoRemoveTab(CustomTab *deleteTab, bool deleteIt, bool notify)
{
	if (deleteTab == NULL) {
		return;
	}

	size_t pageIndex = TabToIndex( deleteTab );
	if (notify) {
		//send event to noitfy that the page has changed
		NotebookEvent event(wxEVT_COMMAND_BOOK_PAGE_CLOSING, GetId());
		event.SetSelection( pageIndex );
		event.SetEventObject( this );
		GetEventHandler()->ProcessEvent(event);

		if (!event.IsAllowed()) {
			return;
		}
	}

	//detach the tab from the tabs container
	if (m_tabsSizer->Detach(deleteTab)) {
		//the  tab was removed successfully
		//locate next item to be selected
		PopPageHistory(deleteTab);

		CustomTab *nextSelection = GetPreviousSelection();
		if ( !nextSelection && this->GetTabsCount() > 0) {
			nextSelection = IndexToTab(0);
		}

		Notebook *book = (Notebook*)GetParent();
		if (book && nextSelection) {
			SetSelection(nextSelection, true);
		}

		//remove the window from the parents' sizer
		GetParent()->GetSizer()->Detach(deleteTab->GetWindow());

		//destroy the window as well?
		if (deleteIt) {
			deleteTab->GetWindow()->Destroy();
		}

		//you can now safely destroy the visual tab button
		deleteTab->Destroy();
	}
	m_tabsSizer->Layout();
	GetParent()->GetSizer()->Layout();

	if (notify) {
		//send event to noitfy that the page has been closed
		NotebookEvent event(wxEVT_COMMAND_BOOK_PAGE_CLOSED, GetId());
		event.SetSelection( pageIndex );
		event.SetEventObject( this );
		GetEventHandler()->ProcessEvent(event);
	}
}

void wxTabContainer::EnsureVisible(CustomTab *tab)
{
	if (!IsVisible(tab)) {
		Freeze();
		//make sure all tabs are visible
		for ( size_t i=0; i< GetTabsCount(); i++ ) {
			if (m_tabsSizer->IsShown(i) == false) {
				m_tabsSizer->Show(i);
			}
		}
		m_tabsSizer->Layout();
		//tab is not visible
		//try to hide the first visible tab, and try again
		size_t index = TabToIndex(tab);
		for ( size_t i=0; i < index; i++  ) {
			CustomTab *t = IndexToTab(i);
			if (t) {
				m_tabsSizer->Hide(i);
				m_tabsSizer->Layout();

				if (IsVisible(tab)) {
					break;
				}
			}
		}
		Thaw();
	}
}

bool wxTabContainer::IsVisible(CustomTab *tab)
{
	wxPoint pos = tab->GetPosition();
	wxSize tabSize = tab->GetSize();
	wxRect rr = GetSize();
	
	bool cond0(true);
	if(rr.width > tabSize.x) {
		//the visible area has enough space to show the entire 
		//tab, force it
		cond0 = !(pos.x + tabSize.x > rr.x + rr.width);
	}
	bool cond1 = !(pos.x > rr.x + rr.width);
	bool cond2 = m_tabsSizer->IsShown(tab);
	return cond0 && cond1 && cond2;
}

void wxTabContainer::OnDeleteTab(wxCommandEvent &e)
{
	CustomTab *tab = (CustomTab*)e.GetEventObject();
	if (tab) {
		DeletePage(tab, true);
	}
}

void wxTabContainer::Resize()
{
	m_tabsSizer->Layout();
	GetSizer()->Layout();
	//refersh all tabs
	wxSizer *sz = m_tabsSizer;
	wxSizerItemList items = sz->GetChildren();

	wxSizerItemList::iterator iter = items.begin();
	for (; iter != items.end(); iter++) {
		wxSizerItem *item = *iter;
		wxWindow *win = item->GetWindow();
		if (win) {
			CustomTab *curtab = (CustomTab*)win;

			//refresh only visible tabs
			if (IsVisible(curtab)) {
				curtab->Refresh();
			}
		}
	}

}

void wxTabContainer::ShowPopupMenu()
{
	PopupMenu( m_rightClickMenu );
}
