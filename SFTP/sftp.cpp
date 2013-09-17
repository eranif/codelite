#include "sftp.h"
#include <wx/xrc/xmlres.h>
#include <wx/menu.h>
#include "SSHAccountManagerDlg.h"
#include "sftp_settings.h"
#include "SFTPBrowserDlg.h"
#include "event_notifier.h"
#include "sftp_workspace_settings.h"
#include "sftp_writer_thread.h"

static SFTP* thePlugin = NULL;
const wxEventType wxEVT_SFTP_OPEN_SSH_ACCOUNT_MANAGER    = ::wxNewEventType();
const wxEventType wxEVT_SFTP_SETUP_WORKSPACE_MIRRORING   = ::wxNewEventType();
const wxEventType wxEVT_SFTP_DISABLE_WORKSPACE_MIRRORING = ::wxNewEventType();

// Define the plugin entry point
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
    
    wxTheApp->Connect(wxEVT_SFTP_OPEN_SSH_ACCOUNT_MANAGER,    wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SFTP::OnSettings), NULL, this);
    wxTheApp->Connect(wxEVT_SFTP_SETUP_WORKSPACE_MIRRORING,   wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SFTP::OnSetupWorkspaceMirroring), NULL, this);
    wxTheApp->Connect(wxEVT_SFTP_DISABLE_WORKSPACE_MIRRORING, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SFTP::OnDisableWorkspaceMirroring), NULL, this);
    wxTheApp->Connect(wxEVT_SFTP_DISABLE_WORKSPACE_MIRRORING, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(SFTP::OnDisableWorkspaceMirroringUI), NULL, this);
    
    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(SFTP::OnWorkspaceOpened), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(SFTP::OnWorkspaceClosed), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_FILE_SAVED, wxCommandEventHandler(SFTP::OnFileSaved), NULL, this);
    
    SFTPWriterThread::Instance()->SetNotifyWindow( this );
    SFTPWriterThread::Instance()->Start();
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
        
        wxMenu *sftpMenu = new wxMenu();
        item = new wxMenuItem(sftpMenu, wxEVT_SFTP_SETUP_WORKSPACE_MIRRORING, _("&Setup..."), wxEmptyString, wxITEM_NORMAL);
        sftpMenu->Append(item);
        
        item = new wxMenuItem(sftpMenu, wxEVT_SFTP_DISABLE_WORKSPACE_MIRRORING, _("&Disable"), wxEmptyString, wxITEM_NORMAL);
        sftpMenu->Append(item);
        
        item = new wxMenuItem(menu, wxID_SEPARATOR);
        menu->Prepend(item);
        menu->Prepend(wxID_ANY, _("Workspace Mirroring"), sftpMenu);
    }
}

void SFTP::UnHookPopupMenu(wxMenu *menu, MenuType type)
{
    wxUnusedVar(menu);
    wxUnusedVar(type);
}

void SFTP::UnPlug()
{
    SFTPWriterThread::Release();
    wxTheApp->Disconnect(wxEVT_SFTP_OPEN_SSH_ACCOUNT_MANAGER, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SFTP::OnSettings), NULL, this);
    wxTheApp->Disconnect(wxEVT_SFTP_SETUP_WORKSPACE_MIRRORING, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SFTP::OnSetupWorkspaceMirroring), NULL, this);
    wxTheApp->Disconnect(wxEVT_SFTP_DISABLE_WORKSPACE_MIRRORING, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SFTP::OnDisableWorkspaceMirroring), NULL, this);
    wxTheApp->Disconnect(wxEVT_SFTP_DISABLE_WORKSPACE_MIRRORING, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(SFTP::OnDisableWorkspaceMirroringUI), NULL, this);
    
    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(SFTP::OnWorkspaceOpened), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(SFTP::OnWorkspaceClosed), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_FILE_SAVED, wxCommandEventHandler(SFTP::OnFileSaved), NULL, this);
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
    SFTPBrowserDlg dlg(wxTheApp->GetTopWindow(), _("Select the remote workspace"), "*.workspace");
    dlg.Initialize(m_workspaceSettings.GetAccount(), m_workspaceSettings.GetRemoteWorkspacePath());
    if ( dlg.ShowModal() == wxID_OK ) {
        m_workspaceSettings.SetRemoteWorkspacePath( dlg.GetPath() );
        m_workspaceSettings.SetAccount( dlg.GetAccount() );
        SFTPWorkspaceSettings::Save(m_workspaceSettings, m_workspaceFile);
    }
}

void SFTP::OnWorkspaceOpened(wxCommandEvent& e)
{
    e.Skip();
    m_workspaceFile = e.GetString();
    SFTPWorkspaceSettings::Load(m_workspaceSettings, m_workspaceFile);
}

void SFTP::OnWorkspaceClosed(wxCommandEvent& e)
{
    e.Skip();
    m_workspaceFile.Clear();
    m_workspaceSettings.Clear();
}

void SFTP::OnFileSaved(wxCommandEvent& e)
{
    e.Skip();
    
    // Sanity
    
    // check if we got a workspace file opened
    if ( !m_workspaceFile.IsOk() )
        return;
        
    // check if mirroring is setup for this workspace
    if ( !m_workspaceSettings.IsOk() )
        return;
    
    wxString local_file = *(wxString*)e.GetClientData();
    wxFileName file( local_file );
    file.MakeRelativeTo( m_workspaceFile.GetPath() );
    file.MakeAbsolute( wxFileName(m_workspaceSettings.GetRemoteWorkspacePath(), wxPATH_UNIX).GetPath());
    wxString remoteFile = file.GetFullPath(wxPATH_UNIX);
    
    SFTPSettings settings;
    SFTPSettings::Load( settings );
    
    SSHAccountInfo account;
    if ( settings.GetAccount(m_workspaceSettings.GetAccount(), account) ) {
        SFTPWriterThread::Instance()->Add( new SFTPWriterThreadRequet(account, remoteFile, local_file) );
        
    } else {
        
        wxString msg;
        msg << _("Failed to synchronize file '") << local_file << "'\n"
            << _("with remote server\n")
            << _("Could not locate account: ") << m_workspaceSettings.GetAccount();
        ::wxMessageBox(msg, "SFTP", wxOK|wxICON_ERROR);
        
        // Disable the workspace mirroring for this workspace
        m_workspaceSettings.Clear();
        SFTPWorkspaceSettings::Save(m_workspaceSettings, m_workspaceFile);
    }
}

void SFTP::OnFileWriteOK(const wxString& message)
{
    wxLogMessage( message );
}

void SFTP::OnFileWriteError(const wxString& errorMessage)
{
    wxLogMessage( errorMessage );
}

void SFTP::OnDisableWorkspaceMirroring(wxCommandEvent& e)
{
    m_workspaceSettings.Clear();
    SFTPWorkspaceSettings::Save(m_workspaceSettings, m_workspaceFile);
}

void SFTP::OnDisableWorkspaceMirroringUI(wxUpdateUIEvent& e)
{
    e.Enable( m_workspaceFile.IsOk() && m_workspaceSettings.IsOk() );
}
