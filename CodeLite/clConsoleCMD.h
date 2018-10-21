#ifndef CLCONSOLECMD_H
#define CLCONSOLECMD_H

#include "clConsoleBase.h" // Base class: clConsoleBase

class StartForDebugger;
class clConsoleCMD : public clConsoleBase
{
public:
    clConsoleCMD();
    virtual ~clConsoleCMD();

public:
    virtual bool Start();
    virtual bool StartForDebugger();
};

#endif // CLCONSOLECMD_H
