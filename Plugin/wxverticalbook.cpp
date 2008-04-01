#include "wxverticalbook.h"
#include "wxverticaltab.h"
#include "wxtabcontainer.h"
#include "wx/sizer.h"

const wxEventType wxEVT_COMMAND_VERTICALBOOK_PAGE_CHANGED = wxNewEventType();
const wxEventType wxEVT_COMMAND_VERTICALBOOK_PAGE_CHANGING = wxNewEventType();
const wxEventType wxEVT_COMMAND_VERTICALBOOK_PAGE_CLOSING = wxNewEventType();
const wxEventType wxEVT_COMMAND_FLATNOTEBOOK_PAGE_CLOSED = wxNewEventType();

wxVerticalBook::wxVerticalBook(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style)
		: wxPanel(parent, id, pos, size)
		, m_style(style)
		, m_aui(NULL)
{
	Initialize();
}

wxVerticalBook::~wxVerticalBook()
{

}

void wxVerticalBook::AddPage(wxWindow *win, const wxString &text, const wxBitmap &bmp, bool selected)
{
	Freeze();

	//create new tab and place it in the sizer
	win->Reparent(this);

	wxVerticalTab *tab = new wxVerticalTab(m_tabs, wxID_ANY, text, bmp, selected, m_tabs->GetOrientation());
	tab->SetWindow(win);

	wxWindow *oldWindow(NULL);
	wxVerticalTab *oldSelection = m_tabs->GetSelection();
	if (oldSelection) {
		oldWindow = oldSelection->GetWindow();
	}

	//add the tab
	m_tabs->AddTab(tab);

	//add the actual window to the book sizer
	wxSizer *sz = GetSizer();
	win->Hide();
	if (tab->GetSelected()) {
		if (oldWindow && sz->GetItem(oldWindow)) {
			//the item indeed exist in the sizer, remove it
			sz->Detach(oldWindow);
			oldWindow->Hide();
		}
		//inert the new item
		if (m_style & wxVB_LEFT) {
			sz->Insert(1, win, 1, wxEXPAND);
		} else {
			sz->Insert(0, win, 1, wxEXPAND);
		}
		win->Show();
	}

	sz->Layout();
	Thaw();
}

void wxVerticalBook::Initialize()
{
	wxBoxSizer *sz = new wxBoxSizer(wxHORIZONTAL);
	SetSizer(sz);

	int ori(wxRIGHT);
	if (m_style & wxVB_LEFT) {
		ori = wxLEFT;
	}

	m_tabs = new wxTabContainer(this, wxID_ANY, ori);
	sz->Add(m_tabs, 0, wxEXPAND);
	sz->Layout();
}

void wxVerticalBook::SetSelection(size_t page)
{
	wxVerticalTab *tab = m_tabs->IndexToTab(page);
	wxVerticalTab *old_tab = m_tabs->GetSelection();
	
	if (old_tab == tab) {
		//same tab, nothing to be done
		return;
	}
	
	if (tab) {
		//the next call will also trigger a call to wxVerticalBook::SetSelection(wxVerticalTab *tab)
		m_tabs->SetSelection(tab);
	}
}

//this function is called from the wxTabContainer class
void wxVerticalBook::SetSelection(wxVerticalTab *tab)
{
	//the tab control is already updated, all left to be done is to set the window
	wxSizer *sz = GetSizer();
	wxWindow *oldWindow(NULL);
	wxVerticalTab *oldSelection = m_tabs->GetSelection();
	if (oldSelection) {
		oldWindow = oldSelection->GetWindow();
	}


	wxWindow *win = tab->GetWindow();
	if (oldWindow == win) {
		//nothing to be done
		return;
	}

	Freeze();
	if (oldWindow && sz->GetItem(oldWindow)) {
		//the item indeed exist in the sizer, remove it
		sz->Detach(oldWindow);
		oldWindow->Hide();
	}

	if (m_style & wxVB_LEFT) {
		sz->Insert(1, win, 1, wxEXPAND);
	} else {
		sz->Insert(0, win, 1, wxEXPAND);
	}
	win->Show();
	Thaw();

	sz->Layout();
}

size_t wxVerticalBook::GetSelection()
{
	wxVerticalTab *tab = m_tabs->GetSelection();
	if (tab) {
		return m_tabs->TabToIndex(tab);
	}
	return wxVerticalBook::npos;
}

wxWindow* wxVerticalBook::GetPage(size_t page)
{
	wxVerticalTab *tab = m_tabs->IndexToTab(page);
	if (tab) {
		return tab->GetWindow();
	}
	return NULL;
}

size_t wxVerticalBook::GetPageCount() const
{
	return m_tabs->GetTabsCount();
}

void wxVerticalBook::RemovePage(size_t page)
{
	//TODO :: implement this
}

void wxVerticalBook::DeletePage(size_t page)
{
	//TODO :: implement this
}

wxString wxVerticalBook::GetPageText(size_t page) const
{
	wxVerticalTab *tab = m_tabs->IndexToTab(page);
	if (tab) {
		return wxEmptyString;
	}
	return tab->GetText();
}

void wxVerticalBook::SetOrientationLeft(bool left)
{
	int remove_style(wxVB_RIGHT);
	int add_style(wxVB_LEFT);
	
	if ( !left ) { //change orientation to the right
		remove_style = wxVB_LEFT;
		add_style = wxVB_RIGHT;
	}

	if (m_style & add_style) {
		//nothing to be done
		return;
	}

	wxSizer *sz = GetSizer();
	m_style &= ~remove_style;
	m_style |= add_style;
	
	int ori(wxRIGHT);
	if (m_style & wxVB_LEFT) {
		ori = wxLEFT;
	}
	m_tabs->SetOrientation(ori);
	
	//detach the tabcontainer class from the sizer
	if (GetPageCount() > 0) {
		if (add_style == wxVB_RIGHT) {
			//the tabs container is on the left=pos 0
			sz->Detach(0);
			sz->Add(m_tabs, 0, wxEXPAND);
		} else {
			sz->Detach(1);
			sz->Insert(0, m_tabs, 0, wxEXPAND);
		}
		m_tabs->Refresh();
		Refresh();
		sz->Layout();
	}
}

void wxVerticalBook::SetAuiManager(wxAuiManager *manager, const wxString &containedPaneName)
{
	if(manager) {
		m_aui = manager;
		m_paneName = containedPaneName;
		m_aui->Connect(wxEVT_AUI_RENDER, wxAuiManagerEventHandler(wxVerticalBook::OnRender), NULL, this);
	} else {
		if(m_aui) {
			m_aui->Disconnect(wxEVT_AUI_RENDER, wxAuiManagerEventHandler(wxVerticalBook::OnRender), NULL, this);
		}
		m_aui = NULL;
		m_paneName = wxEmptyString;
	}
}

void wxVerticalBook::OnRender(wxAuiManagerEvent &e)
{
	if(m_aui) {
		wxAuiPaneInfo info = m_aui->GetPane( m_paneName );
		if(info.IsOk()) {
			//we got the containing pane of the book, test its orientation
			if(info.dock_direction == wxAUI_DOCK_LEFT) {
				SetOrientationLeft(true);
			} else if(info.dock_direction == wxAUI_DOCK_RIGHT) {
				SetOrientationLeft(false);
			}
		}
	}
	e.Skip();
}
