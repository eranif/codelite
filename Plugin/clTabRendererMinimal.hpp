#ifndef CLTABRENDERERCLASSIC_H
#define CLTABRENDERERCLASSIC_H

#include "clTabRenderer.h"
class WXDLLIMPEXP_SDK clTabRendererMinimal : public clTabRenderer
{
public:
    static void InitDarkColours(clTabColours& colours, const wxColour& activeTabBGColour);
    static void InitLightColours(clTabColours& colours, const wxColour& activeTabBGColour);

public:
    clTabRendererMinimal(const wxWindow* parent);
    virtual ~clTabRendererMinimal();
    void Draw(wxWindow* parent, wxDC& dc, wxDC& fontDC, const clTabInfo& tabInfo, const clTabColours& colours,
              size_t style, eButtonState buttonState) override;
    void DrawBottomRect(wxWindow* parent, clTabInfo::Ptr_t tabInfo, const wxRect& clientRect, wxDC& dc,
                        const clTabColours& colours, size_t style) override;
    void DrawBackground(wxWindow* parent, wxDC& dc, const wxRect& rect, const clTabColours& colours,
                        size_t style) override;
    void FinaliseBackground(wxWindow* parent, wxDC& dc, const wxRect& clientRect, const wxRect& activeTabRect,
                            const clTabColours& colours, size_t style) override;
    clTabRenderer* New(const wxWindow* parent) const override { return new clTabRendererMinimal(parent); }
    bool IsVerticalTabSupported() const override { return true; }
};
#endif // CLTABRENDERERCLASSIC_H
