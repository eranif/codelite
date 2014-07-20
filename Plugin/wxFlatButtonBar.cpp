#include "wxFlatButtonBar.h"
#include <wx/dcbuffer.h>

wxFlatButtonBar::wxFlatButtonBar(wxWindow* parent, const wxFlatButton::eTheme theme)
    : wxFlatButtonBarBase(parent)
    , m_theme(theme)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    // Colours - dark theme
    SetBgColour(wxFlatButton::GetBarBgColour(m_theme));
}

wxFlatButtonBar::~wxFlatButtonBar() {}

wxFlatButton* wxFlatButtonBar::AddButton(const wxString& label, const wxBitmap& bmp, const wxSize& size)
{
    wxFlatButton* button = new wxFlatButton(this, label, m_theme, bmp, size);
    m_mainSizer->Add(button, 0, wxEXPAND | wxTOP|wxBOTTOM, 1);
    return button;
}

void wxFlatButtonBar::OnPaint(wxPaintEvent& event)
{
    wxBufferedPaintDC dc(this);
    dc.SetBrush(GetBgColour());
    dc.SetPen(GetBgColour());
    dc.DrawRectangle(GetClientRect());
}
void wxFlatButtonBar::OnSize(wxSizeEvent& event)
{
    event.Skip();
    Refresh();
}

wxWindow* wxFlatButtonBar::AddControl(wxWindow* window, int proportion, int flags)
{
    if(m_theme == wxFlatButton::kThemeDark) {
        window->SetBackgroundColour(wxFlatButton::GetBarBgColour(m_theme));
        window->SetForegroundColour(wxFlatButton::GetBarTextColour(m_theme));
    }
    m_mainSizer->Add(window, proportion, flags, 1);
    return window;
}

void wxFlatButtonBar::OnIdle(wxIdleEvent& event)
{
    
}
