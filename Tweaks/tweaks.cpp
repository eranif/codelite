#include "tweaks.h"
#include <wx/xrc/xmlres.h>
#include "event_notifier.h"
#include "TweaksSettingsDlg.h"
#include "editor_config.h"
#include "workspace.h"

static Tweaks* thePlugin = NULL;

static int ID_TWEAKS_SETTINGS = ::wxNewId();

//Define the plugin entry point
extern "C" EXPORT IPlugin *CreatePlugin(IManager *manager)
{
    if (thePlugin == 0) {
        thePlugin = new Tweaks(manager);
    }
    return thePlugin;
}

extern "C" EXPORT PluginInfo GetPluginInfo()
{
    PluginInfo info;
    info.SetAuthor(wxT("eran"));
    info.SetName(wxT("Tweaks"));
    info.SetDescription(wxT("Tweak codelite"));
    info.SetVersion(wxT("v1.0"));
    return info;
}

extern "C" EXPORT int GetPluginInterfaceVersion()
{
    return PLUGIN_INTERFACE_VERSION;
}

Tweaks::Tweaks(IManager *manager)
    : IPlugin(manager)
{
    m_longName = wxT("Tweak codelite");
    m_shortName = wxT("Tweaks");
    
    m_mgr->GetTheApp()->Connect(ID_TWEAKS_SETTINGS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Tweaks::OnSettings), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_COLOUR_TAB, clColourEventHandler(Tweaks::OnColourTab), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(Tweaks::OnWorkspaceLoaded), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(Tweaks::OnWorkspaceClosed), NULL, this);
}

void Tweaks::UnPlug()
{
    m_mgr->GetTheApp()->Disconnect(ID_TWEAKS_SETTINGS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Tweaks::OnSettings), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_COLOUR_TAB, clColourEventHandler(Tweaks::OnColourTab), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(Tweaks::OnWorkspaceLoaded), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(Tweaks::OnWorkspaceClosed), NULL, this);
}

Tweaks::~Tweaks()
{
}

clToolBar *Tweaks::CreateToolBar(wxWindow *parent)
{
    // Create the toolbar to be used by the plugin
    clToolBar *tb(NULL);
    return tb;
}

void Tweaks::CreatePluginMenu(wxMenu *pluginsMenu)
{
    wxUnusedVar(pluginsMenu);
    wxMenu *menu = new wxMenu;
    menu->Append(ID_TWEAKS_SETTINGS, _("Settings..."));
    pluginsMenu->AppendSubMenu(menu, _("Tweaks Plugin"));
}

void Tweaks::HookPopupMenu(wxMenu *menu, MenuType type)
{
    wxUnusedVar(menu);
    wxUnusedVar(type);
}

void Tweaks::UnHookPopupMenu(wxMenu *menu, MenuType type)
{
    wxUnusedVar(menu);
    wxUnusedVar(type);
}

void Tweaks::OnSettings(wxCommandEvent& e)
{
    wxUnusedVar(e);
    TweaksSettingsDlg dlg( m_mgr->GetTheApp()->GetTopWindow() );
    dlg.ShowModal();
    m_settings.Load(); // Refresh our cached settings
    
    // Refresh the drawings
    m_mgr->GetTheApp()->GetTopWindow()->Refresh();
}

void Tweaks::OnColourTab(clColourEvent& e)
{
    if ( !m_settings.IsEnableTweaks() || !WorkspaceST::Get()->IsOpen() ) {
        e.Skip();
        return;
    }
    
    IEditor* editor = FindEditorByPage( e.GetPage() );
    if ( !editor ) {
        e.Skip();
    
    } else {
        
        const ProjectTweaks& tw = m_settings.GetProjectTweaks( editor->GetProjectName() );
        if ( tw.IsOk() ) {
            e.SetBgColour( e.IsActiveTab() ? EditorConfigST::Get()->GetCurrentOutputviewBgColour() : tw.GetTabBgColour() );
            e.SetFgColour( e.IsActiveTab() ? EditorConfigST::Get()->GetCurrentOutputviewFgColour() : tw.GetTabFgColour() );
            
        } else if ( m_settings.GetGlobalBgColour().IsOk() && m_settings.GetGlobalFgColour().IsOk() ) {
            e.SetBgColour( e.IsActiveTab() ? EditorConfigST::Get()->GetCurrentOutputviewBgColour() : m_settings.GetGlobalBgColour() );
            e.SetFgColour( e.IsActiveTab() ? EditorConfigST::Get()->GetCurrentOutputviewFgColour() : m_settings.GetGlobalFgColour() );
            
        } else {
            e.Skip();
        }
    
    }
}

IEditor* Tweaks::FindEditorByPage(wxWindow* page)
{
    for(size_t i=0; i<m_mgr->GetPageCount(); ++i) {
        if ( m_mgr->GetPage(i) == page ) {
            return dynamic_cast<IEditor*>( m_mgr->GetPage(i) );
        }
    }
    return NULL;
}

void Tweaks::OnWorkspaceLoaded(wxCommandEvent& e)
{
    e.Skip();
    // Refresh the list with the current workspace setup
    m_settings.Load();
}

void Tweaks::OnWorkspaceClosed(wxCommandEvent& e)
{
    e.Skip();
    m_settings.Clear();
}
