#include "Driver.hpp"
#include "dap/Process.hpp"
#include "dap/SocketBase.hpp"
#include "dap/SocketServer.hpp"
#include "dap/StringUtils.hpp"
#include "dap/dap.hpp"
#include <cstdio>
#include <cstdlib>
#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
    // Open stdin and wait for incoming for the initialize request
    try {
        dap::SocketBase::Initialize();
        dap::SocketServer server;
        server.Start("tcp://127.0.0.1:3456");
        dap::SocketBase::Ptr_t conn = server.WaitForNewConnection(100);
        
        // Launch the debugger process
        Driver gdb;
        gdb.Start("C:\\compilers\\mingw64\\bin\\gdb.exe");

        // Block the process until gdb terminates
        gdb.Wait();
    } catch(dap::SocketException& e) {
        cerr << e.what() << endl;
        return -1;
    }
    // We are done
    return 0;
}
