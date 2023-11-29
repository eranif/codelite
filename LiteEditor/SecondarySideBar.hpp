#ifndef SECONDARYSIDEBAR_HPP
#define SECONDARYSIDEBAR_HPP

#include "clSideBarCtrl.hpp"

class SideBar;
class SecondarySideBar : public wxPanel
{
private:
    clSideBarCtrl* m_book = nullptr;
    SideBar* m_sidebar = nullptr;

protected:
    void OnContextMenu(wxContextMenuEvent& event);
    void MoveToPrimarySideBar(int pos);
    void OnSettingsChanged(wxCommandEvent& event);

public:
    SecondarySideBar(wxWindow* parent, long style = wxBORDER_NONE);
    ~SecondarySideBar();
    const char* GetCaption() const { return "Secondary Sidebar"; }
    void SetSideBar(SideBar* sb);
    void AddPage(wxWindow* win, wxBitmap bmp, const wxString& label);
    void SetSelection(int selection);
};

#endif // SECONDARYSIDEBAR_HPP
