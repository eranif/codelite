#ifndef CLTABRENDERERSQUARE_H
#define CLTABRENDERERSQUARE_H

#include "clTabRenderer.h"
class WXDLLIMPEXP_SDK clTabRendererSquare : public clTabRenderer
{
public:
    clTabRendererSquare();
    virtual ~clTabRendererSquare();

    virtual void Draw(wxWindow* parent, wxDC& dc, wxDC& fontDC, const clTabInfo& tabInfo, const clTabColours& colours,
                      size_t style, eButtonState buttonState);
    void DrawBottomRect(wxWindow* parent, clTabInfo::Ptr_t activeTab, const wxRect& clientRect, wxDC& dc,
                        const clTabColours& colours, size_t style);
};
#endif // CLTABRENDERERSQUARE_H
