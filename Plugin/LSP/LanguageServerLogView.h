#pragma once

#include "clWorkspaceEvent.hpp"
#include "cl_command_event.h"
#include "lsp_UI.hpp"

class WXDLLIMPEXP_SDK LSPManager;
class WXDLLIMPEXP_SDK LanguageServerLogView : public LanguageServerLogViewBase
{
public:
    LanguageServerLogView(wxWindow* parent);
    ~LanguageServerLogView() override;

protected:
    void OnWorkspaceClosed(clWorkspaceEvent& event);
    void OnColoursChanged(clCommandEvent& event);

    void DoColourChanged();
};
