#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <wx/event.h>
#include <wx/panel.h>
#include <wx/toolbar.h>

class ToolBar : public wxPanel
{
    wxToolBar* m_wxtb;

public:
    ToolBar(wxWindow* parent);
    virtual ~ToolBar();

    void AddToolbar(wxToolBar* tb);
    void OnClick(wxCommandEvent& e);
};

#endif // TOOLBAR_H
