#include "clTabRendererSquare.h"

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

clTabRendererSquare::clTabRendererSquare(const wxWindow* parent)
    : clTabRendererClassic(parent)
{
    SetName("DEFAULT");
}

clTabRendererSquare::~clTabRendererSquare() {}

void clTabRendererSquare::Draw(wxWindow* parent, wxDC& dc, wxDC& fontDC, const clTabInfo& tabInfo,
                               const clTabColours& colours, size_t style, eButtonState buttonState)
{
    clTabRendererClassic::Draw(parent, dc, fontDC, tabInfo, colours, style, buttonState);
    if(tabInfo.IsActive()) { DrawMarker(dc, tabInfo, colours, style); }
}

void clTabRendererSquare::DrawBottomRect(wxWindow* parent, clTabInfo::Ptr_t activeTab, const wxRect& clientRect,
                                         wxDC& dc, const clTabColours& colours, size_t style)
{
    clTabRendererClassic::DrawBottomRect(parent, activeTab, clientRect, dc, colours, style);
}
