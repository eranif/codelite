#ifndef CLTHEMEDSPLITTERWINDOW_H
#define CLTHEMEDSPLITTERWINDOW_H

#include "cl_command_event.h"
#include "codelite_exports.h"

#include <wx/splitter.h>

class WXDLLIMPEXP_SDK clThemedSplitterWindow : public wxSplitterWindow
{
protected:
    void OnPaint(wxPaintEvent& event);
    void OnEraseBg(wxEraseEvent& event);
    void DoDrawSash(wxDC& dc);
    void DrawSash(wxDC& dc);
    void OnSysColoursChanged(clCommandEvent& event);

public:
    clThemedSplitterWindow() {}
    clThemedSplitterWindow(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                           const wxSize& size = wxDefaultSize, long style = wxSP_3D,
                           const wxString& name = "splitterWindow");
    virtual ~clThemedSplitterWindow();
    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = wxSP_3D, const wxString& name = "splitterWindow");
};

#endif // CLTHEMEDSPLITTERWINDOW_H
