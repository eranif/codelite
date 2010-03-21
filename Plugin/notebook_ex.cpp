#include <wx/app.h>
#include "drawingutils.h"
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

#define DELETE_PAGE_INTERNAL 1234534

#define X_IMG_NONE    -1
#define X_IMG_NORMAL   0
#define X_IMG_PRESSED  1
#define X_IMG_DISABLED 2

class clNotebookArt : public wxAuiDefaultTabArt
{
public:
	clNotebookArt() : wxAuiDefaultTabArt()
	{
		m_base_colour       = DrawingUtils::GetPanelBgColour();
		m_base_colour_pen   = wxPen(m_base_colour);
		m_base_colour_brush = wxBrush(m_base_colour);
		m_border_pen        = wxPen(m_base_colour);

		m_active_close_bmp = wxXmlResource::Get()->LoadBitmap(wxT("tab_x_close"));

	//imgList->Add( wxXmlResource::Get()->LoadBitmap(wxT("tab_x_close_pressed")));
	}

	virtual ~clNotebookArt()
	{
	}

    virtual void DrawBackground( wxDC& dc, wxWindow* wnd, const wxRect& rect)
	{
		wxColour col = DrawingUtils::GetPanelBgColour();
		dc.SetPen(col);
		dc.SetBrush(col);
		dc.DrawRectangle(rect);
	}
};

Notebook::Notebook(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style)
		: wxAuiNotebook(parent, id, pos, size, style | wxNO_BORDER | wxAUI_NB_TAB_MOVE | wxAUI_NB_TAB_SPLIT | wxAUI_NB_WINDOWLIST_BUTTON)
		, m_popupWin(NULL)
		, m_style(style)
		, m_leftDownTabIdx(npos)
		, m_notify (true)
{
	Initialize();

	m_leftDownPos = wxPoint();

	// Connect events
	Connect(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED,    wxAuiNotebookEventHandler(Notebook::OnInternalPageChanged),  NULL, this);
	Connect(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGING,   wxAuiNotebookEventHandler(Notebook::OnInternalPageChanging), NULL, this);
	Connect(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSED,     wxAuiNotebookEventHandler(Notebook::OnInternalPageClosed),   NULL, this);
	Connect(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSE,      wxAuiNotebookEventHandler(Notebook::OnInternalPageClosing),  NULL, this);
	Connect(wxEVT_COMMAND_AUINOTEBOOK_TAB_MIDDLE_DOWN, wxAuiNotebookEventHandler(Notebook::OnTabMiddle),            NULL, this);
	Connect(wxEVT_COMMAND_AUINOTEBOOK_TAB_RIGHT_UP,    wxAuiNotebookEventHandler(Notebook::OnMenu),                 NULL, this);
	Connect(wxEVT_COMMAND_AUINOTEBOOK_TAB_RIGHT_DOWN,  wxAuiNotebookEventHandler(Notebook::OnTabRightDown),         NULL, this);

	Connect(wxEVT_SET_FOCUS,                      wxFocusEventHandler(Notebook::OnFocus),                  NULL, this);
	Connect(wxEVT_NAVIGATION_KEY,                 wxNavigationKeyEventHandler(Notebook::OnNavigationKey),  NULL, this);

	Connect(DELETE_PAGE_INTERNAL, wxEVT_COMMAND_MENU_SELECTED,          wxCommandEventHandler(Notebook::OnInternalDeletePage),   NULL, this);

	SetArtProvider(new clNotebookArt());
}

Notebook::~Notebook()
{
	Disconnect(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED,    wxAuiNotebookEventHandler(Notebook::OnInternalPageChanged),  NULL, this);
	Disconnect(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGING,   wxAuiNotebookEventHandler(Notebook::OnInternalPageChanging), NULL, this);
	Disconnect(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSED,     wxAuiNotebookEventHandler(Notebook::OnInternalPageClosed),   NULL, this);
	Disconnect(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSE,      wxAuiNotebookEventHandler(Notebook::OnInternalPageClosing),  NULL, this);
	Disconnect(wxEVT_COMMAND_AUINOTEBOOK_TAB_MIDDLE_DOWN, wxAuiNotebookEventHandler(Notebook::OnTabMiddle),            NULL, this);
	Disconnect(wxEVT_COMMAND_AUINOTEBOOK_TAB_RIGHT_UP,    wxAuiNotebookEventHandler(Notebook::OnMenu),                 NULL, this);
	Disconnect(wxEVT_COMMAND_AUINOTEBOOK_TAB_RIGHT_DOWN,  wxAuiNotebookEventHandler(Notebook::OnTabRightDown),         NULL, this);

	Disconnect(wxEVT_NAVIGATION_KEY,                 wxNavigationKeyEventHandler(Notebook::OnNavigationKey),  NULL, this);
	Disconnect(wxEVT_SET_FOCUS,                      wxFocusEventHandler(Notebook::OnFocus),                  NULL, this);
	Disconnect(DELETE_PAGE_INTERNAL, wxEVT_COMMAND_MENU_SELECTED,          wxCommandEventHandler(Notebook::OnInternalDeletePage),   NULL, this);
}

bool Notebook::AddPage(wxWindow *win, const wxString &text, bool selected, const wxBitmap& bmp)
{
	win->Reparent(this);
	if(wxAuiNotebook::InsertPage(GetPageCount(), win, text, selected, bmp)){
#ifdef __WXGTK__
		win->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(Notebook::OnKeyDown),  NULL, this);
#endif
		PushPageHistory(win);
		return true;
	}
	return false;
}

bool Notebook::InsertPage(size_t index, wxWindow* win, const wxString& text, bool selected, const wxBitmap &bmp)
{
	win->Reparent(this);
	if(wxAuiNotebook::InsertPage(index, win, text, selected, bmp)){
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
#if 0
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
#endif
}

void Notebook::SetSelection(size_t page, bool notify)
{
	if (page >= GetPageCount())
		return;

	m_notify = notify;
	wxAuiNotebook::SetSelection(page);
	m_notify = true;

	PushPageHistory(GetPage(page));
}

size_t Notebook::GetSelection()
{
	return static_cast<size_t>(wxAuiNotebook::GetSelection());
}

wxWindow* Notebook::GetPage(size_t page)
{
	if (page >= GetPageCount())
		return NULL;

	return wxAuiNotebook::GetPage(page);
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

	bool rc = wxAuiNotebook::RemovePage(page);
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


	bool rc = wxAuiNotebook::DeletePage(page);
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

	return wxAuiNotebook::GetPageText(page);
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
	return wxAuiNotebook::SetPageText(index, text);
}

bool Notebook::DeleteAllPages(bool notify)
{
	bool res = true;
	size_t count = GetPageCount();
	for (size_t i=0; i<count && res; i++) {
		res = this->DeletePage(0, notify);
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

void Notebook::OnInternalPageChanged(wxAuiNotebookEvent &e)
{
	DoPageChangedEvent(e);
}

void Notebook::OnInternalPageChanging(wxAuiNotebookEvent &e)
{
	DoPageChangingEvent(e);
}

void Notebook::DoPageChangedEvent(wxAuiNotebookEvent& e)
{
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

void Notebook::DoPageChangingEvent(wxAuiNotebookEvent& e)
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

void Notebook::OnMenu(wxAuiNotebookEvent& e)
{
	int where = e.GetSelection();
	if(where != wxNOT_FOUND) {
		SetSelection(where, false);
		// dont notify the user about changes
		PopupMenu(m_contextMenu);
	} else
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

void Notebook::OnInternalPageClosing(wxAuiNotebookEvent& e)
{
	e.Skip();

	NotebookEvent event(wxEVT_COMMAND_BOOK_PAGE_CLOSING, GetId());
	event.SetSelection( (int)GetSelection() );
	event.SetEventObject( this );
	GetEventHandler()->ProcessEvent(event);

	if (!event.IsAllowed()) {
		e.Veto();
	}
}

void Notebook::OnInternalPageClosed(wxAuiNotebookEvent& e)
{
	e.Skip();

	NotebookEvent event(wxEVT_COMMAND_BOOK_PAGE_CLOSED, GetId());
	event.SetSelection( (int)GetSelection() );
	event.SetEventObject( this );
	GetEventHandler()->AddPendingEvent(event);
}

void Notebook::OnTabMiddle(wxAuiNotebookEvent& e)
{
	if (e.GetSelection() != wxNOT_FOUND && HasCloseMiddle()) {
		//send event to noitfy that the page is changing
		NotebookEvent event(wxEVT_COMMAND_BOOK_PAGE_MIDDLE_CLICKED, GetId());
		event.SetSelection   ( e.GetSelection() );
		event.SetOldSelection( wxNOT_FOUND );
		event.SetEventObject ( this );
		GetEventHandler()->AddPendingEvent(event);
	}
}

void Notebook::OnTabRightDown(wxAuiNotebookEvent& e)
{
	int where = e.GetSelection();
	if(where != wxNOT_FOUND) {
		SetSelection(where, false);

	} else
		e.Skip();
}
