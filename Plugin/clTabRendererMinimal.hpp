#ifndef CLTABRENDERERCLASSIC_H
#define CLTABRENDERERCLASSIC_H

#include "clTabRenderer.h"

#define TAB_RADIUS 0.0

class WXDLLIMPEXP_SDK clTabRendererMinimal : public clTabRenderer
{
public:
    static void InitDarkColours(clTabColours& colours, const wxColour& activeTabBGColour);
    static void InitLightColours(clTabColours& colours, const wxColour& activeTabBGColour);

protected:
    /**
     * @brief do the actual tab drawing. Return the rect used to round the tab
     */
    wxRect DoDraw(wxWindow* parent, wxDC& dc, wxDC& fontDC, const clTabInfo& tabInfo, size_t tabIndex,
                  const clTabColours& colors, size_t style, eButtonState tabState, eButtonState xButtonState);

public:
    clTabRendererMinimal(const wxWindow* parent);
    virtual ~clTabRendererMinimal();
    void Draw(wxWindow* parent, wxDC& dc, wxDC& fontDC, const clTabInfo& tabInfo, size_t tabIndex,
              const clTabColours& colours, size_t style, eButtonState tabState, eButtonState xButtonState) override;
    void DrawBottomRect(wxWindow* parent, clTabInfo::Ptr_t tabInfo, const wxRect& clientRect, wxDC& dc,
                        const clTabColours& colours, size_t style) override;
    wxColour DrawBackground(wxWindow* parent, wxDC& dc, const wxRect& rect, const clTabColours& colours,
                            size_t style) override;
    clTabRenderer* New(const wxWindow* parent) const override { return new clTabRendererMinimal(parent); }
    bool IsVerticalTabSupported() const override { return true; }
    void FinaliseBackground(wxWindow* parent, wxDC& dc, const wxRect& clientRect, const wxRect& activeTabRect,
                            const clTabColours& colours, size_t style) override;
};
#endif // CLTABRENDERERCLASSIC_H
