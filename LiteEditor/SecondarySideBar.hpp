#ifndef SECONDARYSIDEBAR_HPP
#define SECONDARYSIDEBAR_HPP

#include "clSideBarCtrl.hpp"

class SecondarySideBar : public wxPanel
{
    clSideBarCtrl* m_book = nullptr;

public:
    SecondarySideBar(wxWindow* parent);
    ~SecondarySideBar();
};

#endif // SECONDARYSIDEBAR_HPP
