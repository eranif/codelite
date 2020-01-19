#ifndef PROCESS_H__
#define PROCESS_H__

#include <string>
using namespace std;

namespace dap
{
enum eProcessCreateFlags {
    IProcessCreateConsole = (1 << 0),
    IProcessCreateWithHiddenConsole = (1 << 1),
};

class Process
{
public:
    Process() {}
    virtual ~Process() {}

    virtual bool Read(string& str, string& err_buff) = 0;
    virtual bool Write(const string& str) = 0;
    virtual bool IsAlive() const = 0;
    virtual void Cleanup() = 0;
    virtual void Terminate() = 0;
};
/**
 * @brief Create process and return the handle to it
 * @param cmd process command
 * @param workingDir process's working directory
 * @param flags process creation flags
 * @return pointer to Process object
 */
Process* ExecuteProcess(const string& cmd, const string& workingDir, size_t flags);

};     // namespace dap
#endif // PROCESS_H__
