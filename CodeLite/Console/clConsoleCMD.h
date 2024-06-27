#ifndef CLCONSOLECMD_H
#define CLCONSOLECMD_H

#include "clConsoleBase.h" // Base class: clConsoleBase

class WXDLLIMPEXP_CL clConsoleCMD : public clConsoleBase
{
public:
    clConsoleCMD();
    virtual ~clConsoleCMD();

public:
    bool Start() override;
    bool StartForDebugger() override;
    wxString PrepareCommand() override;
};

#endif // CLCONSOLECMD_H
