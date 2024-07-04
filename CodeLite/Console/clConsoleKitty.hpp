#ifndef CLCONSOLEKITTY_HPP
#define CLCONSOLEKITTY_HPP

#include "clConsoleBase.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_CL clConsoleKitty : public clConsoleBase
{
    wxString m_terminal;

public:
    clConsoleKitty();
    virtual ~clConsoleKitty();

public:
    bool Start() override;
    bool StartForDebugger() override;
    wxString PrepareCommand() override;
};

#endif // CLCONSOLEKITTY_HPP
