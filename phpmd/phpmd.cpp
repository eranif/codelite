#include "asyncprocess.h"
#include "file_logger.h"
#include "globals.h"
#include "phpmd.h"
#include "processreaderthread.h"
#include <event_notifier.h>
#include <wx/sstream.h>
#include <wx/xrc/xmlres.h>

static phpmd* thePlugin = NULL;

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == NULL) {
        thePlugin = new phpmd(manager);
    }
    return thePlugin;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("Anders Jenbo"));
    info.SetName(wxT("phpmd"));
    info.SetDescription(_("Run code style checking on PHP source files"));
    info.SetVersion(wxT("v1.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion()
{
    return PLUGIN_INTERFACE_VERSION;
}

phpmd::phpmd(IManager* manager)
    : IPlugin(manager)
{
    m_longName = _("Run code style checking on PHP source files");
    m_shortName = wxT("phpmd");

    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &phpmd::OnProcessOutput, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &phpmd::OnProcessTerminated, this);
}

phpmd::~phpmd()
{
}

clToolBar* phpmd::CreateToolBar(wxWindow* parent)
{
    // Create the toolbar to be used by the plugin
    clToolBar* tb(NULL);

    // Connect the events to us
    EventNotifier::Get()->Connect(wxEVT_FILE_SAVED, clCommandEventHandler(phpmd::OnFileAction), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_FILE_LOADED, clCommandEventHandler(phpmd::OnFileAction), NULL, this);

    return tb;
}

void phpmd::CreatePluginMenu(wxMenu* pluginsMenu)
{
}

void phpmd::UnPlug()
{
    EventNotifier::Get()->Disconnect(wxEVT_FILE_SAVED, clCommandEventHandler(phpmd::OnFileAction), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_FILE_LOADED, clCommandEventHandler(phpmd::OnFileAction), NULL, this);
}

void phpmd::OnCheck(wxCommandEvent& e)
{
    IEditor* editor(NULL);
    wxString fileToCheck = e.GetString();

    // If we got a file name in the event, use it instead of the active editor
    if(fileToCheck.IsEmpty() == false) {
        editor = m_mgr->FindEditor(fileToCheck);

    } else {
        editor = m_mgr->GetActiveEditor();
    }

    // get the editor that requires checking
    if(!editor)
        return;

    clDEBUG() << "Checking file: '" << editor->GetFileName() << "'" << clEndl;

    m_mgr->SetStatusMessage(
        wxString::Format(wxT("%s: %s..."), _("Checking"), editor->GetFileName().GetFullPath().c_str()), 0);
    DoCheckFile(editor->GetFileName());
    m_mgr->SetStatusMessage(_("Done"), 0);

    clDEBUG() << "Checking file: '" << editor->GetFileName() << "'...is done" << clEndl;
}

void phpmd::OnFileAction(clCommandEvent& e)
{
    e.Skip();

    // Run phpmd
    IEditor* editor = m_mgr->GetActiveEditor();
    CHECK_PTR_RET(editor);

    if(FileExtManager::IsPHPFile(editor->GetFileName())) {
        /* Calling DelAllCompilerMarkers will remove marks placed by other linters
        if(m_mgr->GetActiveEditor()) {
            m_mgr->GetActiveEditor()->DelAllCompilerMarkers();
        }
        */
        phpmd::DoCheckFile(editor->GetFileName());
    }
}

void phpmd::DoCheckFile(const wxFileName& filename)
{
    m_output.Clear();

    // Build the command
    wxString command;
    command = "/usr/bin/php";
    ::WrapWithQuotes(command);

    wxString file = filename.GetFullPath();
    ::WrapWithQuotes(file);

    wxString config = "/home/ajenbo/code/meebookweb/build/phpmd.xml";
    ::WrapWithQuotes(config);

    command << " /usr/bin/phpmd " << file << " xml " << config;

    // Run the check command
    m_process = ::CreateAsyncProcess(this, command);
    if(!m_process) {
        // failed to run the command
        CL_WARNING("phpmd: could not run command '%s'", command);
        DoProcessQueue();
        m_currentFileBeingProcessed.Clear();

    } else {
        CL_DEBUG("phpmd: running check: %s", command);
        m_currentFileBeingProcessed = filename.GetFullPath();
    }
}

void phpmd::DoProcessQueue()
{
    if(!m_process && !m_queue.empty()) {
        wxFileName filename = m_queue.front();
        m_queue.pop_front();
        DoCheckFile(filename);
    }
}

void phpmd::OnProcessTerminated(clProcessEvent& event)
{
    CL_DEBUG("phpmd: process terminated. output: %s", m_output);
    wxDELETE(m_process);
    CallAfter(&phpmd::PhpCSDone, m_output, m_currentFileBeingProcessed);
    // Check the queue for more files
    DoProcessQueue();
}

void phpmd::OnProcessOutput(clProcessEvent& event)
{
    m_output << event.GetOutput();
}

void phpmd::PhpCSDone(const wxString& lintOutput, const wxString& filename)
{
    // Find the editor
    CL_DEBUG("phpmd: searching editor for file: %s", filename);

    IEditor* editor = m_mgr->FindEditor(filename);
    CHECK_PTR_RET(editor);

    wxStringInputStream lintOutputStream(lintOutput);
    wxXmlDocument doc;
    if(!doc.Load(lintOutputStream))
        return;

    // Priority can be 1-5, 5 being the least sevear.
    // By default phpmd seams to award 1, 3 and 5 to it rules. Only very few rules have 5
    // So 3 seams a sensible default, but it might be good to have it configurable,
    // especially if people define different priorities in there phpmd.xml
    int warningLevel = 3;

    wxXmlNode* file = doc.GetRoot()->GetChildren();
    if(!file)
        return;

    wxXmlNode* violation = file->GetChildren();
    while(violation) {
        wxString strLine = violation->GetAttribute("beginline");
        wxString priority = violation->GetAttribute("priority", "1");
        wxString errorMessage = violation->GetNodeContent().Trim().Trim(false);

        long nLine(wxNOT_FOUND);
        if(strLine.ToCLong(&nLine)) {
            CL_DEBUG("phpmd: adding error marker @%d", (int)nLine - 1);

            long nPriority(wxNOT_FOUND);
            if(priority.ToCLong(&nPriority) < warningLevel) {
                editor->SetErrorMarker(nLine - 1, errorMessage);
            } else {
                editor->SetWarningMarker(nLine - 1, errorMessage);
            }
        }

        violation = violation->GetNext();
    }
}
