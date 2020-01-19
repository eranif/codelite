#include "Driver.hpp"
#include "Exception.hpp"
#include "dap/Process.hpp"
#include "utils.hpp"
#include <thread>

Driver::Driver() {}

Driver::~Driver() { DELETE_PTR(m_process); }

void Driver::Start(const string& debuggerCommand, const string& workingDir, const vector<string>& debuggee)
{
    m_process = dap::ExecuteProcess(debuggerCommand, workingDir, dap::IProcessCreateWithHiddenConsole);
    if(!m_process) { throw Exception("Failed to execute debugger: " + debuggerCommand); }
}

void Driver::Wait()
{
    if(!m_process) { return; }
    while(m_process && m_process->IsAlive()) {
        this_thread::sleep_for(chrono::milliseconds(100));
    }
}
