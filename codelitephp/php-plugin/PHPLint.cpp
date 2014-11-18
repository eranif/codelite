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
    if(!m_queue.empty()) {
        wxFileName filename = m_queue.front();
        m_queue.pop_front();
        DoCheckFile(filename);
    }
}

void PHPLint::OnProcessTerminated(wxCommandEvent& event)
{
    ProcessEventData* ped = reinterpret_cast<ProcessEventData*>(event.GetClientData());
    IProcess *prc = ped->GetProcess();
    wxDELETE(prc);
    m_plugin->CallAfter(&PhpPlugin::PhpLintDone, m_output);
    
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
    ::CreateAsyncProcess(this, command);
    CL_DEBUG("PHP: running lint: %s", command);
}
