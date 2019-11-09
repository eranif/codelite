#ifndef CLTABRENDERERSQUARE_H
#define CLTABRENDERERSQUARE_H

#include "clTabRendererClassic.h"

class WXDLLIMPEXP_SDK clTabRendererSquare : public clTabRendererClassic
{
public:
    clTabRendererSquare(const wxWindow* parent);
    virtual ~clTabRendererSquare();

    virtual void Draw(wxWindow* parent, wxDC& dc, wxDC& fontDC, const clTabInfo& tabInfo, const clTabColours& colours,
                      size_t style, eButtonState buttonState);
    void DrawBottomRect(wxWindow* parent, clTabInfo::Ptr_t activeTab, const wxRect& clientRect, wxDC& dc,
                        const clTabColours& colours, size_t style);
    clTabRenderer* New(const wxWindow* parent) const { return new clTabRendererSquare(parent); }
};
#endif // CLTABRENDERERSQUARE_H
