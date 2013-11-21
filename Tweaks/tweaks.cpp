#include "tweaks.h"
#include <wx/xrc/xmlres.h>

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

void Tweaks::UnPlug()
{
    m_mgr->GetTheApp()->Disconnect(ID_TWEAKS_SETTINGS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Tweaks::OnSettings), NULL, this);
}

void Tweaks::OnSettings(wxCommandEvent& e)
{
    wxUnusedVar(e);
}
