#include "clTabRendererDefault.hpp"

#include "drawingutils.h"
#include "editor_config.h"
#include <wx/dcmemory.h>
#include <wx/font.h>
#include <wx/settings.h>

#define DRAW_LINE(__p1, __p2) \
    dc.DrawLine(__p1, __p2);  \
    dc.DrawLine(__p1, __p2);  \
    dc.DrawLine(__p1, __p2);  \
    dc.DrawLine(__p1, __p2);

clTabRendererDefault::clTabRendererDefault(const wxWindow* parent)
    : clTabRendererMinimal(parent)
{
    SetName("DEFAULT");
}

clTabRendererDefault::~clTabRendererDefault() {}

void clTabRendererDefault::Draw(wxWindow* parent, wxDC& dc, wxDC& fontDC, const clTabInfo& tabInfo,
                                const clTabColours& colours, size_t style, eButtonState buttonState)
{
    wxRect tabRect = clTabRendererMinimal::DoDraw(parent, dc, fontDC, tabInfo, colours, style, buttonState);
    if(tabInfo.IsActive()) {
        dc.SetPen(colours.markerColour);
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.DrawRoundedRectangle(tabRect, TAB_RADIUS);
    }
}

void clTabRendererDefault::DrawBottomRect(wxWindow* parent, clTabInfo::Ptr_t activeTab, const wxRect& clientRect,
                                          wxDC& dc, const clTabColours& colours, size_t style)
{
    clTabRendererMinimal::DrawBottomRect(parent, activeTab, clientRect, dc, colours, style);
}
