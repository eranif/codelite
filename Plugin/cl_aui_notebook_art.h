#ifndef CLAUI_TAB_ART_H
#define CLAUI_TAB_ART_H

#include <wx/aui/auibook.h>
#include <wx/pen.h>
#include "codelite_exports.h"

// -- GUI helper classes and functions --

class WXDLLIMPEXP_SDK clAuiGlossyTabArt : public wxAuiDefaultTabArt
{
public:
    clAuiGlossyTabArt();
    virtual ~clAuiGlossyTabArt();
    
    virtual void DrawBackground(wxDC& dc,
                                wxWindow* wnd,
                                const wxRect& rect);
};

#endif // CLAUI_TAB_ART_H
