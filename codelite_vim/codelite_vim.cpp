#include <iostream>
#include "codelite_vim.h"
#include <wx/xrc/xmlres.h>
#include <wx/app.h>
#include <wx/menu.h>
#include <wx/settings.h>
#include <wx/dialog.h>
#include "macros.h"
#include "vim_manager.h"
#include "VimSettingsDlg.h"
#include "event_notifier.h"
#include "VimSettings.h"

static CodeliteVim* thePlugin = NULL;

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == NULL) {
        thePlugin = new CodeliteVim(manager);
    }
    return thePlugin;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("bau"));
    info.SetName(wxT("CodeLite Vim"));
    info.SetDescription(_("vim bindings for CodeLite"));
    info.SetVersion(wxT("v1.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

CodeliteVim::CodeliteVim(IManager* manager)
    : IPlugin(manager)
{
    m_longName = _("vim bindings for CodeLite");
    m_shortName = wxT("CodeLite Vim");

    wxTheApp->Bind(wxEVT_MENU, &CodeliteVim::onVimSetting, this, XRCID("vim_settings"));
    
    // Load the settings from the file system    
    m_settings.Load();
    m_vimM = new VimManager(manager, m_settings);
}

CodeliteVim::~CodeliteVim() {}

clToolBar* CodeliteVim::CreateToolBar(wxWindow* parent)
{
    // Create the toolbar to be used by the plugin
    clToolBar* tb(NULL);

    return tb;
}

void CodeliteVim::CreatePluginMenu(wxMenu* pluginsMenu)
{
    wxMenu* menu = new wxMenu();
    menu->Append(new wxMenuItem(menu, XRCID("vim_settings"), _("Settings...")));
    pluginsMenu->Append(wxID_ANY, GetShortName(), menu);
    wxTheApp->Bind(wxEVT_MENU, &CodeliteVim::onVimSetting, this, XRCID("vim_settings"));
}

void CodeliteVim::UnPlug()
{
    wxTheApp->Unbind(wxEVT_MENU, &CodeliteVim::onVimSetting, this, XRCID("vim_settings"));
    wxDELETE(m_vimM);
}

void CodeliteVim::onVimSetting(wxCommandEvent& event) 
{
    VimSettingsDlg dlg(EventNotifier::Get()->TopFrame());
    if(dlg.ShowModal() == wxID_OK) {
        // Store the settings
        m_settings.SetEnabled(dlg.GetCheckBoxEnabled()->IsChecked()).Save();
        m_vimM->SettingsUpdated();
    }
}
