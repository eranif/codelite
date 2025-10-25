#ifndef LANGUAGESERVERLOGVIEW_H
#define LANGUAGESERVERLOGVIEW_H

#include "LSP/LSPEvent.h"
#include "UI.h"
#include "clWorkspaceEvent.hpp"

class LSPManager;
class LanguageServerLogView : public LanguageServerLogViewBase
{
public:
    LanguageServerLogView(wxWindow* parent, LSPManager* cluster);
    virtual ~LanguageServerLogView();

protected:
    void OnWorkspaceClosed(clWorkspaceEvent& event);
};
#endif // LANGUAGESERVERLOGVIEW_H
