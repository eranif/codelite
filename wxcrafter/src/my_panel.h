#ifndef MYPANEL_H
#define MYPANEL_H

#include <wx/panel.h>

class MyPanel : public wxPanel
{
public:
    MyPanel() = default;
    MyPanel(wxWindow* parent, wxWindowID winid = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL | wxNO_BORDER,
            const wxString& name = wxPanelNameStr)
    {
        Create(parent, winid, pos, size, style, name);
    }
    ~MyPanel() override;
};

#endif // MYPANEL_H
