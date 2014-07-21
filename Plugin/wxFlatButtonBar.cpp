#include "wxFlatButtonBar.h"
#include <wx/dcbuffer.h>

wxFlatButtonBar::wxFlatButtonBar(wxWindow* parent, const wxFlatButton::eTheme theme, int flags)
    : wxFlatButtonBarBase(parent)
    , m_theme(theme)
    , m_style(flags)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    // Colours - dark theme
    SetBgColour(wxFlatButton::GetBarBgColour(theme));
    SetPenColour(wxFlatButton::GetBarBgColour(theme));
    m_mainSizer->Add(2, 0, 0, wxEXPAND, 0);
}

wxFlatButtonBar::~wxFlatButtonBar() {}

wxFlatButton* wxFlatButtonBar::AddButton(const wxString& label, const wxBitmap& bmp, const wxSize& size, int style)
{
    wxFlatButton* button = new wxFlatButton(this, label, m_theme, bmp, size, style);
    m_mainSizer->Add(button, 0, wxEXPAND | wxTOP | wxBOTTOM | wxRIGHT, 2);
    return button;
}

void wxFlatButtonBar::OnPaint(wxPaintEvent& event)
{
    wxBufferedPaintDC dc(this);
    dc.SetBrush(GetBgColour());
    dc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW));
    dc.DrawRectangle(GetClientRect());
}

void wxFlatButtonBar::OnSize(wxSizeEvent& event)
{
    event.Skip();
    Refresh();
}

wxWindow* wxFlatButtonBar::AddControl(wxWindow* window, int proportion, int flags)
{
    m_mainSizer->Add(window, proportion, flags, 1);
    return window;
}

void wxFlatButtonBar::OnIdle(wxIdleEvent& event) { event.Skip(); }
