#ifndef CLCONSOLECMD_H
#define CLCONSOLECMD_H

#include "clConsoleBase.h" // Base class: clConsoleBase

class WXDLLIMPEXP_CL clConsoleCMD : public clConsoleBase
{
public:
    clConsoleCMD() = default;
    ~clConsoleCMD() override = default;

public:
    bool Start() override;
    bool StartForDebugger() override;
    wxString PrepareCommand() override;
};

#endif // CLCONSOLECMD_H
