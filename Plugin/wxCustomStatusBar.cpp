#include "drawingutils.h"
#include "wxCustomStatusBar.h"
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
#endif
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
        }
    }
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

    // Center bitmap
    if(m_bitmap.IsOk()) {
        int bmpHeight = m_bitmap.GetScaledHeight();
        int bmpWidth = m_bitmap.GetScaledWidth();
        wxCoord bmpY = (rect.GetHeight() - bmpHeight) / 2 + rect.y;
        wxCoord bmpX = (rect.GetWidth() - bmpWidth) / 2 + rect.x;
        // Draw the bitmap
        dc.DrawBitmap(m_bitmap, bmpX, bmpY + 1);
    }
}

//========================------------------------------------
//========================------------------------------------

wxCustomStatusBar::wxCustomStatusBar(wxWindow* parent, wxWindowID id, long style)
    : wxStatusBar(parent, id, style)
    , m_art(new wxCustomStatusBarArt("Dark"))
    , m_mainText(new wxCustomStatusBarFieldText(this, 0))
    , m_timer(NULL)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    m_mainText->Cast<wxCustomStatusBarFieldText>()->SetTextAlignment(wxALIGN_LEFT);

    Bind(wxEVT_PAINT, &wxCustomStatusBar::OnPaint, this);
    Bind(wxEVT_ERASE_BACKGROUND, &wxCustomStatusBar::OnEraseBackround, this);
    Bind(wxEVT_LEFT_DOWN, &wxCustomStatusBar::OnLeftDown, this);
    Bind(wxEVT_MOTION, &wxCustomStatusBar::OnMouseMotion, this);
    m_timer = new wxTimer(this);
    Bind(wxEVT_TIMER, &wxCustomStatusBar::OnTimer, this, m_timer->GetId());
}

wxCustomStatusBar::~wxCustomStatusBar()
{
    m_timer->Stop();
    wxDELETE(m_timer);

    Unbind(wxEVT_PAINT, &wxCustomStatusBar::OnPaint, this);
    Unbind(wxEVT_ERASE_BACKGROUND, &wxCustomStatusBar::OnEraseBackround, this);
    Unbind(wxEVT_LEFT_DOWN, &wxCustomStatusBar::OnLeftDown, this);
    Unbind(wxEVT_MOTION, &wxCustomStatusBar::OnMouseMotion, this);
}

void wxCustomStatusBar::OnPaint(wxPaintEvent& event)
{
    wxAutoBufferedPaintDC abdc(this);
    PrepareDC(abdc);
    wxGCDC dc(abdc);
    wxRect rect = GetClientRect();

    dc.SetFont(DrawingUtils::GetDefaultGuiFont());

    // Remember which art name used for painting
    SetLastArtNameUsedForPaint(m_art->GetName());

    // Fill the background
    
    wxColour bgColour = m_art->GetBgColour();
    dc.SetBrush(bgColour);
    dc.SetPen(bgColour);
    dc.DrawRectangle(rect);

    
    // Calculate the fields length
    size_t totalLength = rect.GetWidth();
    size_t fieldsLength = DoGetFieldsWidth();

    // sanity
    size_t offsetX = 0;
    if(totalLength <= fieldsLength) {
        offsetX = 0;
    } else {
        offsetX = totalLength - fieldsLength;
    }

    //===----------------------
    // Draw the main field
    //===----------------------
    // update the rect

    wxRect mainRect(0, rect.y, offsetX, rect.height);
    dc.SetClippingRegion(mainRect);
    m_mainText->SetRect(mainRect);
    m_mainText->Cast<wxCustomStatusBarFieldText>()->Render(dc, mainRect, m_art);
    m_mainText->Cast<wxCustomStatusBarFieldText>()->SetTooltip(m_text);
    dc.DestroyClippingRegion();

    //===----------------------
    // Draw the fields
    //===----------------------

    for(size_t i = 0; i < m_fields.size(); ++i) {
        // Prepare the rect
        wxRect fieldRect(offsetX, rect.y, m_fields.at(i)->GetWidth(), rect.height);
        dc.SetClippingRegion(fieldRect);
        m_fields.at(i)->Render(dc, fieldRect, m_art);
        dc.DestroyClippingRegion();
        offsetX += m_fields.at(i)->GetWidth();
    }
}

void wxCustomStatusBar::OnEraseBackround(wxEraseEvent& event) { wxUnusedVar(event); }

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
    if(index >= m_fields.size()) return wxCustomStatusBarField::Ptr_t(NULL);
    return m_fields.at(index);
}

void wxCustomStatusBar::RemoveField(size_t index)
{
    if(index >= m_fields.size()) return;
    m_fields.erase(m_fields.begin() + index);
    if(m_timer->IsRunning()) { m_timer->Stop(); }
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
    m_text.Clear();
    if(m_timer->IsRunning()) { m_timer->Stop(); }
    Refresh();
}

void wxCustomStatusBar::SetText(const wxString& message, int secondsToLive)
{
    // Stop any timer
    if(m_timer->IsRunning()) { m_timer->Stop(); }

    m_text = message;
    SetToolTip(message);

    // Make sure we draw only when the "art" objects are in sync with the field
    // and with the bar itself
    wxRect mainRect = DoGetMainFieldRect();

    // update the rect
    m_mainText->SetRect(mainRect);
    m_mainText->Cast<wxCustomStatusBarFieldText>()->SetText(m_text);
    m_mainText->Cast<wxCustomStatusBarFieldText>()->SetTooltip(m_text);

    if(secondsToLive != wxNOT_FOUND) { m_timer->Start(secondsToLive * 1000, true); }
}

void wxCustomStatusBar::OnMouseMotion(wxMouseEvent& event)
{
    event.Skip();
    SetToolTip(wxEmptyString);
    wxPoint point = event.GetPosition();
    for(size_t i = 0; i < m_fields.size(); ++i) {
        if(m_fields.at(i)->HitTest(point)) {
            SetToolTip(m_fields.at(i)->GetTooltip());
            return;
        }
    }
    SetToolTip(m_text);
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
    // Calculate the fields length
    wxRect rect = GetClientRect();
    size_t totalLength = rect.GetWidth();
    size_t fieldsLength = DoGetFieldsWidth();

    size_t offsetX = 0;
    if(totalLength <= fieldsLength) {
        offsetX = 0;
    } else {
        offsetX = totalLength - fieldsLength;
    }
    wxRect mainRect(0, rect.y, offsetX, rect.height);
    return mainRect;
}

void wxCustomStatusBar::OnTimer(wxTimerEvent& event) { SetText(""); }

bool wxCustomStatusBarField::HitTest(const wxPoint& point) const { return m_rect.Contains(point); }
