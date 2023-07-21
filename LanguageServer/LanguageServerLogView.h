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
    void OnDiagnosticSelected(wxTreeEvent& event) override;
    void OnCodeActions(LSPEvent& event);
    void OnClearActions(LSPEvent& event);
    void OnWorkspaceClosed(clWorkspaceEvent& event);

private:
    wxTreeItemId FindFile(const wxString& filepath) const;
};
#endif // LANGUAGESERVERLOGVIEW_H
