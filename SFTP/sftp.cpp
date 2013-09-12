#include "sftp.h"
#include <wx/xrc/xmlres.h>
#include <wx/menu.h>
#include "SSHAccountManagerDlg.h"
#include "sftp_settings.h"
#include "SFTPBrowserDlg.h"

static SFTP* thePlugin = NULL;

const wxEventType wxEVT_SFTP_OPEN_SSH_ACCOUNT_MANAGER  = ::wxNewEventType();
const wxEventType wxEVT_SFTP_SETUP_WORKSPACE_MIRRORING = ::wxNewEventType();


//Define the plugin entry point
extern "C" EXPORT IPlugin *CreatePlugin(IManager *manager)
{
    if (thePlugin == 0) {
        thePlugin = new SFTP(manager);
    }
    return thePlugin;
}

extern "C" EXPORT PluginInfo GetPluginInfo()
{
    PluginInfo info;
    info.SetAuthor(wxT("Eran Ifrah"));
    info.SetName(wxT("SFTP"));
    info.SetDescription(wxT("SFTP plugin for codelite IDE"));
    info.SetVersion(wxT("v1.0"));
    return info;
}

extern "C" EXPORT int GetPluginInterfaceVersion()
{
    return PLUGIN_INTERFACE_VERSION;
}

SFTP::SFTP(IManager *manager)
    : IPlugin(manager)
{
    m_longName = wxT("SFTP plugin for codelite IDE");
    m_shortName = wxT("SFTP");
    
    wxTheApp->Connect(wxEVT_SFTP_OPEN_SSH_ACCOUNT_MANAGER,  wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SFTP::OnSettings), NULL, this);
    wxTheApp->Connect(wxEVT_SFTP_SETUP_WORKSPACE_MIRRORING, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SFTP::OnSetupWorkspaceMirroring), NULL, this);
}

SFTP::~SFTP()
{
}

clToolBar *SFTP::CreateToolBar(wxWindow *parent)
{
    // Create the toolbar to be used by the plugin
    clToolBar *tb(NULL);
    return tb;
}

void SFTP::CreatePluginMenu(wxMenu *pluginsMenu)
{
    wxMenu *menu = new wxMenu();
    wxMenuItem *item(NULL);

    item = new wxMenuItem(menu, wxEVT_SFTP_OPEN_SSH_ACCOUNT_MANAGER, _("Open SSH Account Manager"), _("Open SSH Account Manager"), wxITEM_NORMAL);
    menu->Append(item);
    pluginsMenu->Append(wxID_ANY, _("SFTP"), menu);
}

void SFTP::HookPopupMenu(wxMenu *menu, MenuType type)
{
    if (type == MenuTypeFileView_Workspace) {
        //Create the popup menu for the virtual folders
        wxMenuItem *item(NULL);

        item = new wxMenuItem(menu, wxID_SEPARATOR);
        menu->Prepend(item);

        item = new wxMenuItem(menu, wxEVT_SFTP_SETUP_WORKSPACE_MIRRORING, _("&Setup workspace mirroring..."), wxEmptyString, wxITEM_NORMAL);
        menu->Prepend(item);
    }
}

void SFTP::UnHookPopupMenu(wxMenu *menu, MenuType type)
{
    wxUnusedVar(menu);
    wxUnusedVar(type);
}

void SFTP::UnPlug()
{
    wxTheApp->Disconnect(wxEVT_SFTP_OPEN_SSH_ACCOUNT_MANAGER, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SFTP::OnSettings), NULL, this);
    wxTheApp->Disconnect(wxEVT_SFTP_SETUP_WORKSPACE_MIRRORING, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SFTP::OnSetupWorkspaceMirroring), NULL, this);
}

void SFTP::OnSettings(wxCommandEvent& e)
{
    wxUnusedVar(e);
    SSHAccountManagerDlg dlg(wxTheApp->GetTopWindow());
    if ( dlg.ShowModal() == wxID_OK ) {
        
        SFTPSettings settings;
        SFTPSettings::Load( settings );
        
        settings.SetAccounts( dlg.GetAccounts() );
        SFTPSettings::Save( settings );
    }
}

void SFTP::OnSetupWorkspaceMirroring(wxCommandEvent& e)
{
    SFTPBrowserDlg dlg(wxTheApp->GetTopWindow());
    dlg.ShowModal();
}
