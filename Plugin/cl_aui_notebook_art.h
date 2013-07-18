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

    virtual wxAuiTabArt* Clone() {
        return new clAuiGlossyTabArt(*this);
    }
    virtual void DrawBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect) ;
    virtual void DrawTab(wxDC& dc,
                         wxWindow* wnd,
                         const wxAuiNotebookPage& page,
                         const wxRect& in_rect,
                         int close_button_state,
                         wxRect* out_tab_rect,
                         wxRect* out_button_rect,
                         int* x_extent);
    //virtual wxAuiTabArt* Clone() {
    //    return new clAuiGlossyTabArt(*this);
    //}
    //virtual void SetFlags(unsigned int flags) ;
    //
    //virtual void SetSizingInfo(const wxSize& tabCtrlSize, size_t tabCount) ;
    //
    //virtual void SetNormalFont(const wxFont& font) ;
    //virtual void SetSelectedFont(const wxFont& font) ;
    //virtual void SetMeasuringFont(const wxFont& font) ;
    //virtual void SetColour(const wxColour& colour) ;
    //virtual void SetActiveColour(const wxColour& colour) ;
    //
    //virtual void DrawBorder(wxDC& dc, wxWindow* wnd, const wxRect& rect) ;

    //virtual void DrawTab(wxDC& dc, wxWindow* wnd, onst wxAuiNotebookPage& pane, const wxRect& inRect, int closeButtonState, wxRect* outTabRect, wxRect* outButtonRect, int* xExtent) ;
    //virtual void DrawButton( wxDC& dc, wxWindow* wnd, const wxRect& inRect, int bitmapId, int buttonState, int orientation, wxRect* outRect) ;
    virtual wxSize GetTabSize(wxDC& dc, wxWindow* wnd, const wxString& caption, const wxBitmap& bitmap, bool active, int closeButtonState, int* xExtent) ;
    //virtual int ShowDropDown( wxWindow* wnd, const wxAuiNotebookPageArray& items, int activeIdx) ;
    //virtual int GetIndentSize() ;
    //virtual int GetBorderWidth( wxWindow* wnd) ;
    //virtual int GetAdditionalBorderSpace( wxWindow* wnd) ;
    //virtual int GetBestTabCtrlSize( wxWindow* wnd, const wxAuiNotebookPageArray& pages, const wxSize& requiredBmpSize) ;
};

#endif // CLAUI_TAB_ART_H
