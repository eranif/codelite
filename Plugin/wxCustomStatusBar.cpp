#include "wxCustomStatusBar.h"

#include "drawingutils.h"

#include <wx/dcbuffer.h>
#include <wx/dcclient.h>
#include <wx/dcgraph.h>
#include <wx/dcmemory.h>
#include <wx/msgdlg.h>
#include <wx/settings.h>

//========================------------------------------------
//========================------------------------------------

wxDEFINE_EVENT(wxEVT_STATUSBAR_CLICKED, clCommandEvent);

wxCustomStatusBarArt::wxCustomStatusBarArt(const wxString& name)
    : m_name(name)
{
}

void wxCustomStatusBarArt::DrawText(wxDC& dc, wxCoord x, wxCoord y, const wxString& text)
{
    dc.SetTextForeground(GetTextColour());
    dc.DrawText(text, x, y);
}

void wxCustomStatusBarArt::DrawFieldSeparator(wxDC& dc, const wxRect& fieldRect)
{
    // draw border line
    dc.SetPen(GetPenColour());
    wxPoint bottomPt, topPt;

    topPt = fieldRect.GetTopLeft();
    topPt.y += 2;

    bottomPt = fieldRect.GetBottomLeft();
    bottomPt.y += 1;
    dc.DrawLine(topPt, bottomPt);
}

wxColour wxCustomStatusBarArt::GetBgColour() const
{
    wxColour c = clSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
    return c;
}

wxColour wxCustomStatusBarArt::GetPenColour() const { return GetBgColour(); }
wxColour wxCustomStatusBarArt::GetTextColour() const { return clSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT); }
wxColour wxCustomStatusBarArt::GetSeparatorColour() const { return GetBgColour(); }

//========================------------------------------------
//========================------------------------------------

void wxCustomStatusBarSpacerField::Render(wxDC& dc, const wxRect& rect, wxCustomStatusBarArt::Ptr_t art)
{
    m_rect = rect;
    wxSize textSize = dc.GetTextExtent(m_text);

    // draw border line
    art->DrawFieldSeparator(dc, rect);
}

//========================------------------------------------
//========================------------------------------------

void wxCustomStatusBarFieldText::Render(wxDC& dc, const wxRect& rect, wxCustomStatusBarArt::Ptr_t art)
{
    m_rect = rect;
    wxSize textSize = dc.GetTextExtent(m_text);

    // Center text
    wxCoord textY = (rect.GetHeight() - textSize.GetHeight()) / 2 + rect.y;
    wxCoord textX;
    if(m_textAlign == wxALIGN_CENTER) {
        textX = (rect.GetWidth() - textSize.GetWidth()) / 2 + rect.x;
    } else {
        // left
        textX = rect.x + 5;
    }

    // draw border line
    art->DrawFieldSeparator(dc, rect);

    // Draw the text
    art->DrawText(dc, textX, textY + 1, m_text);
}

void wxCustomStatusBarFieldText::SetText(const wxString& text)
{
    m_text = text;
    // Render the new text
    if((m_rect != wxRect()) && m_parent) {
        // valid rect
        wxCustomStatusBarArt::Ptr_t art = m_parent->GetArt();
        if(art->GetName() == m_parent->GetLastArtNameUsedForPaint()) {
#if defined(__WXOSX__) || defined(__WXMSW__)
            m_parent->Refresh();
            return;
#else
            // Make sure we draw only when the "art" objects are in sync with the field
            // and with the bar itself
            if((m_rect.GetHeight() > 0) && (m_rect.GetWidth() > 0)) {
                wxBitmap bmp(m_rect.GetSize());
                wxMemoryDC memoryDC;
                memoryDC.SelectObject(bmp);
                wxGCDC dc(memoryDC);
                m_parent->PrepareDC(dc);

                wxFont font = DrawingUtils::GetDefaultGuiFont();
                dc.SetFont(font);
                wxRect rect(m_rect.GetSize()); // Create the same rect size, but on 0,0

                // Draw the field background
                dc.SetBrush(art->GetBgColour());
                dc.SetPen(art->GetBgColour());
                dc.DrawRectangle(rect);

                // Draw top separator line
                wxPoint topLeft = rect.GetTopLeft();
                wxPoint topRight = rect.GetTopRight();
                topRight.x += 1;
                dc.SetPen(art->GetSeparatorColour());
                dc.DrawLine(topLeft, topRight);

                // Draw the bottom separator using the pen colour
                // this will give a "sink" look to the status bar
                topLeft.y += 1;
                topRight.y += 1;
                dc.SetPen(art->GetPenColour());
                dc.DrawLine(topLeft, topRight);

                // Render will override m_rect, we so keep a copy
                wxRect origRect = m_rect;
                Render(dc, rect, art);
                m_rect = origRect;
                memoryDC.SelectObject(wxNullBitmap);

                // bmp contains the field content, draw it
                wxClientDC cdc(m_parent);
                m_parent->PrepareDC(cdc);
                cdc.DrawBitmap(bmp, m_rect.GetTopLeft(), true);
            }
#endif
        }
    }
}

//========================------------------------------------
// wxCustomStatusBarControlField
//========================------------------------------------

wxCustomStatusBarControlField::wxCustomStatusBarControlField(wxCustomStatusBar* parent, wxControl* control)
    : wxCustomStatusBarField(parent)
    , m_control(control)
{
}

wxCustomStatusBarControlField::~wxCustomStatusBarControlField() {}
void wxCustomStatusBarControlField::Render(wxDC& dc, const wxRect& rect, wxCustomStatusBarArt::Ptr_t art)
{
    CHECK_PTR_RET(m_control);
    wxUnusedVar(dc);
    m_rect = rect;

    // Draw the left side border
    art->DrawFieldSeparator(dc, rect);

    // Position the animation
    wxRect control_rect = m_control->GetRect();
    control_rect = control_rect.CenterIn(m_rect);
    m_control->Move(control_rect.GetTopLeft());
}

//========================------------------------------------
//========================------------------------------------

wxCustomStatusBarAnimationField::wxCustomStatusBarAnimationField(wxCustomStatusBar* parent, const wxBitmap& sprite,
                                                                 wxOrientation spriteOrientation,
                                                                 const wxSize& animSize)
    : wxCustomStatusBarField(parent)
{
    m_animation = new wxPNGAnimation(parent, sprite, spriteOrientation, animSize);

    m_width = animSize.GetWidth() + (2 * 5); // 2*5 here for spaces from the left and right
    // We need to tie the animation mouse events to the status bar, otherwise, whenever we hover or
    // move over the animation control - it will not be seen by the status bar
    m_animation->Bind(wxEVT_LEFT_DOWN, &wxCustomStatusBarAnimationField::OnAnimationClicked, this);
}

wxCustomStatusBarAnimationField::~wxCustomStatusBarAnimationField() {}

void wxCustomStatusBarAnimationField::Render(wxDC& dc, const wxRect& rect, wxCustomStatusBarArt::Ptr_t art)
{
    m_rect = rect;

    // Draw the left side border
    art->DrawFieldSeparator(dc, rect);

    // Position the animation
    wxSize animSize = m_animation->GetSize();
    wxCoord y = (rect.GetHeight() - animSize.GetHeight()) / 2 + rect.y;
    wxCoord x = (rect.GetWidth() - animSize.GetWidth()) / 2 + rect.x;
    m_animation->SetPanelColour(art->GetBgColour());
    m_animation->Move(wxPoint(x, y + 1));
}

void wxCustomStatusBarAnimationField::Start(long refreshRate) { m_animation->Start(refreshRate); }

void wxCustomStatusBarAnimationField::Stop() { m_animation->Stop(); }

void wxCustomStatusBarAnimationField::OnAnimationClicked(wxMouseEvent& event)
{
    event.Skip();
    wxCustomStatusBar* bar = dynamic_cast<wxCustomStatusBar*>(m_animation->GetParent());
    if(bar) {
        // Notify the bar that the animation owned by this field was clicked
        bar->AnimationClicked(this);
    }
}

//========================------------------------------------
//========================------------------------------------

void wxCustomStatusBarBitmapField::Render(wxDC& dc, const wxRect& rect, wxCustomStatusBarArt::Ptr_t art)
{
    m_rect = rect;

    // draw border line
    art->DrawFieldSeparator(dc, rect);

    constexpr int X_SPACER = 5;

    // Center bitmap
    int remaining_width = m_width;
    int xx = rect.GetTopLeft().x;
    if(m_bitmap.IsOk()) {
        wxRect rr{ wxPoint(xx, rect.GetTopLeft().y), m_bitmap.GetScaledSize() };
        rr = rr.CenterIn(rect, wxVERTICAL);
        // Draw the bitmap
        dc.DrawBitmap(m_bitmap, rr.GetTopLeft());
        xx += m_bitmap.GetScaledSize().GetWidth();
        remaining_width -= m_bitmap.GetScaledSize().GetWidth();
    }

    if(!m_label.empty()) {
        xx += X_SPACER;
        remaining_width -= X_SPACER;
        wxString fixed_text;
        DrawingUtils::TruncateText(m_label, remaining_width, dc, fixed_text);
        wxRect text_rect = dc.GetTextExtent(fixed_text);
        text_rect = text_rect.CenterIn(rect, wxVERTICAL);
        text_rect.SetX(xx);
        dc.DrawText(fixed_text, text_rect.GetTopLeft());
    }
}

//========================------------------------------------
//========================------------------------------------

wxCustomStatusBar::wxCustomStatusBar(wxWindow* parent, wxWindowID id, long style)
    : wxStatusBar(parent, id, style)
    , m_art(new wxCustomStatusBarArt("Dark"))
    , m_mainText(new wxCustomStatusBarFieldText(this, 0))
{
    m_mainText->SetAutoWidth(true);

    SetBackgroundStyle(wxBG_STYLE_PAINT);
    m_mainText->Cast<wxCustomStatusBarFieldText>()->SetTextAlignment(wxALIGN_LEFT);

    Bind(wxEVT_PAINT, &wxCustomStatusBar::OnPaint, this);
    Bind(wxEVT_ERASE_BACKGROUND, &wxCustomStatusBar::OnEraseBackground, this);
    Bind(wxEVT_LEFT_DOWN, &wxCustomStatusBar::OnLeftDown, this);
    Bind(wxEVT_MOTION, &wxCustomStatusBar::OnMouseMotion, this);
    m_timer = new wxTimer(this);
    Bind(wxEVT_TIMER, &wxCustomStatusBar::OnTimer, this, m_timer->GetId());

    m_fields.push_back(m_mainText);
    // 1 second event interval
    m_timer->Start(1000);
}

wxCustomStatusBar::~wxCustomStatusBar()
{
    // unbind the timer before we delete it
    Unbind(wxEVT_TIMER, &wxCustomStatusBar::OnTimer, this, m_timer->GetId());

    m_timer->Stop();
    wxDELETE(m_timer);

    Unbind(wxEVT_PAINT, &wxCustomStatusBar::OnPaint, this);
    Unbind(wxEVT_ERASE_BACKGROUND, &wxCustomStatusBar::OnEraseBackground, this);
    Unbind(wxEVT_LEFT_DOWN, &wxCustomStatusBar::OnLeftDown, this);
    Unbind(wxEVT_MOTION, &wxCustomStatusBar::OnMouseMotion, this);
}

void wxCustomStatusBar::OnPaint(wxPaintEvent& event)
{
    wxAutoBufferedPaintDC abdc(this);
    wxGCDC gcdc;
    wxDC& dc = DrawingUtils::GetGCDC(abdc, gcdc);

    PrepareDC(dc);
    wxRect rect = GetClientRect();
    rect.Inflate(1);

    dc.SetFont(DrawingUtils::GetDefaultGuiFont());

    // Remember which art name used for painting
    SetLastArtNameUsedForPaint(m_art->GetName());

    // Fill the background
    wxColour bgColour = m_art->GetBgColour();
    dc.SetBrush(bgColour);
    dc.SetPen(bgColour);
    dc.DrawRectangle(rect);

    // Update the fields width
    Finalize();

    //===----------------------
    // Draw the fields
    //===----------------------
    size_t offset_x = 0;
    for(auto field : m_fields) {
        // Prepare the rect
        size_t width = field->IsAutoWidth() ? field->GetAutoWidth() : field->GetWidth();
        wxRect fieldRect(offset_x, rect.y, width, rect.height);
        dc.SetClippingRegion(fieldRect);
        field->Render(dc, fieldRect, m_art);
        dc.DestroyClippingRegion();
        offset_x += width;
    }
}

void wxCustomStatusBar::OnEraseBackground(wxEraseEvent& event) { wxUnusedVar(event); }

size_t wxCustomStatusBar::DoGetFieldsWidth()
{
    size_t width(0);
    for(size_t i = 0; i < m_fields.size(); ++i) {
        width += m_fields.at(i)->GetWidth();
    }
    return width;
}

wxCustomStatusBarField::Ptr_t wxCustomStatusBar::GetField(size_t index)
{
    if(index >= m_fields.size())
        return wxCustomStatusBarField::Ptr_t(NULL);
    return m_fields.at(index);
}

void wxCustomStatusBar::RemoveField(size_t index)
{
    if(index >= m_fields.size())
        return;
    m_fields.erase(m_fields.begin() + index);
    Refresh();
}

void wxCustomStatusBar::OnLeftDown(wxMouseEvent& event)
{
    event.Skip();
    wxPoint point = event.GetPosition();
    for(size_t i = 0; i < m_fields.size(); ++i) {
        if(m_fields.at(i)->HitTest(point)) {
            // fire an event
            clCommandEvent event(wxEVT_STATUSBAR_CLICKED);
            event.SetEventObject(this);
            event.SetInt(i);
            GetEventHandler()->ProcessEvent(event);
            break;
        }
    }
}

void wxCustomStatusBar::ClearText()
{
    m_text.clear();
    Refresh();
}

void wxCustomStatusBar::UpdateMainTextField()
{
    // Make sure we draw only when the "art" objects are in sync with the field
    // and with the bar itself
    wxRect mainRect = DoGetMainFieldRect();

    // update the rect
    m_mainText->SetRect(mainRect);
    m_mainText->Cast<wxCustomStatusBarFieldText>()->SetText(GetText());
    m_mainText->Cast<wxCustomStatusBarFieldText>()->SetTooltip(GetText());
}

void wxCustomStatusBar::Finalize()
{
    decltype(m_fields) dyn_width_fields;
    size_t fixed_field_width = 0;
    for(auto field : m_fields) {
        if(field->IsAutoWidth()) {
            dyn_width_fields.push_back(field);
        } else {
            fixed_field_width += field->GetWidth();
        }
    }

    if(dyn_width_fields.empty())
        return; // nothing to be done here

    size_t dyn_width = (GetClientRect().GetWidth() - fixed_field_width) / dyn_width_fields.size();
    for(auto field : dyn_width_fields) {
        field->SetAutoWidth(dyn_width);
    }
}

void wxCustomStatusBar::SetText(const wxString& message, int secondsToLive)
{
    if(message.empty()) {
        // passing an empty string suggets that we want to clear the text field completely
        ClearText();
        return;
    }

    if(secondsToLive < 0) {
        secondsToLive = 1; // default to 1 second message
    } else if(secondsToLive == 0) {
        secondsToLive = 5; // this is how forever looks like
    }

    m_text.push_back({ message, time(nullptr) + secondsToLive });

#ifndef __WXMAC__
    // for some reason, macos tooltips are shown in the middle of the screen :)
    SetToolTip(message);
#endif

    UpdateMainTextField();
}

void wxCustomStatusBar::OnMouseMotion(wxMouseEvent& event)
{
    event.Skip();
    wxString current_tip = GetToolTipText();
    wxString tip_text;
    wxPoint point = event.GetPosition();
    for(size_t i = 0; i < m_fields.size(); ++i) {
        if(m_fields.at(i)->HitTest(point)) {
            tip_text = m_fields.at(i)->GetTooltip();
            break;
        }
    }

    if(current_tip != tip_text) {
        SetToolTip(tip_text);
    }
}

void wxCustomStatusBar::AnimationClicked(wxCustomStatusBarField* field)
{
    for(size_t i = 0; i < m_fields.size(); ++i) {
        if(m_fields.at(i).get() == field) {
            // fire an event
            clCommandEvent event(wxEVT_STATUSBAR_CLICKED);
            event.SetEventObject(this);
            event.SetInt(i);
            GetEventHandler()->ProcessEvent(event);
            break;
        }
    }
}

void wxCustomStatusBar::SetArt(wxCustomStatusBarArt::Ptr_t art)
{
    this->m_art = art;
    Refresh();
}

wxRect wxCustomStatusBar::DoGetMainFieldRect()
{
    wxRect rect = GetClientRect();
    size_t offset_x = 0;
    for(auto field : m_fields) {
        // Prepare the rect
        if(field.get() == m_mainText.get()) {
            // found the main text field
            break;
        }
        size_t width = field->IsAutoWidth() ? field->GetAutoWidth() : field->GetWidth();
        offset_x += width;
    }

    // Calculate the fields length
    size_t totalLength = rect.GetWidth();
    size_t fieldsLength = DoGetFieldsWidth();
    wxRect mainRect(offset_x, rect.y, m_mainText->IsAutoWidth() ? m_mainText->GetAutoWidth() : m_mainText->GetWidth(),
                    rect.height);
    return mainRect;
}

void wxCustomStatusBar::OnTimer(wxTimerEvent& event)
{
    event.Skip();

    time_t current_timestamp = time(nullptr);

    // filter expired items from the list and display the back() of the queue
    // find the best text to display
    decltype(m_text) filtered_queue;
    for(const auto& entry : m_text) {
        if(entry.second > current_timestamp) {
            // we keep this entry
            filtered_queue.push_back(entry);
        }
    }
    m_text.swap(filtered_queue);

    // this will use the back of the stack as the display item
    UpdateMainTextField();
}

bool wxCustomStatusBarField::HitTest(const wxPoint& point) const { return m_rect.Contains(point); }

thread_local wxString EMPTY_STRING;
const wxString& wxCustomStatusBar::GetText() const
{
    if(m_text.empty()) {
        return EMPTY_STRING;
    }
    return m_text.back().first;
}
