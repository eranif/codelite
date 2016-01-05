#ifndef CLTABRENDERERCURVED_H
#define CLTABRENDERERCURVED_H

#include "clTabRenderer.h"

class WXDLLIMPEXP_SDK clTabRendererCurved : public clTabRenderer
{
public:
    clTabRendererCurved();
    virtual ~clTabRendererCurved();

    virtual void Draw(wxDC& dc, const clTabInfo& tabInfo, const clTabColours& colours, size_t style);
};

#endif // CLTABRENDERERCURVED_H
