#include "phprefactoring.h"
#include "asyncprocess.h"
#include "clEditorStateLocker.h"
#include "clPatch.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileutils.h"
#include "globals.h"
#include "phpoptions.h"
#include "phprefactoringdlg.h"
#include "phprefactoringoptions.h"
#include "string"
#include <wx/filename.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/textdlg.h>
#include <wx/xrc/xmlres.h>

class to_string;
static PHPRefactoring* thePlugin = NULL;

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == NULL) {
        thePlugin = new PHPRefactoring(manager);
    }
    return thePlugin;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("Anders Jenbo"));
    info.SetName(wxT("PHPRefactoring"));
    info.SetDescription(_("Uses PHP Refactoring Browser to provide refactoring capabilities for php"));
    info.SetVersion(wxT("v1.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

PHPRefactoring::PHPRefactoring(IManager* manager)
    : IPlugin(manager)
{
    m_manager = manager;
    m_longName = _("Uses PHP Refactoring Browser to provide refactoring capabilities for php");
    m_shortName = wxT("PHPRefactoring");
    m_settings.Load();
    m_settingsPhp.Load();
}

PHPRefactoring::~PHPRefactoring() {}

clToolBar* PHPRefactoring::CreateToolBar(wxWindow* parent)
{
    // Create the toolbar to be used by the plugin
    clToolBar* tb(NULL);
    return tb;
}

void PHPRefactoring::CreatePluginMenu(wxMenu* pluginsMenu)
{
    wxMenu* menu = new wxMenu();
    wxMenuItem* item(NULL);
    item = new wxMenuItem(menu, wxID_SETTINGS, _("Options..."), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);
    pluginsMenu->Append(wxID_ANY, _("PHP Refactoring"), menu);

    wxTheApp->Bind(wxEVT_MENU, &PHPRefactoring::OnMenuCommand, this, wxID_SETTINGS);
    wxTheApp->Bind(wxEVT_MENU, &PHPRefactoring::OnExtractMethod, this, wxID_EXTRACT_METHOD);
    wxTheApp->Bind(wxEVT_MENU, &PHPRefactoring::OnRenameLocalVariable, this, wxID_RENAME_LOCAL_VARIABLE);
    wxTheApp->Bind(wxEVT_MENU, &PHPRefactoring::OnRenameClassProperty, this, wxID_RENAME_CLASS_PROPERTY);
    wxTheApp->Bind(wxEVT_MENU, &PHPRefactoring::OnConvertLocalToInstanceVariable, this,
                   wxID_CONVERT_LOCAL_TO_INSTANCE_VARIABLE);
    wxTheApp->Bind(wxEVT_MENU, &PHPRefactoring::OnRenameClassAndNamespaces, this, wxID_RENAME_CLASS_AND_NAMESPACES);
    wxTheApp->Bind(wxEVT_MENU, &PHPRefactoring::OnOptimizeUseStatements, this, wxID_OPTIMIZE_USE_STATEMENTS);
    EventNotifier::Get()->Bind(wxEVT_CONTEXT_MENU_EDITOR, &PHPRefactoring::OnEditorContextMenu, this);
    EventNotifier::Get()->Bind(wxEVT_PHP_SETTINGS_CHANGED, &PHPRefactoring::OnPhpSettingsChanged, this);
}

void PHPRefactoring::UnPlug()
{
    wxTheApp->Unbind(wxEVT_MENU, &PHPRefactoring::OnMenuCommand, this, wxID_SETTINGS);
    wxTheApp->Unbind(wxEVT_MENU, &PHPRefactoring::OnExtractMethod, this, wxID_EXTRACT_METHOD);
    wxTheApp->Unbind(wxEVT_MENU, &PHPRefactoring::OnRenameLocalVariable, this, wxID_RENAME_LOCAL_VARIABLE);
    wxTheApp->Unbind(wxEVT_MENU, &PHPRefactoring::OnRenameClassProperty, this, wxID_RENAME_CLASS_PROPERTY);
    wxTheApp->Unbind(wxEVT_MENU, &PHPRefactoring::OnConvertLocalToInstanceVariable, this,
                     wxID_CONVERT_LOCAL_TO_INSTANCE_VARIABLE);
    wxTheApp->Unbind(wxEVT_MENU, &PHPRefactoring::OnRenameClassAndNamespaces, this, wxID_RENAME_CLASS_AND_NAMESPACES);
    wxTheApp->Unbind(wxEVT_MENU, &PHPRefactoring::OnOptimizeUseStatements, this, wxID_OPTIMIZE_USE_STATEMENTS);
    EventNotifier::Get()->Unbind(wxEVT_CONTEXT_MENU_EDITOR, &PHPRefactoring::OnEditorContextMenu, this);
    EventNotifier::Get()->Unbind(wxEVT_PHP_SETTINGS_CHANGED, &PHPRefactoring::OnPhpSettingsChanged, this);
}

void PHPRefactoring::OnEditorContextMenu(clContextMenuEvent& event)
{
    event.Skip();

    IEditor* editor = m_manager->GetActiveEditor();
    if(!editor || !FileExtManager::IsPHPFile(editor->GetFileName())) {
        return; // Not in php file, do not add options to context menu
    }

    wxMenu* refactoringMenu = new wxMenu;
    refactoringMenu->Append(wxID_EXTRACT_METHOD, _("Extract Method"));
    refactoringMenu->Append(wxID_RENAME_LOCAL_VARIABLE, _("Rename Local Variable"));
    refactoringMenu->Append(wxID_RENAME_CLASS_PROPERTY, _("Rename Class Property"));
    refactoringMenu->Append(wxID_CONVERT_LOCAL_TO_INSTANCE_VARIABLE, _("Convert Local to Instance Variable"));
    refactoringMenu->Append(wxID_RENAME_CLASS_AND_NAMESPACES, _("Rename Class and Namespaces"));
    refactoringMenu->Append(wxID_OPTIMIZE_USE_STATEMENTS, _("Optimize use statements"));

    event.GetMenu()->AppendSeparator();
    event.GetMenu()->Append(wxID_ANY, _("Refactoring"), refactoringMenu);
}

void PHPRefactoring::OnMenuCommand(wxCommandEvent& e)
{
    wxUnusedVar(e);

    PHPRefactoringDlg dlg(EventNotifier::Get()->TopFrame());
    if(dlg.ShowModal() == wxID_OK) {
        // Store the settings
        m_settings.SetPhprefactoringPhar(dlg.GetFilePickerPhprefactoringPhar()->GetFileName()).Save();
    }
}

void PHPRefactoring::OnExtractMethod(wxCommandEvent& e)
{
    IEditor* editor = m_manager->GetActiveEditor();
    if(!editor) {
        return;
    }

    int startLine = editor->LineFromPos(editor->GetSelectionStart()) + 1;
    int endLine = editor->LineFromPos(editor->GetSelectionEnd()) + 1;
    wxString method = wxGetTextFromUser("Name the new methode");
    if(method.IsEmpty()) {
        return;
    }

    if(method.Contains(" ")) {
        ::wxMessageBox(_("Methode name may not contain spaces"), "PHPRefactoring", wxICON_ERROR | wxOK | wxCENTER);
        return;
    }

    std::string range = std::to_string(startLine) + "-" + std::to_string(endLine);
    wxString parameters = range + " " + method;
    RefactorFile("extract-method", parameters, editor);
}

void PHPRefactoring::OnRenameLocalVariable(wxCommandEvent& e) { RenameVariable("rename-local-variable"); }

void PHPRefactoring::OnRenameClassProperty(wxCommandEvent& e) { RenameVariable("rename-property"); }

void PHPRefactoring::RenameVariable(const wxString& action)
{
    IEditor* editor = m_manager->GetActiveEditor();
    if(!editor) {
        return;
    }

    wxString line = std::to_string(editor->GetCurrentLine() + 1);
    wxString oldName = editor->GetWordAtCaret();
    if(oldName.StartsWith("$")) {
        oldName = oldName.Mid(1);
    }
    if(oldName.IsEmpty()) {
        return;
    }

    wxString newName = wxGetTextFromUser("New name for " + oldName);
    newName.Trim().Trim(false);

    // If it starts with $ sign, remove it
    if(newName.StartsWith("$")) {
        newName = newName.Mid(1);
    }

    // Sanity
    if(newName.IsEmpty()) {
        return;
    }

    wxString parameters = line + " " + oldName + " " + newName;
    RefactorFile(action, parameters, editor);
}

void PHPRefactoring::OnConvertLocalToInstanceVariable(wxCommandEvent& e)
{
    IEditor* editor = m_manager->GetActiveEditor();
    if(!editor) {
        return;
    }

    wxString line = std::to_string(editor->GetCurrentLine() + 1);
    wxString oldName = editor->GetWordAtCaret();
    if(oldName.StartsWith("$")) {
        oldName = oldName.Mid(1);
    }
    if(oldName.IsEmpty()) {
        return;
    }

    wxString parameters = line + " " + oldName;
    RefactorFile("convert-local-to-instance-variable", parameters, editor);
}

void PHPRefactoring::OnRenameClassAndNamespaces(wxCommandEvent& e)
{
    IEditor* editor = m_manager->GetActiveEditor();
    if(!editor) {
        return;
    }

    wxString path = editor->GetFileName().GetPath();
    RunCommand("fix-class-names " + path);
}

void PHPRefactoring::OnOptimizeUseStatements(wxCommandEvent& e)
{
    IEditor* editor = m_manager->GetActiveEditor();
    if(!editor) {
        return;
    }

    RefactorFile("optimize-use", "", editor);
}

void PHPRefactoring::RefactorFile(const wxString& action, const wxString& extraParameters, IEditor*& editor)
{
    wxString filePath, tmpfile, parameters, output;
    filePath = editor->GetFileName().GetFullPath();
    tmpfile << filePath << "-refactoring-browser.php";
    wxString oldContent = editor->GetEditorText();
    if(!FileUtils::WriteFileContent(tmpfile, oldContent)) {
        ::wxMessageBox(_("Can not refactor file:\nFailed to write temporary file"), "PHP Refactoring",
                       wxICON_ERROR | wxOK | wxCENTER);
        return;
    }

    // Ensure that the temporary file is deleted once we are done with it
    FileUtils::Deleter fd(tmpfile);

    ::WrapWithQuotes(tmpfile);
    parameters = action + " " + tmpfile + " " + extraParameters;

    // Notify about indentation about to start
    wxCommandEvent evt(wxEVT_CODEFORMATTER_INDENT_STARTING);
    evt.SetString(filePath);
    EventNotifier::Get()->ProcessEvent(evt);

    m_manager->SetStatusMessage(wxString::Format(wxT("%s: %s..."), _("Refactoring"), filePath.c_str()), 0);

    RunCommand(parameters);

    if(!FileUtils::ReadFileContent(tmpfile, output)) {
        ::wxMessageBox(_("Can not refactor file:\nfailed to read temporary file content"), "PHP Refactoring",
                       wxICON_ERROR | wxOK | wxCENTER);
        return;
    }

    if(oldContent != output) {
        // Update the editor
        clEditorStateLocker lk(editor->GetCtrl());
        editor->GetCtrl()->BeginUndoAction();
        editor->SetEditorText(output);
        editor->GetCtrl()->EndUndoAction();
    }

    m_manager->SetStatusMessage(_("Done"), 0);

    // Notify that a file was indented
    wxCommandEvent evtDone(wxEVT_CODEFORMATTER_INDENT_COMPLETED);
    evtDone.SetString(filePath);
    EventNotifier::Get()->AddPendingEvent(evtDone);
}

void PHPRefactoring::RunCommand(const wxString& parameters)
{
    wxString phpPath, refactorPath, command;

    wxFileName php(m_settingsPhp.GetPhpExe());
    if(!php.Exists()) {
        ::wxMessageBox(_("Can not refactor file: Missing PHP executable path"), "PHP Refactoring",
                       wxICON_ERROR | wxOK | wxCENTER);
        return;
    }
    phpPath = php.GetFullPath();
    ::WrapWithQuotes(phpPath);

    wxFileName refactor(m_settings.GetPhprefactoringPhar());
    if(!refactor.Exists()) {
        ::wxMessageBox(_("Can not refactor file: Missing PHP Refactoring Browser path"), "PHP Refactoring",
                       wxICON_ERROR | wxOK | wxCENTER);
        return;
    }
    refactorPath = refactor.GetFullPath();
    ::WrapWithQuotes(refactorPath);

    command = phpPath + " " + refactorPath + " " + parameters;
    clDEBUG() << "PHPRefactoring running:" << command << clEndl;
    ::WrapInShell(command);
    IProcess::Ptr_t process(::CreateSyncProcess(command, IProcessCreateDefault | IProcessCreateWithHiddenConsole));
    // CHECK_PTR_RET_FALSE(process);

    wxString patch, tmpfile;
    process->WaitForTerminate(patch);
    clDEBUG() << "PHPRefactoring ouput:" << patch << clEndl;
    if (!patch.StartsWith("--- a/")) { // not a patch
        if (patch.Contains("RefactoringException")) { // has an error exception
            int start = patch.Find("RefactoringException");
            wxString errorMessage = patch.Mid(start).AfterFirst('\n').BeforeFirst('\n');
            errorMessage = errorMessage.Trim().Trim(false);
            ::wxMessageBox(errorMessage, "PHP Refactoring", wxICON_ERROR | wxOK | wxCENTER);
        }
        return;
    }


    int fd1;
    char name[] = "/tmp/diff-XXXXXX";
    fd1 = mkstemp(name);
    tmpfile = name;
    if(!FileUtils::WriteFileContent(tmpfile, patch)) {
        ::wxMessageBox(_("Can not refactor file:\nFailed to write temporary file"), "PHP Refactoring",
                       wxICON_ERROR | wxOK | wxCENTER);
        return;
    }
    FileUtils::Deleter fd(tmpfile);

    // Apply the patch
    try {
        clPatch patcher;
        patcher.Patch(tmpfile, "", "-p1 < ");
    } catch(clException& e) {
        wxMessageBox(e.What(), "CodeLite", wxICON_ERROR | wxOK | wxCENTER, EventNotifier::Get()->TopFrame());
    }
}

void PHPRefactoring::OnPhpSettingsChanged(clCommandEvent& event)
{
    event.Skip();
    m_settingsPhp.Load();
}
