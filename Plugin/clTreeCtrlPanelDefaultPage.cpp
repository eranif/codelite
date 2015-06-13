#include "clTreeCtrlPanelDefaultPage.h"
#include "clFileOrFolderDropTarget.h"

clTreeCtrlPanelDefaultPage::clTreeCtrlPanelDefaultPage(wxWindow* parent)
    : clTreeCtrlPanelDefaultPageBase(parent)
{
    SetDropTarget(new clFileOrFolderDropTarget(parent));
}

clTreeCtrlPanelDefaultPage::~clTreeCtrlPanelDefaultPage()
{
}

