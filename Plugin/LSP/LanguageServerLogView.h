#pragma once

#include "UI.h"
#include "clWorkspaceEvent.hpp"

class WXDLLIMPEXP_SDK LSPManager;
class WXDLLIMPEXP_SDK LanguageServerLogView : public LanguageServerLogViewBase
{
public:
    LanguageServerLogView(wxWindow* parent);
    virtual ~LanguageServerLogView();

protected:
    void OnWorkspaceClosed(clWorkspaceEvent& event);
};
