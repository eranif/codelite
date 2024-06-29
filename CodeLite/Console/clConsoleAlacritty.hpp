#ifndef CLCONSOLEALACRITTY_HPP
#define CLCONSOLEALACRITTY_HPP

#include "clConsoleBase.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_CL clConsoleAlacritty : public clConsoleBase
{
    wxString m_terminal;

public:
    clConsoleAlacritty();
    virtual ~clConsoleAlacritty();

public:
    bool Start() override;
    bool StartForDebugger() override;
    wxString PrepareCommand() override;
};

#endif // CLCONSOLEALACRITTY_HPP
