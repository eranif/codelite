#ifndef CLAUIDOCKART_H
#define CLAUIDOCKART_H

#include <wx/window.h>
#include <wx/aui/framemanager.h>
#include <wx/dc.h>
#include <wx/gdicmn.h>
#include <wx/aui/dockart.h>
#include "imanager.h"
#include "codelite_exports.h"
#include <wx/aui/auibar.h>

class WXDLLIMPEXP_SDK clAuiDockArt : public wxAuiDefaultDockArt
{
    IManager *m_manager;
    wxBitmap m_bmpClose;
    wxBitmap m_bmpCloseInactive;

public:
    clAuiDockArt(IManager *manager);
    virtual ~clAuiDockArt();
    
    virtual void DrawBorder(wxDC& dc, wxWindow* window, const wxRect& rect, wxAuiPaneInfo& pane);
    virtual void DrawCaption(wxDC& dc, wxWindow *window,
                             const wxString& text,
                             const wxRect& rect,
                             wxAuiPaneInfo& pane);

    virtual void DrawPaneButton(wxDC& dc, wxWindow *window,
                                int button,
                                int button_state,
                                const wxRect& _rect,
                                wxAuiPaneInfo& pane);
    virtual void DrawBackground(wxDC& dc, wxWindow* window, int orientation, const wxRect& rect);
    
    virtual void DrawSash(wxDC& dc, wxWindow* window, int orientation, const wxRect &rect);
};

#endif // CLAUIDOCKART_H
