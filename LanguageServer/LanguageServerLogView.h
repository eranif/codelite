#ifndef LANGUAGESERVERLOGVIEW_H
#define LANGUAGESERVERLOGVIEW_H

#include "LSP/LSPEvent.h"
#include "UI.h"
#include "clWorkspaceEvent.hpp"

class LanguageServerCluster;
class LanguageServerLogView : public LanguageServerLogViewBase
{
    LanguageServerCluster* m_cluster = nullptr;

public:
    LanguageServerLogView(wxWindow* parent, LanguageServerCluster* cluster);
    virtual ~LanguageServerLogView();

protected:
    void OnWorkspaceClosed(clWorkspaceEvent& event);
};
#endif // LANGUAGESERVERLOGVIEW_H
