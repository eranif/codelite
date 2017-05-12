#include <iostream>
#include "codelite_vim.h"
#include <wx/xrc/xmlres.h>
#include <wx/app.h>
#include <wx/menu.h>
#include <wx/settings.h>
#include <wx/dialog.h>
#include "macros.h"
#include "vim_manager.h"

static CodeliteVim* thePlugin = NULL;



// Define the plugin entry point
CL_PLUGIN_API IPlugin *CreatePlugin(IManager *manager)
{
    if (thePlugin == NULL) {
        thePlugin = new CodeliteVim(manager);
    }
    return thePlugin;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("bau"));
    info.SetName(wxT("CodeliteVim"));
    info.SetDescription(_("vim bindings for codelite"));
    info.SetVersion(wxT("v1.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion()
{
    return PLUGIN_INTERFACE_VERSION;
}

CodeliteVim::CodeliteVim(IManager *manager)
    : IPlugin(manager)
{
    m_longName = _("vim bindings for codelite");
    m_shortName = wxT("CodeliteVim");

	wxTheApp->Bind(wxEVT_MENU, &CodeliteVim::onVimSetting, this, XRCID("vim_binds"));
	
	m_vimM = new VimManager();
}

CodeliteVim::~CodeliteVim()
{
}

clToolBar *CodeliteVim::CreateToolBar(wxWindow *parent)
{
    // Create the toolbar to be used by the plugin
    clToolBar *tb(NULL);
	
    return tb;
}

void CodeliteVim::CreatePluginMenu(wxMenu *pluginsMenu)
{
    wxMenu* menu = new wxMenu();
    // menu->Append(XRCID("vim_binds"), _("Use vim key bindings"));
	// menu->AppendSeparator();
	menu->Append(XRCID("vim_settings"), _("Settings"));
	pluginsMenu->Append(wxID_ANY, GetShortName(), menu);
}

void CodeliteVim::UnPlug()
{
	wxTheApp->Unbind(wxEVT_MENU, &CodeliteVim::onVimSetting, this, XRCID("vim_binds"));

}


void CodeliteVim::onVimSetting(wxCommandEvent &event)
{

}

