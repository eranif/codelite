#ifndef TOOLBOXPANEL_H
#define TOOLBOXPANEL_H

#include "wxcrafter_gui.h"

class GUICraftMainPanel;
class ToolBoxPanel : public ToolBoxPanelBaseClass
{
protected:
    // virtual void OnCustomControl(wxRibbonToolBarEvent& event);
    // virtual void OnNewControl(wxRibbonToolBarEvent& event);
    // virtual void OnMenuSelection(wxCommandEvent &e);

    GUICraftMainPanel* m_mainView;

public:
    ToolBoxPanel(wxWindow* parent, GUICraftMainPanel* mainView);
    virtual ~ToolBoxPanel();
    void OnControlUI(wxUpdateUIEvent& event);
};
#endif // TOOLBOXPANEL_H
