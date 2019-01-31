#ifndef MYPANEL_H
#define MYPANEL_H

#include "wxc_aui_manager.h"
#include <wx/panel.h>

class MyPanel : public wxPanel
{
public:
    MyPanel();
    virtual ~MyPanel();
    MyPanel(wxWindow* parent, wxWindowID winid = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL | wxNO_BORDER,
            const wxString& name = wxPanelNameStr)
    {
        Create(parent, winid, pos, size, style, name);
    }
};

#endif // MYPANEL_H
