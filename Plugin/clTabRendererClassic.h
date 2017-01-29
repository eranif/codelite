#ifndef CLTABRENDERERCLASSIC_H
#define CLTABRENDERERCLASSIC_H

#include "clTabRenderer.h"

class WXDLLIMPEXP_SDK clTabRendererClassic : public clTabRenderer
{
protected:
    void InitDarkColours(clTabColours& colours);
    void InitLightColours(clTabColours& colours);

public:
    clTabRendererClassic();
    virtual ~clTabRendererClassic();
    void Draw(wxDC& dc, const clTabInfo& tabInfo, const clTabColours& colours, size_t style);
    void DrawBottomRect(
        clTabInfo::Ptr_t tabInfo, const wxRect& clientRect, wxDC& dc, const clTabColours& colours, size_t style);
};

#endif // CLTABRENDERERCLASSIC_H
