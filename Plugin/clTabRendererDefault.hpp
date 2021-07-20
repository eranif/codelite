#ifndef CLTABRENDERERSQUARE_H
#define CLTABRENDERERSQUARE_H

#include "clTabRendererMinimal.hpp"

class WXDLLIMPEXP_SDK clTabRendererDefault : public clTabRendererMinimal
{
public:
    clTabRendererDefault(const wxWindow* parent);
    virtual ~clTabRendererDefault();

    void Draw(wxWindow* parent, wxDC& dc, wxDC& fontDC, const clTabInfo& tabInfo, const clTabColours& colours,
              size_t style, eButtonState buttonState) override;
    void DrawBottomRect(wxWindow* parent, clTabInfo::Ptr_t activeTab, const wxRect& clientRect, wxDC& dc,
                        const clTabColours& colours, size_t style) override;
    clTabRenderer* New(const wxWindow* parent) const override { return new clTabRendererDefault(parent); }
    bool IsVerticalTabSupported() const override { return false; }
};
#endif // CLTABRENDERERSQUARE_H
