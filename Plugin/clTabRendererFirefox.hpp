#ifndef CLTABRENDERERFIREFOX_HPP
#define CLTABRENDERERFIREFOX_HPP

#include "clTabRenderer.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK clTabRendererFirefox : public clTabRenderer
{
public:
    clTabRendererFirefox(const wxWindow* parent);
    ~clTabRendererFirefox();

private:
    void DrawShadow(const clTabColours& colours, const wxRect& rect, wxDC& dc);

public:
    clTabRenderer* New(const wxWindow* parent) const override;
    void Draw(wxWindow* parent, wxDC& dc, wxDC& fontDC, const clTabInfo& tabInfo, const clTabColours& colours,
              size_t style, eButtonState buttonState) override;
    void DrawBottomRect(wxWindow* parent, clTabInfo::Ptr_t activeTab, const wxRect& clientRect, wxDC& dc,
                        const clTabColours& colours, size_t style) override;
    bool IsVerticalTabSupported() const override { return true; }
};

#endif // CLTABRENDERERFIREFOX_HPP
