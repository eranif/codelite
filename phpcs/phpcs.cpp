#include "asyncprocess.h"
#include "file_logger.h"
#include "globals.h"
#include "phpcs.h"
#include "processreaderthread.h"
#include <event_notifier.h>
#include <wx/sstream.h>
#include <wx/xrc/xmlres.h>

static phpcs* thePlugin = NULL;

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == NULL) {
        thePlugin = new phpcs(manager);
    }
    return thePlugin;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("Anders Jenbo"));
    info.SetName(wxT("phpcs"));
    info.SetDescription(_("Run code style checking on PHP source files"));
    info.SetVersion(wxT("v1.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion()
{
    return PLUGIN_INTERFACE_VERSION;
}

phpcs::phpcs(IManager* manager)
    : IPlugin(manager)
{
    m_longName = _("Run code style checking on PHP source files");
    m_shortName = wxT("phpcs");

    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &phpcs::OnProcessOutput, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &phpcs::OnProcessTerminated, this);
}

phpcs::~phpcs()
{
}

clToolBar* phpcs::CreateToolBar(wxWindow* parent)
{
    // Create the toolbar to be used by the plugin
    clToolBar* tb(NULL);

    // Connect the events to us
    EventNotifier::Get()->Connect(wxEVT_FILE_SAVED, clCommandEventHandler(phpcs::OnFileAction), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_FILE_LOADED, clCommandEventHandler(phpcs::OnFileAction), NULL, this);

    return tb;
}

void phpcs::CreatePluginMenu(wxMenu* pluginsMenu)
{
}

void phpcs::UnPlug()
{
    EventNotifier::Get()->Disconnect(wxEVT_FILE_SAVED, clCommandEventHandler(phpcs::OnFileAction), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_FILE_LOADED, clCommandEventHandler(phpcs::OnFileAction), NULL, this);
}

void phpcs::OnCheck(wxCommandEvent& e)
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

void phpcs::OnFileAction(clCommandEvent& e)
{
    e.Skip();

    // Run phpcs
    IEditor* editor = m_mgr->GetActiveEditor();
    CHECK_PTR_RET(editor);

    if(FileExtManager::IsPHPFile(editor->GetFileName())) {
        /* Calling DelAllCompilerMarkers will remove marks placed by other linters
        if(m_mgr->GetActiveEditor()) {
            m_mgr->GetActiveEditor()->DelAllCompilerMarkers();
        }
        */
        phpcs::DoCheckFile(editor->GetFileName());
    }
}

void phpcs::DoCheckFile(const wxFileName& filename)
{
    m_output.Clear();

    // Build the command
    wxString command;
    command = "/usr/bin/php";
    ::WrapWithQuotes(command);

    wxString file = filename.GetFullPath();
    ::WrapWithQuotes(file);

    command << " /usr/bin/phpcs --report=xml " << file;

    // Run the check command
    m_process = ::CreateAsyncProcess(this, command);
    if(!m_process) {
        // failed to run the command
        CL_WARNING("phpcs: could not run command '%s'", command);
        DoProcessQueue();
        m_currentFileBeingProcessed.Clear();

    } else {
        CL_DEBUG("phpcs: running check: %s", command);
        m_currentFileBeingProcessed = filename.GetFullPath();
    }
}

void phpcs::DoProcessQueue()
{
    if(!m_process && !m_queue.empty()) {
        wxFileName filename = m_queue.front();
        m_queue.pop_front();
        DoCheckFile(filename);
    }
}

void phpcs::OnProcessTerminated(clProcessEvent& event)
{
    CL_DEBUG("phpcs: process terminated. output: %s", m_output);
    wxDELETE(m_process);
    CallAfter(&phpcs::PhpCSDone, m_output, m_currentFileBeingProcessed);
    // Check the queue for more files
    DoProcessQueue();
}

void phpcs::OnProcessOutput(clProcessEvent& event)
{
    m_output << event.GetOutput();
}

void phpcs::PhpCSDone(const wxString& lintOutput, const wxString& filename)
{
    // Find the editor
    CL_DEBUG("phpcs: searching editor for file: %s", filename);

    IEditor* editor = m_mgr->FindEditor(filename);
    CHECK_PTR_RET(editor);

    wxStringInputStream lintOutputStream(lintOutput);
    wxXmlDocument doc;
    if(!doc.Load(lintOutputStream))
        return;

    wxXmlNode* file = doc.GetRoot()->GetChildren();
    if(!file)
        return;

    wxXmlNode* violation = file->GetChildren();
    while(violation) {
        wxString strLine = violation->GetAttribute("line");
        wxString errorMessage = violation->GetNodeContent().Trim().Trim(false);

        long nLine(wxNOT_FOUND);
        if(strLine.ToCLong(&nLine)) {
            CL_DEBUG("phpcs: adding error marker @%d", (int)nLine - 1);

            if(violation->GetName() == "error") {
                editor->SetErrorMarker(nLine - 1, errorMessage);
            } else {
                editor->SetWarningMarker(nLine - 1, errorMessage);
            }
        }

        violation = violation->GetNext();
    }
}
