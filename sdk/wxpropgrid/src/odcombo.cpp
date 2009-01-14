/////////////////////////////////////////////////////////////////////////////
// Name:        odcombo.cpp
// Purpose:     wxPGOwnerDrawnComboBox and related classes implementation
// Author:      Jaakko Salli
// Modified by:
// Created:     Jan-25-2005
// RCS-ID:      $Id:
// Copyright:   (c) 2005 Jaakko Salli
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_COMBOBOX

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/log.h"

    #include "wx/button.h"
    #include "wx/combobox.h"
    #include "wx/textctrl.h"
    #include "wx/dcclient.h"
    #include "wx/settings.h"
    #include "wx/dialog.h"

#endif

#include "wx/dcbuffer.h"
#include "wx/tooltip.h"
#include "wx/timer.h"

#if defined(__WXMSW__) && !defined(__WXUNIVERSAL__)
    #include "wx/msw/uxtheme.h"
#endif

#include <wx/propgrid/propgrid.h>

#if !wxPG_USING_WXOWNERDRAWNCOMBOBOX

#include "wx/propgrid/odcombo.h"

//
// THESE GO TO BASE FILE
//

#define BMP_BUTTON_MARGIN                       4

//#define DEFAULT_POPUP_HEIGHT                    -1
#define DEFAULT_POPUP_HEIGHT                    300

#define DEFAULT_TEXT_INDENT                     3


#if defined(__WXMSW__)

#define ALLOW_FAKE_POPUP              0 // Use only on plats with problems with wxPopupWindow
#define USE_TRANSIENT_POPUP           1 // Use wxPopupWindowTransient (preferred, if it works properly on platform)

//#undef wxUSE_POPUPWIN
//#define wxUSE_POPUPWIN 0

#elif defined(__WXGTK__)

// Fake popup windows cause focus problems on GTK2 (but enable on GTK1.2, just in case)
#if defined(__WXGTK20__)
    #define ALLOW_FAKE_POPUP          0 // Use only on plats with problems with wxPopupWindow
#else
    #define ALLOW_FAKE_POPUP          1 // Use only on plats with problems with wxPopupWindow
#endif
#define USE_TRANSIENT_POPUP           1 // Use wxPopupWindowTransient (preferred, if it works properly on platform)

#elif defined(__WXMAC__)

#define ALLOW_FAKE_POPUP              1 // Use only on plats with problems with wxPopupWindow
#define USE_TRANSIENT_POPUP           0 // Use wxPopupWindowTransient (preferred, if it works properly on platform)

#else

#define ALLOW_FAKE_POPUP              1 // Use only on plats with problems with wxPopupWindow
#define USE_TRANSIENT_POPUP           0 // Use wxPopupWindowTransient (preferred, if it works properly on platform)

#endif


// Popupwin is really only supported on wxMSW (not WINCE) and wxGTK, regardless
// what the wxUSE_POPUPWIN says.
#if (!defined(__WXMSW__) && !defined(__WXGTK__)) || defined(__WXWINCE__)
#undef wxUSE_POPUPWIN
#define wxUSE_POPUPWIN 0
#endif

#if wxUSE_POPUPWIN
    #include "wx/popupwin.h"
#else
    #undef USE_TRANSIENT_POPUP
    #define USE_TRANSIENT_POPUP 0
#endif

// For versions < 2.6.2, don't enable transient popup. There may be
// problems I don't have time to test properly.
#if !wxCHECK_VERSION(2, 6, 2)
    #undef USE_TRANSIENT_POPUP
    #define USE_TRANSIENT_POPUP 0
#endif


#if USE_TRANSIENT_POPUP
    #undef ALLOW_FAKE_POPUP
    #define ALLOW_FAKE_POPUP 0

    #define wxPGComboPopupWindowBase wxPopupTransientWindow
    #define INSTALL_TOPLEV_HANDLER       0

#elif wxUSE_POPUPWIN

    #define wxPGComboPopupWindowBase wxPopupWindow
    #define INSTALL_TOPLEV_HANDLER       1

#else

    #define wxPGComboPopupWindowBase wxDialog
    #if !ALLOW_FAKE_POPUP
        #define INSTALL_TOPLEV_HANDLER      0 // Doesn't need since can monitor active event
    #else
        #define INSTALL_TOPLEV_HANDLER      1
    #endif

#endif


//
// THESE GO TO GENERIC FILE
//

// Some adjustments to make the generic more bearable
#if defined(__WXUNIVERSAL__)

#define TEXTCTRLXADJUST                 0 // position adjustment for wxTextCtrl, with zero indent
#define TEXTCTRLYADJUST                 0
#define TEXTXADJUST                     0 // how much is read-only text's x adjusted
#define DEFAULT_DROPBUTTON_WIDTH        19

#elif defined(__WXMSW__)

#define TEXTCTRLXADJUST                 2 // position adjustment for wxTextCtrl, with zero indent
#define TEXTCTRLYADJUST                 3
#define TEXTXADJUST                     0 // how much is read-only text's x adjusted
#define DEFAULT_DROPBUTTON_WIDTH        17

#elif defined(__WXGTK__)

#define TEXTCTRLXADJUST                 -1 // position adjustment for wxTextCtrl, with zero indent
#define TEXTCTRLYADJUST                 0
#define TEXTXADJUST                     1 // how much is read-only text's x adjusted
#define DEFAULT_DROPBUTTON_WIDTH        23

#elif defined(__WXMAC__)

#define TEXTCTRLXADJUST                 0 // position adjustment for wxTextCtrl, with zero indent
#define TEXTCTRLYADJUST                 0
#define TEXTXADJUST                     0 // how much is read-only text's x adjusted
#define DEFAULT_DROPBUTTON_WIDTH        19

#else

#define TEXTCTRLXADJUST                 0 // position adjustment for wxTextCtrl, with zero indent
#define TEXTCTRLYADJUST                 0
#define TEXTXADJUST                     0 // how much is read-only text's x adjusted
#define DEFAULT_DROPBUTTON_WIDTH        19

#endif


// constants
// ----------------------------------------------------------------------------

// TO BASE
// the margin between the text control and the combo button
static const wxCoord g_comboMargin = 2;


// ----------------------------------------------------------------------------
// wxPGComboFrameEventHandler takes care of hiding the popup when events happen
// in its top level parent.
// ----------------------------------------------------------------------------

#if INSTALL_TOPLEV_HANDLER

//
// This will no longer be necessary after wxTransientPopupWindow
// works well on all platforms.
//

class wxPGComboFrameEventHandler : public wxEvtHandler
{
public:
    wxPGComboFrameEventHandler( wxPGComboControlBase* pCb );
    ~wxPGComboFrameEventHandler();

    void OnPopup();

    void OnIdle( wxIdleEvent& event );
    void OnMouseEvent( wxMouseEvent& event );
    void OnActivate( wxActivateEvent& event );
    void OnResize( wxSizeEvent& event );
    void OnMove( wxMoveEvent& event );
    void OnMenuEvent( wxMenuEvent& event );
    void OnClose( wxCloseEvent& event );

protected:
    wxWindow*                       m_focusStart;
    wxPGComboControlBase*     m_combo;

private:
    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxPGComboFrameEventHandler, wxEvtHandler)
    EVT_IDLE(wxPGComboFrameEventHandler::OnIdle)
    EVT_LEFT_DOWN(wxPGComboFrameEventHandler::OnMouseEvent)
    EVT_RIGHT_DOWN(wxPGComboFrameEventHandler::OnMouseEvent)
    EVT_SIZE(wxPGComboFrameEventHandler::OnResize)
    EVT_MOVE(wxPGComboFrameEventHandler::OnMove)
    EVT_MENU_HIGHLIGHT(wxID_ANY,wxPGComboFrameEventHandler::OnMenuEvent)
    EVT_MENU_OPEN(wxPGComboFrameEventHandler::OnMenuEvent)
    EVT_ACTIVATE(wxPGComboFrameEventHandler::OnActivate)
    EVT_CLOSE(wxPGComboFrameEventHandler::OnClose)
END_EVENT_TABLE()

wxPGComboFrameEventHandler::wxPGComboFrameEventHandler( wxPGComboControlBase* combo )
    : wxEvtHandler()
{
    m_combo = combo;
}

wxPGComboFrameEventHandler::~wxPGComboFrameEventHandler()
{
}

void wxPGComboFrameEventHandler::OnPopup()
{
    m_focusStart = ::wxWindow::FindFocus();
}

void wxPGComboFrameEventHandler::OnIdle( wxIdleEvent& event )
{
    wxWindow* winFocused = ::wxWindow::FindFocus();

    wxWindow* popup = m_combo->GetPopupControl();
    wxWindow* winpopup = m_combo->GetPopupWindow();

    if (
         winFocused != m_focusStart &&
         winFocused != popup &&
         winFocused->GetParent() != popup &&
         winFocused != winpopup &&
         winFocused->GetParent() != winpopup &&
         winFocused != m_combo &&
         winFocused != m_combo->GetButton() // GTK (atleast) requires this
        )
    {
        m_combo->HidePopup();
    }

    event.Skip();
}

void wxPGComboFrameEventHandler::OnMenuEvent( wxMenuEvent& event )
{
    m_combo->HidePopup();
    event.Skip();
}

void wxPGComboFrameEventHandler::OnMouseEvent( wxMouseEvent& event )
{
    m_combo->HidePopup();
    event.Skip();
}

void wxPGComboFrameEventHandler::OnClose( wxCloseEvent& event )
{
    m_combo->HidePopup();
    event.Skip();
}

void wxPGComboFrameEventHandler::OnActivate( wxActivateEvent& event )
{
    m_combo->HidePopup();
    event.Skip();
}

void wxPGComboFrameEventHandler::OnResize( wxSizeEvent& event )
{
    m_combo->HidePopup();
    event.Skip();
}

void wxPGComboFrameEventHandler::OnMove( wxMoveEvent& event )
{
    m_combo->HidePopup();
    event.Skip();
}

#endif // INSTALL_TOPLEV_HANDLER

// ----------------------------------------------------------------------------
// wxPGComboPopupWindow is wxPopupWindow customized for
// wxComboControl.
// ----------------------------------------------------------------------------

class wxPGComboPopupWindow : public wxPGComboPopupWindowBase
{
public:

    wxPGComboPopupWindow( wxPGComboControlBase *parent, int style = wxBORDER_NONE );

#if USE_TRANSIENT_POPUP
    virtual bool ProcessLeftDown(wxMouseEvent& event);
#endif

    void OnKeyEvent(wxKeyEvent& event);

    void OnMouseEvent( wxMouseEvent& event );
#if !wxUSE_POPUPWIN
    void OnActivate( wxActivateEvent& event );
#endif

protected:

#if USE_TRANSIENT_POPUP
    virtual void OnDismiss();
#endif

private:
    DECLARE_EVENT_TABLE()
};


BEGIN_EVENT_TABLE(wxPGComboPopupWindow, wxPGComboPopupWindowBase)
    EVT_MOUSE_EVENTS(wxPGComboPopupWindow::OnMouseEvent)
#if !wxUSE_POPUPWIN
    EVT_ACTIVATE(wxPGComboPopupWindow::OnActivate)
#endif
    EVT_KEY_DOWN(wxPGComboPopupWindow::OnKeyEvent)
    EVT_KEY_UP(wxPGComboPopupWindow::OnKeyEvent)
END_EVENT_TABLE()


wxPGComboPopupWindow::wxPGComboPopupWindow( wxPGComboControlBase *parent,
                                        int style )
#if wxUSE_POPUPWIN
                                       : wxPGComboPopupWindowBase(parent,style)
#else
                                       : wxPGComboPopupWindowBase(parent,
                                                                wxID_ANY,
                                                                wxEmptyString,
                                                                wxPoint(-21,-21),
                                                                wxSize(20,20),
                                                                style)
#endif
{
}

void wxPGComboPopupWindow::OnKeyEvent( wxKeyEvent& event )
{
    // Relay keyboard event to the main child controls
    // (just skipping may just cause the popup to close)
    wxWindowList children = GetChildren();
    wxWindowList::iterator node = children.begin();
    wxWindow* child = (wxWindow*)*node;
    child->AddPendingEvent(event);
}

void wxPGComboPopupWindow::OnMouseEvent( wxMouseEvent& event )
{
    event.Skip();
}

#if !wxUSE_POPUPWIN
void wxPGComboPopupWindow::OnActivate( wxActivateEvent& event )
{
    if ( !event.GetActive() )
    {
        // Tell combo control that we are dismissed.
        wxPGComboControl* combo = (wxPGComboControl*) GetParent();
        wxASSERT( combo );
        wxASSERT( combo->IsKindOf(CLASSINFO(wxPGComboControl)) );

        combo->HidePopup();

        event.Skip();
    }
}
#endif

#if USE_TRANSIENT_POPUP
bool wxPGComboPopupWindow::ProcessLeftDown(wxMouseEvent& event )
{
    return wxPGComboPopupWindowBase::ProcessLeftDown(event);
}
#endif

#if USE_TRANSIENT_POPUP
// First thing that happens when a transient popup closes is that this method gets called.
void wxPGComboPopupWindow::OnDismiss()
{
    wxPGComboControlBase* combo = (wxPGComboControlBase*) GetParent();
    wxASSERT ( combo->IsKindOf(CLASSINFO(wxPGComboControlBase)) );

    combo->OnPopupDismiss();
}
#endif

// ----------------------------------------------------------------------------
// wxPGComboPopup methods
//
// ----------------------------------------------------------------------------

wxPGComboPopup::~wxPGComboPopup()
{
}

void wxPGComboPopup::OnPopup()
{
}

void wxPGComboPopup::OnDismiss()
{
}

wxSize wxPGComboPopup::GetAdjustedSize( int minWidth,
                                      int prefHeight,
                                      int WXUNUSED(maxHeight) )
{
    return wxSize(minWidth,prefHeight);
}

void wxPGComboPopup::PaintComboControl( wxDC& dc, const wxRect& rect )
{
    if ( m_combo->GetWindowStyle() & wxCB_READONLY ) // ie. no textctrl
    {
        m_combo->DrawFocusBackground(dc,rect,0);

        dc.DrawText( GetStringValue(),
                     rect.x + m_combo->GetTextIndent(),
                     (rect.height-dc.GetCharHeight())/2 + m_combo->m_widthCustomBorder );
    }
}

void wxPGComboPopup::OnComboKeyEvent( wxKeyEvent& event )
{
    event.Skip();
}

void wxPGComboPopup::OnComboDoubleClick()
{
}

void wxPGComboPopup::SetStringValue( const wxString& WXUNUSED(value) )
{
}

bool wxPGComboPopup::LazyCreate()
{
    return false;
}

void wxPGComboPopup::Dismiss()
{
    m_combo->HidePopup();
}

// ----------------------------------------------------------------------------
// wxPGVListBoxComboPopup is a wxVListBox customized to act as a popup control
//
// ----------------------------------------------------------------------------


BEGIN_EVENT_TABLE(wxPGVListBoxComboPopup, wxVListBox)
    EVT_MOTION(wxPGVListBoxComboPopup::OnMouseMove)
    EVT_KEY_DOWN(wxPGVListBoxComboPopup::OnKey)
    EVT_LEFT_UP(wxPGVListBoxComboPopup::OnLeftClick)
#if wxCHECK_VERSION(2,8,0)
    EVT_MOUSE_CAPTURE_LOST(wxPGVListBoxComboPopup::OnMouseCaptureLost)
#endif
END_EVENT_TABLE()


wxPGVListBoxComboPopup::wxPGVListBoxComboPopup(wxPGComboControl* combo)
                                           : wxVListBox(), wxPGComboPopup(combo)
{
    //m_callback = callback;
    m_widestWidth = 0;
    m_avgCharWidth = 0;
    m_baseImageWidth = 0;
    m_itemHeight = 0;
    m_value = -1;
    m_itemHover = -1;
    m_clientDataItemsType = wxClientData_None;
}

bool wxPGVListBoxComboPopup::Create(wxWindow* parent)
{
    if ( !wxVListBox::Create(parent,
                             wxID_ANY,
                             wxDefaultPosition,
                             wxDefaultSize,
                             wxBORDER_SIMPLE | wxLB_INT_HEIGHT | wxWANTS_CHARS) )
        return false;

    m_font = m_combo->GetFont();

    wxVListBox::SetItemCount(m_strings.GetCount());

    // TODO: Move this to SetFont
    m_itemHeight = GetCharHeight() + 0;

    return true;
}

wxPGVListBoxComboPopup::~wxPGVListBoxComboPopup()
{
    Clear();
}

bool wxPGVListBoxComboPopup::LazyCreate()
{
    // NB: There is a bug with wxVListBox that can be avoided by creating
    //     it later (bug causes empty space to be shown if initial selection
    //     is at the end of a list longer than the control can show at once).
    return true;
}

// paint the control itself
void wxPGVListBoxComboPopup::PaintComboControl( wxDC& dc, const wxRect& rect )
{
    if ( !(m_combo->GetWindowStyle() & wxODCB_STD_CONTROL_PAINT) )
    {
        int flags = wxPGCC_PAINTING_CONTROL;

        if ( m_combo->ShouldDrawFocus() )
            flags |= wxPGCC_PAINTING_SELECTED;

        m_combo->DrawFocusBackground(dc,rect,0);
        if ( m_combo->OnDrawItem(dc,rect,m_value,flags) )
            return;
    }

    wxPGComboPopup::PaintComboControl(dc,rect);
}

void wxPGVListBoxComboPopup::OnDrawItem( wxDC& dc, const wxRect& rect, size_t n) const
{
    dc.SetFont(m_font);

    bool isHilited = wxVListBox::GetSelection() == (int) n;

    // Set correct text colour for selected items
    // (must always set the correct colour - atleast GTK may have lost it
    // in between calls).
    if ( isHilited )
        dc.SetTextForeground( wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT) );
    else
        dc.SetTextForeground( wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT) );

    if ( !m_combo->OnDrawItem(dc,rect,(int)n,0) )
        dc.DrawText( GetString(n), rect.x + 2, rect.y );
}

wxCoord wxPGVListBoxComboPopup::OnMeasureItem(size_t n) const
{
    int itemHeight = m_combo->OnMeasureItem(n);
    if ( itemHeight < 0 )
        itemHeight = m_itemHeight;

    return itemHeight;
}

void wxPGVListBoxComboPopup::OnDrawBackground(wxDC& dc, const wxRect& rect, size_t n) const
{
    // we need to render selected and current items differently
    if ( IsCurrent(n) )
    {
        m_combo->DrawFocusBackground( dc, rect, wxCONTROL_ISSUBMENU|wxCONTROL_SELECTED );
    }
    //else: do nothing for the normal items
}

void wxPGVListBoxComboPopup::DismissWithEvent()
{
    int selection = wxVListBox::GetSelection();

    Dismiss();

    if ( selection != wxNOT_FOUND )
        m_stringValue = m_strings[selection];
    else
        m_stringValue = wxEmptyString;

    if ( m_stringValue != m_combo->GetValue() )
        m_combo->SetValue(m_stringValue);

    m_value = selection;

    SendComboBoxEvent(selection);
}

void wxPGVListBoxComboPopup::SendComboBoxEvent( int selection )
{
    wxCommandEvent evt(wxEVT_COMMAND_COMBOBOX_SELECTED,m_combo->GetId());

    evt.SetEventObject(m_combo);

    evt.SetInt(selection);

    // Set client data, if any
    if ( selection >= 0 && (int)m_clientDatas.GetCount() > selection )
    {
        void* clientData = m_clientDatas[selection];
        if ( m_clientDataItemsType == wxClientData_Object )
            evt.SetClientObject((wxClientData*)clientData);
        else
            evt.SetClientData(clientData);
    }

    m_combo->GetEventHandler()->AddPendingEvent(evt);
}

// returns true if key was consumed
bool wxPGVListBoxComboPopup::HandleKey( int keycode, bool saturate )
{
    int value = m_value;
    int itemCount = GetCount();

    if ( itemCount == 0 )
        return false;

    if ( keycode == WXK_DOWN || keycode == WXK_RIGHT )
    {
        value++;
    }
    else if ( keycode == WXK_UP || keycode == WXK_LEFT )
    {
        value--;
    }
    else if ( keycode == WXK_PAGEDOWN )
    {
        value+=10;
    }
    else if ( keycode == WXK_PAGEUP )
    {
        value-=10;
    }
    /*
    else if ( keycode == WXK_END )
    {
        value = itemCount-1;
    }
    else if ( keycode == WXK_HOME )
    {
        value = 0;
    }
    */
    else
        return false;

    if ( saturate )
    {
        if ( value >= itemCount )
            value = itemCount - 1;
        else if ( value < 0 )
            value = 0;
    }
    else
    {
        if ( value >= itemCount )
            value -= itemCount;
        else if ( value < 0 )
            value += itemCount;
    }

    if ( value == m_value )
        // Even if value was same, don't skip the event
        // (good for consistency)
        return true;

    m_value = value;

    wxString valStr;
    if ( value >= 0 )
        m_combo->SetValue(m_strings[value]);

    SendComboBoxEvent(m_value);

    return true;
}

void wxPGVListBoxComboPopup::OnComboDoubleClick()
{
    // Cycle on dclick (disable saturation to allow true cycling).
    if ( !::wxGetKeyState(WXK_SHIFT) )
        HandleKey(WXK_DOWN,false);
    else
        HandleKey(WXK_UP,false);
}

void wxPGVListBoxComboPopup::OnComboKeyEvent( wxKeyEvent& event )
{
    // Saturated key movement on
    if ( !HandleKey(event.GetKeyCode(),true) )
        event.Skip();
}

void wxPGVListBoxComboPopup::OnPopup()
{
    // *must* set value after size is set (this is because of a vlbox bug)
    wxVListBox::SetSelection(m_value);
}

void wxPGVListBoxComboPopup::OnMouseMove(wxMouseEvent& event)
{
    // Move selection to cursor if it is inside the popup
    int itemHere = GetItemAtPosition(event.GetPosition());
    if ( itemHere >= 0 )
        wxVListBox::SetSelection(itemHere);

    event.Skip();
}

void wxPGVListBoxComboPopup::OnLeftClick(wxMouseEvent& WXUNUSED(event))
{
    DismissWithEvent();
}

void wxPGVListBoxComboPopup::OnKey(wxKeyEvent& event)
{
    // Select item if ENTER is pressed
    if ( event.GetKeyCode() == WXK_RETURN || event.GetKeyCode() == WXK_NUMPAD_ENTER )
    {
        DismissWithEvent();
    }
    // Hide popup if ESC is pressed
    else if ( event.GetKeyCode() == WXK_ESCAPE )
        Dismiss();
    else
        event.Skip();
}

void wxPGVListBoxComboPopup::CheckWidth( int pos )
{
    wxCoord x = m_combo->OnMeasureItemWidth(pos);

    if ( x < 0 )
    {
        if ( !m_font.Ok() )
            m_font = m_combo->GetFont();

        wxCoord y;
        m_combo->GetTextExtent(m_strings[pos], &x, &y, 0, 0, &m_font);
        x += 4;
    }

    if ( m_widestWidth < x )
    {
        m_widestWidth = x;
    }
}

void wxPGVListBoxComboPopup::Insert( const wxString& item, int pos )
{
    // Need to change selection?
    wxString strValue;
    if ( !(m_combo->GetWindowStyle() & wxCB_READONLY) &&
         m_combo->GetValue() == item )
        m_value = pos;
    else if ( m_value >= pos )
        m_value++;

    m_strings.Insert(item,pos);

    if ( IsCreated() )
        wxVListBox::SetItemCount( wxVListBox::GetItemCount()+1 );

    // Calculate width
    CheckWidth(pos);
}

int wxPGVListBoxComboPopup::Append(const wxString& item)
{
    int pos = (int)m_strings.GetCount();

    if ( m_combo->GetWindowStyle() & wxCB_SORT )
    {
        // Find position
        // TODO: Could be optimized with binary search
        wxArrayString strings = m_strings;
        unsigned int i;

        for ( i=0; i<strings.GetCount(); i++ )
        {
            if ( item.Cmp(strings.Item(i)) < 0 )
            {
                pos = (int)i;
                break;
            }
        }
    }

    Insert(item,pos);

    return pos;
}

void wxPGVListBoxComboPopup::Clear()
{
    wxASSERT(m_combo);

    m_strings.Empty();

    ClearClientDatas();

    if ( IsCreated() )
        wxVListBox::SetItemCount(0);
}

void wxPGVListBoxComboPopup::ClearClientDatas()
{
    if ( m_clientDataItemsType == wxClientData_Object )
    {
        size_t i;
        for ( i=0; i<m_clientDatas.GetCount(); i++ )
            delete (wxClientData*) m_clientDatas[i];
    }

    m_clientDatas.Empty();
}

void wxPGVListBoxComboPopup::SetItemClientData( wxODCIndex n,
                                              void* clientData,
                                              wxClientDataType clientDataItemsType )
{
    // It should be sufficient to update this variable only here
    m_clientDataItemsType = clientDataItemsType;

    m_clientDatas.SetCount(n+1,NULL);
    m_clientDatas[n] = clientData;
}

void* wxPGVListBoxComboPopup::GetItemClientData(wxODCIndex n) const
{
    if ( ((wxODCIndex)m_clientDatas.GetCount()) > n )
        return m_clientDatas[n];

    return NULL;
}

void wxPGVListBoxComboPopup::Delete( wxODCIndex item )
{
    // Remove client data, if set
    if ( m_clientDatas.GetCount() )
    {
        if ( m_clientDataItemsType == wxClientData_Object )
            delete (wxClientData*) m_clientDatas[item];

        m_clientDatas.RemoveAt(item);
    }

    m_strings.RemoveAt(item);

    int sel = GetSelection();

    if ( IsCreated() )
        wxVListBox::SetItemCount( wxVListBox::GetItemCount()-1 );

    if ( (int)item < sel )
        SetSelection(sel-1);
    else if ( (int)item == sel )
        SetSelection(wxNOT_FOUND);
}

int wxPGVListBoxComboPopup::FindString(const wxString& s) const
{
    return m_strings.Index(s);
}

wxODCCount wxPGVListBoxComboPopup::GetCount() const
{
    return m_strings.GetCount();
}

wxString wxPGVListBoxComboPopup::GetString( int item ) const
{
    return m_strings[item];
}

void wxPGVListBoxComboPopup::SetString( int item, const wxString& str )
{
    m_strings[item] = str;
}

wxString wxPGVListBoxComboPopup::GetStringValue() const
{
    return m_stringValue;
}

void wxPGVListBoxComboPopup::SetSelection( int item )
{
    // This seems to be necessary (2.5.3 w/ MingW atleast)
    if ( item < -1 || item >= (int)m_strings.GetCount() )
        item = -1;

    m_value = item;
    if ( item >= 0 )
        m_stringValue = m_strings[item];
    else
        m_stringValue = wxEmptyString;

    if ( IsCreated() )
        wxVListBox::SetSelection(item);
}

int wxPGVListBoxComboPopup::GetSelection() const
{
    return m_value;
}

void wxPGVListBoxComboPopup::SetStringValue( const wxString& value )
{
    int index = m_strings.Index(value);

    m_stringValue = value;

    if ( index >= 0 && index < (int)wxVListBox::GetItemCount() )
    {
        wxVListBox::SetSelection(index);
        m_value = index;
    }
}

wxSize wxPGVListBoxComboPopup::GetAdjustedSize( int minWidth, int prefHeight, int maxHeight )
{
    int height = 250;

    if ( m_strings.GetCount() )
    {
        if ( prefHeight > 0 )
            height = prefHeight;

        if ( height > maxHeight )
            height = maxHeight;

        int totalHeight = GetTotalHeight(); // + 3;
        if ( height >= totalHeight )
        {
            height = totalHeight;
        }
        else
        {
            // Adjust height to a multiple of the height of the first item
            // NB: Calculations that take variable height into account
            //     are unnecessary.
            int fih = GetLineHeight(0);
            int shown = height/fih;
            height = shown * fih;
        }
    }
    else
        height = 50;

    // JACS
#if defined(__WXMAC__)
    // Set a minimum height since otherwise scrollbars won't draw properly
    height = wxMax(50, height);
#endif

    // Take scrollbar into account in width calculations
    int widestWidth = m_widestWidth + wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);
    return wxSize(minWidth > widestWidth ? minWidth : widestWidth,
                  height+2);
}

void wxPGVListBoxComboPopup::Populate( int n, const wxString choices[] )
{
    int i;

    for ( i=0; i<n; i++ )
    {
        const wxString& item = choices[i];
        m_strings.Add(item);
        CheckWidth(i);
    }

    if ( IsCreated() )
        wxVListBox::SetItemCount(n);

    // Sort the initial choices
    if ( m_combo->GetWindowStyle() & wxCB_SORT )
        m_strings.Sort();

    // Find initial selection
    wxString strValue = m_combo->GetValue();
    if ( strValue.Length() )
        m_value = m_strings.Index(strValue);
}

#if wxCHECK_VERSION(2,8,0)
void wxPGVListBoxComboPopup::OnMouseCaptureLost(wxMouseCaptureLostEvent& event)
{
    event.Skip(false);  // we don't want the event processed anywhere else
}
#endif

// ----------------------------------------------------------------------------
// input handling
// ----------------------------------------------------------------------------

//
// This is pushed to the event handler queue of either combo box
// or its textctrl (latter if not readonly combo).
//
class wxPGComboBoxTextCtrlHandler : public wxEvtHandler
{
public:

    wxPGComboBoxTextCtrlHandler( wxPGComboControlBase* combo )
        : wxEvtHandler()
    {
        m_combo = combo;
    }
    ~wxPGComboBoxTextCtrlHandler() { }
    void OnKey(wxKeyEvent& event);
    void OnFocus(wxFocusEvent& event);

protected:
    wxPGComboControlBase*   m_combo;

private:
    DECLARE_EVENT_TABLE()
};


BEGIN_EVENT_TABLE(wxPGComboBoxTextCtrlHandler, wxEvtHandler)
    EVT_KEY_DOWN(wxPGComboBoxTextCtrlHandler::OnKey)
    EVT_SET_FOCUS(wxPGComboBoxTextCtrlHandler::OnFocus)
END_EVENT_TABLE()


void wxPGComboBoxTextCtrlHandler::OnKey(wxKeyEvent& event)
{
    // Let the wxComboCtrl event handler have a go first.
    wxPGComboControlBase* combo = m_combo;
    wxObject* prevObj = event.GetEventObject();

    event.SetId(combo->GetId());
    event.SetEventObject(combo);
    combo->GetEventHandler()->ProcessEvent(event);

    event.SetId(((wxWindow*)prevObj)->GetId());
    event.SetEventObject(prevObj);
}

void wxPGComboBoxTextCtrlHandler::OnFocus(wxFocusEvent& event)
{
    // FIXME: This code does run when control is clicked,
    //        yet on Windows it doesn't select all the text.
    if ( !(m_combo->GetInternalFlags() & wxPGCC_NO_TEXT_AUTO_SELECT) )
    {
        if ( m_combo->GetTextCtrl() )
            m_combo->GetTextCtrl()->SelectAll();
        else
            m_combo->SetSelection(-1,-1);
    }

    // Send focus indication to parent.
    // NB: This is needed for cases where the textctrl gets focus
    //     instead of its parent. We'll check if the focus came from
    //     in order to prevent a possible infinite recursion.
    if ( m_combo->ConsumingTextCtrlFocusEvent() )
    {
        wxFocusEvent evt2(wxEVT_SET_FOCUS,m_combo->GetId());
        evt2.SetEventObject(m_combo);
        m_combo->GetEventHandler()->ProcessEvent(evt2);
    }

    event.Skip();
}


//
// This is pushed to the event handler queue of the control in popup.
//

class wxPGComboPopupExtraEventHandler : public wxEvtHandler
{
public:

    wxPGComboPopupExtraEventHandler( wxPGComboControlBase* combo )
        : wxEvtHandler()
    {
        m_combo = combo;
        m_beenInside = false;
    }
    ~wxPGComboPopupExtraEventHandler() { }

    void OnMouseEvent( wxMouseEvent& event );

    // Called from wxPGComboControlBase::OnPopupDismiss
    void OnPopupDismiss()
    {
        m_beenInside = false;
    }

protected:
    wxPGComboControlBase*     m_combo;

    bool                      m_beenInside;

private:
    DECLARE_EVENT_TABLE()
};


BEGIN_EVENT_TABLE(wxPGComboPopupExtraEventHandler, wxEvtHandler)
    EVT_MOUSE_EVENTS(wxPGComboPopupExtraEventHandler::OnMouseEvent)
END_EVENT_TABLE()


void wxPGComboPopupExtraEventHandler::OnMouseEvent( wxMouseEvent& event )
{
    wxPoint pt = event.GetPosition();
    wxSize sz = m_combo->GetPopupControl()->GetClientSize();
    int evtType = event.GetEventType();
    bool isInside = pt.x >= 0 && pt.y >= 0 && pt.x < sz.x && pt.y < sz.y;

    if ( evtType == wxEVT_MOTION ||
         evtType == wxEVT_LEFT_DOWN ||
         evtType == wxEVT_RIGHT_DOWN )
    {
        // Block motion and click events outside the popup
        if ( !isInside )
        {
            event.Skip(false);
            return;
        }
    }
    else if ( evtType == wxEVT_LEFT_UP )
    {
        // Don't let left-down events in if outside
        if ( evtType == wxEVT_LEFT_DOWN )
        {
            if ( !isInside )
                return;
        }

        if ( !m_beenInside )
        {
            if ( isInside )
            {
                m_beenInside = true;
            }
            else
            {
                //
                // Some mouse events to popup that happen outside it, before cursor
                // has been inside the popu, need to be ignored by it but relayed to
                // the dropbutton.
                //
                wxWindow* btn = m_combo->GetButton();
                if ( btn )
                    btn->GetEventHandler()->AddPendingEvent(event);
                else
                    m_combo->GetEventHandler()->AddPendingEvent(event);

                return;
            }

            event.Skip();
        }
    }

    event.Skip();
}

// ----------------------------------------------------------------------------
// wxPGComboControlBase
// ----------------------------------------------------------------------------


BEGIN_EVENT_TABLE(wxPGComboControlBase, wxControl)
    EVT_TEXT(wxID_ANY,wxPGComboControlBase::OnTextCtrlEvent)
    EVT_SIZE(wxPGComboControlBase::OnSizeEvent)
    EVT_KEY_DOWN(wxPGComboControlBase::OnKeyEvent)
    EVT_SET_FOCUS(wxPGComboControlBase::OnFocusEvent)
    EVT_KILL_FOCUS(wxPGComboControlBase::OnFocusEvent)
    //EVT_BUTTON(wxID_ANY,wxPGComboControlBase::OnButtonClickEvent)
    EVT_TEXT_ENTER(wxID_ANY,wxPGComboControlBase::OnTextCtrlEvent)
    EVT_SYS_COLOUR_CHANGED(wxPGComboControlBase::OnSysColourChanged)
END_EVENT_TABLE()


IMPLEMENT_ABSTRACT_CLASS(wxPGComboControlBase, wxControl)

// Have global double buffer - should be enough for multiple combos
static wxBitmap* gs_doubleBuffer = (wxBitmap*) NULL;

void wxPGComboControlBase::Init()
{
    m_winPopup = (wxWindow *)NULL;
    m_popup = (wxWindow *)NULL;
    m_isPopupShown = false;
    m_btn = (wxWindow*) NULL;
    m_text = (wxTextCtrl*) NULL;
    m_popupInterface = (wxPGComboPopup*) NULL;

    m_popupExtraHandler = (wxEvtHandler*) NULL;
    m_textEvtHandler = (wxEvtHandler*) NULL;

#if INSTALL_TOPLEV_HANDLER
    m_toplevEvtHandler = (wxEvtHandler*) NULL;
#endif

    m_heightPopup = -1;

    m_widthMinPopup = -1;

    m_widthCustomPaint = 0;

    m_widthCustomBorder = 0;

    m_btnState = 0;

    m_btnWidDefault = 0;

    m_blankButtonBg = false;

    m_btnWid = m_btnHei = 0;
    m_btnSide = wxRIGHT;
    m_btnSpacingX = 0;

    m_extLeft = 0;
    m_extRight = 0;

    m_absIndent = -1;

    m_iFlags = 0;

    m_fakePopupUsage = 0;
    m_skipTextCtrlFocusEvents = 0;

    m_timeCanAcceptClick = 0;
}

bool wxPGComboControlBase::Create(wxWindow *parent,
                                    wxWindowID id,
                                    const wxString& value,
                                    const wxPoint& pos,
                                    const wxSize& size,
                                    long style,
                                    const wxValidator& validator,
                                    const wxString& name)
{
    if ( !wxControl::Create(parent,
                            id,
                            pos,
                            size,
                            style | wxWANTS_CHARS,
                            validator,
                            name) )
        return false;

    m_valueString = value;

    // Get colours
    OnThemeChange();
    m_absIndent = GetNativeTextIndent();

    return true;
}

void wxPGComboControlBase::InstallInputHandlers()
{
    if ( m_text )
    {
        m_textEvtHandler = new wxPGComboBoxTextCtrlHandler(this);
        m_text->PushEventHandler(m_textEvtHandler);
    }
}

void wxPGComboControlBase::CreateTextCtrl( int extraStyle, const wxValidator& validator )
{
    if ( !(m_windowStyle & wxCB_READONLY) )
    {
        m_text = new wxTextCtrl(this,
                                12345,
                                m_valueString,
                                wxDefaultPosition,
                                wxDefaultSize,
                                // wxTE_PROCESS_TAB is needed because on Windows, wxTAB_TRAVERSAL is
                                // not used by the wxPropertyGrid and therefore the tab is
                                // processed by looking at ancestors to see if they have
                                // wxTAB_TRAVERSAL. The navigation event is then sent to
                                // the wrong window.
                                wxTE_PROCESS_TAB |
                                wxTE_PROCESS_ENTER |
                                //wxWANTS_CHARS |
                                extraStyle,
                                validator);

    #if defined(__WXMSW__) && !defined(__WXWINCE__)
        ::SendMessage(GetHwndOf(m_text), EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELONG(3, 3));
    #endif

        // This is required for some platforms (GTK+ atleast)
        m_text->SetSizeHints(2,4);
    }
}

void wxPGComboControlBase::OnThemeChange()
{
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
}

bool wxPGComboControlBase::Destroy()
{
    return wxControl::Destroy();
}

wxPGComboControlBase::~wxPGComboControlBase()
{
    if ( HasCapture() )
        ReleaseMouse();

    HidePopup();

    delete gs_doubleBuffer;
    gs_doubleBuffer = (wxBitmap*) NULL;

#if INSTALL_TOPLEV_HANDLER
    delete ((wxPGComboFrameEventHandler*)m_toplevEvtHandler);
    m_toplevEvtHandler = (wxEvtHandler*) NULL;
#endif

    if ( m_popup )
        m_popup->RemoveEventHandler(m_popupExtraHandler);

    delete m_popupExtraHandler;

    delete m_popupInterface;
    delete m_winPopup;

    if ( m_text )
        m_text->RemoveEventHandler(m_textEvtHandler);

    delete m_textEvtHandler;
}


// ----------------------------------------------------------------------------
// geometry stuff
// ----------------------------------------------------------------------------

// Recalculates button and textctrl areas
void wxPGComboControlBase::CalculateAreas( int btnWidth )
{
    wxSize sz = GetClientSize();
    int customBorder = m_widthCustomBorder;
    bool buttonOutside;
    int btnBorder; // border for button only

    if ( ( (m_iFlags & wxPGCC_BUTTON_OUTSIDE_BORDER) || m_blankButtonBg ) &&
         m_btnSpacingX == 0 && m_btnWid == 0 && m_btnHei == 0 &&
         (!m_bmpNormal.Ok() || m_blankButtonBg) )
    {
        buttonOutside = true;
        m_iFlags |= wxPGCC_IFLAG_BUTTON_OUTSIDE;
        btnBorder = 0;
    }
    else
    {
        buttonOutside = false;
        m_iFlags &= ~(wxPGCC_IFLAG_BUTTON_OUTSIDE);
        btnBorder = customBorder;
    }

    // Defaul indentation
    if ( m_absIndent < 0 )
        m_absIndent = GetNativeTextIndent();

    int butWidth = btnWidth;

    if ( butWidth <= 0 )
        butWidth = m_btnWidDefault;
    else
        m_btnWidDefault = butWidth;

    if ( butWidth <= 0 )
        return;

    // Adjust button width
    if ( m_btnWid < 0 )
        butWidth += m_btnWid;
    else if ( m_btnWid > 0 )
        butWidth = m_btnWid;

    int butHeight = sz.y;

    butHeight -= btnBorder*2;

    // Adjust button height
    if ( m_btnHei < 0 )
        butHeight += m_btnHei;
    else if ( m_btnHei > 0 )
        butHeight = m_btnHei;

    // Use size of normal bitmap if...
    //   It is larger
    //   OR
    //   button width is set to default and blank button bg is not drawn
    if ( m_bmpNormal.Ok() )
    {
        int bmpReqWidth = m_bmpNormal.GetWidth();
        int bmpReqHeight = m_bmpNormal.GetHeight();

        // If drawing blank button background, we need to add some margin.
        if ( m_blankButtonBg )
        {
            bmpReqWidth += BMP_BUTTON_MARGIN*2;
            bmpReqHeight += BMP_BUTTON_MARGIN*2;
        }

        if ( butWidth < bmpReqWidth || ( m_btnWid == 0 && !m_blankButtonBg ) )
            butWidth = bmpReqWidth;
        if ( butHeight < bmpReqHeight || ( m_btnHei == 0 && !m_blankButtonBg ) )
            butHeight = bmpReqHeight;

        // Need to fix height?
        if ( (sz.y-(customBorder*2)) < butHeight && btnWidth == 0 )
        {
            int newY = butHeight+(customBorder*2);
            SetClientSize(-1,newY);
            sz.y = newY;
        }
    }

    int butAreaWid = butWidth + (m_btnSpacingX*2);

    m_btnSize.x = butWidth;
    m_btnSize.y = butHeight;

    m_btnArea.x = ( m_btnSide==wxRIGHT ? sz.x - butAreaWid - btnBorder : btnBorder );
    m_btnArea.y = btnBorder;
    m_btnArea.width = butAreaWid;
    m_btnArea.height = sz.y - (btnBorder*2);

    if ( m_bmpNormal.Ok() || m_btnArea.width != butWidth || m_btnArea.height != butHeight )
        m_iFlags |= wxPGCC_IFLAG_HAS_NONSTANDARD_BUTTON;
    else
        m_iFlags &= ~wxPGCC_IFLAG_HAS_NONSTANDARD_BUTTON;

    m_tcArea.x = ( m_btnSide==wxRIGHT ? 0 : butAreaWid ) + customBorder;
    m_tcArea.y = customBorder;
    m_tcArea.width = sz.x - butAreaWid - (customBorder*2);
    m_tcArea.height = sz.y - (customBorder*2);

/*
    if ( m_text )
    {
        ::wxMessageBox(wxString::Format(wxT("ButtonArea (%i,%i,%i,%i)\n"),m_btnArea.x,m_btnArea.y,m_btnArea.width,m_btnArea.height) +
                       wxString::Format(wxT("TextCtrlArea (%i,%i,%i,%i)"),m_tcArea.x,m_tcArea.y,m_tcArea.width,m_tcArea.height));
    }
*/
}

void wxPGComboControlBase::PositionTextCtrl( int textCtrlXAdjust, int textCtrlYAdjust )
{
    if ( !m_text )
        return;

    wxSize sz = GetClientSize();
    int customBorder = m_widthCustomBorder;

    if ( (m_text->GetWindowStyleFlag() & wxBORDER_MASK) == wxNO_BORDER )
    {
        // Centre textctrl
        int tcSizeY = m_text->GetBestSize().y;
        int diff = sz.y - tcSizeY;
        int y = textCtrlYAdjust + (diff/2);

        if ( y < customBorder )
            y = customBorder;

        m_text->SetSize( m_tcArea.x + m_widthCustomPaint + m_absIndent + textCtrlXAdjust,
                         y,
                         m_tcArea.width - g_comboMargin -
                         (textCtrlXAdjust + m_widthCustomPaint + m_absIndent),
                         -1 );

        // Make sure textctrl doesn't exceed the bottom custom border
        wxSize tsz = m_text->GetSize();
        diff = (y + tsz.y) - (sz.y - customBorder);
        if ( diff >= 0 )
        {
            tsz.y = tsz.y - diff - 1;
            m_text->SetSize(tsz);
        }
    }
    else
    {
        m_text->SetSize( m_tcArea.x,
                         0,
                         sz.x - m_btnArea.x - m_widthCustomPaint - customBorder,
                         sz.y );
    }
}

wxSize wxPGComboControlBase::DoGetBestSize() const
{
    wxSize sizeText(150,0);

    if ( m_text )
        sizeText = m_text->GetBestSize();

    // TODO: Better method to calculate close-to-native control height.

    int fhei;
    if ( m_font.Ok() )
        fhei = (m_font.GetPointSize()*2) + 5;
    else if ( wxNORMAL_FONT->Ok() )
        fhei = (wxNORMAL_FONT->GetPointSize()*2) + 5;
    else
        fhei = sizeText.y + 4;

    // Need to force height to accomodate bitmap?
    int btnSizeY = m_btnSize.y;
    if ( m_bmpNormal.Ok() && fhei < btnSizeY )
        fhei = btnSizeY;

    // Control height doesn't depend on border
/*
    // Add border
    int border = m_windowStyle & wxBORDER_MASK;
    if ( border == wxSIMPLE_BORDER )
        fhei += 2;
    else if ( border == wxNO_BORDER )
        fhei += (m_widthCustomBorder*2);
    else
        // Sunken etc.
        fhei += 4;
*/

    // Final adjustments
#ifdef __WXGTK__
    fhei += 1;
#endif

    wxSize ret(sizeText.x + g_comboMargin + DEFAULT_DROPBUTTON_WIDTH,
               fhei);

    CacheBestSize(ret);
    return ret;
}

void wxPGComboControlBase::DoMoveWindow(int x, int y, int width, int height)
{
    // SetSize is called last in create, so it marks the end of creation
    m_iFlags |= wxPGCC_IFLAG_CREATED;

    wxControl::DoMoveWindow(x, y, width, height);
}

void wxPGComboControlBase::OnSizeEvent( wxSizeEvent& event )
{
    if ( !IsCreated() )
        return;

    // defined by actual wxComboControls
    OnResize();

    event.Skip();
}

// ----------------------------------------------------------------------------
// standard operations
// ----------------------------------------------------------------------------

bool wxPGComboControlBase::Enable(bool enable)
{
    if ( !wxControl::Enable(enable) )
        return false;

    if ( m_btn )
        m_btn->Enable(enable);
    if ( m_text )
        m_text->Enable(enable);

    return true;
}

bool wxPGComboControlBase::Show(bool show)
{
    if ( !wxControl::Show(show) )
        return false;

    if (m_btn)
        m_btn->Show(show);

    if (m_text)
        m_text->Show(show);

    return true;
}

bool wxPGComboControlBase::SetFont ( const wxFont& font )
{
    if ( !wxControl::SetFont(font) )
        return false;

    if (m_text)
        m_text->SetFont(font);

    return true;
}

#if wxUSE_TOOLTIPS
void wxPGComboControlBase::DoSetToolTip(wxToolTip *tooltip)
{
    wxControl::DoSetToolTip(tooltip);

    // Set tool tip for button and text box
    if ( tooltip )
    {
        const wxString &tip = tooltip->GetTip();
        if ( m_text ) m_text->SetToolTip(tip);
        if ( m_btn ) m_btn->SetToolTip(tip);
    }
    else
    {
        if ( m_text ) m_text->SetToolTip( (wxToolTip*) NULL );
        if ( m_btn ) m_btn->SetToolTip( (wxToolTip*) NULL );
    }
}
#endif // wxUSE_TOOLTIPS

// ----------------------------------------------------------------------------
// painting
// ----------------------------------------------------------------------------

// draw focus background on area in a way typical on platform
void wxPGComboControlBase::DrawFocusBackground( wxDC& dc, const wxRect& rect, int flags )
{
    wxSize sz = GetClientSize();
    bool isEnabled;
    bool doDrawFocusRect; // also selected

    // For smaller size control (and for disabled background) use less spacing
    int focusSpacingX;
    int focusSpacingY;

    if ( !(flags & wxCONTROL_ISSUBMENU) )
    {
        // Drawing control
        isEnabled = IsEnabled();
        doDrawFocusRect = ShouldDrawFocus();

        // Windows-style: for smaller size control (and for disabled background) use less spacing
        //focusSpacingX = isEnabled ? 2 : 1;
        focusSpacingX = 1;
        focusSpacingY = sz.y > (GetCharHeight()+500) && isEnabled ? 2 : 1;
    }
    else
    {
        // Drawing a list item
        isEnabled = true; // they are never disabled
        doDrawFocusRect = flags & wxCONTROL_SELECTED ? true : false;

        focusSpacingX = 0;
        focusSpacingY = 0;
    }

    // Set the background sub-rectangle for selection, disabled etc
    wxRect selRect(rect);
    selRect.y += focusSpacingY;
    selRect.height -= (focusSpacingY*2);
    int wcp = 0;

    if ( !(flags & wxCONTROL_ISSUBMENU) )
        wcp += m_widthCustomPaint;

    selRect.x += wcp + focusSpacingX;
    selRect.width -= wcp + (focusSpacingX*2);

    wxColour bgCol;
    bool doDrawSelRect = true;

    if ( isEnabled )
    {
        // If popup is hidden and this control is focused,
        // then draw the focus-indicator (selbgcolor background etc.).
        if ( doDrawFocusRect )
        {
            dc.SetTextForeground( wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT) );
            bgCol = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
        }
        else
        {
            dc.SetTextForeground( wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT) );
            bgCol = GetBackgroundColour();
            doDrawSelRect = false;
        }
    }
    else
    {
        dc.SetTextForeground( wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT) );
        bgCol = GetBackgroundColour();
    }

    dc.SetBrush( bgCol );
    if ( doDrawSelRect )
    {
        dc.SetPen( bgCol );
        dc.DrawRectangle( selRect );
    }
}

void wxPGComboControlBase::DrawButton( wxDC& dc, const wxRect& rect, int flags )
{
    int drawState = m_btnState;

    if ( (m_iFlags & wxPGCC_BUTTON_STAYS_DOWN) &&
         IsPopupShown() )
        drawState |= wxCONTROL_PRESSED;

    wxRect drawRect(rect.x+m_btnSpacingX,
                    rect.y+((rect.height-m_btnSize.y)/2),
                    m_btnSize.x,
                    m_btnSize.y);

    // Make sure area is not larger than the control
    if ( drawRect.y < rect.y )
        drawRect.y = rect.y;
    if ( drawRect.height > rect.height )
        drawRect.height = rect.height;

    bool enabled = IsEnabled();

    if ( !enabled )
        drawState |= wxCONTROL_DISABLED;

    if ( !m_bmpNormal.Ok() )
    {
        if ( flags & Button_BitmapOnly )
            return;

        // Need to clear button background even if m_btn is present
        if ( flags & Button_PaintBackground )
        {
            wxColour bgCol;

            if ( m_iFlags & wxPGCC_IFLAG_BUTTON_OUTSIDE )
                bgCol = GetParent()->GetBackgroundColour();
            else
                bgCol = GetBackgroundColour();

            dc.SetBrush(bgCol);
            dc.SetPen(bgCol);
            dc.DrawRectangle(rect);
        }

        // Draw standard button
        wxRendererNative::Get().DrawComboBoxDropButton(this,
                                                       dc,
                                                       drawRect,
                                                       drawState);
    }
    else
    {
        // Draw bitmap

        wxBitmap* pBmp;

        if ( !enabled )
            pBmp = &m_bmpDisabled;
        else if ( m_btnState & wxCONTROL_PRESSED )
            pBmp = &m_bmpPressed;
        else if ( m_btnState & wxCONTROL_CURRENT )
            pBmp = &m_bmpHover;
        else
            pBmp = &m_bmpNormal;

#if wxCHECK_VERSION(2, 7, 0)
        if ( m_blankButtonBg )
        {
            // If using blank button background, we need to clear its background
            // with button face colour instead of colour for rest of the control.
            if ( flags & Button_PaintBackground )
            {
                wxColour bgCol = GetParent()->GetBackgroundColour(); //wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
                //wxColour bgCol = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
                dc.SetPen(bgCol);
                dc.SetBrush(bgCol);
                dc.DrawRectangle(rect);
            }

            if ( !(flags & Button_BitmapOnly) )
            {
                wxRendererNative::Get().DrawPushButton(this,
                                                       dc,
                                                       drawRect,
                                                       drawState);
            }
        }
        else
#endif
        {
            // Need to clear button background even if m_btn is present
            // (assume non-button background was cleared just before this call so brushes are good)
            if ( flags & Button_PaintBackground )
                dc.DrawRectangle(rect);
        }

        // Draw bitmap centered in drawRect
        dc.DrawBitmap(*pBmp,
                      drawRect.x + (drawRect.width-pBmp->GetWidth())/2,
                      drawRect.y + (drawRect.height-pBmp->GetHeight())/2,
                      true);
    }
}

void wxPGComboControlBase::RecalcAndRefresh()
{
    if ( IsCreated() )
    {
        wxSizeEvent evt(GetSize(),GetId());
        GetEventHandler()->ProcessEvent(evt);
        Refresh();
    }
}

bool wxPGComboControlBase::OnDrawItem( wxDC& WXUNUSED(dc),
                                       const wxRect& WXUNUSED(rect),
                                       int WXUNUSED(item),
                                       int WXUNUSED(flags) ) const
{
    return false; // signals caller to make default drawing
}

wxCoord wxPGComboControlBase::OnMeasureItem( size_t WXUNUSED(item) ) const
{
    return -1; // signals caller to use default
}

wxCoord wxPGComboControlBase::OnMeasureItemWidth( size_t WXUNUSED(item) ) const
{
    return -1; // signals caller to use default
}

// ----------------------------------------------------------------------------
// miscellaneous event handlers
// ----------------------------------------------------------------------------

void wxPGComboControlBase::OnTextCtrlEvent(wxCommandEvent& event)
{
    // Change event id and relay it forward
    event.SetId(GetId());
    event.Skip();
}

// call if cursor is on button area or mouse is captured for the button
bool wxPGComboControlBase::HandleButtonMouseEvent( wxMouseEvent& event,
                                                   int flags )
{
    int type = event.GetEventType();

    if ( type == wxEVT_MOTION )
    {
        if ( flags & wxPGCC_MF_ON_BUTTON )
        {
            if ( !(m_btnState & wxCONTROL_CURRENT) )
            {
                // Mouse hover begins
                m_btnState |= wxCONTROL_CURRENT;
                if ( HasCapture() ) // Retain pressed state.
                    m_btnState |= wxCONTROL_PRESSED;
                Refresh();
            }
        }
        else if ( (m_btnState & wxCONTROL_CURRENT) )
        {
            // Mouse hover ends
            m_btnState &= ~(wxCONTROL_CURRENT|wxCONTROL_PRESSED);
            Refresh();
        }
    }
    else if ( type == wxEVT_LEFT_DOWN )
    {
        // Only accept event if it wasn't right after popup dismiss
        //if ( ::wxGetLocalTimeMillis() > m_timeCanClick )
        {
            // Need to test this, because it might be outside.
            if ( flags & wxPGCC_MF_ON_BUTTON )
            {
                m_btnState |= wxCONTROL_PRESSED;
                Refresh();

                if ( !(m_iFlags & wxPGCC_POPUP_ON_MOUSE_UP) )
                    OnButtonClick();
                else
                    // If showing popup now, do not capture mouse or there will be interference
                    CaptureMouse();
            }
        }
        /*else
        {
            m_btnState = 0;
        }*/
    }
    else if ( type == wxEVT_LEFT_UP )
    {

        // Only accept event if mouse was left-press was previously accepted
        if ( HasCapture() )
            ReleaseMouse();

        if ( m_btnState & wxCONTROL_PRESSED )
        {
            // If mouse was inside, fire the click event.
            if ( m_iFlags & wxPGCC_POPUP_ON_MOUSE_UP )
            {
                if ( flags & wxPGCC_MF_ON_BUTTON )
                    OnButtonClick();
            }

            m_btnState &= ~(wxCONTROL_PRESSED);
            Refresh();
        }
    }
    else if ( type == wxEVT_LEAVE_WINDOW )
    {
        if ( m_btnState & (wxCONTROL_CURRENT|wxCONTROL_PRESSED) )
        {
            m_btnState &= ~(wxCONTROL_CURRENT);

            // Mouse hover ends
            if ( !m_isPopupShown )
            {
                m_btnState &= ~(wxCONTROL_PRESSED);
                Refresh();
            }
        }
    }
    else
        return false;

    return true;
}

// Conversion to double-clicks and some basic filtering
// returns true if event was consumed or filtered
bool wxPGComboControlBase::PreprocessMouseEvent( wxMouseEvent& event,
                                                 int WXUNUSED(flags) )
{
    wxLongLong t = ::wxGetLocalTimeMillis();
    int evtType = event.GetEventType();

    if ( m_isPopupShown &&
         ( evtType == wxEVT_LEFT_DOWN || evtType == wxEVT_RIGHT_DOWN ) )
    {
        HidePopup();
        return true;
    }

    //
    // Generate our own double-clicks
    // (to allow on-focus dc-event on double-clicks instead of triple-clicks)
    /*if ( (m_windowStyle & wxPGCC_DCLICK_CYCLES) &&
         !m_isPopupShown &&
         //!(handlerFlags & wxPGCC_MF_ON_BUTTON) )
         !(flags & wxPGCC_MF_ON_BUTTON) )
    {
        if ( evtType == wxEVT_LEFT_DOWN )
        {
            // Set value to avoid up-events without corresponding downs
            m_downReceived = true;
        }
        else if ( evtType == wxEVT_LEFT_DCLICK )
        {
            // We'll make our own double-clicks
            //evtType = 0;
            event.SetEventType(0);
            return true;
        }
        else if ( evtType == wxEVT_LEFT_UP )
        {
            if ( m_downReceived || m_timeLastMouseUp == 1 )
            {
                wxLongLong timeFromLastUp = (t-m_timeLastMouseUp);

                if ( timeFromLastUp < DOUBLE_CLICK_CONVERSION_TRESHOLD )
                {
                    //type = wxEVT_LEFT_DCLICK;
                    event.SetEventType(wxEVT_LEFT_DCLICK);
                    m_timeLastMouseUp = 1;
                }
                else
                {
                    m_timeLastMouseUp = t;
                }

                //m_downReceived = false;
            }
        }
    }*/

    // Filter out clicks on button immediately after popup dismiss (Windows like behaviour)
    if ( evtType == wxEVT_LEFT_DOWN && t < m_timeCanAcceptClick )
    {
        event.SetEventType(0);
        return true;
    }

    return false;
}

void wxPGComboControlBase::HandleNormalMouseEvent( wxMouseEvent& event )
{
    int evtType = event.GetEventType();

    if ( (evtType == wxEVT_LEFT_DOWN || evtType == wxEVT_LEFT_DCLICK) &&
         (m_windowStyle & wxCB_READONLY) )
    {
        if ( m_isPopupShown )
        {
    #if !wxUSE_POPUPWIN
        // Normally do nothing - evt handler should close it for us
      #if ALLOW_FAKE_POPUP
        if ( m_fakePopupUsage == 2 )
            HidePopup();
      #endif
    #elif !USE_TRANSIENT_POPUP
            // Click here always hides the popup.
            HidePopup();
    #endif
        }
        else
        {
            if ( !(m_windowStyle & wxPGCC_DCLICK_CYCLES) )
            {
                // In read-only mode, clicking the text is the
                // same as clicking the button.
                OnButtonClick();
            }
            else if ( /*evtType == wxEVT_LEFT_UP || */evtType == wxEVT_LEFT_DCLICK )
            {
                //if ( m_popupInterface->CycleValue() )
                //    Refresh();
                if ( m_popupInterface )
                    m_popupInterface->OnComboDoubleClick();
            }
        }
    }
    else
    if ( m_isPopupShown )
    {
        // relay (some) mouse events to the popup
        if ( evtType == wxEVT_MOUSEWHEEL )
            m_popup->AddPendingEvent(event);
    }
    else if ( evtType )
        event.Skip();
}

void wxPGComboControlBase::OnKeyEvent( wxKeyEvent& event )
{
    int keycode = event.GetKeyCode();

    if ( keycode == WXK_TAB &&
         !IsPopupShown() )
    {
        wxNavigationKeyEvent evt;
        evt.SetFlags(wxNavigationKeyEvent::FromTab|
                     (!event.ShiftDown()?wxNavigationKeyEvent::IsForward:
                                         wxNavigationKeyEvent::IsBackward));
        evt.SetEventObject(this);
        GetParent()->GetEventHandler()->AddPendingEvent(evt);
        return;
    }

    if ( IsPopupShown() )
    {
        // pass it to the popped up control
        GetPopupControl()->AddPendingEvent(event);
    }
    else // no popup
    {
        int comboStyle = GetWindowStyle();
        wxPGComboPopup* popupInterface = GetPopup();

        if ( !popupInterface )
        {
            event.Skip();
            return;
        }

        if ( (comboStyle & wxCB_READONLY) ||
             ( keycode != WXK_RIGHT && keycode != WXK_LEFT )
            )
        {
            // Alternate keys: UP and DOWN show the popup instead of cycling
            if ( (comboStyle & wxPGCC_ALT_KEYS) )
            {
                if ( keycode == WXK_UP || keycode == WXK_DOWN )
                {
                    OnButtonClick();
                    return;
                }
                else
                    event.Skip();
            }
            else
                popupInterface->OnComboKeyEvent(event);
        }
        else
            event.Skip();
    }
}

void wxPGComboControlBase::OnFocusEvent( wxFocusEvent& event )
{
    if ( event.GetEventType() == wxEVT_SET_FOCUS )
    {
        if ( m_text && m_text != ::wxWindow::FindFocus() )
        {
            m_skipTextCtrlFocusEvents++;
            m_text->SetFocus();
        }
    }

    Refresh();
}

void wxPGComboControlBase::OnSysColourChanged(wxSysColourChangedEvent& WXUNUSED(event))
{
    OnThemeChange();
    // indentation may also have changed
    if ( !(m_iFlags & wxPGCC_IFLAG_INDENT_SET) )
        m_absIndent = GetNativeTextIndent();
    RecalcAndRefresh();
}

// ----------------------------------------------------------------------------
// popup handling
// ----------------------------------------------------------------------------

// Create popup window and the child control
void wxPGComboControlBase::CreatePopup()
{
    wxPGComboPopup* popupInterface = m_popupInterface;
    wxWindow* popup;

    if ( !m_winPopup )
        m_winPopup = new wxPGComboPopupWindow( this, wxNO_BORDER );

    popupInterface->Create(m_winPopup);
    m_popup = popup = popupInterface->GetControl();

    m_popupExtraHandler = new wxPGComboPopupExtraEventHandler(this);
    popup->PushEventHandler( m_popupExtraHandler );

    popupInterface->m_iFlags |= wxPGCP_IFLAG_CREATED;
}

void wxPGComboControlBase::SetPopup( wxPGComboPopup* iface )
{
    delete m_popupInterface;
    delete m_winPopup;

    m_popupInterface = iface;

#if ALLOW_FAKE_POPUP
    m_fakePopupUsage = 0;
#endif

    if ( !iface->LazyCreate() || m_winPopup )
    {
        CreatePopup();
        /*
        m_winPopup = new wxPGComboPopupWindow( this, wxNO_BORDER );

        // Create popup right away
        iface->Create(m_winPopup);
        m_popup = iface->GetControl();
        m_popupExtraHandler = new wxPGComboPopupExtraEventHandler(this);
        m_popup->PushEventHandler( m_popupExtraHandler );

        // Add interface as event handler
        //m_popup->PushEventHandler( iface );
        */

        // This bypasses wxGTK popupwindow bug
        //   (i.e. window is not initially hidden when it should be)
        m_winPopup->Hide();

#if ALLOW_FAKE_POPUP
        m_fakePopupUsage = 1;
#endif
    }
    else
    {
        m_popup = (wxWindow*) NULL;
    }

    // This must be after creation
    if ( m_valueString.length() )
        iface->SetStringValue(m_valueString);

}

void wxPGComboControlBase::OnButtonClick()
{
    // Derived classes can override this method for totally custom
    // popup action
    ShowPopup();
}

void wxPGComboControlBase::ShowPopup()
{
    wxCHECK_RET( m_popupInterface, wxT("no popup interface set for wxComboControl") );
    wxCHECK_RET( !IsPopupShown(), wxT("popup window already shown") );

    SetFocus();

    // Space above and below
    int screenHeight;
    wxPoint scrPos;
    int spaceAbove;
    int spaceBelow;
    int maxHeightPopup;
    wxSize ctrlSz = GetSize();

#if ALLOW_FAKE_POPUP

    int existingHeight = 200;
    if ( m_popup )
        existingHeight = m_popup->GetSize().y;

    int screenWidth;
    GetParent()->GetClientSize(&screenWidth,&screenHeight);
    screenWidth -= 2;
    scrPos = GetPosition();

    spaceAbove = scrPos.y - 2;
    spaceBelow = screenHeight - spaceAbove - ctrlSz.y - 4;

    maxHeightPopup = spaceBelow;
    if ( spaceAbove > spaceBelow )
        maxHeightPopup = spaceAbove;

    if ( maxHeightPopup >= existingHeight )
    {
        if ( m_winPopup && m_fakePopupUsage!=2 )
        {
            delete m_winPopup;
            m_winPopup = (wxWindow*) NULL;
            m_popup = (wxWindow*) NULL;
        }
        m_fakePopupUsage = 2;
    }
    else
    {
        if ( m_winPopup && m_fakePopupUsage!=1 )
        {
            delete m_winPopup;
            m_winPopup = (wxWindow*) NULL;
            m_popup = (wxWindow*) NULL;
        }
        m_fakePopupUsage = 1;
#else
    {
#endif

        screenHeight = wxSystemSettings::GetMetric( wxSYS_SCREEN_Y );
        scrPos = GetParent()->ClientToScreen(GetPosition());

        spaceAbove = scrPos.y;
        spaceBelow = screenHeight - spaceAbove - ctrlSz.y;

        maxHeightPopup = spaceBelow;
        if ( spaceAbove > spaceBelow )
            maxHeightPopup = spaceAbove;

    }

    // Width
    int widthPopup = ctrlSz.x + m_extLeft + m_extRight;

    if ( widthPopup < m_widthMinPopup )
        widthPopup = m_widthMinPopup;

    wxWindow* winPopup = m_winPopup;
    wxWindow* popup;

    // Need to disable tab traversal of parent
    //
    // NB: This is to fix a bug in wxMSW. In theory it could also be fixed
    //     by, for instance, adding check to window.cpp:wxWindowMSW::MSWProcessMessage
    //     that if transient popup is open, then tab traversal is to be ignored.
    //     However, I think this code would still be needed for cases where
    //     transient popup doesn't work yet (wxWINCE?).
    wxWindow* parent = GetParent();
    int parentFlags = parent->GetWindowStyle();
    if ( parentFlags & wxTAB_TRAVERSAL )
    {
        parent->SetWindowStyle( parentFlags & ~(wxTAB_TRAVERSAL) );
        m_iFlags |= wxPGCC_IFLAG_PARENT_TAB_TRAVERSAL;
    }

    if ( !winPopup )
    {
#if ALLOW_FAKE_POPUP
        if ( m_fakePopupUsage == 2 )
        {
            winPopup = new wxWindow();
        #ifdef __WXMSW__
            // Only wxMSW supports this
            winPopup->Hide();
        #endif
            winPopup->Create( GetParent(), -1 );
            m_winPopup = winPopup;
        }
#endif
        CreatePopup();
        winPopup = m_winPopup;
        popup = m_popup;
    }
    else
    {
        popup = m_popup;
    }

    wxASSERT( !m_popup || m_popup == popup ); // Consistency check.

    wxSize adjustedSize = m_popupInterface->GetAdjustedSize(widthPopup,
                                                            m_heightPopup<=0?DEFAULT_POPUP_HEIGHT:m_heightPopup,
                                                            maxHeightPopup);

    popup->SetSize(adjustedSize);
    popup->Move(0,0);
    m_popupInterface->OnPopup();

#if ALLOW_FAKE_POPUP
    // Make sure fake popup didn't get too big
    if ( m_fakePopupUsage == 2 && popup->GetSize().x > screenWidth )
    {
        popup->SetSize(screenWidth-2,popup->GetSize().y);
    }
#endif

    //
    // Reposition and resize popup window
    //

    wxSize szp = popup->GetSize();

    int popupX;
    int popupY = scrPos.y + ctrlSz.y;

    // Anchor popup to the side the dropbutton is on
    if ( m_btnSide == wxRIGHT )
        popupX = scrPos.x + ctrlSz.x + m_extRight- szp.x;
    else
        popupX = scrPos.x - m_extLeft;

#if ALLOW_FAKE_POPUP
    if ( m_fakePopupUsage == 2 )
    {
        if ( spaceBelow < szp.y )
        {
            if ( spaceAbove > spaceBelow )
            {
                if ( szp.y > spaceAbove )
                {
                    popup->SetSize(szp.x,spaceAbove);
                    szp.y = spaceAbove;
                }
                popupY = scrPos.y - szp.y;
            }
            else
            {
                if ( szp.y > spaceBelow )
                {
                    popup->SetSize(szp.x,spaceBelow);
                    szp.y = spaceBelow;
                }
            }
        }
    }
    else
#endif
    if ( spaceBelow < szp.y )
    {
        popupY = scrPos.y - szp.y;
    }

    // Move to position
    //wxLogDebug(wxT("popup scheduled position1: %i,%i"),ptp.x,ptp.y);
    //wxLogDebug(wxT("popup position1: %i,%i"),winPopup->GetPosition().x,winPopup->GetPosition().y);

    // Some platforms (GTK) may need these two to be separate
    winPopup->SetSize( szp.x, szp.y );
    winPopup->Move( popupX, popupY );

    //wxLogDebug(wxT("popup position2: %i,%i"),winPopup->GetPosition().x,winPopup->GetPosition().y);

    m_popup = popup;

    // Set string selection (must be this way instead of SetStringSelection)
    if ( m_text )
    {
        if ( !(m_iFlags & wxPGCC_NO_TEXT_AUTO_SELECT) )
            m_text->SelectAll();

        m_popupInterface->SetStringValue( m_text->GetValue() );
    }
    else
    {
        // This is neede since focus/selection indication may change when popup is shown
        Refresh();
    }

    // This must be after SetStringValue
    m_isPopupShown = true;

    // Show it
#if USE_TRANSIENT_POPUP
    ((wxPopupTransientWindow*)winPopup)->Popup(popup);
#else
    winPopup->Show();
#endif

#if INSTALL_TOPLEV_HANDLER
    // If our real popup is wxDialog, then only install handler
    // incase of fake popup.
  #if !wxUSE_POPUPWIN
    if ( m_fakePopupUsage != 2 )
    {
        if ( m_toplevEvtHandler )
        {
            delete m_toplevEvtHandler;
            m_toplevEvtHandler = (wxEvtHandler*) NULL;
        }
    }
    else
  #endif
    {
        // Put top level window event handler into place
        if ( !m_toplevEvtHandler )
            m_toplevEvtHandler = new wxPGComboFrameEventHandler(this);

        wxWindow* toplev = ::wxGetTopLevelParent( this );
        wxASSERT( toplev );
        ((wxPGComboFrameEventHandler*)m_toplevEvtHandler)->OnPopup();
        toplev->PushEventHandler( m_toplevEvtHandler );
    }
#endif

}

void wxPGComboControlBase::OnPopupDismiss()
{
    // Just in case, avoid double dismiss
    if ( !m_isPopupShown )
        return;

    // *Must* set this before focus etc.
    m_isPopupShown = false;

    // Inform popup control itself
    m_popupInterface->OnDismiss();

    //((wxComboDropButton*)m_btn)->SetPopup( (wxWindow*) NULL );

    if ( m_popupExtraHandler )
        ((wxPGComboPopupExtraEventHandler*)m_popupExtraHandler)->OnPopupDismiss();

#if INSTALL_TOPLEV_HANDLER
    // Remove top level window event handler
    if ( m_toplevEvtHandler )
    {
        wxWindow* toplev = ::wxGetTopLevelParent( this );
        if ( toplev )
            toplev->RemoveEventHandler( m_toplevEvtHandler );
    }
#endif

#if !wxUSE_POPUPWIN
    if ( m_fakePopupUsage != 2 )
        GetParent()->SetFocus();
#endif

    m_timeCanAcceptClick = ::wxGetLocalTimeMillis() + 150;

    // If cursor not on dropdown button, then clear its state
    // (technically not required by all ports, but do it for all just in case)
    if ( !m_btnArea.wxPGRectContains(ScreenToClient(::wxGetMousePosition())) )
        m_btnState = 0;

    // Return parent's tab traversal flag.
    // See ShowPopup for notes.
    if ( m_iFlags & wxPGCC_IFLAG_PARENT_TAB_TRAVERSAL )
    {
        wxWindow* parent = GetParent();
        parent->SetWindowStyle( parent->GetWindowStyle() | wxTAB_TRAVERSAL );
        m_iFlags &= ~(wxPGCC_IFLAG_PARENT_TAB_TRAVERSAL);
    }

    // refresh control (necessary even if m_text)
    Refresh();

}

void wxPGComboControlBase::HidePopup()
{
    // Should be able to call this without popup interface
    //wxCHECK_RET( m_popupInterface, _T("no popup interface") );
    if ( !m_isPopupShown )
        return;

    // transfer value and show it in textctrl, if any
    SetValue( m_popupInterface->GetStringValue() );

#if USE_TRANSIENT_POPUP
    ((wxPopupTransientWindow*)m_winPopup)->Dismiss();
#else
    m_winPopup->Hide();
#endif

    OnPopupDismiss();
}

// ----------------------------------------------------------------------------
// customization methods
// ----------------------------------------------------------------------------

void wxPGComboControlBase::SetButtonPosition( int width, int height,
                                                    int side, int spacingX )
{
    m_btnWid = width;
    m_btnHei = height;
    m_btnSide = side;
    m_btnSpacingX = spacingX;

    RecalcAndRefresh();
}

void wxPGComboControlBase::SetButtonBitmaps( const wxBitmap& bmpNormal,
                                               bool blankButtonBg,
                                               const wxBitmap& bmpPressed,
                                               const wxBitmap& bmpHover,
                                               const wxBitmap& bmpDisabled )
{
    m_bmpNormal = bmpNormal;
    m_blankButtonBg = blankButtonBg;

    if ( bmpPressed.Ok() )
        m_bmpPressed = bmpPressed;
    else
        m_bmpPressed = bmpNormal;

    if ( bmpHover.Ok() )
        m_bmpHover = bmpHover;
    else
        m_bmpHover = bmpNormal;

    if ( bmpDisabled.Ok() )
        m_bmpDisabled = bmpDisabled;
    else
        m_bmpDisabled = bmpNormal;

    RecalcAndRefresh();
}

void wxPGComboControlBase::SetCustomPaintWidth( int width )
{
    if ( m_text )
    {
        // move textctrl accordingly
        wxRect r = m_text->GetRect();
        int inc = width - m_widthCustomPaint;
        r.x += inc;
        r.width -= inc;
        m_text->SetSize( r );
    }

    m_widthCustomPaint = width;

    RecalcAndRefresh();
}

void wxPGComboControlBase::SetTextIndent( int indent )
{
    if ( indent < 0 )
    {
        m_absIndent = GetNativeTextIndent();
        m_iFlags &= ~(wxPGCC_IFLAG_INDENT_SET);
    }
    else
    {
        m_absIndent = indent;
        m_iFlags |= wxPGCC_IFLAG_INDENT_SET;
    }

    RecalcAndRefresh();
}

wxCoord wxPGComboControlBase::GetNativeTextIndent() const
{
    return DEFAULT_TEXT_INDENT;
}

// ----------------------------------------------------------------------------
// methods forwarded to wxTextCtrl
// ----------------------------------------------------------------------------

wxString wxPGComboControlBase::GetValue() const
{
    if ( m_text )
        return m_text->GetValue();
    return m_valueString;
}

void wxPGComboControlBase::SetValue(const wxString& value)
{
    if ( m_text )
    {
        m_text->SetValue(value);
        if ( !(m_iFlags & wxPGCC_NO_TEXT_AUTO_SELECT) )
            m_text->SelectAll();
    }

    // Since wxPGComboPopup may want to paint the combo as well, we need
    // to set the string value here (as well as sometimes in ShowPopup).
    if ( m_valueString != value && m_popupInterface )
    {
        m_popupInterface->SetStringValue(value);
    }

    m_valueString = value;

    Refresh();
}

void wxPGComboControlBase::Copy()
{
    if ( m_text )
        m_text->Copy();
}

void wxPGComboControlBase::Cut()
{
    if ( m_text )
        m_text->Cut();
}

void wxPGComboControlBase::Paste()
{
    if ( m_text )
        m_text->Paste();
}

void wxPGComboControlBase::SetInsertionPoint(long pos)
{
    if ( m_text )
        m_text->SetInsertionPoint(pos);
}

void wxPGComboControlBase::SetInsertionPointEnd()
{
    if ( m_text )
        m_text->SetInsertionPointEnd();
}

long wxPGComboControlBase::GetInsertionPoint() const
{
    if ( m_text )
        return m_text->GetInsertionPoint();

    return 0;
}

long wxPGComboControlBase::GetLastPosition() const
{
    if ( m_text )
        return m_text->GetLastPosition();

    return 0;
}

void wxPGComboControlBase::Replace(long from, long to, const wxString& value)
{
    if ( m_text )
        m_text->Replace(from, to, value);
}

void wxPGComboControlBase::Remove(long from, long to)
{
    if ( m_text )
        m_text->Remove(from, to);
}

void wxPGComboControlBase::SetSelection(long from, long to)
{
    if ( m_text )
        m_text->SetSelection(from, to);
}

void wxPGComboControlBase::Undo()
{
    if ( m_text )
        m_text->Undo();
}


// ----------------------------------------------------------------------------
// wxPGGenericComboControl
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxPGGenericComboControl, wxPGComboControlBase)
    //EVT_SIZE(wxPGGenericComboControl::OnSizeEvent)
    EVT_PAINT(wxPGGenericComboControl::OnPaintEvent)
    EVT_MOUSE_EVENTS(wxPGGenericComboControl::OnMouseEvent)
END_EVENT_TABLE()


IMPLEMENT_DYNAMIC_CLASS(wxPGGenericComboControl, wxPGComboControlBase)

void wxPGGenericComboControl::Init()
{
}

bool wxPGGenericComboControl::Create(wxWindow *parent,
                                   wxWindowID id,
                                   const wxString& value,
                                   const wxPoint& pos,
                                   const wxSize& size,
                                   long style,
                                   const wxValidator& validator,
                                   const wxString& name)
{

    // Set border
    long border = style & wxBORDER_MASK;

#if defined(__WXUNIVERSAL__)
    if ( !border )
        border = wxBORDER_SIMPLE;
#elif defined(__WXMSW__)
    if ( !border )
        border = wxBORDER_SIMPLE;
#else
    if ( !border )
    {
        border = wxBORDER_NONE;
        m_widthCustomBorder = 1;
    }

    Customize( wxPGCC_BUTTON_OUTSIDE_BORDER |
               wxPGCC_NO_TEXT_AUTO_SELECT |
               wxPGCC_BUTTON_STAYS_DOWN );

#endif

    style = (style & ~(wxBORDER_MASK)) | border;

    // create main window
    if ( !wxPGComboControlBase::Create(parent,
                                       id,
                                       value,
                                       wxDefaultPosition,
                                       wxDefaultSize,
                                       style | wxFULL_REPAINT_ON_RESIZE,
                                       wxDefaultValidator,
                                       name) )
        return false;

    // Create textctrl, if necessary
    CreateTextCtrl( wxBORDER_NONE, validator );

    // Add keyboard input handlers for main control and textctrl
    InstallInputHandlers();

    // Set background
    SetBackgroundStyle( wxBG_STYLE_CUSTOM ); // for double-buffering

    // SetSize should be called last
    SetSize(pos.x,pos.y,size.x,size.y);

    return true;
}

wxPGGenericComboControl::~wxPGGenericComboControl()
{
}

void wxPGGenericComboControl::OnResize()
{

    // Recalculates button and textctrl areas
    CalculateAreas(DEFAULT_DROPBUTTON_WIDTH);

#if 0
    // Move separate button control, if any, to correct position
    if ( m_btn )
    {
        wxSize sz = GetClientSize();
        m_btn->SetSize( m_btnArea.x + m_btnSpacingX,
                        (sz.y-m_btnSize.y)/2,
                        m_btnSize.x,
                        m_btnSize.y );
    }
#endif

    // Move textctrl, if any, accordingly
    PositionTextCtrl( TEXTCTRLXADJUST, TEXTCTRLYADJUST );
}

void wxPGGenericComboControl::OnPaintEvent( wxPaintEvent& WXUNUSED(event) )
{
    wxSize sz = GetClientSize();

#if !wxCHECK_VERSION(2, 7, 1)
    // If size is larger, recalculate double buffer bitmap
    if ( !gs_doubleBuffer ||
         sz.x > gs_doubleBuffer->GetWidth() ||
         sz.y > gs_doubleBuffer->GetHeight() )
    {
        delete gs_doubleBuffer;
        gs_doubleBuffer = new wxBitmap(sz.x+25,sz.y);
    }

    wxBufferedPaintDC dc(this,*gs_doubleBuffer);
#else
    wxAutoBufferedPaintDC dc(this);
#endif

    const wxRect& rectb = m_btnArea;
    wxRect rect = m_tcArea;

    // artificial simple border
    if ( m_widthCustomBorder )
    {
        int customBorder = m_widthCustomBorder;

        // Set border colour
        wxPen pen1( wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT),
                    customBorder,
                    wxSOLID );
        dc.SetPen( pen1 );

        // area around both controls
        wxRect rect2(0,0,sz.x,sz.y);
        if ( m_iFlags & wxPGCC_IFLAG_BUTTON_OUTSIDE )
        {
            rect2 = m_tcArea;
            if ( customBorder == 1 )
            {
                rect2.Inflate(1);
            }
            else
            {
            #ifdef __WXGTK__
                rect2.x -= 1;
                rect2.y -= 1;
            #else
                rect2.x -= customBorder;
                rect2.y -= customBorder;
            #endif
                rect2.width += 1 + customBorder;
                rect2.height += 1 + customBorder;
            }
        }

        dc.SetBrush( *wxTRANSPARENT_BRUSH );
        dc.DrawRectangle(rect2);
    }

    wxColour winCol = GetBackgroundColour();
    dc.SetBrush(winCol);
    dc.SetPen(winCol);

    //wxLogDebug(wxT("hei: %i tcy: %i tchei: %i"),GetClientSize().y,m_tcArea.y,m_tcArea.height);
    //wxLogDebug(wxT("btnx: %i tcx: %i tcwid: %i"),m_btnArea.x,m_tcArea.x,m_tcArea.width);

    // clear main background
    dc.DrawRectangle(rect);

    if ( !m_btn )
        // Standard button rendering
        DrawButton(dc, rectb);

    // paint required portion on the control
    if ( !m_text || m_widthCustomPaint )
    {
        wxASSERT( m_widthCustomPaint >= 0 );

        // this is intentionally here to allow drawed rectangle's
        // right edge to be hidden
        if ( m_text )
            rect.width = m_widthCustomPaint;

        dc.SetFont( GetFont() );

        dc.SetClippingRegion(rect);
        m_popupInterface->PaintComboControl(dc, rect);
    }
}

void wxPGGenericComboControl::OnMouseEvent( wxMouseEvent& event )
{
    bool isOnButtonArea = m_btnArea.wxPGRectContains(event.m_x,event.m_y);
    int handlerFlags = isOnButtonArea ? wxPGCC_MF_ON_BUTTON : 0;

    // Preprocessing fabricates double-clicks and prevents
    // (it may also do other common things in future)
    if ( PreprocessMouseEvent(event,handlerFlags) )
        return;

#ifdef __WXMSW__
    const bool ctrlIsButton = true;
#else
    const bool ctrlIsButton = false;
#endif

    if ( ctrlIsButton &&
         (m_windowStyle & (wxPGCC_DCLICK_CYCLES|wxCB_READONLY)) == wxCB_READONLY )
    {
        // if no textctrl and no special double-click, then the entire control acts
        // as a button
        handlerFlags |= wxPGCC_MF_ON_BUTTON;
        if ( HandleButtonMouseEvent(event,handlerFlags) )
            return;
    }
    else
    {
        if ( isOnButtonArea || m_btnState & wxCONTROL_PRESSED )
        {
            if ( HandleButtonMouseEvent(event,handlerFlags) )
                return;
        }
        else if ( m_btnState )
        {
            // otherwise need to clear the hover status
            m_btnState = 0;
            RefreshRect(m_btnArea);
        }
    }

    //
    // This will handle left_down and left_dclick events outside button in a Windows/GTK-like manner.
    // See header file for further information on this method.
    HandleNormalMouseEvent(event);

}

// ----------------------------------------------------------------------------
// wxComboControl
// ----------------------------------------------------------------------------

#if defined(__WXMSW__) && !defined(__WXUNIVERSAL__)

// Change to #if 1 to include tmschema.h for easier testing of theme
// parameters.
#if 0
    #include <tmschema.h>
    #include <VSStyle.h>
#else
    //----------------------------------
    #define EP_EDITTEXT         1
    #define ETS_NORMAL          1
    #define ETS_HOT             2
    #define ETS_SELECTED        3
    #define ETS_DISABLED        4
    #define ETS_FOCUSED         5
    #define ETS_READONLY        6
    #define ETS_ASSIST          7
    #define TMT_FILLCOLOR       3802
    #define TMT_TEXTCOLOR       3803
    #define TMT_BORDERCOLOR     3801
    #define TMT_EDGEFILLCOLOR   3808
    #define TMT_BGTYPE          4001

    #define BT_IMAGEFILE        0
    #define BT_BORDERFILL       1

    #define CP_DROPDOWNBUTTON           1
    #define CP_BACKGROUND               2 // This and above are Vista and later only
    #define CP_TRANSPARENTBACKGROUND    3
    #define CP_BORDER                   4
    #define CP_READONLY                 5
    #define CP_DROPDOWNBUTTONRIGHT      6
    #define CP_DROPDOWNBUTTONLEFT       7
    #define CP_CUEBANNER                8

    #define CBXS_NORMAL                 1
    #define CBXS_HOT                    2
    #define CBXS_PRESSED                3
    #define CBXS_DISABLED               4

    #define CBXSR_NORMAL                1
    #define CBXSR_HOT                   2
    #define CBXSR_PRESSED               3
    #define CBXSR_DISABLED              4

    #define CBXSL_NORMAL                1
    #define CBXSL_HOT                   2
    #define CBXSL_PRESSED               3
    #define CBXSL_DISABLED              4

    #define CBTBS_NORMAL                1
    #define CBTBS_HOT                   2
    #define CBTBS_DISABLED              3
    #define CBTBS_FOCUSED               4

    #define CBB_NORMAL                  1
    #define CBB_HOT                     2
    #define CBB_FOCUSED                 3
    #define CBB_DISABLED                4

    #define CBRO_NORMAL                 1
    #define CBRO_HOT                    2
    #define CBRO_PRESSED                3
    #define CBRO_DISABLED               4

    #define CBCB_NORMAL                 1
    #define CBCB_HOT                    2
    #define CBCB_PRESSED                3
    #define CBCB_DISABLED               4

#endif

#define NATIVE_TEXT_INDENT_XP       4
#define NATIVE_TEXT_INDENT_CLASSIC  2

#define TEXTCTRLXADJUST_XP          0
#define TEXTCTRLYADJUST_XP          4
#define TEXTCTRLXADJUST_CLASSIC     0
#define TEXTCTRLYADJUST_CLASSIC     4


BEGIN_EVENT_TABLE(wxPGComboControl, wxPGComboControlBase)
    EVT_PAINT(wxPGComboControl::OnPaintEvent)
    EVT_MOUSE_EVENTS(wxPGComboControl::OnMouseEvent)
END_EVENT_TABLE()


IMPLEMENT_DYNAMIC_CLASS(wxPGComboControl, wxPGComboControlBase)

void wxPGComboControl::Init()
{
}

bool wxPGComboControl::Create(wxWindow *parent,
                                   wxWindowID id,
                                   const wxString& value,
                                   const wxPoint& pos,
                                   const wxSize& size,
                                   long style,
                                   const wxValidator& validator,
                                   const wxString& name)
{

    // Set border
    long border = style & wxBORDER_MASK;

    wxUxThemeEngine* theme = wxUxThemeEngine::GetIfActive();

    if ( !border )
    {
        // For XP, have 1-width custom border, for older version use sunken
        if ( theme )
        {
            border = wxBORDER_NONE;
            m_widthCustomBorder = 1;
        }
        else
            border = wxBORDER_SUNKEN;

        style = (style & ~(wxBORDER_MASK)) | border;
    }

    //Customize( wxPGCC_BUTTON_OUTSIDE_BORDER );

    // create main window
    if ( !wxPGComboControlBase::Create(parent,
                            id,
                            value,
                            wxDefaultPosition,
                            wxDefaultSize,
                            style | wxFULL_REPAINT_ON_RESIZE,
                            wxDefaultValidator,
                            name) )
        return false;

    if ( theme )
    {
#if wxCHECK_VERSION(2, 8, 0)
        const bool isVista = (::wxGetWinVersion() >= wxWinVersion_6);
#else
        int Major = 0;
        int family = wxGetOsVersion(&Major, NULL);
        const bool isVista = ((family == wxWINDOWS_NT) && (Major >= 6));
#endif

        if ( isVista )
            m_iFlags |= wxPGCC_BUTTON_STAYS_DOWN;
    }

    // Create textctrl, if necessary
    CreateTextCtrl( wxNO_BORDER, validator );

    // Add keyboard input handlers for main control and textctrl
    InstallInputHandlers();

    // Prepare background for double-buffering
    SetBackgroundStyle( wxBG_STYLE_CUSTOM );

    // SetSize should be called last
    SetSize(pos.x,pos.y,size.x,size.y);

    return true;
}

wxPGComboControl::~wxPGComboControl()
{
}

void wxPGComboControl::OnThemeChange()
{
    wxUxThemeEngine* theme = wxUxThemeEngine::GetIfActive();
    if ( theme )
    {
        wxUxThemeHandle hTheme(this, L"COMBOBOX");

        COLORREF col;
        theme->GetThemeColor(hTheme,EP_EDITTEXT,ETS_NORMAL,TMT_FILLCOLOR,&col);
        SetBackgroundColour(wxRGBToColour(col));
        theme->GetThemeColor(hTheme,EP_EDITTEXT,ETS_NORMAL,TMT_TEXTCOLOR,&col);
        SetForegroundColour(wxRGBToColour(col));
    }
    else
    {
        SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
        SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
    }
}

//void wxPGComboControl::OnSizeEvent( wxSizeEvent& event )
void wxPGComboControl::OnResize()
{
    //
    // Recalculates button and textctrl areas

    int textCtrlXAdjust;
    int textCtrlYAdjust;

    if ( wxUxThemeEngine::GetIfActive() )
    {
        textCtrlXAdjust = TEXTCTRLXADJUST_XP;
        textCtrlYAdjust = TEXTCTRLYADJUST_XP;
    }
    else
    {
        textCtrlXAdjust = TEXTCTRLXADJUST_CLASSIC;
        textCtrlYAdjust = TEXTCTRLYADJUST_CLASSIC;
    }

    // Technically Classic Windows style combo has more narrow button,
    // but the native renderer doesn't paint it well like that.
    int btnWidth = 17;
    CalculateAreas(btnWidth);

    // Position textctrl using standard routine
    PositionTextCtrl(textCtrlXAdjust,textCtrlYAdjust);
}

/*
// Draws non-XP GUI dotted line around the focus area
static void wxMSWDrawFocusRect( wxDC& dc, const wxRect& rect )
{
#if !defined(__WXWINCE__)
    dc.SetLogicalFunction(wxINVERT);

    wxPen pen(*wxBLACK,1,wxDOT);
    pen.SetCap(wxCAP_BUTT);
    dc.SetPen(pen);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);

    dc.DrawRectangle(rect);

    dc.SetLogicalFunction(wxCOPY);
#else
    dc.SetLogicalFunction(wxINVERT);

    dc.SetPen(wxPen(*wxBLACK,1,wxDOT));
    dc.SetBrush(*wxTRANSPARENT_BRUSH);

    dc.DrawRectangle(rect);

    dc.SetLogicalFunction(wxCOPY);
#endif
}
*/
/*
// draw focus background on area in a way typical on platform
void wxPGComboControl::DrawFocusBackground( wxDC& dc, const wxRect& rect, int flags )
{
    wxUxThemeEngine* theme = (wxUxThemeEngine*) NULL;
    wxUxThemeHandle hTheme(this, L"COMBOBOX");
    //COLORREF cref;

    wxSize sz = GetClientSize();
    bool isEnabled;
    bool isFocused; // also selected

    // For smaller size control (and for disabled background) use less spacing
    int focusSpacingX;
    int focusSpacingY;

    if ( !(flags & wxCONTROL_ISSUBMENU) )
    {
        // Drawing control
        isEnabled = IsEnabled();
        isFocused = ShouldDrawFocus();

        // Windows-style: for smaller size control (and for disabled background) use less spacing
        if ( hTheme )
        {
            // WinXP  Theme
            focusSpacingX = isEnabled ? 2 : 1;
            focusSpacingY = sz.y > (GetCharHeight()+2) && isEnabled ? 2 : 1;
        }
        else
        {
            // Classic Theme
            if ( isEnabled )
            {
                focusSpacingX = 1;
                focusSpacingY = 1;
            }
            else
            {
                focusSpacingX = 0;
                focusSpacingY = 0;
            }
        }
    }
    else
    {
        // Drawing a list item
        isEnabled = true; // they are never disabled
        isFocused = flags & wxCONTROL_SELECTED ? true : false;

        focusSpacingX = 0;
        focusSpacingY = 0;
    }

    // Set the background sub-rectangle for selection, disabled etc
    wxRect selRect(rect);
    selRect.y += focusSpacingY;
    selRect.height -= (focusSpacingY*2);
    int wcp = 0;

    if ( !(flags & wxCONTROL_ISSUBMENU) )
        wcp += m_widthCustomPaint;

    selRect.x += wcp + focusSpacingX;
    selRect.width -= wcp + (focusSpacingX*2);

    if ( hTheme )
        theme = wxUxThemeEngine::GetIfActive();

    wxColour bgCol;
    bool drawDottedEdge = false;

    if ( isEnabled )
    {
        // If popup is hidden and this control is focused,
        // then draw the focus-indicator (selbgcolor background etc.).
        if ( isFocused )
        {
        #if 0
            // TODO: Proper theme color getting (JMS: I don't know which parts/colors to use,
            //       those below don't work)
            if ( hTheme )
            {
                theme->GetThemeColor(hTheme,EP_EDITTEXT,ETS_SELECTED,TMT_TEXTCOLOR,&cref);
                dc.SetTextForeground( wxRGBToColour(cref) );
                theme->GetThemeColor(hTheme,EP_EDITTEXT,ETS_SELECTED,TMT_FILLCOLOR,&cref);
                bgCol = wxRGBToColour(cref);
            }
            else
        #endif
            {
                dc.SetTextForeground( wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT) );
                bgCol = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
                if ( m_windowStyle & wxCB_READONLY )
                    drawDottedEdge = true;
            }
        }
        else
        {
            bgCol = GetBackgroundColour();
        }
    }
    else
    {
        dc.SetTextForeground( wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT) );
        bgCol = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
    }

    dc.SetBrush(bgCol);
    dc.SetPen(bgCol);
    dc.DrawRectangle(selRect);
    //if ( drawDottedEdge )
    //    wxMSWDrawFocusRect(dc,selRect);
}
*/
void wxPGComboControl::OnPaintEvent( wxPaintEvent& WXUNUSED(event) )
{
    // TODO: Convert drawing in this function to Windows API Code

    wxSize sz = GetClientSize();

#if !wxCHECK_VERSION(2, 7, 1)
    // If size is larger, recalculate double buffer bitmap
    if ( !gs_doubleBuffer ||
         sz.x > gs_doubleBuffer->GetWidth() ||
         sz.y > gs_doubleBuffer->GetHeight() )
    {
        delete gs_doubleBuffer;
        gs_doubleBuffer = new wxBitmap(sz.x+25,sz.y);
    }

    wxBufferedPaintDC dc(this,*gs_doubleBuffer);
#else
    wxAutoBufferedPaintDC dc(this);
#endif

    const wxRect& rectButton = m_btnArea;
    wxRect rectTextField = m_tcArea;
    const bool isEnabled = IsEnabled();
    wxColour bgCol = GetBackgroundColour();

    HDC hDc = wxPG_GetHDCOf(dc);
    HWND hWnd = GetHwndOf(this);

    wxUxThemeEngine* theme = NULL;
    wxUxThemeHandle hTheme(this, L"COMBOBOX");

    if ( hTheme )
        theme = wxUxThemeEngine::GetIfActive();

    wxRect borderRect(0,0,sz.x,sz.y);

    if ( m_iFlags & wxPGCC_IFLAG_BUTTON_OUTSIDE )
    {
        borderRect = m_tcArea;
        borderRect.Inflate(1);
    }

    int drawButFlags = 0;

    if ( hTheme )
    {
#if wxCHECK_VERSION(2, 8, 0)
        const bool useVistaComboBox = (::wxGetWinVersion() >= wxWinVersion_6);
#else
        int Major = 0;
        int family = wxGetOsVersion(&Major, NULL);
        const bool useVistaComboBox = ((family == wxWINDOWS_NT) && (Major >= 6));
#endif

        RECT rFull;
        wxCopyRectToRECT(borderRect, rFull);

        RECT rButton;
        wxCopyRectToRECT(rectButton, rButton);

        RECT rBorder;
        wxCopyRectToRECT(borderRect, rBorder);

        bool isNonStdButton = (m_iFlags & wxPGCC_IFLAG_BUTTON_OUTSIDE) ||
                              (m_iFlags & wxPGCC_IFLAG_HAS_NONSTANDARD_BUTTON);

        //
        // Get some states for themed drawing
        int butState;

        if ( !isEnabled )
        {
            butState = CBXS_DISABLED;
        }
        // Vista will display the drop-button as depressed always
        // when the popup window is visilbe
        else if ( (m_btnState & wxCONTROL_PRESSED) ||
                  (useVistaComboBox && IsPopupShown()) )
        {
            butState = CBXS_PRESSED;
        }
        else if ( m_btnState & wxCONTROL_CURRENT )
        {
            butState = CBXS_HOT;
        }
        else
        {
            butState = CBXS_NORMAL;
        }

        int comboBoxPart = 0;  // For XP, use the 'default' part
        RECT* rUseForBg = &rBorder;

        bool drawFullButton = false;
        int bgState = butState;
        const bool isFocused = IsFocused();

        if ( useVistaComboBox )
        {
            // FIXME: Either SetBackgroundColour or GetBackgroundColour
            //        doesn't work under Vista, so here's a temporary
            //        workaround.
            bgCol = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);

            // Draw the entire control as a single button?
            /*
            if ( !isNonStdButton )
            {
                if ( HasFlag(wxCB_READONLY) )
                    drawFullButton = true;
            }
            */

            if ( drawFullButton )
            {
                comboBoxPart = CP_READONLY;
                rUseForBg = &rFull;

                // It should be safe enough to update this flag here.
                m_iFlags |= wxPGCC_FULL_BUTTON;
            }
            else
            {
                comboBoxPart = CP_BORDER;
                m_iFlags &= ~wxPGCC_FULL_BUTTON;

                if ( isFocused )
                    bgState = CBB_FOCUSED;
                else
                    bgState = CBB_NORMAL;
            }
        }

        //
        // Draw parent's background, if necessary
        RECT* rUseForTb = NULL;

        if ( theme->IsThemeBackgroundPartiallyTransparent( hTheme, comboBoxPart, bgState ) )
            rUseForTb = &rFull;
        else if ( m_iFlags & wxPGCC_IFLAG_BUTTON_OUTSIDE )
            rUseForTb = &rButton;

        if ( rUseForTb )
            theme->DrawThemeParentBackground( hWnd, hDc, rUseForTb );

        //
        // Draw the control background (including the border)
        if ( m_widthCustomBorder > 0 )
        {
            theme->DrawThemeBackground( hTheme, hDc, comboBoxPart, bgState, rUseForBg, NULL );
        }
        else
        {
            // No border. We can't use theme, since it cannot be relied on
            // to deliver borderless drawing, even with DrawThemeBackgroundEx.
            dc.SetBrush(bgCol);
            dc.SetPen(bgCol);
            dc.DrawRectangle(borderRect);
        }

        //
        // Draw the drop-button
        if ( !isNonStdButton )
        {
            drawButFlags = Button_BitmapOnly;

            int butPart = CP_DROPDOWNBUTTON;

            if ( useVistaComboBox && m_widthCustomBorder > 0 )
            {
                if ( drawFullButton )
                {
                    // We need to alter the button style slightly before
                    // drawing the actual button (but it was good above
                    // when background etc was done).
                    if ( butState == CBXS_HOT || butState == CBXS_PRESSED )
                        butState = CBXS_NORMAL;
                }

                if ( m_btnSide == wxRIGHT )
                    butPart = CP_DROPDOWNBUTTONRIGHT;
                else
                    butPart = CP_DROPDOWNBUTTONLEFT;

            }
            theme->DrawThemeBackground( hTheme, hDc, butPart, butState, &rButton, NULL );
        }
        else if ( useVistaComboBox &&
                  (m_iFlags & wxPGCC_IFLAG_BUTTON_OUTSIDE) )
        {
            // We'll do this, because DrawThemeParentBackground
            // doesn't seem to be reliable on Vista.
            drawButFlags |= Button_PaintBackground;
        }
    }
    else
    {
        // Windows 2000 and earlier
        drawButFlags = Button_PaintBackground;

        dc.SetBrush(bgCol);
        dc.SetPen(bgCol);
        dc.DrawRectangle(borderRect);
    }

    // Button rendering (may only do the bitmap on button, depending on the flags)
    DrawButton( dc, rectButton, drawButFlags );

    // Paint required portion of the custom image on the control
    if ( (!m_text || m_widthCustomPaint) )
    {
        wxASSERT( m_widthCustomPaint >= 0 );

        // this is intentionally here to allow drawed rectangle's
        // right edge to be hidden
        if ( m_text )
            rectTextField.width = m_widthCustomPaint;

        dc.SetFont( GetFont() );

        dc.SetClippingRegion(rectTextField);
        m_popupInterface->PaintComboControl(dc,rectTextField);
    }
}

void wxPGComboControl::OnMouseEvent( wxMouseEvent& event )
{
    bool isOnButtonArea = m_btnArea.wxPGRectContains(event.m_x,event.m_y);
    int handlerFlags = isOnButtonArea ? wxPGCC_MF_ON_BUTTON : 0;

    // Preprocessing fabricates double-clicks and prevents
    // (it may also do other common things in future)
    if ( PreprocessMouseEvent(event,isOnButtonArea) )
        return;

    if ( (m_windowStyle & (wxPGCC_DCLICK_CYCLES|wxCB_READONLY)) == wxCB_READONLY )
    {
        // if no textctrl and no special double-click, then the entire control acts
        // as a button
        handlerFlags |= wxPGCC_MF_ON_BUTTON;
        if ( HandleButtonMouseEvent(event,handlerFlags) )
            return;
    }
    else
    {
        if ( isOnButtonArea || m_btnState & wxCONTROL_PRESSED )
        {
            if ( HandleButtonMouseEvent(event,handlerFlags) )
                return;
        }
        else if ( m_btnState )
        {
            // otherwise need to clear the hover status
            m_btnState = 0;
            RefreshRect(m_btnArea);
        }
    }

    //
    // This will handle left_down and left_dclick events outside button in a Windows-like manner.
    // See header file for further information on this method.
    HandleNormalMouseEvent(event);

}

wxCoord wxPGComboControl::GetNativeTextIndent() const
{
    if ( wxUxThemeEngine::GetIfActive() )
        return NATIVE_TEXT_INDENT_XP;
    return NATIVE_TEXT_INDENT_CLASSIC;
}

#else

IMPLEMENT_DYNAMIC_CLASS(wxPGComboControl, wxPGComboControlBase)

#endif // #if defined(__WXMSW__) && !defined(__WXUNIVERSAL__)

// ----------------------------------------------------------------------------
// wxPGOwnerDrawnComboBox
// ----------------------------------------------------------------------------


IMPLEMENT_DYNAMIC_CLASS(wxPGOwnerDrawnComboBox, wxPGComboControl)


BEGIN_EVENT_TABLE(wxPGOwnerDrawnComboBox, wxPGComboControl)
END_EVENT_TABLE()


void wxPGOwnerDrawnComboBox::Init()
{
}

bool wxPGOwnerDrawnComboBox::Create(wxWindow *parent,
                            wxWindowID id,
                            const wxString& value,
                            const wxPoint& pos,
                            const wxSize& size,
                            long style,
                            const wxValidator& validator,
                            const wxString& name)
{
    return wxPGComboControl::Create(parent,id,value,pos,size,style,validator,name);
}

wxPGOwnerDrawnComboBox::wxPGOwnerDrawnComboBox(wxWindow *parent,
                       wxWindowID id,
                       const wxString& value,
                       const wxPoint& pos,
                       const wxSize& size,
                       const wxArrayString& choices,
                       long style,
                       const wxValidator& validator,
                       const wxString& name)
    : wxPGComboControl()
{
    Init();

    Create(parent,id,value,pos,size,choices,style, validator, name);
}

bool wxPGOwnerDrawnComboBox::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxString& value,
                        const wxPoint& pos,
                        const wxSize& size,
                        const wxArrayString& choices,
                        long style,
                        const wxValidator& validator,
                        const wxString& name)
{
    wxCArrayString chs(choices);

    return Create(parent, id, value, pos, size, chs.GetCount(),
                  chs.GetStrings(), /*callback,*/ style, validator, name);
}

bool wxPGOwnerDrawnComboBox::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxString& value,
                        const wxPoint& pos,
                        const wxSize& size,
                        int n,
                        const wxString choices[],
                        long style,
                        const wxValidator& validator,
                        const wxString& name)
{

    if ( !Create(parent, id, value, pos, size, style,
                 validator, name) )
    {
        return false;
    }

    wxPGVListBoxComboPopup* iface = new wxPGVListBoxComboPopup(this);
    SetPopup(iface);
    m_popupInterface = iface;

    // Add initial choices to the interface
    iface->Populate(n,choices);

    return true;
}

wxPGOwnerDrawnComboBox::~wxPGOwnerDrawnComboBox()
{
    if ( m_popupInterface )
        m_popupInterface->ClearClientDatas();
}

// ----------------------------------------------------------------------------
// wxPGOwnerDrawnComboBox item manipulation methods
// ----------------------------------------------------------------------------

void wxPGOwnerDrawnComboBox::Clear()
{
    wxASSERT( m_popupInterface );

    m_popupInterface->Clear();

    GetTextCtrl()->SetValue(wxEmptyString);
}

void wxPGOwnerDrawnComboBox::Delete(wxODCIndex n)
{
    wxCHECK_RET( (n >= 0) && (n < GetCount()), _T("invalid index in wxPGOwnerDrawnComboBox::Delete") );

    if ( GetSelection() == (int) n )
        SetValue(wxEmptyString);

    m_popupInterface->Delete(n);
}

wxODCCount wxPGOwnerDrawnComboBox::GetCount() const
{
    wxASSERT( m_popupInterface );
    return m_popupInterface->GetCount();
}

wxString wxPGOwnerDrawnComboBox::GetString(wxODCIndex n) const
{
    wxCHECK_MSG( (n >= 0) && (n < GetCount()), wxEmptyString, _T("invalid index in wxPGOwnerDrawnComboBox::GetString") );
    return m_popupInterface->GetString(n);
}

void wxPGOwnerDrawnComboBox::SetString(wxODCIndex n, const wxString& s)
{
    wxCHECK_RET( (n >= 0) && (n < GetCount()), _T("invalid index in wxPGOwnerDrawnComboBox::SetString") );
    m_popupInterface->SetString(n,s);
}

int wxPGOwnerDrawnComboBox::FindString(const wxString& s) const
{
    wxASSERT( m_popupInterface );
    return m_popupInterface->FindString(s);
}

void wxPGOwnerDrawnComboBox::Select(int n)
{
    wxCHECK_RET( (n >= -1) && (n < (int)GetCount()), _T("invalid index in wxPGOwnerDrawnComboBox::Select") );
    wxASSERT( m_popupInterface );

    m_popupInterface->SetSelection(n);

    wxString str;
    if ( n >= 0 )
        str = m_popupInterface->GetString(n);

    // Refresh text portion in control
    if ( m_text )
        m_text->SetValue( str );
    else
        m_valueString = str;

    Refresh();
}

int wxPGOwnerDrawnComboBox::GetSelection() const
{
    wxASSERT( m_popupInterface );
    return m_popupInterface->GetSelection();
}

int wxPGOwnerDrawnComboBox::DoAppend(const wxString& item)
{
    wxASSERT( m_popupInterface );
    return m_popupInterface->Append(item);
}

int wxPGOwnerDrawnComboBox::DoInsert(const wxString& item, wxODCIndex pos)
{
    wxCHECK_MSG(!(GetWindowStyle() & wxCB_SORT), -1, wxT("can't insert into sorted list"));
    wxCHECK_MSG((pos>=0) && (pos<=GetCount()), -1, wxT("invalid index"));

    m_popupInterface->Insert(item,pos);

    return pos;
}

#if wxCHECK_VERSION(2,9,0)
int wxPGOwnerDrawnComboBox::DoInsertItems(const wxArrayStringsAdapter& items,
                                          unsigned int pos,
                                          void **clientData,
                                          wxClientDataType type)
{
    const unsigned int count = items.GetCount();

    if ( HasFlag(wxCB_SORT) )
    {
        int n = pos;

        for( unsigned int i = 0; i < count; ++i )
        {
            int n = GetVListBoxComboPopup()->Append(items[i]);
            AssignNewItemClientData(n, clientData, i, type);
        }

        return n;
    }
    else
    {
        for( unsigned int i = 0; i < count; ++i, ++pos )
        {
            GetVListBoxComboPopup()->Insert(items[i], pos);
            AssignNewItemClientData(pos, clientData, i, type);
        }

        return pos - 1;
    }
}
#endif

void wxPGOwnerDrawnComboBox::DoSetItemClientData(wxODCIndex n, void* clientData)
{
    wxASSERT(m_popupInterface);
    m_popupInterface->SetItemClientData(n,clientData,
#if wxCHECK_VERSION(2,9,0)
        GetClientDataType()
#else
        m_clientDataItemsType
#endif
        );
}

void* wxPGOwnerDrawnComboBox::DoGetItemClientData(wxODCIndex n) const
{
    wxASSERT(m_popupInterface);
    return m_popupInterface->GetItemClientData(n);
}

void wxPGOwnerDrawnComboBox::DoSetItemClientObject(wxODCIndex n, wxClientData* clientData)
{
    DoSetItemClientData(n, (void*) clientData);
}

wxClientData* wxPGOwnerDrawnComboBox::DoGetItemClientObject(wxODCIndex n) const
{
    return (wxClientData*) DoGetItemClientData(n);
}

#endif // wxPG_USING_WXOWNERDRAWNCOMBOBOX

#endif // wxUSE_COMBOBOX

