#pragma once

#include "clConsoleBase.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_CL clConsoleKitty : public clConsoleBase
{
    wxString m_terminal;

public:
    clConsoleKitty();
    ~clConsoleKitty() override = default;

public:
    bool Start() override;
    bool StartForDebugger() override;
    wxString PrepareCommand() override;
};
