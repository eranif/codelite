#include "clButtonBase.h"
#include <wx/anybutton.h>
#include <wx/buffer.h>
#include <wx/dcbuffer.h>
#include <wx/dcgraph.h>
#include <wx/renderer.h>
#include <wx/settings.h>

#include "clSystemSettings.h"
#include <wx/gdicmn.h>

#if wxCHECK_VERSION(3, 1, 0)
#define TEXT_SPACER FromDIP(5)
#else
#define TEXT_SPACER 5
#define SetFractionalPointSize SetPointSize
#define wxFONTWEIGHT_SEMIBOLD wxFONTWEIGHT_BOLD
#endif

#ifdef __WXMSW__
#define BUTTON_RADIUS 0.0
#else
#define BUTTON_RADIUS 3.0
#endif

#define RIGHT_ARROW L"\u276f  "

#if wxUSE_NATIVE_BUTTON
clButtonBase::clButtonBase() {}

clButtonBase::clButtonBase(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos,
                           const wxSize& size, long style, const wxValidator& validator, const wxString& name)
    : wxButton(parent, id, label, pos, size, style, validator, name)
{
}

bool clButtonBase::Create(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos,
                          const wxSize& size, long style, const wxValidator& validator, const wxString& name)
{
    return wxButton::Create(parent, id, label, pos, size, style, validator, name);
}
clButtonBase::~clButtonBase() {}
#else
clButtonBase::clButtonBase() {}

clButtonBase::clButtonBase(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos,
                           const wxSize& size, long style, const wxValidator& validator, const wxString& name)
    : wxControl(parent, id, pos, size, wxTAB_TRAVERSAL | wxNO_BORDER | wxWANTS_CHARS)
    , m_buttonStyle(style)
{
    wxUnusedVar(name);
    wxUnusedVar(validator);
    SetText(label);
    Initialise();
}

bool clButtonBase::Create(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos,
                          const wxSize& size, long style, const wxValidator& validator, const wxString& name)
{
    wxUnusedVar(name);
    wxUnusedVar(validator);
    m_buttonStyle = style;
    SetText(label);
    if(!wxControl::Create(parent, id, pos, size, wxTAB_TRAVERSAL | wxNO_BORDER | wxWANTS_CHARS)) {
        return false;
    }
    Initialise();
    return true;
}

clButtonBase::~clButtonBase() { UnBindEvents(); }

#endif

#if !wxUSE_NATIVE_BUTTON
void clButtonBase::BindEvents()
{
    Bind(wxEVT_PAINT, &clButtonBase::OnPaint, this);
    Bind(wxEVT_ERASE_BACKGROUND, &clButtonBase::OnErasebg, this);
    Bind(wxEVT_LEFT_DOWN, &clButtonBase::OnLeftDown, this);
    Bind(wxEVT_LEFT_DCLICK, &clButtonBase::OnLeftDown, this);
    Bind(wxEVT_LEFT_UP, &clButtonBase::OnLeftUp, this);
    Bind(wxEVT_ENTER_WINDOW, &clButtonBase::OnEnter, this);
    Bind(wxEVT_LEAVE_WINDOW, &clButtonBase::OnLeave, this);
    Bind(wxEVT_SET_FOCUS, &clButtonBase::OnFocus, this);
    Bind(wxEVT_KILL_FOCUS, &clButtonBase::OnFocus, this);
    Bind(wxEVT_KEY_DOWN, &clButtonBase::OnKeyDown, this);
    Bind(wxEVT_IDLE, &clButtonBase::OnIdle, this);
    Bind(wxEVT_SIZE, &clButtonBase::OnSize, this);
}

void clButtonBase::UnBindEvents()
{
    Unbind(wxEVT_PAINT, &clButtonBase::OnPaint, this);
    Unbind(wxEVT_ERASE_BACKGROUND, &clButtonBase::OnErasebg, this);
    Unbind(wxEVT_LEFT_DOWN, &clButtonBase::OnLeftDown, this);
    Unbind(wxEVT_LEFT_DCLICK, &clButtonBase::OnLeftDown, this);
    Unbind(wxEVT_LEFT_UP, &clButtonBase::OnLeftUp, this);
    Unbind(wxEVT_ENTER_WINDOW, &clButtonBase::OnEnter, this);
    Unbind(wxEVT_LEAVE_WINDOW, &clButtonBase::OnLeave, this);
    Unbind(wxEVT_SET_FOCUS, &clButtonBase::OnFocus, this);
    Unbind(wxEVT_KILL_FOCUS, &clButtonBase::OnFocus, this);
    Unbind(wxEVT_KEY_DOWN, &clButtonBase::OnKeyDown, this);
    Unbind(wxEVT_IDLE, &clButtonBase::OnIdle, this);
    Unbind(wxEVT_SIZE, &clButtonBase::OnSize, this);
}

void clButtonBase::OnPaint(wxPaintEvent& event)
{
    wxUnusedVar(event);
    wxAutoBufferedPaintDC abdc(this);
    wxGCDC dc(abdc);
    PrepareDC(dc);
    Render(dc);
    m_lastPaintFlags = GetDrawingFlags();
    if(HasFocus()) {
        wxRect clientRect = GetClientRect();
        wxRect focusRect = clientRect.Deflate(3);
        focusRect = focusRect.CenterIn(clientRect);
        dc.SetPen(wxPen(clSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT), 1, wxPENSTYLE_DOT));
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.DrawRoundedRectangle(focusRect, 3);
    }
}

void clButtonBase::OnErasebg(wxEraseEvent& event) { wxUnusedVar(event); }

void clButtonBase::OnLeftDown(wxMouseEvent& event)
{
    event.Skip();
    if(!IsEnabled()) {
        return;
    }

    m_state = eButtonState::kPressed;
    CaptureMouse();
    Refresh();
}

void clButtonBase::OnLeftUp(wxMouseEvent& event)
{
    event.Skip();
    if(HasCapture()) {
        ReleaseMouse();
    }
    if(!IsEnabled()) {
        return;
    }

    wxRect rect = GetClientRect();
    if(rect.Contains(event.GetPosition())) {
        m_state = eButtonState::kHover;
        wxCommandEvent eventClick(wxEVT_BUTTON);
        eventClick.SetEventObject(this);
        GetEventHandler()->ProcessEvent(eventClick);
    } else {
        m_state = eButtonState::kNormal;
    }
    Refresh();
}

void clButtonBase::Initialise()
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    BindEvents();
    m_colours.InitFromColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    SetSizeHints(GetBestSize());
}

static void DrawLabel(wxDC& dc, const wxRect& rr, const wxString& text, const wxColour& textColour)
{
    dc.SetTextForeground(textColour);
    dc.SetClippingRegion(rr);
    // Truncate the text to fit the drawing area
    wxString fixedText;
    DrawingUtils::TruncateText(text, rr.GetWidth(), dc, fixedText);
    dc.DrawText(fixedText, rr.x, rr.y);
    dc.DestroyClippingRegion();
}

void clButtonBase::Render(wxDC& dc)
{
    wxRect clientRect = GetClientRect();
    wxRect rect = clientRect;
#ifdef __WXOSX__
    clientRect.Inflate(1);
#endif

    wxColour parentbgColour = clSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
    dc.SetBrush(parentbgColour);
    dc.SetPen(parentbgColour);
    dc.DrawRectangle(clientRect);

    bool isDisabled = !IsEnabled();
    wxColour bgColour = clSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
    bool isDark = DrawingUtils::IsDark(bgColour);
    wxColour borderColour = isDark ? bgColour.ChangeLightness(50) : bgColour.ChangeLightness(80);

    switch(m_state) {
    case eButtonState::kNormal:
    case eButtonState::kHover:
        bgColour = bgColour.ChangeLightness(isDark ? 80 : 95);
        break;
    case eButtonState::kPressed:
        bgColour = bgColour.ChangeLightness(isDark ? 70 : 80);
        break;
    case eButtonState::kDisabled:
        break;
    }

    if(isDisabled) {
        bgColour = bgColour.ChangeLightness(110);
        borderColour = borderColour.ChangeLightness(110);
    }

    // Draw the background
    const int button_radius = GetSubText().empty() ? BUTTON_RADIUS : 5;
    if(!isDisabled) {
        if(IsNormal() || IsHover()) {
            // fill the button bg colour with gradient
            // draw the border
            dc.SetPen(borderColour);
            dc.SetBrush(clSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
            dc.DrawRoundedRectangle(rect, button_radius);
        } else if(m_state == eButtonState::kPressed) {
            // pressed button is drawns with flat bg colour and border
            wxColour pressedBgColour = bgColour.ChangeLightness(90);
            borderColour = borderColour.ChangeLightness(70);
            dc.SetPen(borderColour);
            dc.SetBrush(pressedBgColour);
            dc.DrawRoundedRectangle(rect, button_radius);
        }
    } else {
        // Draw the button border
        dc.SetPen(borderColour);
        dc.SetBrush(bgColour);
        dc.DrawRoundedRectangle(rect, button_radius);
    }

    if(!isDisabled && !IsPressed()) {
        wxColour topLineColour = isDark ? bgColour.ChangeLightness(115) : bgColour.ChangeLightness(150);
        wxRect rr = rect;
        rr.Deflate(2);
        dc.SetPen(topLineColour);
        dc.DrawLine(rr.GetTopLeft(), rr.GetTopRight());
    }

    wxRect bitmap_rect;
    wxRect text_rect;
    wxRect arrow_rect;
    wxRect sub_text_rect;

    if(GetBitmap().IsOk()) {
        bitmap_rect.SetX(2 * TEXT_SPACER);
        bitmap_rect.SetWidth(GetBitmap().GetScaledWidth() + TEXT_SPACER);
        bitmap_rect.SetHeight(GetBitmap().GetScaledHeight());
        bitmap_rect.SetY(0);
    }

    if(HasDropDownMenu()) {
        arrow_rect.SetWidth((2 * TEXT_SPACER) + rect.GetHeight());
        arrow_rect.SetX(rect.GetWidth() - arrow_rect.GetWidth());
        arrow_rect.SetY(0);
        arrow_rect.SetHeight(rect.GetHeight());
    }

    wxString buttonText = GetText();
    wxString subtext = GetSubText();
    int sub_text_x_spacer = 0;
    if(!buttonText.IsEmpty()) {
        wxFont font = DrawingUtils::GetDefaultGuiFont();
        if(!subtext.empty()) {
            font.SetFractionalPointSize((double)1.5 * (double)font.GetPointSize());
            font.SetWeight(wxFONTWEIGHT_SEMIBOLD);
        }
        dc.SetFont(font);

        if(!subtext.empty()) {
            wxString prefix = RIGHT_ARROW;
            sub_text_x_spacer = dc.GetTextExtent(prefix).x;
            buttonText.Prepend(prefix);
        }

        text_rect.SetX(GetBitmap().IsOk() ? bitmap_rect.GetRight() : TEXT_SPACER);
        text_rect.SetY(0);
        text_rect.SetRight(HasDropDownMenu() ? arrow_rect.GetLeft() : (rect.GetRight() - TEXT_SPACER));

        wxSize text_size = dc.GetTextExtent(buttonText);
        text_rect.SetHeight(text_size.GetHeight());
        text_rect.SetWidth(text_size.GetWidth());
    }

    // Draw the bitmap first
    if(GetBitmap().IsOk()) {
        wxRect bitmapRect = bitmap_rect;
        bitmapRect = bitmapRect.CenterIn(rect, wxVERTICAL);
        dc.DrawBitmap(GetBitmap(), bitmapRect.GetTopLeft());
    }

    // Setup some colours (text and dropdown)
    wxColour textColour = clSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT);
    wxColour dropDownColour = textColour.ChangeLightness(isDark ? 90 : 110);

    if(isDisabled) {
        dropDownColour = textColour = m_colours.GetGrayText();
    } else if(IsPressed()) {
        textColour = m_colours.GetItemTextColour().ChangeLightness(isDark ? 70 : 110);
        dropDownColour = dropDownColour.ChangeLightness(isDark ? 70 : 110);
    }

    if(!buttonText.IsEmpty()) {
        bool has_sub_text = !subtext.empty();
        wxRect textBoundingRect = text_rect;
        textBoundingRect = textBoundingRect.CenterIn(rect, (has_sub_text ? (wxVERTICAL) : (wxVERTICAL | wxHORIZONTAL)));
        if(has_sub_text) {
            textBoundingRect.x += TEXT_SPACER;
            sub_text_rect = textBoundingRect;
            sub_text_rect.width = dc.GetTextExtent(subtext).x;
            sub_text_rect.x += sub_text_x_spacer; // align the text with the actual text and not the with the arrow
            sub_text_rect.y += sub_text_rect.height;
            sub_text_rect.y += TEXT_SPACER; // spacer between texts
            textBoundingRect.y -= (sub_text_rect.height / 2);
            sub_text_rect.y -= (sub_text_rect.height / 2);
        }

#ifdef __WXMAC__
        wxFont font = DrawingUtils::GetDefaultGuiFont();
        if(has_sub_text) {
            font.SetFractionalPointSize((double)font.GetPointSize()*1.4);
        }
        dc.SetFont(font);
#endif
        DrawLabel(dc, textBoundingRect, buttonText, textColour);
        if(has_sub_text) {
            wxFont font = DrawingUtils::GetDefaultGuiFont();
            dc.SetFont(font);
            DrawLabel(dc, sub_text_rect, subtext, textColour);
        }
    }

    if(HasDropDownMenu()) {
        // Draw an arrow
        wxRect arrowRect(0, 0, rect.GetHeight(), rect.GetHeight());
        arrowRect = arrowRect.CenterIn(arrow_rect);
        int arrowHeight = arrowRect.GetHeight() / 4;
        int arrowWidth = arrowRect.GetWidth() / 2;
        wxRect r(0, 0, arrowWidth, arrowHeight);
        r = r.CenterIn(arrowRect);

        wxPoint downCenterPoint = wxPoint(r.GetBottomLeft().x + r.GetWidth() / 2, r.GetBottom());
        dc.SetPen(wxPen(dropDownColour, 2));
        dc.DrawLine(r.GetTopLeft(), downCenterPoint);
        dc.DrawLine(r.GetTopRight(), downCenterPoint);
    }
}

void clButtonBase::OnEnter(wxMouseEvent& event)
{
    event.Skip();
    if(!HasCapture()) {
        m_state = eButtonState::kHover;
        Refresh();
    }
}

void clButtonBase::OnLeave(wxMouseEvent& event)
{
    event.Skip();
    if(!HasCapture()) {
        m_state = eButtonState::kNormal;
        Refresh();
    }
}

wxSize clButtonBase::GetBestSize() const
{
    wxBitmap bmp(1, 1);
    wxMemoryDC memDC(bmp);
    wxGCDC dc(memDC);

    wxFont f = DrawingUtils::GetDefaultGuiFont();
    if(!GetSubText().empty()) {
        f.SetFractionalPointSize((double)f.GetPointSize() * (double)1.5);
        f.SetWeight(wxFONTWEIGHT_SEMIBOLD);
    }
    dc.SetFont(f);

#if defined(__WXGTK__) || defined(__WXMAC__)
    int inflateSize = 8;
#else
    int inflateSize = 4;
#endif

    int buttonHeight = 0;
    int buttonWidth = 0;
    {
        wxRect r = dc.GetTextExtent("Tp");
        r.Inflate(inflateSize);
        buttonHeight = r.GetHeight();
    }

    wxString mainText = GetText();
    if(!GetSubText().empty()) {
        mainText.Prepend(RIGHT_ARROW);
    }

    wxString defaultText = "TTTpppTTTpp";
    wxString sampleText =
        GetSubText().empty() ? mainText : (mainText.length() > GetSubText().length() ? mainText : GetSubText());

    if(!(m_buttonStyle & wxBU_EXACTFIT) && defaultText.length() > sampleText.length()) {
        sampleText = defaultText;
    }

    wxRect sampleTextRect = dc.GetTextExtent(sampleText);
    wxRect textRect = dc.GetTextExtent(m_text);

    // If the text does not fit into the default button size, increase it
    buttonWidth += TEXT_SPACER;

    // Add bitmap length
    if(GetBitmap().IsOk()) {
        buttonWidth += GetBitmap().GetScaledWidth();
        buttonWidth += TEXT_SPACER;
    }
    // Text width
    buttonWidth += wxMax(textRect.GetWidth(), sampleTextRect.GetWidth());
    buttonWidth += TEXT_SPACER;

    // Drop down arrow width
    if(HasDropDownMenu()) {
        buttonWidth += buttonHeight;
    }

    if(!GetSubText().empty()) {
        buttonHeight *= 2;
        buttonHeight += TEXT_SPACER;
    }
    return wxSize(buttonWidth, buttonHeight);
}

void clButtonBase::SetColours(const clColours& colours)
{
    this->m_colours = colours;
    Refresh();
}

void clButtonBase::OnFocus(wxFocusEvent& event)
{
    event.Skip();
    Refresh();
}

void clButtonBase::OnKeyDown(wxKeyEvent& event)
{
    event.Skip();
    if((event.GetKeyCode() == WXK_SPACE) || (event.GetKeyCode() == WXK_NUMPAD_ENTER) ||
       (event.GetKeyCode() == WXK_RETURN)) {
        PostClickEvent();
        event.Skip(false);
    } else if(event.GetKeyCode() == WXK_TAB) {
        Navigate(event.ShiftDown() ? wxNavigationKeyEvent::IsBackward : wxNavigationKeyEvent::IsForward);
    }
}

void clButtonBase::PostClickEvent()
{
    wxCommandEvent eventClick(wxEVT_BUTTON);
    eventClick.SetEventObject(this);
    GetEventHandler()->AddPendingEvent(eventClick);
}

void clButtonBase::OnIdle(wxIdleEvent& event)
{
    event.Skip();
    size_t flags = GetDrawingFlags();
    if(flags != m_lastPaintFlags) {
        // We need to refresh the window
        Refresh();
    }
}

size_t clButtonBase::GetDrawingFlags() const
{
    size_t flags = 0;
    if(IsEnabled()) {
        flags |= kDrawingFlagEnabled;
    }
    return flags;
}
void clButtonBase::SetDefault() {}

void clButtonBase::OnSize(wxSizeEvent& event)
{
    event.Skip();
    Refresh();
}
void clButtonBase::SetBitmap(const wxBitmap& bmp)
{
    m_bitmap = bmp;
    SetSizeHints(GetBestSize());
    if(GetParent() && GetParent()->GetSizer()) {
        GetParent()->Layout();
    }
    Refresh();
}

const wxBitmap& clButtonBase::GetBitmap() const { return m_bitmap; }
#endif

void clButtonBase::SetHasDropDownMenu(bool hasDropDownMenu)
{
    m_hasDropDownMenu = hasDropDownMenu;
    Refresh();
}

void clButtonBase::ShowMenu(wxMenu& menu, wxPoint* point)
{
#if !wxUSE_NATIVE_BUTTON
    SetPressed();
    Refresh();
#endif

    wxPoint menuPos;
    if(point) {
        menuPos = *point;
    } else {
        menuPos = GetClientRect().GetBottomLeft();
#ifdef __WXOSX__
        menuPos.y += 5;
#endif
    }
    PopupMenu(&menu, menuPos);
#if !wxUSE_NATIVE_BUTTON
    SetNormal();
    Refresh();
#endif
}

void clButtonBase::SetText(const wxString& text)
{
#if wxUSE_NATIVE_BUTTON
    wxButton::SetLabel(text);
#else
    // strip menemonics, not very efficient...
    wxString tmp = text;
    tmp.Replace("&&", "@@");
    tmp.Replace("&", "");
    tmp.Replace("@@", "&");
    m_text = tmp;
    SetSizeHints(GetBestSize());
    if(GetParent() && GetParent()->GetSizer()) {
        GetParent()->Layout();
    }
    Refresh();
#endif
}

void clButtonBase::SetSubText(const wxString& subText)
{
    m_subText = subText;
    SetSizeHints(GetBestSize());
    Refresh();
}
