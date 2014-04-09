#include <iostream>
#include "LLDBProtocol/LLDBConnector.h"

int main(int argc, char **argv)
{
    LLDBConnector connector;
    connector.ConnectToDebugger();
    wxSleep(100);
    return 0;
}

