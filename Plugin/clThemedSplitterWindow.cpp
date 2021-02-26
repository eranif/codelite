#include "clSystemSettings.h"
#include "clThemedSplitterWindow.h"
#include "drawingutils.h"
#include "event_notifier.h"
#include <wx/dcbuffer.h>

clThemedSplitterWindow::clThemedSplitterWindow(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
                                               long style, const wxString& name)
{
    Create(parent, id, pos, size, style);
}

clThemedSplitterWindow::~clThemedSplitterWindow()
{
#ifndef __WXGTK__
    Unbind(wxEVT_PAINT, &clThemedSplitterWindow::OnPaint, this);
    Unbind(wxEVT_ERASE_BACKGROUND, &clThemedSplitterWindow::OnEraseBg, this);
    EventNotifier::Get()->Unbind(wxEVT_SYS_COLOURS_CHANGED, &clThemedSplitterWindow::OnSysColoursChanged, this);
#endif
}

bool clThemedSplitterWindow::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style,
                                    const wxString& name)
{
    wxUnusedVar(style);
    bool res = wxSplitterWindow::Create(parent, id, pos, size, wxSP_LIVE_UPDATE | wxBORDER_NONE, name);
    if(!res) {
        return false;
    }
#ifndef __WXGTK__
    Bind(wxEVT_PAINT, &clThemedSplitterWindow::OnPaint, this);
    Bind(wxEVT_ERASE_BACKGROUND, &clThemedSplitterWindow::OnEraseBg, this);
    EventNotifier::Get()->Bind(wxEVT_SYS_COLOURS_CHANGED, &clThemedSplitterWindow::OnSysColoursChanged, this);
#endif
    return res;
}

void clThemedSplitterWindow::OnPaint(wxPaintEvent& event)
{
    wxUnusedVar(event);
    wxAutoBufferedPaintDC dc(this);
    DoDrawSash(dc);
}

void clThemedSplitterWindow::OnEraseBg(wxEraseEvent& event) { wxUnusedVar(event); }

void clThemedSplitterWindow::DoDrawSash(wxDC& dc)
{
    wxColour c = clSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
    c = c.ChangeLightness(DrawingUtils::IsDark(c) ? 115 : 85);

    wxRect rect = GetClientRect();
    dc.SetPen(c);
    dc.SetBrush(c);
    dc.DrawRectangle(rect);
}

void clThemedSplitterWindow::OnSysColoursChanged(clCommandEvent& event)
{
    event.Skip();
    Refresh();
}

void clThemedSplitterWindow::DrawSash(wxDC& dc)
{
#ifdef __WXGTK__
    wxSplitterWindow::DrawSash(dc);
#else
    wxUnusedVar(dc);
    Refresh();
#endif
}
