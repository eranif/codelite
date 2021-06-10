#ifndef CLTABRENDERERSQUARE_H
#define CLTABRENDERERSQUARE_H

#include "clTabRendererClassic.h"

class WXDLLIMPEXP_SDK clTabRendererSquare : public clTabRendererClassic
{
public:
    clTabRendererSquare(const wxWindow* parent);
    virtual ~clTabRendererSquare();

    void Draw(wxWindow* parent, wxDC& dc, wxDC& fontDC, const clTabInfo& tabInfo, const clTabColours& colours,
              size_t style, eButtonState buttonState) override;
    void DrawBottomRect(wxWindow* parent, clTabInfo::Ptr_t activeTab, const wxRect& clientRect, wxDC& dc,
                        const clTabColours& colours, size_t style) override;
    clTabRenderer* New(const wxWindow* parent) const override { return new clTabRendererSquare(parent); }
    bool IsVerticalTabSupported() const override { return false; }
};
#endif // CLTABRENDERERSQUARE_H
