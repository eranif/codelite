#ifndef CLTABRENDERERSQUARE_H
#define CLTABRENDERERSQUARE_H

#include "clTabRenderer.h"

class WXDLLIMPEXP_SDK clTabRendererSquare : public clTabRenderer
{
public:
    clTabRendererSquare();
    virtual ~clTabRendererSquare();

    virtual void Draw(wxDC& dc, const clTabInfo& tabInfo, const clTabColours& colours, size_t style);
    void DrawBottomRect(
        clTabInfo::Ptr_t activeTab, const wxRect& clientRect, wxDC& dc, const clTabColours& colours, size_t style);
};

#endif // CLTABRENDERERSQUARE_H
