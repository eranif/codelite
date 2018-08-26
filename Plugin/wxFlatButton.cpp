#include "wxFlatButton.h"
#include <wx/dcbuffer.h>
#include <wx/dcmemory.h>
#include <wx/msgdlg.h>
#include "drawingutils.h"

//++++++++---------------------------------
// wxFlatButtonEvent
//++++++++---------------------------------

static wxBitmap ConvertToDisabled(const wxBitmap& bmp)
{
    return DrawingUtils::CreateDisabledBitmap(bmp);
}

wxDEFINE_EVENT(wxEVT_CMD_FLATBUTTON_CLICK, wxFlatButtonEvent);
wxDEFINE_EVENT(wxEVT_CMD_FLATBUTTON_MENU_SHOWING, wxFlatButtonEvent);

wxFlatButtonEvent::wxFlatButtonEvent(wxEventType commandType, int winid)
    : wxCommandEvent(commandType, winid)
    , m_menu(NULL)
{
}

wxFlatButtonEvent& wxFlatButtonEvent::operator=(const wxFlatButtonEvent& src)
{
    // Copy wxCommandEvent members here
    m_eventType = src.m_eventType;
    m_id = src.m_id;
    m_cmdString = src.m_cmdString;
    m_commandInt = src.m_commandInt;
    m_extraLong = src.m_extraLong;
    // this members
    m_menu = src.m_menu;
    return *this;
}

//++++++++---------------------------------
// wxFlatButton
//++++++++---------------------------------
#define X_SPACER 5
#define Y_SPACER 3
#define BTN_RADIUS 2

wxFlatButton::wxFlatButton(wxWindow* parent,
    const wxString& label,
    const wxFlatButton::eTheme theme,
    const wxBitmap& bmp,
    const wxSize& size,
    int style)
    : wxFlatButtonBase(parent)
    , m_theme(theme)
    , m_state(kStateNormal)
    , m_text(label)
    , m_bmp(bmp)
    , m_accelIndex(wxNOT_FOUND)
    , m_kind(kKindNormal)
    , m_isChecked(false)
    , m_contextMenu(NULL)
    , m_isDisabled(false)
    , m_style(style)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    // Parse the label
    wxString tmpLabel;
    m_text.Replace("&&", "@@");
    // Parse the label
    for(size_t i = 0; i < m_text.length(); ++i) {
        if(m_accelIndex == wxNOT_FOUND && m_text.GetChar(i) == '&') {
            m_accelIndex = i;
            continue;
        } else {
            tmpLabel << m_text.GetChar(i);
        }
    }

    tmpLabel.Replace("@@", "&");
    m_text.swap(tmpLabel);

    // Colours - dark theme
    if(m_theme == kThemeDark) {
        SetPenNormalColour("rgb(48, 48, 48)");
        SetPenPressedColour("rgb(125, 125, 125)");
        SetBgPressedColour("rgb(48, 48, 48)");
        SetBgHoverColour("rgb(80, 80, 80)");
        SetBgColour("rgb(65, 65, 65)");
        m_penHoverColourInner = "rgb(160, 160, 160)";
        m_penHoverOuterColour = GetPenNormalColour();
        SetTextColour("rgb(248, 248, 242)");
        SetTextColourDisabled("rgb(109, 109, 109)");
        if(m_bmp.IsOk()) {
            m_bmpDisabled = ConvertToDisabled(m_bmp);
        }

    } else {
        wxColour paneColour = DrawingUtils::GetButtonBgColour();
        wxColour bgColour = paneColour;
        wxColour penColour = paneColour.ChangeLightness(90);

        SetPenNormalColour(penColour);
        SetBgColour(bgColour);

        SetPenPressedColour("rgb(90, 90, 90)");
        SetBgPressedColour("rgb(120, 120, 120)");

        SetBgHoverColour(bgColour);
        m_penHoverColourInner = "WHITE";
        m_penHoverOuterColour = "TURQUOISE";
        SetTextColour("rgb(15, 15, 15)");
        SetTextColourDisabled(wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT));
        if(m_bmp.IsOk()) {
            m_bmpDisabled = ConvertToDisabled(m_bmp);
        }
    }

    SetTextFont(DrawingUtils::GetDefaultGuiFont());
    if(size != wxDefaultSize) {
        SetMinSize(size);
    } else {
        SetMinSize(GetBestSize());
    }
}

wxFlatButton::~wxFlatButton() { wxDELETE(m_contextMenu); }

void wxFlatButton::OnEnterWindow(wxMouseEvent& event)
{
    if(!IsEnabled()) return;
    if(!m_isChecked) {
        m_state = kStateHover;
        Refresh();
    }
}

void wxFlatButton::OnKeyDown(wxKeyEvent& event) { event.Skip(); }

void wxFlatButton::OnLeaveWindow(wxMouseEvent& event)
{
    if(!IsEnabled()) return;

    if(!m_isChecked) {
        m_state = kStateNormal;
        Refresh();
    }
}

void wxFlatButton::OnLeftUp(wxMouseEvent& event) { DoActivate(); }

void wxFlatButton::DoActivate()
{
    if(!IsEnabled()) return;

    wxFlatButtonEvent btnEvent(wxEVT_CMD_FLATBUTTON_CLICK);
    btnEvent.SetEventObject(this);

    if(m_kind == kKindChek) {
        if(HasMenu()) {
            if(!IsChecked()) {
                m_state = kStatePressed;
                m_isChecked = true;
                Refresh();
                CallAfter(&wxFlatButton::DoShowContextMenu);

            } else {
                m_state = kStateNormal;
                m_isChecked = false;
                Refresh();
            }

            // We don't fire an event
            return;
        } else {
            if(IsChecked()) {
                // a checked button is about to be unchecked
                btnEvent.SetInt(0);
                m_state = kStateNormal;
                m_isChecked = false;
            } else {
                // Checking the button
                btnEvent.SetInt(1);
                m_isChecked = true;
                m_state = kStatePressed;
            }
        }
    } else {
        m_state = kStateHover;
    }
    GetEventHandler()->AddPendingEvent(btnEvent);
    Refresh();
}

void wxFlatButton::OnEraseBackground(wxEraseEvent& event) { wxUnusedVar(event); }

void wxFlatButton::OnPaint(wxPaintEvent& event)
{
    wxUnusedVar(event);
    wxAutoBufferedPaintDC paintDC(this);
    PrepareDC(paintDC);

    wxGCDC gdc;
    GetGCDC(paintDC, gdc);

    wxRect clientRect = GetClientRect();
    // Draw the background
    gdc.SetPen(GetBarBgColour(m_theme));
    gdc.SetBrush(GetBarBgColour(m_theme));
    gdc.DrawRectangle(clientRect);

    switch(m_state) {
    case kStateHover: {
        // do nothing
        gdc.SetBrush(GetBgColour());
        gdc.SetPen(GetPenNormalColour());
        gdc.DrawRoundedRectangle(clientRect, BTN_RADIUS);
        break;
    }
    case kStateNormal: {
        break;
    }
    case kStatePressed: {
        // Pressed
        gdc.SetBrush(GetBgPressedColour());
        gdc.SetPen(GetPenPressedColour());
        gdc.DrawRoundedRectangle(clientRect, BTN_RADIUS);
        break;
    }
    }

    // Draw text
    gdc.SetFont(GetTextFont());
    wxColour textColour = IsEnabled() ? GetTextColour() : GetTextColourDisabled();
    wxBitmap bmp = IsEnabled() ? m_bmp : m_bmpDisabled;

    wxCoord textY;
    wxCoord bmpY;

    wxCoord totalLen = 0;
    const int spacer = 2;
    if(bmp.IsOk()) {
        // we got a bitmap
        totalLen += bmp.GetScaledWidth();
    }

    wxSize textSize;
    if(!m_text.IsEmpty()) {
        textSize = gdc.GetTextExtent(m_text);
        totalLen += spacer;
        totalLen += textSize.x;
    }

    wxCoord offset = (clientRect.GetWidth() - totalLen) / 2;
    bmpY = (clientRect.GetHeight() - bmp.GetScaledHeight()) / 2;
    textY = (clientRect.GetHeight() - textSize.y) / 2;
    if(bmp.IsOk()) {
        gdc.DrawBitmap(bmp, offset, bmpY);
        offset += bmp.GetScaledWidth();
        offset += spacer;
    }

    if(!m_text.IsEmpty()) {
        gdc.DrawText(m_text, offset, textY);
        offset += textSize.x;
        offset += spacer;
    }
}

void wxFlatButton::OnLeftDown(wxMouseEvent& event)
{
    event.Skip();
    m_state = kStatePressed;
    Refresh();
}

void wxFlatButton::GetGCDC(wxAutoBufferedPaintDC& dc, wxGCDC& gdc)
{
    wxGraphicsRenderer* const renderer = wxGraphicsRenderer::GetDefaultRenderer();
    wxGraphicsContext* context = renderer->CreateContext(dc);
    gdc.SetGraphicsContext(context);
}

wxSize wxFlatButton::GetBestSize()
{
    wxBitmap bmp(1, 1);
    wxMemoryDC memDc(bmp);
    wxGCDC gdc;
    wxGraphicsRenderer* const renderer = wxGraphicsRenderer::GetDefaultRenderer();
    wxGraphicsContext* context = renderer->CreateContext(memDc);
    gdc.SetGraphicsContext(context);

    int buttonWidth(0);
    int buttonHeight(0);

    gdc.SetFont(GetTextFont());

    // Get the button width
    // [spacer | text + spacer | image + spacer ]
    buttonWidth += X_SPACER;
    if(!m_text.IsEmpty()) {
        buttonWidth += gdc.GetTextExtent(m_text).x;
        buttonWidth += X_SPACER;
    }

    if(m_bmp.IsOk()) {
        buttonWidth += m_bmp.GetWidth();
        buttonWidth += X_SPACER;
    }

    wxSize textExtent = gdc.GetTextExtent(m_text);
    int bmpHeight = m_bmp.IsOk() ? m_bmp.GetHeight() : 0;

    buttonHeight = std::max(textExtent.GetHeight(), bmpHeight);
    buttonHeight += 2 * Y_SPACER;
    return wxSize(buttonWidth + 2 * X_SPACER, buttonHeight);
}

void wxFlatButton::OnSize(wxSizeEvent& event)
{
    event.Skip();
    Refresh();
}

void wxFlatButton::Check(bool check)
{
    wxASSERT_MSG(m_kind == kKindChek, "Can only check a toggle button");
    m_isChecked = check;
    m_state = check ? kStatePressed : kStateNormal;
    Refresh();
}

void wxFlatButton::SetTogglable(bool b)
{
    m_kind = b ? kKindChek : kKindNormal;
    m_isChecked = false; // default value
    Refresh();
}

void wxFlatButton::SetPopupWindow(wxMenu* menu)
{
    m_contextMenu = menu;
    if(m_contextMenu) {
        m_kind = kKindChek;
    } else {
        m_kind = kKindNormal;
    }
    Refresh();
}

void wxFlatButton::DoShowContextMenu()
{
    if(!HasMenu()) {
        return;
    }
    wxPoint pt = GetClientRect().GetBottomLeft();
    pt.y += 1;

    // Notify about menu is about to be shown
    wxFlatButtonEvent event(wxEVT_CMD_FLATBUTTON_MENU_SHOWING);
    event.SetMenu(m_contextMenu);
    event.SetEventObject(this);
    GetEventHandler()->ProcessEvent(event);

    PopupMenu(m_contextMenu, pt);
    m_state = kStateNormal;
    m_isChecked = false;
    Refresh();
}

void wxFlatButton::OnLeftDClick(wxMouseEvent& event) { OnLeftDown(event); }
bool wxFlatButton::HasMenu() const { return m_contextMenu != NULL; }

bool wxFlatButton::Enable(bool enable)
{
    m_isDisabled = !enable;
    Refresh();
    return wxFlatButtonBase::Enable(enable);
}

wxColour wxFlatButton::GetBarBgColour(wxFlatButton::eTheme theme)
{
    wxUnusedVar(theme);
    return DrawingUtils::GetPanelBgColour();
}

wxColour wxFlatButton::GetBarTextColour(wxFlatButton::eTheme theme)
{
    wxUnusedVar(theme);
    return DrawingUtils::GetPanelTextColour();
}
