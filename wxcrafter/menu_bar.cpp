#include "menu_bar.h"
#include "allocator_mgr.h"
#include <event_notifier.h>
#include <wx/bitmap.h>
#include <wx/dcbuffer.h>
#include <wx/dcmemory.h>
#include <wx/msgdlg.h>
#include <wx/settings.h>

#define MARGIN_X 5
#define MARGIN_Y 5

const wxEventType wxEVT_PREVIEW_BAR_SELECTED = wxNewEventType();

BEGIN_EVENT_TABLE(MenuBar, wxPanel)
EVT_PAINT(MenuBar::OnPaint)
EVT_ERASE_BACKGROUND(MenuBar::OnEraseBG)
EVT_LEFT_DOWN(MenuBar::OnLeftDown)
EVT_RIGHT_DOWN(MenuBar::OnLeftDown)
END_EVENT_TABLE()

MenuBar::MenuBar(wxWindow* parent, wxMenuBar* mb)
    : wxPanel(parent)
    , m_mb(mb)
    , m_controlHeight(30)
{
    for(size_t i = 0; i < m_mb->GetMenuCount(); i++) {
        MenuInfo mi;
        mi.label = m_mb->GetMenuLabelText(i);
        mi.menu = m_mb->GetMenu(i);
        m_menus.push_back(mi);
    }

    // Remove the menus
    size_t nCount = m_mb->GetMenuCount();
    for(size_t i = 0; i < nCount; ++i) {
        m_mb->Remove(0);
    }
    m_mbname = wxT("MENU_BAR_ID");

#ifndef __WXGTK__
    // The parent (hidden frame) will destroy it for us)
    m_mb->Destroy();
#endif

    int width;
    // Initialize a temporary memory dc and calculate the window height
    wxBitmap bmp(30, 30);
    wxMemoryDC memDc(bmp);

    wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    memDc.GetTextExtent(wxT("Tp"), &width, &m_controlHeight, NULL, NULL, &font);

    m_controlHeight += 2 * MARGIN_Y;
    SetSizeHints(-1, m_controlHeight);
}

MenuBar::~MenuBar()
{
    for(size_t i = 0; i < m_menus.size(); i++) {
        wxDELETE(m_menus.at(i).menu);
    }
    m_menus.clear();
}

void MenuBar::OnEraseBG(wxEraseEvent& e) { wxUnusedVar(e); }

void MenuBar::OnLeftDown(wxMouseEvent& e)
{
    size_t where = wxString::npos;
    wxPoint pt = e.GetPosition();
    for(size_t i = 0; i < m_menus.size(); i++) {
        if(m_menus.at(i).rect.Contains(pt)) {
            where = i;
            break;
        }
    }

    if(where == wxString::npos) {
        wxCommandEvent evt(wxEVT_PREVIEW_BAR_SELECTED);
        evt.SetString(wxT(""));
        EventNotifier::Get()->AddPendingEvent(evt);
        return;
    }

    wxString name;
    if(m_menus.at(where).menu) {
        wxMenu* menu = m_menus.at(where).menu;

        wxCommandEvent evt(wxEVT_PREVIEW_BAR_SELECTED);
        wxString s;
        s << m_mbname << wxT(":") << m_menus.at(where).label;
        evt.SetString(s);
        evt.SetInt(ID_WXMENUBAR);
        EventNotifier::Get()->AddPendingEvent(evt);
        PopupMenu(menu, m_menus.at(where).rect.GetBottomLeft());
    }
}

void MenuBar::OnPaint(wxPaintEvent& e)
{
    int width, height;
    wxBufferedPaintDC dc(this);
    wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);

    wxColour bgColor = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);

    // Color the background
    dc.SetPen(bgColor);
    dc.SetBrush(bgColor);
    dc.DrawRectangle(GetClientRect());

    int posx = 0;
    int posy = MARGIN_Y;

    size_t count = m_menus.size();
    for(size_t i = 0; i < count; i++) {
        wxString menuTitle = m_menus.at(i).label;
        dc.GetTextExtent(menuTitle, &width, &height, NULL, NULL, &font);

        wxRect labelRect(posx, 0, MARGIN_X + width + MARGIN_X, m_controlHeight);
        dc.SetFont(font);
        dc.DrawText(menuTitle, labelRect.GetX() + MARGIN_X, posy);
        posx += labelRect.GetWidth();
        m_menus.at(i).rect = labelRect;
    }
}
