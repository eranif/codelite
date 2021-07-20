#ifndef CLTABRENDERERGTK3_H
#define CLTABRENDERERGTK3_H

#include "clTabRenderer.h"

class WXDLLIMPEXP_SDK clTabRendererGTK3 : public clTabRenderer
{
public:
    clTabRendererGTK3(const wxWindow* parent);
    virtual ~clTabRendererGTK3();

    virtual void Draw(wxWindow* parent, wxDC& dc, wxDC& fontDC, const clTabInfo& tabInfo, const clTabColours& colours,
                      size_t style, eButtonState buttonState) override;
    void DrawBottomRect(wxWindow* parent, clTabInfo::Ptr_t activeTab, const wxRect& clientRect, wxDC& dc,
                        const clTabColours& colours, size_t style) override;
    wxColour DrawBackground(wxWindow* parent, wxDC& dc, const wxRect& rect, const clTabColours& colours,
                            size_t style) override;
    void FinaliseBackground(wxWindow* parent, wxDC& dc, const wxRect& rect, const wxRect& activeTabRect,
                            const clTabColours& colours, size_t style) override;
    void AdjustColours(clTabColours& colours, size_t style) override;
    clTabRenderer* New(const wxWindow* parent) const override { return new clTabRendererGTK3(parent); }
    bool IsVerticalTabSupported() const override { return false; }
};

#endif // CLTABRENDERERGTK3_H
