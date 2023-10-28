#ifndef SECONDARYSIDEBAR_HPP
#define SECONDARYSIDEBAR_HPP

#include "clSideBarCtrl.hpp"

class SideBar;
class SecondarySideBar : public wxPanel
{
    clSideBarCtrl* m_book = nullptr;
    SideBar* m_sidebar = nullptr;

public:
    SecondarySideBar(wxWindow* parent);
    ~SecondarySideBar();
};

#endif // SECONDARYSIDEBAR_HPP
