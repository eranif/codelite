#ifndef MENUBAR_H
#define MENUBAR_H

#include <vector>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/menu.h>
#include <wx/panel.h>

struct MenuInfo {
    wxString label;
    wxMenu* menu;
    wxRect rect;
};

extern const wxEventType wxEVT_PREVIEW_BAR_SELECTED;

class MenuBar : public wxPanel
{
    using Vector_t = std::vector<MenuInfo>;

protected:
    wxMenuBar* m_mb;
    Vector_t m_menus;
    int m_controlHeight;
    wxString m_mbname;

public:
    MenuBar(wxWindow* parent, wxMenuBar* mb);
    ~MenuBar() override;

    DECLARE_EVENT_TABLE()
    void OnPaint(wxPaintEvent& e);
    void OnEraseBG(wxEraseEvent& e);
    void OnLeftDown(wxMouseEvent& e);
};

#endif // MENUBAR_H
