#include "asyncprocess.h"
#include "file_logger.h"
#include "globals.h"
#include "lintoptions.h"
#include "phplint.h"
#include "phplintdlg.h"
#include "phpoptions.h"
#include "processreaderthread.h"
#include "wx/menu.h"
#include <event_notifier.h>
#include <wx/msgdlg.h>
#include <wx/sstream.h>
#include <wx/xrc/xmlres.h>

static PHPLint* thePlugin = NULL;

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == NULL) { thePlugin = new PHPLint(manager); }
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

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

PHPLint::PHPLint(IManager* manager)
    : IPlugin(manager)
    , m_process(NULL)
{
    m_longName = _("Run code style checking on PHP source files");
    m_shortName = wxT("PHPLint");

    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &PHPLint::OnProcessOutput, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &PHPLint::OnProcessTerminated, this);

    m_settings.Load();
    m_settingsPhp.Load();

    m_mgr->GetTheApp()->Bind(wxEVT_MENU, &PHPLint::OnMenuRunLint, this, 2005);
    m_mgr->GetTheApp()->Bind(wxEVT_MENU, &PHPLint::OnMenuCommand, this, 2006);

    EventNotifier::Get()->Bind(wxEVT_FILE_LOADED, &PHPLint::OnLoadFile, this);
    EventNotifier::Get()->Bind(wxEVT_FILE_SAVED, &PHPLint::OnSaveFile, this);
    EventNotifier::Get()->Bind(wxEVT_PHP_SETTINGS_CHANGED, &PHPLint::OnPhpSettingsChanged, this);
}

PHPLint::~PHPLint() {}

void PHPLint::CreateToolBar(clToolBar* toolbar) { wxUnusedVar(toolbar); }

void PHPLint::CreatePluginMenu(wxMenu* pluginsMenu)
{
    wxMenu* menu = new wxMenu();
    wxMenuItem* item(NULL);
    item = new wxMenuItem(menu, 2005, _("Lint Current Source"), _("Lint Current Source"), wxITEM_NORMAL);
    menu->Append(item);
    menu->AppendSeparator();
    item = new wxMenuItem(menu, 2006, _("Options..."), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);
    pluginsMenu->Append(wxID_ANY, _("PHP Linter"), menu);
}

void PHPLint::OnMenuCommand(wxCommandEvent& e)
{
    wxUnusedVar(e);

    PHPLintDlg dlg(EventNotifier::Get()->TopFrame());
    if(dlg.ShowModal() == wxID_OK) {
        // Store the settings
        m_settings.SetLintOnFileLoad(dlg.GetCheckBoxLintOnLoad()->IsChecked())
            .SetLintOnFileSave(dlg.GetCheckBoxLintOnSave()->IsChecked())
            .SetPhpcsPhar(dlg.GetFilePickerPhpcsPhar()->GetFileName())
            .SetPhpmdPhar(dlg.GetFilePickerPhpmdPhar()->GetFileName())
            .SetPhpmdRules(dlg.GetFilePickerPhpmdRules()->GetFileName())
            .SetPhpstanPhar(dlg.GetFilePickerPhpstanPhar()->GetFileName())
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
    m_mgr->GetTheApp()->Unbind(wxEVT_MENU, &PHPLint::OnMenuRunLint, this, 2005);
    m_mgr->GetTheApp()->Unbind(wxEVT_MENU, &PHPLint::OnMenuCommand, this, 2006);

    EventNotifier::Get()->Unbind(wxEVT_FILE_LOADED, &PHPLint::OnLoadFile, this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_SAVED, &PHPLint::OnSaveFile, this);
    EventNotifier::Get()->Unbind(wxEVT_PHP_SETTINGS_CHANGED, &PHPLint::OnPhpSettingsChanged, this);
}

void PHPLint::OnLoadFile(clCommandEvent& e)
{
    e.Skip();

    if(!m_settings.IsLintOnFileLoad()) { return; }

    RunLint();
}

void PHPLint::OnSaveFile(clCommandEvent& e)
{
    e.Skip();

    if(!m_settings.IsLintOnFileSave()) { return; }

    RunLint();
}

void PHPLint::RunLint()
{
    IEditor* editor = m_mgr->GetActiveEditor();
    CHECK_PTR_RET(editor);

    if(FileExtManager::IsPHPFile(editor->GetFileName())) {
        if(m_mgr->GetActiveEditor()) { m_mgr->GetActiveEditor()->DelAllCompilerMarkers(); }
        PHPLint::DoCheckFile(editor->GetFileName());
    }
}

void PHPLint::DoCheckFile(const wxFileName& filename)
{
    wxString file = filename.GetFullPath();
    ::WrapWithQuotes(file);

    wxFileName php(m_settingsPhp.GetPhpExe());
    if(!php.Exists()) {
        clGetManager()->SetStatusMessage(_("PHPLint: can not lint file. Missing PHP executable path"), 5);
        return;
    }

    wxString phpPath = php.GetFullPath();
    ::WrapWithQuotes(phpPath);

    m_queue.push_back(phpPath + " -l " + file);
    QueuePhpcsCommand(phpPath, file);
    QueuePhpmdCommand(phpPath, file);
    QueuePhpstanCommand(phpPath, file);

    DoProcessQueue();
}

void PHPLint::QueuePhpcsCommand(const wxString& phpPath, const wxString& file)
{
    wxFileName phpcs(m_settings.GetPhpcsPhar());
    if(!phpcs.Exists()) {
        clDEBUG() << "PHPLint: Could not find the PHP-CS application. Ignoring" << clEndl;
        return;
    }

    wxString phpcsPath = phpcs.GetFullPath();
    ::WrapWithQuotes(phpcsPath);

    m_queue.push_back(phpPath + " " + phpcsPath + " --report=xml -q " + file);
}

void PHPLint::QueuePhpmdCommand(const wxString& phpPath, const wxString& file)
{
    wxFileName phpmd(m_settings.GetPhpmdPhar());
    if(!phpmd.Exists()) {
        clDEBUG() << "PHPLint: Could not find the PHPMD application. Ignoring" << clEndl;
        return;
    }

    wxString phpmdPath = phpmd.GetFullPath();
    ::WrapWithQuotes(phpmdPath);

    wxString phpmdRules(m_settings.GetPhpmdRules());
    if(phpmdRules.IsEmpty()) { phpmdRules = "cleancode,codesize,controversial,design,naming,unusedcode"; }
    ::WrapWithQuotes(phpmdRules);

    m_queue.push_back(phpPath + " " + phpmdPath + " " + file + " xml " + phpmdRules);
}

void PHPLint::QueuePhpstanCommand(const wxString& phpPath, const wxString& file)
{
    wxFileName phpstan(m_settings.GetPhpstanPhar());
    if(!phpstan.Exists()) {
        clDEBUG() << "PHPLint: Could not find the Phpstan application. Ignoring" << clEndl;
        return;
    }

    wxString phpstanPath = phpstan.GetFullPath();
    ::WrapWithQuotes(phpstanPath);

    m_queue.push_back(phpPath + " " + phpstanPath + " analyze --errorFormat=checkstyle --no-progress " + file);
}

void PHPLint::DoProcessQueue()
{
    if(!m_process && !m_queue.empty()) {
        wxString command = m_queue.front();
        m_queue.pop_front();
        DispatchCommand(command);
    }
}

void PHPLint::DispatchCommand(const wxString& command)
{
    // Run the check command
    m_output.clear();
    m_process = ::CreateAsyncProcess(this, command);
    if(!m_process) {
        // failed to run the command
        clWARNING() << "PHPLint: Could not run command:" << command << clEndl;
        DoProcessQueue();
    }
}

void PHPLint::OnProcessTerminated(clProcessEvent& event)
{
    clDEBUG() << "PHPLint: process terminated. output:" << m_output << clEndl;
    wxDELETE(m_process);
    CallAfter(&PHPLint::OnLintingDone, m_output.Clone());
    // Check the queue for more files
    DoProcessQueue();
}

void PHPLint::OnProcessOutput(clProcessEvent& event) { m_output << event.GetOutput(); }

void PHPLint::OnLintingDone(const wxString& lintOutput)
{
    if(lintOutput.Contains("Errors parsing ")) {
        ProcessPhpError(lintOutput);
        return;
    }

    ProcessXML(lintOutput);
}

void PHPLint::ProcessPhpError(const wxString& lintOutput)
{
    wxRegEx reLine("[ \t]*on line ([0-9]+)");
    // get the line number
    if(reLine.Matches(lintOutput)) {
        wxString strLine = reLine.GetMatch(lintOutput, 1);
        strLine.Trim().Trim(false);

        int start = lintOutput.Find("error:") + 6;
        int end = lintOutput.Find(" in ");
        wxString errorMessage = lintOutput.Mid(start, end - start);
        errorMessage.Trim().Trim(false);

        // Find the editor
        int fileStart = lintOutput.Find("Errors parsing ") + 15;
        wxString filename = lintOutput.Mid(fileStart);
        filename.Trim().Trim(false);

        clDEBUG() << "PHPLint: searching editor for file:" << filename << clEndl;
        IEditor* editor = m_mgr->FindEditor(filename);
        CHECK_PTR_RET(editor);

        MarkError(errorMessage, strLine, editor);
    }
}

void PHPLint::ProcessXML(const wxString& lintOutput)
{
    wxStringInputStream lintOutputStream(lintOutput);
    wxXmlDocument doc;
    if(!doc.Load(lintOutputStream)) return;

    wxXmlNode* file = doc.GetRoot()->GetChildren();
    if(!file) return;

    // Find the editor
    wxString filename = file->GetAttribute("name");
    clDEBUG() << "PHPLint: searching editor for file:" << filename << clEndl;
    IEditor* editor = m_mgr->FindEditor(filename);
    CHECK_PTR_RET(editor);

    wxString linter = doc.GetRoot()->GetName();

    wxXmlNode* violation = file->GetChildren();
    while(violation) {
        wxString errorMessage = violation->GetNodeContent();
        if(errorMessage.IsEmpty()) { errorMessage = violation->GetAttribute("message"); }
        wxString strLine = violation->GetAttribute(linter == "pmd" ? "beginline" : "line");
        bool isWarning = IsWarning(violation, linter);
        MarkError(errorMessage, strLine, editor, isWarning);

        violation = violation->GetNext();
    }
}

bool PHPLint::IsWarning(wxXmlNode* violation, const wxString& linter)
{
    if(linter == "pmd") {
        wxString priority = violation->GetAttribute("priority", "1");
        long nPriority(wxNOT_FOUND);
        priority.ToCLong(&nPriority);
        return (nPriority > 2);
    }

    if(linter == "checkstyle") {
        wxString priority = violation->GetAttribute("severity");
        return priority != "error";
    }

    return violation->GetName() == "warning";
}

void PHPLint::MarkError(wxString& errorMessage, const wxString& strLine, IEditor*& editor, bool isWarning)
{
    errorMessage = errorMessage.Trim().Trim(false);

    long nLine(wxNOT_FOUND);
    if(strLine.ToCLong(&nLine)) {
        clDEBUG() << "PHPLint: adding error marker @%d" << (nLine - 1) << clEndl;

        if(isWarning) {
            editor->SetWarningMarker(nLine - 1, errorMessage);
            return;
        }

        editor->SetErrorMarker(nLine - 1, errorMessage);
    }
}

void PHPLint::OnPhpSettingsChanged(clCommandEvent& event)
{
    event.Skip();
    m_settingsPhp.Load();
}
