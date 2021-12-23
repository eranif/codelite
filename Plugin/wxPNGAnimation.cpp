#include "wxPNGAnimation.h"

#include <wx/dcbuffer.h>

wxPNGAnimation::wxPNGAnimation(wxWindow* parent, const wxBitmap& pngSprite, wxOrientation spriteOrientation,
                               const wxSize& singleImageSize, wxWindowID id)
    : wxPanel(parent, id)
    , m_index(0)
{
    m_bgColour = GetParent()->GetBackgroundColour();
    Hide();
    m_timer = new wxTimer(this);
    if(spriteOrientation == wxVERTICAL) {
        int numImages = pngSprite.GetHeight() / singleImageSize.GetHeight();
        wxPoint copyFromPt;
        for(int i = 0; i < numImages; ++i) {
            copyFromPt.x = 0;
            copyFromPt.y = i * singleImageSize.GetHeight();
            wxBitmap bmp = pngSprite.GetSubBitmap(wxRect(copyFromPt, singleImageSize));
            m_bitmaps.push_back(bmp);
        }
    } else {
        int numImages = pngSprite.GetWidth() / singleImageSize.GetWidth();
        wxPoint copyFromPt;
        for(int i = 0; i < numImages; ++i) {
            copyFromPt.x = i * singleImageSize.GetWidth();
            copyFromPt.y = 0;
            wxBitmap bmp = pngSprite.GetSubBitmap(wxRect(copyFromPt, singleImageSize));
            m_bitmaps.push_back(bmp);
        }
    }

    Bind(wxEVT_PAINT, &wxPNGAnimation::OnPaint, this);
    Bind(wxEVT_ERASE_BACKGROUND, &wxPNGAnimation::OnEraseBG, this);
    Bind(wxEVT_TIMER, &wxPNGAnimation::OnTimer, this, m_timer->GetId());
    SetSizeHints(singleImageSize);
    SetSize(singleImageSize);
}

wxPNGAnimation::~wxPNGAnimation()
{
    Unbind(wxEVT_TIMER, &wxPNGAnimation::OnTimer, this, m_timer->GetId());
    Unbind(wxEVT_PAINT, &wxPNGAnimation::OnPaint, this);
    Unbind(wxEVT_ERASE_BACKGROUND, &wxPNGAnimation::OnEraseBG, this);

    m_timer->Stop();
    wxDELETE(m_timer);
}

void wxPNGAnimation::OnEraseBG(wxEraseEvent& event) { wxUnusedVar(event); }

void wxPNGAnimation::OnPaint(wxPaintEvent& event)
{
    wxBufferedPaintDC dc(this);
    wxRect rect = GetClientRect();

#ifdef __WXOSX__
    rect.Inflate(1);
#endif

    dc.SetPen(m_bgColour);
    dc.SetBrush(m_bgColour);
    dc.DrawRectangle(rect);
    dc.DrawBitmap(m_bitmaps.at(m_index), wxPoint(0, 0), true);
}

void wxPNGAnimation::OnTimer(wxTimerEvent& event)
{
    ++m_index;
    if(m_index >= m_bitmaps.size()) {
        m_index = 0;
    }
    Refresh();
}

void wxPNGAnimation::Start(int refereshRate)
{
    m_timer->Start(refereshRate);
    Show();
}

void wxPNGAnimation::Stop()
{
    m_timer->Stop();
    Hide();
}

void wxPNGAnimation::SetPanelColour(const wxColour& colour) { m_bgColour = colour; }
