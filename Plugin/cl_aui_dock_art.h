#ifndef CLAUIDOCKART_H
#define CLAUIDOCKART_H

#include <wx/window.h>
#include <wx/aui/framemanager.h>
#include <wx/dc.h>
#include <wx/gdicmn.h>
#include <wx/aui/dockart.h>
#include "imanager.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK clAuiDockArt : public wxAuiDefaultDockArt
{
    IManager *m_manager;
    wxBitmap m_bmpClose;
    wxBitmap m_bmpCloseInactive;

public:
    clAuiDockArt(IManager *manager);
    virtual ~clAuiDockArt();

    virtual void DrawCaption(wxDC& dc, wxWindow *window,
                             const wxString& text,
                             const wxRect& rect,
                             wxAuiPaneInfo& pane);

    virtual void DrawPaneButton(wxDC& dc, wxWindow *window,
                                int button,
                                int button_state,
                                const wxRect& _rect,
                                wxAuiPaneInfo& pane);
};

#endif // CLAUIDOCKART_H
