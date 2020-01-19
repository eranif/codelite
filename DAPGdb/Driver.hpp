#ifndef DRIVER_HPP
#define DRIVER_HPP

#include <dap/Process.hpp>
#include <string>
#include <vector>

using namespace std;
/// Control interface to the underlying debugger
class Driver
{
    dap::Process* m_process = nullptr;

public:
    Driver();
    ~Driver();

    /**
     * @brief Start the debugger. The debuggee is not yet launched at this point
     */
    void Start(const string& debuggerCommand, const string& workingDir = "", const vector<string>& debuggee = {});

    /**
     * @brief wait for the debugger to terminate. This function blocks the execution of the calling thread
     */
    void Wait();
};

#endif // DRIVER_HPP
