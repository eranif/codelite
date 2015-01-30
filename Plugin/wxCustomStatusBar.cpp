#include "wxCustomStatusBar.h"
#include <wx/dcbuffer.h>
#include <wx/msgdlg.h>

//========================------------------------------------
//========================------------------------------------

wxDEFINE_EVENT(wxEVT_STATUSBAR_CLICKED, clCommandEvent);

wxCustomStatusBarArt::wxCustomStatusBarArt()
{
    m_penColour = wxColour(125, 125, 125);
    m_bgColour = wxColour(86, 86, 86);
    m_textColour = *wxWHITE;
    m_textShadowColour = *wxBLACK;
}

void wxCustomStatusBarArt::DrawText(wxDC& dc, wxCoord x, wxCoord y, const wxString& text)
{
    dc.SetTextForeground(GetTextShadowColour());
    dc.DrawText(text, x, y - 1);

    dc.SetTextForeground(GetTextColour());
    dc.DrawText(text, x, y);
}

//========================------------------------------------
//========================------------------------------------

void wxCustomStatusBarFieldText::Render(wxDC& dc, const wxRect& rect, wxCustomStatusBarArt::Ptr_t art)
{
    m_rect = rect;
    wxSize textSize = dc.GetTextExtent(m_text);

    // Center text
    wxCoord textY = (rect.GetHeight() - textSize.GetHeight()) / 2 + rect.y;
    wxCoord textX = (rect.GetWidth() - textSize.GetWidth()) / 2 + rect.x;

    // draw border line
    dc.SetPen(art->GetPenColour());
    dc.DrawLine(rect.GetTopLeft(), rect.GetBottomLeft());

    // Draw the text
    art->DrawText(dc, textX, textY, m_text);
}

//========================------------------------------------
//========================------------------------------------

void wxCustomStatusBarBitmapField::Render(wxDC& dc, const wxRect& rect, wxCustomStatusBarArt::Ptr_t art)
{
    m_rect = rect;
    // draw border line
    dc.SetPen(art->GetPenColour());
    dc.DrawLine(rect.GetTopLeft(), rect.GetBottomLeft());
    
    // Center bitmap
    if(m_bitmap.IsOk()) {
        wxCoord bmpY = (rect.GetHeight() - m_bitmap.GetHeight()) / 2 + rect.y;
        wxCoord bmpX = (rect.GetWidth() - m_bitmap.GetWidth()) / 2 + rect.x;
        // Draw the bitmap
        dc.DrawBitmap(m_bitmap, bmpX, bmpY);
    }
}

//========================------------------------------------
//========================------------------------------------

wxCustomStatusBar::wxCustomStatusBar(wxWindow* parent, wxWindowID id, long style)
    : wxStatusBar(parent, id, style)
    , m_art(new wxCustomStatusBarArt)
{
    Bind(wxEVT_PAINT, &wxCustomStatusBar::OnPaint, this);
    Bind(wxEVT_ERASE_BACKGROUND, &wxCustomStatusBar::OnEraseBackround, this);
    Bind(wxEVT_LEFT_DOWN, &wxCustomStatusBar::OnLeftDown, this);
}

wxCustomStatusBar::~wxCustomStatusBar()
{
    Unbind(wxEVT_PAINT, &wxCustomStatusBar::OnPaint, this);
    Unbind(wxEVT_ERASE_BACKGROUND, &wxCustomStatusBar::OnEraseBackround, this);
    Unbind(wxEVT_LEFT_DOWN, &wxCustomStatusBar::OnLeftDown, this);
}

void wxCustomStatusBar::OnPaint(wxPaintEvent& event)
{
    wxBufferedPaintDC dc(this);
    wxRect rect = GetClientRect();

    // Fill the background
    dc.SetBrush(m_art->GetBgColour());
    dc.SetPen(m_art->GetBgColour());
    dc.DrawRectangle(rect);

    dc.SetPen(m_art->GetPenColour());
    wxPoint topLeft = rect.GetTopLeft();
    wxPoint topRight = rect.GetTopRight();
    dc.DrawLine(topLeft, topRight);

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
    wxRect mainRect(0, rect.y, offsetX, rect.height);
    dc.SetClippingRegion(mainRect);
    wxSize textSize = dc.GetTextExtent(m_text);
    wxCoord textY = (rect.GetHeight() - textSize.GetHeight()) / 2 + rect.y;
    wxCoord textX = 3;
    m_art->DrawText(dc, textX, textY, m_text);
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
    m_fields.erase(m_fields.begin()+index);
    Refresh();
}

void wxCustomStatusBar::OnLeftDown(wxMouseEvent& event)
{
    event.Skip();
    wxPoint point = event.GetPosition();
    for(size_t i=0; i<m_fields.size(); ++i) {
        if(m_fields.at(i)->HitTest(point)) {
            // fire an event
            clCommandEvent event(wxEVT_STATUSBAR_CLICKED);
            event.SetEventObject(this);
            event.SetInt(i);
            GetEventHandler()->AddPendingEvent(event);
            break;
        }
    }
}

void wxCustomStatusBar::ClearText()
{
    m_text.Clear();
    Refresh();
}

void wxCustomStatusBar::SetText(const wxString& message)
{
    m_text = message;
    Refresh();
}

bool wxCustomStatusBarField::HitTest(const wxPoint& point) const
{
    return m_rect.Contains(point);
}
