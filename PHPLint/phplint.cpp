#include "asyncprocess.h"
#include "file_logger.h"
#include "globals.h"
#include "phplint.h"
#include "processreaderthread.h"
#include <event_notifier.h>
#include <wx/sstream.h>
#include <wx/xrc/xmlres.h>
#include <wx/msgdlg.h>
#include "wx/menu.h"
#include "phplintdlg.h"
#include "lintoptions.h"
#include "lintphpoptions.h"

static PHPLint* thePlugin = NULL;

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == NULL) {
        thePlugin = new PHPLint(manager);
    }
    return thePlugin;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("Anders Jenbo"));
    info.SetName(wxT("PHPLint"));
    info.SetDescription(_("Run code style checking on PHP source files"));
    info.SetVersion(wxT("v1.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion()
{
    return PLUGIN_INTERFACE_VERSION;
}

PHPLint::PHPLint(IManager* manager)
    : IPlugin(manager)
{
    m_longName = _("Run code style checking on PHP source files");
    m_shortName = wxT("PHPLint");

    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &PHPLint::OnProcessOutput, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &PHPLint::OnProcessTerminated, this);

    m_settings.Load();
    m_settingsPhp.Load();
}

PHPLint::~PHPLint()
{
}

clToolBar* PHPLint::CreateToolBar(wxWindow* parent)
{
    // Create the toolbar to be used by the plugin
    clToolBar* tb(NULL);

    // Connect the events to us
    EventNotifier::Get()->Connect(wxEVT_FILE_LOADED, clCommandEventHandler(PHPLint::OnLoadFile), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_FILE_SAVED, clCommandEventHandler(PHPLint::OnSaveFile), NULL, this);

    return tb;
}

void PHPLint::CreatePluginMenu(wxMenu* pluginsMenu)
{
    wxMenu* menu = new wxMenu();
    wxMenuItem* item(NULL);
    item = new wxMenuItem(
        menu, 2005, _("Lint Current Source"), _("Lint Current Source"), wxITEM_NORMAL);
    menu->Append(item);
    menu->AppendSeparator();
    item = new wxMenuItem(menu, 2006, _("Options..."), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);
    pluginsMenu->Append(wxID_ANY, _("PHP Linter"), menu);


    menu->Connect(2005, wxEVT_COMMAND_MENU_SELECTED,
                  wxCommandEventHandler(PHPLint::OnMenuRunLint), NULL, (wxEvtHandler*)this);
    menu->Connect(2006, wxEVT_COMMAND_MENU_SELECTED,
                  wxCommandEventHandler(PHPLint::OnMenuCommand), NULL, (wxEvtHandler*)this);
}

void PHPLint::OnMenuCommand(wxCommandEvent& e)
{
    wxUnusedVar(e);

    PHPLintDlg dlg(EventNotifier::Get()->TopFrame());
    if(dlg.ShowModal() == wxID_OK) {
        // Store the settings
        m_settings
        .SetLintOnFileLoad(dlg.GetCheckBoxLintOnLoad()->IsChecked())
        .SetLintOnFileSave(dlg.GetCheckBoxLintOnSave()->IsChecked())
        .SetPhpcsPhar(dlg.GetFilePickerPhpcsPhar()->GetFileName())
        .SetPhpmdPhar(dlg.GetFilePickerPhpmdPhar()->GetFileName())
        .SetPhpmdRules(dlg.GetFilePickerPhpmdRules()->GetFileName())
        .Save();
    }
}

void PHPLint::OnMenuRunLint(wxCommandEvent& e)
{
    wxUnusedVar(e);

    RunLint();
}

void PHPLint::UnPlug()
{
    m_mgr->GetTheApp()->Disconnect(2005, wxEVT_COMMAND_MENU_SELECTED,
                                   wxCommandEventHandler(PHPLint::OnMenuRunLint), NULL, (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Disconnect(2006, wxEVT_COMMAND_MENU_SELECTED,
                                   wxCommandEventHandler(PHPLint::OnMenuCommand), NULL, (wxEvtHandler*)this);

    EventNotifier::Get()->Disconnect(wxEVT_FILE_LOADED, clCommandEventHandler(PHPLint::OnLoadFile), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_FILE_SAVED, clCommandEventHandler(PHPLint::OnSaveFile), NULL, this);
}

void PHPLint::OnLoadFile(clCommandEvent& e)
{
    e.Skip();

    if(!m_settings.IsLintOnFileLoad()) {
        return;
    }

    RunLint();
}

void PHPLint::OnSaveFile(clCommandEvent& e)
{
    e.Skip();

    if(!m_settings.IsLintOnFileSave()) {
        return;
    }

    RunLint();
}

void PHPLint::RunLint()
{
    IEditor* editor = m_mgr->GetActiveEditor();
    CHECK_PTR_RET(editor);

    if(FileExtManager::IsPHPFile(editor->GetFileName())) {
        if(m_mgr->GetActiveEditor()) {
            m_mgr->GetActiveEditor()->DelAllCompilerMarkers();
        }
        PHPLint::DoCheckFile(editor->GetFileName());
    }
}

void PHPLint::DoCheckFile(const wxFileName& filename)
{
    m_output.Clear();

    // Build the commands
    wxString command;

    wxString file = filename.GetFullPath();
    ::WrapWithQuotes(file);

    wxFileName php(m_settingsPhp.GetPhpExecutable());
    if(!php.Exists()) {
        ::wxMessageBox(_("Can not lint file: Missing PHP executable path"),
                       "PHPLint", wxICON_ERROR | wxOK | wxCENTER);
        return;
    }
    wxString phpPath = php.GetFullPath();
    ::WrapWithQuotes(phpPath);

    command = phpPath + " -l " + file;
    DispatchCommand(command, filename);

    wxFileName phpcs(m_settings.GetPhpcsPhar());
    if(phpcs.Exists()) {
        wxString phpcsPath = phpcs.GetFullPath();
        ::WrapWithQuotes(phpcsPath);

        command = phpPath + " " + phpcsPath + " --report=xml " + file;
        DispatchCommand(command, filename);
    }

    wxFileName phpmd(m_settings.GetPhpmdPhar());
    wxFileName phpmdRules(m_settings.GetPhpmdRules());
    if(phpmd.Exists() && phpmdRules.Exists()) {
        wxString phpmdPath = phpmd.GetFullPath();
        ::WrapWithQuotes(phpmdPath);
        wxString phpmdRulesPath = phpmdRules.GetFullPath();
        ::WrapWithQuotes(phpmdRulesPath);

        command = phpPath + " " + phpmdPath + " " + file + " xml " + phpmdRulesPath;
        DispatchCommand(command, filename);
    }
}

void PHPLint::DispatchCommand(const wxString& command, const wxFileName& filename)
{
    // Run the check command
    m_process = ::CreateAsyncProcess(this, command);
    if(!m_process) {
        // failed to run the command
        CL_WARNING("PHPLint: could not run command '%s'", command);
        DoProcessQueue();
        m_currentFileBeingProcessed.Clear();

    } else {
        CL_DEBUG("PHPLint: running check: %s", command);
        m_currentFileBeingProcessed = filename.GetFullPath();
    }
}

void PHPLint::DoProcessQueue()
{
    if(!m_process && !m_queue.empty()) {
        wxFileName filename = m_queue.front();
        m_queue.pop_front();
        DoCheckFile(filename);
    }
}

void PHPLint::OnProcessTerminated(clProcessEvent& event)
{
    CL_DEBUG("PHPLint: process terminated. output: %s", m_output);
    wxDELETE(m_process);
    wxString output = m_output.Clone();
    m_output.clear();
    CallAfter(&PHPLint::OnLintingDone, output, m_currentFileBeingProcessed);
    // Check the queue for more files
    DoProcessQueue();
}

void PHPLint::OnProcessOutput(clProcessEvent& event)
{
    m_output << event.GetOutput();
}

void PHPLint::OnLintingDone(const wxString& lintOutput, const wxString& filename)
{
    // Find the editor
    CL_DEBUG("PHPLint: searching editor for file: %s", filename);

    IEditor* editor = m_mgr->FindEditor(filename);
    CHECK_PTR_RET(editor);

    if (lintOutput.Contains("Errors parsing")) {
        ProcessPhpError(lintOutput, editor);
        return;
    }

    ProcessXML(lintOutput, editor);
}

void PHPLint::ProcessPhpError(const wxString& lintOutput, IEditor*& editor)
{
    wxRegEx reLine("[ \t]*on line ([0-9]+)");
    // get the line number
    if(reLine.Matches(lintOutput)) {
        wxString strLine = reLine.GetMatch(lintOutput, 1);

        int start = lintOutput.Find("error:");
        int end = lintOutput.Find(" in ");
        wxString errorMessage = lintOutput.Mid(start + 6, end - 6 - start);

        MarkError(errorMessage, strLine, editor);
    }
}

void PHPLint::ProcessXML(const wxString& lintOutput, IEditor*& editor)
{
    wxStringInputStream lintOutputStream(lintOutput);
    wxXmlDocument doc;
    if(!doc.Load(lintOutputStream))
        return;

    wxXmlNode* file = doc.GetRoot()->GetChildren();
    if(!file)
        return;

    wxString linter = doc.GetRoot()->GetName();

    wxXmlNode* violation = file->GetChildren();
    while(violation) {
        wxString errorMessage = violation->GetNodeContent();
        wxString strLine = violation->GetAttribute(linter == "pmd" ? "beginline" : "line");
        bool isWarning = IsWarning(violation, linter);
        MarkError(errorMessage, strLine, editor, isWarning);

        violation = violation->GetNext();
    }
}

bool PHPLint::IsWarning(wxXmlNode* violation, const wxString& linter)
{
    if (linter == "pmd") {
        wxString priority = violation->GetAttribute("priority", "1");
        long nPriority(wxNOT_FOUND);
        return priority.ToCLong(&nPriority) > 2;
    }

    return violation->GetName() == "warning";
}

void PHPLint::MarkError(wxString& errorMessage, const wxString& strLine, IEditor*& editor, bool isWarning)
{
    errorMessage = errorMessage.Trim().Trim(false);

    long nLine(wxNOT_FOUND);
    if(strLine.ToCLong(&nLine)) {
        CL_DEBUG("PHPLint: adding error marker @%d", (int)nLine - 1);

        if(isWarning) {
            editor->SetWarningMarker(nLine - 1, errorMessage);
            return;
        }

        editor->SetErrorMarker(nLine - 1, errorMessage);
    }
}
