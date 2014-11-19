#include "PHPLint.h"
#include "processreaderthread.h"
#include "php_configuration_data.h"
#include "globals.h"
#include "file_logger.h"
#include "php.h"
#include "asyncprocess.h"

BEGIN_EVENT_TABLE(PHPLint, wxEvtHandler)
EVT_COMMAND(wxID_ANY, wxEVT_PROC_TERMINATED, PHPLint::OnProcessTerminated)
EVT_COMMAND(wxID_ANY, wxEVT_PROC_DATA_READ, PHPLint::OnProcessOutput)
END_EVENT_TABLE()

PHPLint::PHPLint(PhpPlugin* plugin)
    : m_plugin(plugin)
    , m_process(NULL)
{
}

PHPLint::~PHPLint() {}

void PHPLint::CheckCode(const wxFileName& filename)
{
    m_queue.push_back(filename);
    DoProcessQueue();
}

void PHPLint::DoProcessQueue()
{
    if(!m_process && !m_queue.empty()) {
        wxFileName filename = m_queue.front();
        m_queue.pop_front();
        DoCheckFile(filename);
    }
}

void PHPLint::OnProcessTerminated(wxCommandEvent& event)
{
    CL_DEBUG("PHPLint: process terminated. output: %s", m_output);
    ProcessEventData* ped = reinterpret_cast<ProcessEventData*>(event.GetClientData());
    wxDELETE(ped);
    wxDELETE(m_process);
    m_plugin->CallAfter(&PhpPlugin::PhpLintDone, m_output, m_currentFileBeingProcessed);
    // Check the queue for more files
    DoProcessQueue();
}

void PHPLint::OnProcessOutput(wxCommandEvent& event)
{
    ProcessEventData* ped = reinterpret_cast<ProcessEventData*>(event.GetClientData());
    m_output << ped->GetData();
    wxDELETE(ped);
}

void PHPLint::DoCheckFile(const wxFileName& filename)
{
    m_output.Clear();
    PHPConfigurationData data;
    data.Load();

    // no php ?
    if(data.GetPhpExe().IsEmpty()) {
        m_queue.clear();
        m_currentFileBeingProcessed.Clear();
        return;
    }
    
    // Build the command
    wxString command;
    command << data.GetPhpExe();
    ::WrapWithQuotes(command);
    
    wxString file = filename.GetFullPath();
    ::WrapWithQuotes(file);
    
    command << " -l " << file;
    
    // Run the lint command
    m_process = ::CreateAsyncProcess(this, command);
    if(!m_process) {
        // failed to run the command
        CL_WARNING("PHPLint: could not run command '%s'", command);
        DoProcessQueue();
        m_currentFileBeingProcessed.Clear();
        
    } else {
        CL_DEBUG("PHP: running lint: %s", command);
        m_currentFileBeingProcessed = filename.GetFullPath();
    }
}
