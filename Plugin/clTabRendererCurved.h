#ifndef CLTABRENDERERCURVED_H
#define CLTABRENDERERCURVED_H

#include "clTabRenderer.h"

class WXDLLIMPEXP_SDK clTabRendererCurved : public clTabRenderer
{
public:
    clTabRendererCurved();
    virtual ~clTabRendererCurved();

    void Draw(wxWindow* parent, wxDC& dc, const clTabInfo& tabInfo, const clTabColours& colours, size_t style);
    void DrawBottomRect(wxWindow* parent, clTabInfo::Ptr_t activeTab, const wxRect& clientRect, wxDC& dc,
                        const clTabColours& colours, size_t style);
};

#endif // CLTABRENDERERCURVED_H
