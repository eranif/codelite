//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : dockablepane.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifndef __dockablepane__
#define __dockablepane__

#include "Notebook.h"
#include "codelite_exports.h"
#include "imanager.h"

#include <wx/panel.h>
#include <wx/toolbar.h>

extern WXDLLIMPEXP_SDK const wxEventType wxEVT_CMD_NEW_DOCKPANE;
extern WXDLLIMPEXP_SDK const wxEventType wxEVT_CMD_DELETE_DOCKPANE;

class WXDLLIMPEXP_SDK DockablePane : public wxPanel
{
    wxWindow* m_child = nullptr;
    wxString m_text;
    int m_paneId = wxNOT_FOUND;
    bool m_notifiedDestroyed = false;

protected:
    void ClosePane(wxCommandEvent& e);
    void OnEraseBg(wxEraseEvent& e) { wxUnusedVar(e); }
    void OnPaint(wxPaintEvent& e);

public:
    DockablePane(wxWindow* parent, PaneId pane_id, const wxString& title, bool initialFloat = true,
                 const wxSize& size = wxDefaultSize);
    virtual ~DockablePane();
    wxString GetName() { return m_text; }
    void SetChildNoReparent(wxWindow* child);
};
#endif // __dockablepane__
