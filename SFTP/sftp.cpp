#include "sftp.h"
#include <wx/xrc/xmlres.h>
#include "SSHAccountManagerDlg.h"
#include "sftp_settings.h"

static SFTP* thePlugin = NULL;

const wxEventType wxEVT_OPEN_SSH_ACCOUNT_MANAGER = ::wxNewEventType();

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
    
    wxTheApp->Connect(wxEVT_OPEN_SSH_ACCOUNT_MANAGER, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SFTP::OnSettings), NULL, this);
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

    item = new wxMenuItem(menu, wxEVT_OPEN_SSH_ACCOUNT_MANAGER, _("Open SSH Account Manager"), _("Open SSH Account Manager"), wxITEM_NORMAL);
    menu->Append(item);
    pluginsMenu->Append(wxID_ANY, _("SFTP"), menu);
}

void SFTP::HookPopupMenu(wxMenu *menu, MenuType type)
{
    if (type == MenuTypeEditor) {
        //TODO::Append items for the editor context menu
    } else if (type == MenuTypeFileExplorer) {
        //TODO::Append items for the file explorer context menu
    } else if (type == MenuTypeFileView_Workspace) {
        //TODO::Append items for the file view / workspace context menu
    } else if (type == MenuTypeFileView_Project) {
        //TODO::Append items for the file view/Project context menu
    } else if (type == MenuTypeFileView_Folder) {
        //TODO::Append items for the file view/Virtual folder context menu
    } else if (type == MenuTypeFileView_File) {
        //TODO::Append items for the file view/file context menu
    }
}

void SFTP::UnHookPopupMenu(wxMenu *menu, MenuType type)
{
    if (type == MenuTypeEditor) {
        //TODO::Unhook items for the editor context menu
    } else if (type == MenuTypeFileExplorer) {
        //TODO::Unhook  items for the file explorer context menu
    } else if (type == MenuTypeFileView_Workspace) {
        //TODO::Unhook  items for the file view / workspace context menu
    } else if (type == MenuTypeFileView_Project) {
        //TODO::Unhook  items for the file view/Project context menu
    } else if (type == MenuTypeFileView_Folder) {
        //TODO::Unhook  items for the file view/Virtual folder context menu
    } else if (type == MenuTypeFileView_File) {
        //TODO::Unhook  items for the file view/file context menu
    }
}

void SFTP::UnPlug()
{
    wxTheApp->Disconnect(wxEVT_OPEN_SSH_ACCOUNT_MANAGER, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SFTP::OnSettings), NULL, this);
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
