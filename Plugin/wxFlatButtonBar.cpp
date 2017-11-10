#include "wxFlatButtonBar.h"
#include <wx/dcbuffer.h>
#include "drawingutils.h"

wxFlatButtonBar::wxFlatButtonBar(wxWindow* parent, const wxFlatButton::eTheme theme, int rows, int cols)
    : wxFlatButtonBarBase(parent)
    , m_theme(theme)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    
    m_mainSizer = new wxFlexGridSizer(rows, cols, 0, 0);
    SetSizer(m_mainSizer);
    
    SetBgColour(DrawingUtils::GetPanelBgColour());
    SetPenColour(DrawingUtils::GetPanelBgColour());
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
    wxAutoBufferedPaintDC dc(this);
    PrepareDC(dc);
    
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
    m_mainSizer->Add(window, proportion, flags, 2);
    return window;
}

void wxFlatButtonBar::OnIdle(wxIdleEvent& event) { event.Skip(); }

void wxFlatButtonBar::AddSpacer(int size)
{
    wxPanel* spacer = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(size, -1));
    spacer->SetBackgroundColour(DrawingUtils::GetPanelBgColour());
    m_mainSizer->Add(spacer, 0, wxEXPAND, 0);
}

void wxFlatButtonBar::SetExpandableColumn(int col)
{
    m_mainSizer->AddGrowableCol(col, 1);
}
