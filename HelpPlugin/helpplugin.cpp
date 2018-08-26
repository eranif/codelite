#include "HelpPluginMessageDlg.h"
#include "HelpPluginSettings.h"
#include "HelpPluginSettingsDlg.h"
#include "clKeyboardManager.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileextmanager.h"
#include "fileutils.h"
#include "helpplugin.h"
#include <wx/msgdlg.h>
#include <wx/stc/stc.h>
#include <wx/uri.h>
#include <wx/xrc/xmlres.h>

static HelpPlugin* thePlugin = NULL;

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == 0) { thePlugin = new HelpPlugin(manager); }
    return thePlugin;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("Eran Ifrah"));
    info.SetName(wxT("HelpPlugin"));
    info.SetDescription(wxT("Provide help based on selected words"));
    info.SetVersion(wxT("v1.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

HelpPlugin::HelpPlugin(IManager* manager)
    : IPlugin(manager)
{
    m_longName = _("Provide help based on selected words");
    m_shortName = _("HelpPlugin");
    wxTheApp->Bind(wxEVT_MENU, &HelpPlugin::OnHelp, this, XRCID("ID_ZEAL_HELP"));
    Bind(wxEVT_MENU, &HelpPlugin::OnHelpSettings, this, XRCID("ID_ZEAL_SETTINGS"));
    EventNotifier::Get()->Bind(wxEVT_CONTEXT_MENU_EDITOR, &HelpPlugin::OnEditorContextMenu, this);
    clKeyboardManager::Get()->AddGlobalAccelerator("ID_ZEAL_HELP", "F1", "Help::Search the docs for selected text");
}

HelpPlugin::~HelpPlugin() {}

void HelpPlugin::CreateToolBar(clToolBar* toolbar) { wxUnusedVar(toolbar); }

void HelpPlugin::CreatePluginMenu(wxMenu* pluginsMenu)
{
    wxMenu* menu = new wxMenu;
    menu->Append(XRCID("ID_ZEAL_SETTINGS"), _("Settings..."));
    pluginsMenu->Append(wxID_ANY, _("Help Plugin"), menu);
    menu->SetNextHandler(this);
    this->SetPreviousHandler(menu);
}

void HelpPlugin::UnPlug()
{
    wxTheApp->Unbind(wxEVT_MENU, &HelpPlugin::OnHelp, this, XRCID("ID_ZEAL_HELP"));
    Unbind(wxEVT_MENU, &HelpPlugin::OnHelpSettings, this, XRCID("ID_ZEAL_SETTINGS"));
    EventNotifier::Get()->Unbind(wxEVT_CONTEXT_MENU_EDITOR, &HelpPlugin::OnEditorContextMenu, this);
}

void HelpPlugin::OnEditorContextMenu(clContextMenuEvent& event)
{
    event.Skip();
    IEditor* editor = m_mgr->GetActiveEditor();
    CHECK_PTR_RET(editor);

    if(!editor->GetCtrl()->HasSelection()) return;
    wxString selection = editor->GetCtrl()->GetSelectedText();
    wxString modSelection = selection.BeforeFirst('\n');
    modSelection.Trim().Trim(false);
    if(modSelection.IsEmpty()) return;

    // Ensure we only use 15 chars of the selected text, otherwise the menu label
    // will overflow
    if(modSelection.length() > 15) { modSelection = modSelection.Mid(0, 15); }
    if(selection.Contains("\n")) { modSelection << "..."; }

    // Get the context menu
    wxMenu* menu = event.GetMenu();
    wxBitmap helpBitmap = wxXmlResource::Get()->LoadBitmap("svn_info");
    menu->AppendSeparator();
    wxString label;
    label << _("Search the docs for '") << modSelection << "'";
    menu->Append(XRCID("ID_ZEAL_HELP"), label)->SetBitmap(helpBitmap);
    menu->AppendSeparator();
}

void HelpPlugin::OnHelp(wxCommandEvent& event)
{
    wxUnusedVar(event);
    CallAfter(&HelpPlugin::DoHelp);
}

wxString HelpPlugin::DoBuildQueryString()
{
    IEditor* editor = m_mgr->GetActiveEditor();
    CHECK_PTR_RET_EMPTY_STRING(editor);

    // if no selection is available, just launch the help with an empty query
    // so Zeal will come to front
    if(!editor->GetCtrl()->HasSelection()) return "dash-plugin://";

    wxString selection = editor->GetCtrl()->GetSelectedText();

    HelpPluginSettings settings;
    settings.Load();

    // Auto detect the language
    wxString language;
    wxString label;
    FileExtManager::FileType type = FileExtManager::GetType(editor->GetFileName().GetFullName());
    switch(type) {
    case FileExtManager::TypeCMake:
        language << settings.GetCmakeDocset();
        break;
    case FileExtManager::TypeHeader:
    case FileExtManager::TypeSourceC:
    case FileExtManager::TypeSourceCpp:
        language << settings.GetCxxDocset();
        break;
    case FileExtManager::TypeHtml:
        language << settings.GetHtmlDocset();
        break;
    case FileExtManager::TypeCSS:
        language << settings.GetCssDocset();
        break;
    case FileExtManager::TypeJS:
        language << settings.GetJsDocset();
        break;
    case FileExtManager::TypePhp:
        language << settings.GetPhpDocset();
        break;
    case FileExtManager::TypeJava:
        language << settings.GetJavaDocset();
        break;
    default:
        break;
    }

    wxString q;
    if(!language.IsEmpty()) {
        // Build context aware search string
        q << "dash-plugin://keys=" << language << "&query=" << selection;
    } else {
        q << "dash-plugin://query=" << selection;
    }
    q = FileUtils::EncodeURI(q);
    return q;
}

void HelpPlugin::OnHelpSettings(wxCommandEvent& event)
{
    wxUnusedVar(event);
    HelpPluginSettingsDlg dlg(EventNotifier::Get()->TopFrame());
    dlg.ShowModal();
}

void HelpPlugin::DoHelp()
{
    wxString query = DoBuildQueryString();
    if(query.IsEmpty()) return;
#ifdef __WXGTK__
    wxFileName fnZeal("/usr/bin", "zeal");
    if(!fnZeal.Exists()) {
        HelpPluginMessageDlg dlg(EventNotifier::Get()->TopFrame());
        dlg.ShowModal();
    }
    wxString command;
    command << fnZeal.GetFullPath() << " "
            << "\"" << query << "\"";
    clDEBUG() << "Help Plugin:" << command << clEndl;
    ::wxExecute(command);
#else
    clDEBUG() << "Help Plugin:" << query << clEndl;
    if(!::wxLaunchDefaultBrowser(query)) {
        HelpPluginMessageDlg dlg(EventNotifier::Get()->TopFrame());
        dlg.ShowModal();
    }
#endif
}
