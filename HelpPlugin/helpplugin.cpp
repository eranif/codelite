#include "helpplugin.h"
#include <wx/xrc/xmlres.h>
#include <wx/stc/stc.h>
#include "event_notifier.h"
#include "codelite_events.h"
#include <wx/uri.h>
#include <wx/xrc/xmlres.h>
#include "fileextmanager.h"
#include "clKeyboardManager.h"
#include <wx/msgdlg.h>
#include "HelpPluginMessageDlg.h"

static HelpPlugin* thePlugin = NULL;

// Define the plugin entry point
extern "C" EXPORT IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == 0) {
        thePlugin = new HelpPlugin(manager);
    }
    return thePlugin;
}

extern "C" EXPORT PluginInfo GetPluginInfo()
{
    PluginInfo info;
    info.SetAuthor(wxT("Eran Ifrah"));
    info.SetName(wxT("HelpPlugin"));
    info.SetDescription(wxT("Provide help based on selected words"));
    info.SetVersion(wxT("v1.0"));
    return info;
}

extern "C" EXPORT int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

HelpPlugin::HelpPlugin(IManager* manager)
    : IPlugin(manager)
{
    m_longName = _("Provide help based on selected words");
    m_shortName = _("HelpPlugin");
    wxTheApp->Bind(wxEVT_MENU, &HelpPlugin::OnHelp, this, XRCID("ID_ZEAL_HELP"));
    EventNotifier::Get()->Bind(wxEVT_CONTEXT_MENU_EDITOR, &HelpPlugin::OnEditorContextMenu, this);

    clKeyboardManager::Get()->AddGlobalAccelerator("ID_ZEAL_HELP", "F1", "Help::Search the docs for selected text");
}

HelpPlugin::~HelpPlugin() {}

clToolBar* HelpPlugin::CreateToolBar(wxWindow* parent)
{
    // Create the toolbar to be used by the plugin
    clToolBar* tb(NULL);
    return tb;
}

void HelpPlugin::CreatePluginMenu(wxMenu* pluginsMenu) {}

void HelpPlugin::UnPlug()
{
    wxTheApp->Unbind(wxEVT_MENU, &HelpPlugin::OnHelp, this, XRCID("ID_ZEAL_HELP"));
    EventNotifier::Get()->Unbind(wxEVT_CONTEXT_MENU_EDITOR, &HelpPlugin::OnEditorContextMenu, this);
}

void HelpPlugin::OnEditorContextMenu(clContextMenuEvent& event)
{
    event.Skip();
    IEditor* editor = m_mgr->GetActiveEditor();
    CHECK_PTR_RET(editor);

    if(!editor->GetCtrl()->HasSelection()) return;
    wxString selection = editor->GetCtrl()->GetSelectedText();

    // Get the context menu
    wxMenu* menu = event.GetMenu();
    wxBitmap helpBitmap = wxXmlResource::Get()->LoadBitmap("svn_info");
    menu->AppendSeparator();
    wxString label;
    label << _("Search the docs for '") << selection << "'";
    menu->Append(XRCID("ID_ZEAL_HELP"), label)->SetBitmap(helpBitmap);
    menu->AppendSeparator();
}

void HelpPlugin::OnHelp(wxCommandEvent& event)
{
    wxString query = DoBuildQueryString();
    if(query.IsEmpty()) return;
    if(!::wxLaunchDefaultApplication(query)) {
        HelpPluginMessageDlg dlg(EventNotifier::Get()->TopFrame());
        dlg.ShowModal();
    }
}

wxString HelpPlugin::DoBuildQueryString()
{
    IEditor* editor = m_mgr->GetActiveEditor();
    CHECK_PTR_RET_EMPTY_STRING(editor);
    
    // if no selection is available, just launch the help with an empty query
    // so Zeal will come to front
    if(!editor->GetCtrl()->HasSelection()) return "dash-plugin://";

    wxString selection = editor->GetCtrl()->GetSelectedText();
    
    // Auto detect the language
    wxString language;
    wxString label;
    FileExtManager::FileType type = FileExtManager::GetType(editor->GetFileName().GetFullName());
    switch(type) {
    case FileExtManager::TypeCMake:
        language << "cmake";
        break;
    case FileExtManager::TypeHeader:
    case FileExtManager::TypeSourceC:
    case FileExtManager::TypeSourceCpp:
        language << "cpp,net,boost,qt,cvcpp,cocos2dx,c,manpages";
        break;
    case FileExtManager::TypeHtml:
    case FileExtManager::TypeCSS:
    case FileExtManager::TypeJS:
        language
            << "html,svg,css,bootstrap,less,foundation,awesome,statamic,javascript,jquery,jqueryui,jquerym,angularjs,"
               "backbone,marionette,meteor,moo,prototype,ember,lodash,underscore,sencha,extjs,knockout,zepto,"
               "cordova,phonegap,yui";
        break;
    case FileExtManager::TypePhp:
        language << "php,wordpress,drupal,zend,laravel,yii,joomla,ee,codeigniter,cakephp,phpunit,symfony,typo3,twig,"
                    "smarty,phpp,html,statamic,mysql,sqlite,mongodb,psql,redis,zend framework 1,zend framework 2";
        break;
    case FileExtManager::TypeJava:
        language << "java,javafx,grails,groovy,playjava,spring,cvj,processing";
        break;
    }

    wxString q;
    if(!language.IsEmpty()) {
        // Build context aware search string
        q << "dash-plugin://keys=" << language << "&query=" << selection;
        wxURI uri(q);
        q = uri.BuildURI();
    } else {
        q << "dash-plugin://query=" << selection;
        wxURI uri(q);
        q = uri.BuildURI();
    }
    return q;
}
