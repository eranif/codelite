#ifndef DEFAULTWORKSPACEPAGE_H
#define DEFAULTWORKSPACEPAGE_H

#include "wxcrafter.h"
#include "cl_command_event.h"

class DefaultWorkspacePage : public DefaultWorkspacePageBase
{
protected:
    void OnFolderDropped(clCommandEvent& event);
    void DoDropFolders(const wxArrayString& folders);

public:
    DefaultWorkspacePage(wxWindow* parent);
    virtual ~DefaultWorkspacePage();
};
#endif // DEFAULTWORKSPACEPAGE_H
