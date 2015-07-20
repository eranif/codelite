#ifndef CLTREECTRLPANELDEFAULTPAGE_H
#define CLTREECTRLPANELDEFAULTPAGE_H
#include "wxcrafter_plugin.h"

class clTreeCtrlPanelDefaultPage : public clTreeCtrlPanelDefaultPageBase
{
public:
    clTreeCtrlPanelDefaultPage(wxWindow* parent);
    virtual ~clTreeCtrlPanelDefaultPage();

protected:
    virtual void OnDefaultPageContextMenu(wxContextMenuEvent& event);
    void OnOpenFolder(wxCommandEvent& event);
};
#endif // CLTREECTRLPANELDEFAULTPAGE_H
