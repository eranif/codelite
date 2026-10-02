#pragma once

#include "LSP/LSPEvent.h"
#include "clWorkspaceEvent.hpp"
#include "cl_command_event.h"
#include "lsp_UI.hpp"

#include <vector>

class WXDLLIMPEXP_SDK LSPManager;
class WXDLLIMPEXP_SDK LanguageServerLogView : public LanguageServerLogViewBase
{
public:
    LanguageServerLogView(wxWindow* parent);
    ~LanguageServerLogView() override;

protected:
    void OnWorkspaceClosed(clWorkspaceEvent& event);
    void OnColoursChanged(clCommandEvent& event);
    void OnProgress(LSPEvent& event);

    void DoColourChanged();
    void ClearProgress();

    /// Row keys (server name + token) of the progress list, in the same order as the rows.
    /// Row items are index based, so they are not stable after a delete: keep the keys instead.
    std::vector<wxString> m_progressKeys;
};
