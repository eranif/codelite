#include <wx/app.h>
#include <wx/xrc/xmlres.h>
#include <wx/choicebk.h>
#include <wx/notebook.h>
#include "notebook_ex_nav_dlg.h"
#include "notebook_ex.h"
#include <wx/button.h>
#include "wx/sizer.h"
#include <wx/log.h>
#include <wx/wupdlock.h>

#ifdef __WXMSW__
#    include <wx/button.h>
#    include <wx/imaglist.h>
#    include <wx/image.h>
#elif defined(__WXGTK__)
#    include <gtk/gtk.h>
#    include <wx/imaglist.h>
#endif

const wxEventType wxEVT_COMMAND_BOOK_PAGE_CHANGED         = XRCID("notebook_page_changing");
const wxEventType wxEVT_COMMAND_BOOK_PAGE_CHANGING        = XRCID("notebook_page_changed");
const wxEventType wxEVT_COMMAND_BOOK_PAGE_CLOSING         = XRCID("notebook_page_closing");
const wxEventType wxEVT_COMMAND_BOOK_PAGE_CLOSED          = XRCID("notebook_page_closed");
const wxEventType wxEVT_COMMAND_BOOK_PAGE_MIDDLE_CLICKED  = XRCID("notebook_page_middle_clicked");
const wxEventType wxEVT_COMMAND_BOOK_PAGE_X_CLICKED       = XRCID("notebook_page_x_btn_clicked");
const wxEventType wxEVT_COMMAND_BOOK_SWAP_PAGES           = XRCID("notebook_swap_pages");

/**
 * @brief helper method
 */
static bool IsWindowsOrGTK()
{
#if defined(__WXMSW__) || defined(__WXGTK__)
	return true;
#else
	return false;
#endif
}
#define DELETE_PAGE_INTERNAL 1234534

#define X_IMG_NONE    -1
#define X_IMG_NORMAL   0
#define X_IMG_PRESSED  1
#define X_IMG_DISABLED 2

Notebook::Notebook(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style)
		: wxNotebook(parent, id, pos, size, style & wxNB_LEFT ? wxNB_LEFT : wxNB_DEFAULT)
		, m_popupWin(NULL)
		, m_style(style)
		, m_leftDownTabIdx(npos)
		, m_notify (true)
{
	Initialize();

	m_leftDownPos = wxPoint();

	// Connect events
	Connect(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED,  wxNotebookEventHandler(Notebook::OnIternalPageChanged),  NULL, this);
	Connect(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING, wxNotebookEventHandler(Notebook::OnIternalPageChanging), NULL, this);
	Connect(wxEVT_SET_FOCUS,                      wxFocusEventHandler(Notebook::OnFocus),                  NULL, this);
	Connect(wxEVT_NAVIGATION_KEY,                 wxNavigationKeyEventHandler(Notebook::OnNavigationKey),  NULL, this);
	Connect(wxEVT_LEFT_DOWN,                      wxMouseEventHandler(Notebook::OnLeftDown),               NULL, this);
	Connect(wxEVT_LEFT_UP,                        wxMouseEventHandler(Notebook::OnLeftUp  ),               NULL, this);
	Connect(wxEVT_MIDDLE_DOWN,                    wxMouseEventHandler(Notebook::OnMouseMiddle),            NULL, this);
	Connect(wxEVT_MOTION,                         wxMouseEventHandler(Notebook::OnMouseMove),              NULL, this);
	Connect(wxEVT_LEAVE_WINDOW,                   wxMouseEventHandler(Notebook::OnLeaveWindow),            NULL, this);
	Connect(wxEVT_CONTEXT_MENU,                   wxContextMenuEventHandler(Notebook::OnMenu),             NULL, this);

	Connect(DELETE_PAGE_INTERNAL, wxEVT_COMMAND_MENU_SELECTED,          wxCommandEventHandler(Notebook::OnInternalDeletePage),   NULL, this);

}

Notebook::~Notebook()
{
	Disconnect(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED,  wxNotebookEventHandler(Notebook::OnIternalPageChanged),  NULL, this);
	Disconnect(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING, wxNotebookEventHandler(Notebook::OnIternalPageChanging), NULL, this);
	Disconnect(wxEVT_NAVIGATION_KEY,                 wxNavigationKeyEventHandler(Notebook::OnNavigationKey),  NULL, this);
	Disconnect(wxEVT_LEFT_DOWN,                      wxMouseEventHandler(Notebook::OnLeftDown),               NULL, this);
	Disconnect(wxEVT_LEFT_UP,                        wxMouseEventHandler(Notebook::OnLeftUp  ),               NULL, this);
	Disconnect(wxEVT_MIDDLE_DOWN,                    wxMouseEventHandler(Notebook::OnMouseMiddle),            NULL, this);
	Disconnect(wxEVT_MOTION,                         wxMouseEventHandler(Notebook::OnMouseMove),              NULL, this);
	Disconnect(wxEVT_LEAVE_WINDOW,                   wxMouseEventHandler(Notebook::OnLeaveWindow),            NULL, this);
	Disconnect(wxEVT_CONTEXT_MENU,                   wxContextMenuEventHandler(Notebook::OnMenu),             NULL, this);
	Disconnect(wxEVT_SET_FOCUS,                      wxFocusEventHandler(Notebook::OnFocus),                  NULL, this);
	Disconnect(DELETE_PAGE_INTERNAL, wxEVT_COMMAND_MENU_SELECTED,          wxCommandEventHandler(Notebook::OnInternalDeletePage),   NULL, this);
}

bool Notebook::AddPage(wxWindow *win, const wxString &text, bool selected, int imgid)
{
	win->Reparent(this);

	int imageToInsert = (IsWindowsOrGTK() && HasCloseButton()) ? X_IMG_NORMAL : imgid;

#ifdef __WXGTK__
	// Eran: I dont know what is wrong here, but it seems like the first page
	// is adding 2 bitmaps...
	if(GetPageCount() == 0 && HasCloseButton())
		imageToInsert = -1;
#endif

	if (wxNotebook::InsertPage(GetPageCount(), win, text, selected, imageToInsert)) {
#ifdef __WXGTK__
		win->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(Notebook::OnKeyDown),  NULL, this);
#endif
		PushPageHistory(win);
		return true;
	}
	return false;
}

bool Notebook::InsertPage(size_t index, wxWindow* win, const wxString& text, bool selected, int imgid)
{
	win->Reparent(this);

	int imageToInsert = (IsWindowsOrGTK() && HasCloseButton()) ? X_IMG_NORMAL : imgid;

	if (wxNotebook::InsertPage(index, win, text, selected, imageToInsert)) {
#ifdef __WXGTK__
		win->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(Notebook::OnKeyDown),  NULL, this);
#endif

		PushPageHistory(win);
		return true;
	}
	return false;
}


void Notebook::Initialize()
{
#if defined(__WXMSW__)
	wxImageList *imgList = new wxImageList(18, 18, true);

	imgList->Add( wxXmlResource::Get()->LoadBitmap(wxT("tab_x_close")));
	imgList->Add( wxXmlResource::Get()->LoadBitmap(wxT("tab_x_close_pressed")));
	imgList->Add( wxXmlResource::Get()->LoadBitmap(wxT("tab_x_close_disabled")));

	AssignImageList(imgList);

#elif defined(__WXGTK__)
	wxImageList *imgList = new wxImageList(16, 16, true);

	imgList->Add( wxXmlResource::Get()->LoadBitmap(wxT("tab_x_close_red")));
	imgList->Add( wxXmlResource::Get()->LoadBitmap(wxT("tab_x_close_pressed_red")));
	imgList->Add( wxXmlResource::Get()->LoadBitmap(wxT("tab_x_close_disabled_red")));

	AssignImageList(imgList);
#endif
}

void Notebook::SetSelection(size_t page, bool notify)
{
	if (page >= GetPageCount())
		return;

	m_notify = notify;
	wxNotebook::SetSelection(page);
	m_notify = true;

	PushPageHistory(GetPage(page));
}

size_t Notebook::GetSelection()
{
	return static_cast<size_t>(wxNotebook::GetSelection());
}

wxWindow* Notebook::GetPage(size_t page)
{
	if (page >= GetPageCount())
		return NULL;

	return wxNotebook::GetPage(page);
}

bool Notebook::RemovePage(size_t page, bool notify)
{
	if (notify) {
		//send event to noitfy that the page has changed
		NotebookEvent event(wxEVT_COMMAND_BOOK_PAGE_CLOSING, GetId());
		event.SetSelection( page );
		event.SetEventObject( this );
		GetEventHandler()->ProcessEvent(event);

		if (!event.IsAllowed()) {
			return false;
		}
	}

	wxWindow* win = GetPage(page);
#ifdef __WXGTK__
	win->Disconnect(wxEVT_KEY_DOWN, wxKeyEventHandler(Notebook::OnKeyDown),  NULL, this);
#endif

	bool rc = wxNotebook::RemovePage(page);
	if (rc) {
		PopPageHistory(win);
	}

#ifdef __WXGTK__
	// Unparent
	if(win->GetParent())
		win->GetParent()->RemoveChild(win);
#endif

	if (rc && notify) {
		//send event to noitfy that the page has been closed
		NotebookEvent event(wxEVT_COMMAND_BOOK_PAGE_CLOSED, GetId());
		event.SetSelection( page );
		event.SetEventObject( this );
		GetEventHandler()->AddPendingEvent(event);
	}

	return rc;
}

bool Notebook::DeletePage(size_t page, bool notify)
{
	if (page >= GetPageCount())
		return false;

	if (notify) {
		//send event to noitfy that the page has changed
		NotebookEvent event(wxEVT_COMMAND_BOOK_PAGE_CLOSING, GetId());
		event.SetSelection( page );
		event.SetEventObject( this );
		GetEventHandler()->ProcessEvent(event);

		if (!event.IsAllowed()) {
			return false;
		}
	}

	wxWindow* win = GetPage(page);
#ifdef __WXGTK__
	win->Disconnect(wxEVT_KEY_DOWN, wxKeyEventHandler(Notebook::OnKeyDown),  NULL, this);
#endif


	bool rc = wxNotebook::DeletePage(page);
	if (rc) {
		PopPageHistory(win);
	}

	if (rc && notify) {
		//send event to noitfy that the page has been closed
		NotebookEvent event(wxEVT_COMMAND_BOOK_PAGE_CLOSED, GetId());
		event.SetSelection( page );
		event.SetEventObject( this );
		GetEventHandler()->ProcessEvent(event);
	}

	return rc;

}

wxString Notebook::GetPageText(size_t page) const
{
	if (page >= GetPageCount())
		return wxT("");

	return wxNotebook::GetPageText(page);
}

void Notebook::OnNavigationKey(wxNavigationKeyEvent &e)
{
	if ( e.IsWindowChange() ) {
		if (DoNavigate())
			return;
	}
	e.Skip();
}

const wxArrayPtrVoid& Notebook::GetHistory() const
{
	return m_history;
}

void Notebook::SetRightClickMenu(wxMenu* menu)
{
	m_contextMenu = menu;
}

wxWindow* Notebook::GetCurrentPage()
{
	size_t selection = GetSelection();
	if (selection != Notebook::npos) {
		return GetPage(selection);
	}
	return NULL;
}

size_t Notebook::GetPageIndex(wxWindow *page)
{
	if ( !page )
		return Notebook::npos;

	for (size_t i=0; i< GetPageCount(); i++) {
		if (GetPage(i) == page) {
			return i;
		}
	}
	return Notebook::npos;
}

size_t Notebook::GetPageIndex(const wxString& text)
{
	for (size_t i=0; i< GetPageCount(); i++) {

		if (GetPageText(i) == text) {
			return i;
		}
	}
	return Notebook::npos;
}

bool Notebook::SetPageText(size_t index, const wxString &text)
{
	if (index >= GetPageCount())
		return false;
	return wxNotebook::SetPageText(index, text);
}

bool Notebook::DeleteAllPages(bool notify)
{
	bool res = true;
	size_t count = GetPageCount();
	for (size_t i=0; i<count && res; i++) {
		res = DeletePage(0, notify);
	}
	return res;
}

void Notebook::PushPageHistory(wxWindow *page)
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

void Notebook::PopPageHistory(wxWindow *page)
{
	int where = m_history.Index(page);
	while (where != wxNOT_FOUND) {
		wxWindow *tab = static_cast<wxWindow *>(m_history.Item(where));
		m_history.Remove(tab);

		//remove all appearances of this page
		where = m_history.Index(page);
	}
}

wxWindow* Notebook::GetPreviousSelection()
{
	if (m_history.empty()) {
		return NULL;
	}
	//return the top of the heap
	return static_cast<wxWindow*>( m_history.Item(0));
}

void Notebook::OnIternalPageChanged(wxNotebookEvent &e)
{
	DoPageChangedEvent(e);
}

void Notebook::OnIternalPageChanging(wxNotebookEvent &e)
{
	DoPageChangingEvent(e);
}

void Notebook::OnLeftDown(wxMouseEvent &e)
{
	long flags(0);
	size_t curSel = GetSelection();
	int where = HitTest( e.GetPosition(), &flags );
	if (where != wxNOT_FOUND) {

		// Keep the left-down position
		m_leftDownPos    = e.GetPosition();
		m_leftDownTabIdx = where;

#if defined(__WXMSW__)||defined(__WXGTK__)
		if (HasCloseButton() && (flags & wxBK_HITTEST_ONICON) && where == (int)curSel) {
			SetPageImage(where, X_IMG_PRESSED);
		}
#endif

	}
	e.Skip();
}

void Notebook::OnLeftUp(wxMouseEvent &e)
{
	long flags(0);
	int where = HitTest( e.GetPosition(), &flags );

	// Is Drag-N-Drop allowed?
	if( (m_style & wxVB_NODND) == 0 ) {
		if(m_leftDownPos != wxPoint()) {

			if(m_leftDownTabIdx != Notebook::npos && where != wxNOT_FOUND && m_leftDownTabIdx != (size_t)where) {

				e.Skip();
				m_leftDownPos    = wxPoint();

				// Notify parent to swap pages
				NotebookEvent event(wxEVT_COMMAND_BOOK_SWAP_PAGES, GetId());
				event.SetSelection   ( (int)where );   // The new location
				event.SetOldSelection( (int)m_leftDownTabIdx ); // Old location
				event.SetEventObject ( this );
				GetEventHandler()->AddPendingEvent(event);

				m_leftDownTabIdx = npos;
				return;
			}
		}
	}

#if defined(__WXMSW__)||defined(__WXGTK__)

	bool onImage = flags & wxNB_HITTEST_ONICON;
	bool pressed = m_leftDownTabIdx != npos && (GetPageImage((size_t)m_leftDownTabIdx) == 1);
	bool sameTab = (size_t)where == m_leftDownTabIdx;

	if (sameTab && pressed && onImage) {

		//send event to noitfy that the page is changing
		NotebookEvent event(wxEVT_COMMAND_BOOK_PAGE_X_CLICKED, GetId());
		event.SetSelection   ( (int)where );
		event.SetOldSelection( wxNOT_FOUND );
		event.SetEventObject ( this );
		GetEventHandler()->AddPendingEvent(event);
		m_leftDownTabIdx = npos;
		return;

	} else if (!sameTab && pressed) {
		SetPageImage( m_leftDownTabIdx, X_IMG_NORMAL );

	} else if (sameTab && !onImage && pressed) {
		SetPageImage( m_leftDownTabIdx, X_IMG_NORMAL );

	}
#endif
	m_leftDownTabIdx = npos;
	e.Skip();
}

void Notebook::OnLeaveWindow(wxMouseEvent &e)
{
#if defined(__WXMSW__)||defined(__WXGTK__)
	if (m_leftDownTabIdx != npos && GetPageImage((size_t)m_leftDownTabIdx) == 1 /* pressed */) {
		SetPageImage( m_leftDownTabIdx, X_IMG_NORMAL );
	}
#endif

	m_leftDownTabIdx = npos;
	m_leftDownPos = wxPoint();
	e.Skip();
}

void Notebook::OnMouseMiddle(wxMouseEvent &e)
{
	long flags(0);
	int where = HitTest( e.GetPosition(), &flags );

	if (where != wxNOT_FOUND && HasCloseMiddle()) {
		//send event to noitfy that the page is changing
		NotebookEvent event(wxEVT_COMMAND_BOOK_PAGE_MIDDLE_CLICKED, GetId());
		event.SetSelection   ( (int)where );
		event.SetOldSelection( wxNOT_FOUND );
		event.SetEventObject ( this );
		GetEventHandler()->AddPendingEvent(event);
		return;
	}
	e.Skip();
}

void Notebook::DoPageChangedEvent(wxBookCtrlBaseEvent& e)
{
	// Update icon
	if(IsWindowsOrGTK() && HasCloseButton()) {
		int selection = e.GetSelection();
		for(size_t i=0; i<GetPageCount(); i++) {
			SetPageImage(i, i == (size_t)selection ? X_IMG_NORMAL : X_IMG_DISABLED);
		}
	}

	if (!m_notify) {
		e.Skip();
		return;
	}

	//send event to noitfy that the page is changing
	NotebookEvent event(wxEVT_COMMAND_BOOK_PAGE_CHANGED, GetId());
	event.SetSelection   ( e.GetSelection()    );
	event.SetOldSelection( e.GetOldSelection() );
	event.SetEventObject ( this );
	GetEventHandler()->AddPendingEvent(event);

	PushPageHistory( GetPage(e.GetSelection()) );
	e.Skip();
}

void Notebook::DoPageChangingEvent(wxBookCtrlBaseEvent& e)
{
	if (!m_notify) {
		e.Skip();
		return;
	}


	//send event to noitfy that the page is changing
	NotebookEvent event(wxEVT_COMMAND_BOOK_PAGE_CHANGING, GetId());
	event.SetSelection   ( e.GetSelection()    );
	event.SetOldSelection( e.GetOldSelection() );
	event.SetEventObject ( this );
	GetEventHandler()->ProcessEvent(event);

	if ( !event.IsAllowed() ) {
		e.Veto();
	}
	e.Skip();
}


void Notebook::OnHasPages(wxUpdateUIEvent& e)
{
	e.Enable( GetPageCount() );
}

void Notebook::OnKeyDown(wxKeyEvent& e)
{
	if (e.GetKeyCode() == WXK_TAB && e.m_controlDown ) {
		if (DoNavigate())
			return;

	} else {
		e.Skip();
	}
}

bool Notebook::DoNavigate()
{
	if ( !m_popupWin && GetPageCount() > 0) {

		m_popupWin = new NotebookNavDialog( this );
		m_popupWin->ShowModal();

		wxWindow *page = m_popupWin->GetSelection();
		m_popupWin->Destroy();
		m_popupWin = NULL;

		SetSelection( GetPageIndex(page), true );


		return true;
	}
	return false;
}

void Notebook::OnMenu(wxContextMenuEvent& e)
{
	int where = HitTest( ScreenToClient(::wxGetMousePosition()) );
	if(where != wxNOT_FOUND) {
		SetSelection(where, false);
		// dont notify the user about changes
		PopupMenu(m_contextMenu);
	}
	e.Skip();
}

void Notebook::OnFocus(wxFocusEvent& e)
{
	if( m_style & wxVB_PASS_FOCUS) {
		wxWindow *w = GetCurrentPage();
		if(w) {
			w->SetFocus();
		}
	}
	e.Skip();
}

void Notebook::OnInternalDeletePage(wxCommandEvent& e)
{
	// Delete the current selection
	size_t pos = (size_t)e.GetInt();
	if(pos != Notebook::npos)
		DeletePage(pos, true);
}

void Notebook::OnMouseMove(wxMouseEvent& e)
{
	e.Skip();
	if(m_leftDownPos != wxPoint() && e.LeftIsDown()) {

	} else if(m_leftDownPos != wxPoint() && !e.LeftIsDown()) {
		m_leftDownPos = wxPoint();
	}
}
