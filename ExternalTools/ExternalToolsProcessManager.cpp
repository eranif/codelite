#include "ExternalToolsProcessManager.h"
#include "asyncprocess.h"
#include "dirsaver.h"
#include "environmentconfig.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "globals.h"
#include "imanager.h"
#include "macromanager.h"
#include "processreaderthread.h"
#include <algorithm>
#include <wx/msgdlg.h>
#include <wx/process.h>
#include <wx/utils.h>
#ifndef __WXMSW__
#include <signal.h>
#endif

/**
 * @class MyProcess
 * @brief handler class for capturing non-redirected processes termination
 */
class ExtToolsMyProcess : public wxProcess
{
public:
    ExtToolsMyProcess()
    {
    }
    virtual ~ExtToolsMyProcess()
    {
    }
    void OnTerminate(int pid, int status)
    {
        ToolsTaskManager::Instance()->ProcessTerminated(pid);
        delete this;
    }
};

ToolsTaskManager* ToolsTaskManager::ms_instance = 0;

ToolsTaskManager::ToolsTaskManager()
{
    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &ToolsTaskManager::OnProcessOutput, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &ToolsTaskManager::OnProcessEnd, this);
}

ToolsTaskManager::~ToolsTaskManager()
{
    Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &ToolsTaskManager::OnProcessOutput, this);
    Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &ToolsTaskManager::OnProcessEnd, this);
}

ToolsTaskManager* ToolsTaskManager::Instance()
{
    if(ms_instance == 0) {
        ms_instance = new ToolsTaskManager();
    }
    return ms_instance;
}

void ToolsTaskManager::Release()
{
    if(ms_instance) {
        delete ms_instance;
    }
    ms_instance = 0;
}

void ToolsTaskManager::OnProcessEnd(clProcessEvent& event)
{
    clGetManager()->AppendOutputTabText(kOutputTab_Output, event.GetOutput());
    clGetManager()->AppendOutputTabText(kOutputTab_Output, "\n");

    // delete the process
    IProcess* proc = event.GetProcess();
    ProcessTerminated(proc->GetPid());
    delete proc;

    // Notify codelite to test for any modified bufferes
    EventNotifier::Get()->PostReloadExternallyModifiedEvent();
}

void ToolsTaskManager::OnProcessOutput(clProcessEvent& event)
{
    clGetManager()->AppendOutputTabText(kOutputTab_Output, event.GetOutput());
}

void ToolsTaskManager::StartTool(const ToolInfo& ti, const wxString& filename)
{
    wxString command, working_dir;
    command << ti.GetPath();
    //::WrapWithQuotes(command);
    if(!filename.IsEmpty()) {
        // If an input file was given, append it to the command
        wxString fileName = filename;
        ::WrapWithQuotes(fileName);
        command << " " << fileName;
    }
    working_dir = ti.GetWd();
    command = MacroManager::Instance()->Expand(
        command, clGetManager(),
        (clGetManager()->GetWorkspace() ? clGetManager()->GetWorkspace()->GetActiveProjectName() : ""));
    working_dir = MacroManager::Instance()->Expand(
        working_dir, clGetManager(),
        (clGetManager()->GetWorkspace() ? clGetManager()->GetWorkspace()->GetActiveProjectName() : ""));

    wxString projectName;
    wxString configName;
    if(clCxxWorkspaceST::Get()->IsOpen()) {
        projectName = clCxxWorkspaceST::Get()->GetActiveProjectName();
        BuildConfigPtr bldConf = clCxxWorkspaceST::Get()->GetProjBuildConf(projectName, wxEmptyString);
        if(bldConf) {
            configName = bldConf->GetName();
        }
    }

    EnvSetter envGuard(clGetManager()->GetEnv(), NULL, projectName, configName);
    clDEBUG() << "Running command:" << command << clEndl;

    int pid = wxNOT_FOUND;
    if(ti.GetCaptureOutput()) {
        IProcess* proc = ::CreateAsyncProcess(this, command, IProcessCreateConsole | IProcessWrapInShell, working_dir);
        if(!proc) {
            ::wxMessageBox(_("Failed to launch tool\n'") + command + "'", "CodeLite", wxICON_ERROR | wxOK | wxCENTER,
                           EventNotifier::Get()->TopFrame());
            return;
        }

        pid = proc->GetPid();

    } else {
        // Set the working directory and launch the process
        DirSaver ds;
        ::wxSetWorkingDirectory(working_dir);
        pid = ::wxExecute(command, wxEXEC_ASYNC | wxEXEC_MAKE_GROUP_LEADER, new ExtToolsMyProcess());
    }

    if(pid > 0) {
        ExternalToolItemData item(command, pid);
        m_tools.insert(std::make_pair(pid, item));
    }
}

void ToolsTaskManager::ProcessTerminated(int pid)
{
    if(m_tools.find(pid) != m_tools.end()) {
        m_tools.erase(pid);
    }
}

void ToolsTaskManager::StopAll()
{
    std::for_each(m_tools.begin(), m_tools.end(), [&](const std::pair<int, ExternalToolItemData>& p) {
        ::wxKill(p.second.m_pid, wxSIGKILL, NULL, wxKILL_CHILDREN);
    });
}

void ToolsTaskManager::Stop(int pid)
{
    if(m_tools.find(pid) != m_tools.end()) {
        ::wxKill(pid, wxSIGKILL, NULL, wxKILL_CHILDREN);
    }
}

ExternalToolItemData::Map_t& ToolsTaskManager::GetTools()
{
#ifdef __WXOSX__
    // Check that the processes are still alive before we continue
    ExternalToolItemData::Map_t tools;
    std::for_each(m_tools.begin(), m_tools.end(), [&](const std::pair<int, ExternalToolItemData>& p) {
        if(kill(p.first, 0) == 0) {
            // alive
            tools.insert(p);
        }
    });
    m_tools.swap(tools);
#endif
    return m_tools;
}
