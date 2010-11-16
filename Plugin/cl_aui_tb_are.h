#ifndef CL_AUI_TOOLBAR_ART_H
#define CL_AUI_TOOLBAR_ART_H

#include <wx/pen.h>
#include <wx/sizer.h>
#include <wx/bitmap.h>
#include "drawingutils.h"
#include "cl_defs.h"
#include "codelite_exports.h"

#if USE_AUI_TOOLBAR

#include <wx/aui/auibar.h>

class WXDLLIMPEXP_SDK CLMainAuiTBArt : public wxAuiDefaultToolBarArt
{
	bool m_mswWithThemeEnabled;
	
public:
	CLMainAuiTBArt();
	virtual ~CLMainAuiTBArt();

    virtual void DrawGripper( wxDC& dc, wxWindow* wnd, const wxRect& rect);
	virtual void DrawBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect);
	virtual void DrawSeparator(wxDC& dc, wxWindow* wnd, const wxRect& rect);
};
#endif

#endif // CL_AUI_TOOLBAR_ART_H
