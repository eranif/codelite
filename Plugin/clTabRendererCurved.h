#ifndef CLTABRENDERERCURVED_H
#define CLTABRENDERERCURVED_H

#include "clTabRenderer.h"
#if !USE_AUI_NOTEBOOK
class WXDLLIMPEXP_SDK clTabRendererCurved : public clTabRenderer
{
public:
    clTabRendererCurved(const wxWindow* parent);
    virtual ~clTabRendererCurved();

    void Draw(wxWindow* parent, wxDC& dc, wxDC& fontDC, const clTabInfo& tabInfo, const clTabColours& colours,
              size_t style, eButtonState buttonState) override;
    void DrawBottomRect(wxWindow* parent, clTabInfo::Ptr_t activeTab, const wxRect& clientRect, wxDC& dc,
                        const clTabColours& colours, size_t style) override;
    clTabRenderer* New(const wxWindow* parent) const override { return new clTabRendererCurved(parent); }
    bool IsVerticalTabSupported() const override { return false; }
};
#endif
#endif // CLTABRENDERERCURVED_H
