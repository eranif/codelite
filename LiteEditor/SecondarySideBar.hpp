#pragma once

// clang-format off
#include "cl_command_event.h"
#include "clSideBarCtrl.hpp"
// clang-format on

class SideBar;
class SecondarySideBar : public wxPanel
{
protected:
    void OnContextMenu(clContextMenuEvent& event);
    void MoveToPrimarySideBar(int pos);
    void OnSettingsChanged(wxCommandEvent& event);

public:
    SecondarySideBar(wxWindow* parent, long style = wxBORDER_NONE);
    ~SecondarySideBar();
    const char* GetCaption() const { return "Secondary Sidebar"; }
    void SetSideBar(SideBar* sb);
    void AddPage(wxWindow* win, const wxString& bmpname, const wxString& label);
    void SetSelection(int selection);
    clSideBarCtrl* GetNotebook() { return m_book; }

private:
    clSideBarCtrl* m_book{nullptr};
    SideBar* m_sidebar{nullptr};
};
