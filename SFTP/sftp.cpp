#include "sftp.h"
#include <wx/xrc/xmlres.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include "SSHAccountManagerDlg.h"
#include "sftp_settings.h"
#include "SFTPBrowserDlg.h"
#include "event_notifier.h"
#include "sftp_workspace_settings.h"
#include "sftp_worker_thread.h"
#include <wx/xrc/xmlres.h>
#include "cl_command_event.h"
#include "json_node.h"
#include "SFTPStatusPage.h"
#include "SFTPTreeView.h"
#include <wx/log.h>

static SFTP* thePlugin = NULL;
const wxEventType wxEVT_SFTP_OPEN_SSH_ACCOUNT_MANAGER    = ::wxNewEventType();
const wxEventType wxEVT_SFTP_SETUP_WORKSPACE_MIRRORING   = ::wxNewEventType();
const wxEventType wxEVT_SFTP_DISABLE_WORKSPACE_MIRRORING = ::wxNewEventType();

// Exposed API (via events)
// SFTP plugin provides SFTP functionality for codelite based on events
// It uses the event type clCommandEvent to accept requests from codelite's code
// the SFTP uses the event GetString() method to read a string in the form of JSON format
// For example, to instruct the plugin to connect over SSH to a remote server and save a remote file:
// the GetString() should retrun this JSON string:
//  {
//      account : "account-name-to-use",
//      local_file : "/path/to/local/file",
//      remote_file : "/path/to/remote/file",
//  }

// Event type: clCommandEvent
// Request SFTP to save a file remotely (see above for more details)
const wxEventType wxEVT_SFTP_SAVE_FILE = XRCID("wxEVT_SFTP_SAVE_FILE");

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
    EventNotifier::Get()->Connect(wxEVT_FILE_SAVED, clCommandEventHandler(SFTP::OnFileSaved), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_EDITOR_CLOSING, wxCommandEventHandler(SFTP::OnEditorClosed), NULL, this);
    
    // API support
    EventNotifier::Get()->Connect(wxEVT_SFTP_SAVE_FILE, clCommandEventHandler(SFTP::OnSaveFile), NULL, this);
    
    SFTPImages images;
    m_outputPane = new SFTPStatusPage( m_mgr->GetOutputPaneNotebook(), this );
    m_mgr->GetOutputPaneNotebook()->AddPage(m_outputPane, "SFTP", false, images.Bitmap("sftp_tab"));
    
    m_treeView = new SFTPTreeView(m_mgr->GetWorkspacePaneNotebook(), this);
    m_mgr->GetWorkspacePaneNotebook()->AddPage(m_treeView, "SFTP", false);
    
    SFTPWorkerThread::Instance()->SetNotifyWindow( m_outputPane );
    SFTPWorkerThread::Instance()->SetSftpPlugin( this );
    SFTPWorkerThread::Instance()->Start();
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
    // Find our page and release it
    // before this plugin is un-plugged we must remove the tab we added
    for (size_t i=0; i<m_mgr->GetOutputPaneNotebook()->GetPageCount(); ++i) {
        if (m_outputPane == m_mgr->GetOutputPaneNotebook()->GetPage(i)) {
            m_mgr->GetOutputPaneNotebook()->RemovePage(i);
            m_outputPane->Destroy();
            break;
        }
    }
    
    for (size_t i=0; i<m_mgr->GetWorkspacePaneNotebook()->GetPageCount(); ++i) {
        if (m_treeView == m_mgr->GetWorkspacePaneNotebook()->GetPage(i)) {
            m_mgr->GetWorkspacePaneNotebook()->RemovePage(i);
            m_treeView->Destroy();
            break;
        }
    }
    
    SFTPWorkerThread::Release();
    wxTheApp->Disconnect(wxEVT_SFTP_OPEN_SSH_ACCOUNT_MANAGER, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SFTP::OnSettings), NULL, this);
    wxTheApp->Disconnect(wxEVT_SFTP_SETUP_WORKSPACE_MIRRORING, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SFTP::OnSetupWorkspaceMirroring), NULL, this);
    wxTheApp->Disconnect(wxEVT_SFTP_DISABLE_WORKSPACE_MIRRORING, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SFTP::OnDisableWorkspaceMirroring), NULL, this);
    wxTheApp->Disconnect(wxEVT_SFTP_DISABLE_WORKSPACE_MIRRORING, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(SFTP::OnDisableWorkspaceMirroringUI), NULL, this);
    
    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(SFTP::OnWorkspaceOpened), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(SFTP::OnWorkspaceClosed), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_FILE_SAVED, clCommandEventHandler(SFTP::OnFileSaved), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_EDITOR_CLOSING, wxCommandEventHandler(SFTP::OnEditorClosed), NULL, this);
    
    EventNotifier::Get()->Disconnect(wxEVT_SFTP_SAVE_FILE, clCommandEventHandler(SFTP::OnSaveFile), NULL, this);
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

void SFTP::OnFileSaved(clCommandEvent& e)
{
    e.Skip();
    
    // --------------------------------------
    // Sanity
    // --------------------------------------
    wxString local_file = e.GetString();
    local_file.Trim().Trim(false);
    
    if ( local_file.IsEmpty() )
        return;
    
    // Check to see if this file is part of a remote files managed by our plugin
    if ( m_remoteFiles.count(local_file) ) {
        // ----------------------------------------------------------------------------------------------
        // this file was opened by the SFTP explorer
        // ----------------------------------------------------------------------------------------------
        DoSaveRemoteFile( m_remoteFiles.find(local_file)->second );
        
    } else {
        // ----------------------------------------------------------------------------------------------
        // Not a remote file, see if have a sychronization setup between this workspace and a remote one
        // ----------------------------------------------------------------------------------------------
        
        // check if we got a workspace file opened
        if ( !m_workspaceFile.IsOk() )
            return;
            
        // check if mirroring is setup for this workspace
        if ( !m_workspaceSettings.IsOk() )
            return;
        
        wxFileName file( local_file );
        file.MakeRelativeTo( m_workspaceFile.GetPath() );
        file.MakeAbsolute( wxFileName(m_workspaceSettings.GetRemoteWorkspacePath(), wxPATH_UNIX).GetPath());
        wxString remoteFile = file.GetFullPath(wxPATH_UNIX);
        
        SFTPSettings settings;
        SFTPSettings::Load( settings );
        
        SSHAccountInfo account;
        if ( settings.GetAccount(m_workspaceSettings.GetAccount(), account) ) {
            SFTPWorkerThread::Instance()->Add( new SFTPThreadRequet(account, remoteFile, local_file) );
            
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

void SFTP::OnSaveFile(clCommandEvent& e)
{
//  {
//      account : "account-name-to-use",
//      local_file : "/path/to/local/file",
//      remote_file : "/path/to/remote/file",
//  }

    wxString requestString = e.GetString();
    // ignore files which are located under our special tmporary folder
    wxFileName fnFileName( requestString );
    if ( fnFileName.GetPath().Contains( RemoteFileInfo::GetTempFolder() ) )
        return;
        
    JSONRoot root(requestString);
    JSONElement element = root.toElement();
    if ( !element.hasNamedObject("account") || !element.hasNamedObject("local_file") || !element.hasNamedObject("remote_file") ) {
        wxLogMessage(wxString() << "SFTP: Invalid save request");
        return;
    }
    
    SFTPSettings settings;
    SFTPSettings::Load( settings );
    
    wxString accName    = element.namedObject("account").toString();
    wxString localFile  = element.namedObject("local_file").toString();
    wxString remoteFile = element.namedObject("remote_file").toString();
    
    SSHAccountInfo account;
    if ( settings.GetAccount(accName, account) ) {
        SFTPWorkerThread::Instance()->Add( new SFTPThreadRequet(account, remoteFile, localFile) );
        
    } else {
        wxString msg;
        msg << _("Failed to synchronize file '") << localFile << "'\n"
            << _("with remote server\n")
            << _("Could not locate account: ") << accName;
        ::wxMessageBox(msg, "SFTP", wxOK|wxICON_ERROR);
        
    }
}

void SFTP::DoSaveRemoteFile(const RemoteFileInfo& remoteFile)
{
    SFTPWorkerThread::Instance()->Add( new SFTPThreadRequet(remoteFile.GetAccount(), remoteFile.GetRemoteFile(), remoteFile.GetLocalFile()) );
}

void SFTP::FileDownloadedSuccessfully(const wxString& localFileName)
{
    m_mgr->OpenFile(localFileName);
}

void SFTP::AddRemoteFile(const RemoteFileInfo& remoteFile)
{
    if ( m_remoteFiles.count(remoteFile.GetLocalFile()) ) {
        m_remoteFiles.erase( remoteFile.GetLocalFile() );
    }
    m_remoteFiles.insert( std::make_pair(remoteFile.GetLocalFile(), remoteFile) );
}

void SFTP::OnEditorClosed(wxCommandEvent& e)
{
    e.Skip();
    IEditor* editor = (IEditor*) e.GetClientData();
    if ( editor ) {
        wxString localFile = editor->GetFileName().GetFullPath();
        if( m_remoteFiles.count(localFile) ) {
            
            wxLogNull noLog;
            
            // Remove the file from our cache
            ::wxRemoveFile( localFile );
            m_remoteFiles.erase( localFile );
            
        }
    }
}
