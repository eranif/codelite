#ifndef EXTERNALTOOLSPROCESSMANAGER_H
#define EXTERNALTOOLSPROCESSMANAGER_H

#include <wx/event.h>
#include "cl_command_event.h"
#include "externaltoolsdata.h"
#include <vector>

struct ExternalToolItemData {
    wxString m_command;
    int m_pid;

    ExternalToolItemData(const wxString& command, int pid)
        : m_command(command)
        , m_pid(pid)
    {
    }

    ExternalToolItemData* Clone() const { return new ExternalToolItemData(m_command, m_pid); }
    ~ExternalToolItemData() {}
    typedef std::map<int, ExternalToolItemData> Map_t;
};

class ToolsTaskManager : public wxEvtHandler
{
    static ToolsTaskManager* ms_instance;

protected:
    ExternalToolItemData::Map_t m_tools;

protected:
    void OnProcessEnd(clProcessEvent& event);
    void OnProcessOutput(clProcessEvent& event);

public:
    static ToolsTaskManager* Instance();
    static void Release();
    /**
     * @brief launch a tool
     */
    void StartTool(const ToolInfo& ti);
    /**
     * @brief a tool process terminated, remove it from our internal tracking table
     */
    void ProcessTerminated(int pid);
    /**
     * @brief stop all processes
     */
    void StopAll();
    /**
     * @brief stop a single tool
     */
    void Stop(int pid);

    ExternalToolItemData::Map_t& GetTools();

private:
    ToolsTaskManager();
    virtual ~ToolsTaskManager();
};

#endif // EXTERNALTOOLSPROCESSMANAGER_H
