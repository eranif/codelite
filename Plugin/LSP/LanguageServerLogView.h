#pragma once

#include "UI.h"
#include "clWorkspaceEvent.hpp"
#include "cl_command_event.h"

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
