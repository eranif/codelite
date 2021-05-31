#include "codedesigner.h"
#include "codedesignerdlg.h"
#include "project.h"
#include "confcodedesigner.h"
#include "procutils.h"
#include "workspace.h"
#include "processreaderthread.h"
#include "asyncprocess.h"
#include "globals.h"
#include "virtualdirectoryselector.h"
#include <wx/menu.h>
#include <wx/xrc/xmlres.h>
#include <wx/mimetype.h>
#include <wx/file.h>
#include <wx/listimpl.cpp>
#include <wx/tokenzr.h>

#include <vector>
#include <algorithm>

#include "icons/ClassDiag.xpm"
#include "icons/StateChartDiag.xpm"
#include "icons/SimpleStateChartDiag.xpm"

#define TOPIC wxT("CODEDESIGNER SESSION")

wxEventType wxEVT_IPC_POKE = wxNewEventType();

WX_DEFINE_LIST(ConnectionList);

static CodeDesigner* thePlugin = NULL;

// Define the plugin entry point
extern "C" EXPORT IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == 0) {
        thePlugin = new CodeDesigner(manager);
    }
    return thePlugin;
}

extern "C" EXPORT PluginInfo GetPluginInfo()
{
    PluginInfo info;
    info.SetAuthor(wxT("Michal Bližňák"));
    info.SetName(wxT("CodeDesigner"));
    info.SetDescription(_("CodeDesigner RAD integration with CodeLite"));
    info.SetVersion(wxT("v1.0"));
    return info;
}

extern "C" EXPORT int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

CodeDesigner::CodeDesigner(IManager* manager)
    : IPlugin(manager)
    , m_addFileMenu(true)
{
    m_longName = _("CodeDesigner RAD integration with CodeLite");
    m_shortName = wxT("CodeDesigner");

    m_Server = NULL;

    m_mgr->GetTheApp()->Connect(XRCID("cd_open"), wxEVT_COMMAND_MENU_SELECTED,
                                wxCommandEventHandler(CodeDesigner::OnOpenWithCD), NULL, this);
    m_mgr->GetTheApp()->Connect(XRCID("cd_open"), wxEVT_UPDATE_UI,
                                wxUpdateUIEventHandler(CodeDesigner::OnUpdateOpenWithCD), NULL, this);
    m_mgr->GetTheApp()->Connect(XRCID("cd_new_simplestatechart"), wxEVT_COMMAND_MENU_SELECTED,
                                wxCommandEventHandler(CodeDesigner::OnNewSCH), NULL, this);
    m_mgr->GetTheApp()->Connect(XRCID("cd_new_hierarchicalstatechart"), wxEVT_COMMAND_MENU_SELECTED,
                                wxCommandEventHandler(CodeDesigner::OnNewHSCH), NULL, this);
    m_mgr->GetTheApp()->Connect(XRCID("cd_new_classdiagram"), wxEVT_COMMAND_MENU_SELECTED,
                                wxCommandEventHandler(CodeDesigner::OnNewCD), NULL, this);

    m_mgr->GetTheApp()->Connect(wxEVT_TREE_ITEM_FILE_ACTIVATED, wxCommandEventHandler(CodeDesigner::OnOpenFile), NULL,
                                this);

    m_mgr->GetTheApp()->Connect(wxID_ANY, wxEVT_PROC_TERMINATED, wxCommandEventHandler(CodeDesigner::OnCDTerminated),
                                NULL, this);

    Connect(wxID_ANY, wxEVT_IPC_POKE, wxCommandEventHandler(CodeDesigner::OnPokeData));

    // start IPC server
    StartServer();
}

CodeDesigner::~CodeDesigner() { delete m_Server; }

clToolBar* CodeDesigner::CreateToolBar(wxWindow* parent)
{
    // Create the toolbar to be used by the plugin
    clToolBar* tb(NULL);

    /*
        // You can use the below code a snippet:
        // First, check that CodeLite allows plugin to register plugins
        if (m_mgr->AllowToolbar()) {
            // Support both toolbars icon size
            int size = m_mgr->GetToolbarIconSize();

            // Allocate new toolbar, which will be freed later by CodeLite
            tb = new clToolBar(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, clTB_DEFAULT_STYLE);

            // Set the toolbar size
            tb->SetToolBitmapSize(wxSize(size, size));

            // Add tools to the plugins toolbar. You must provide 2 sets of icons: 24x24 and 16x16
            if (size == 24) {
                tb->AddTool(XRCID("new_plugin"), wxT("New CodeLite Plugin Project"),
       wxXmlResource::Get()->LoadBitmap(wxT("plugin24")), wxT("New Plugin Wizard..."));
                tb->AddTool(XRCID("new_class"), wxT("Create New Class"),
       wxXmlResource::Get()->LoadBitmap(wxT("class24")), wxT("New Class..."));
                tb->AddTool(XRCID("new_wx_project"), wxT("New wxWidget Project"),
       wxXmlResource::Get()->LoadBitmap(wxT("new_wx_project24")), wxT("New wxWidget Project"));
            } else {
                tb->AddTool(XRCID("new_plugin"), wxT("New CodeLite Plugin Project"),
       wxXmlResource::Get()->LoadBitmap(wxT("plugin16")), wxT("New Plugin Wizard..."));
                tb->AddTool(XRCID("new_class"), wxT("Create New Class"),
       wxXmlResource::Get()->LoadBitmap(wxT("class16")), wxT("New Class..."));
                tb->AddTool(XRCID("new_wx_project"), wxT("New wxWidget Project"),
       wxXmlResource::Get()->LoadBitmap(wxT("new_wx_project16")), wxT("New wxWidget Project"));
            }
            // And finally, we must call 'Realize()'
            tb->Realize();
        }

        // return the toolbar, it can be NULL if CodeLite does not allow plugins to register toolbars
        // or in case the plugin simply does not require toolbar
    */
    return tb;
}

void CodeDesigner::CreatePluginMenu(wxMenu* pluginsMenu)
{
    // You can use the below code a snippet:
    wxMenu* menu = new wxMenu();
    wxMenuItem* item(NULL);

    item = new wxMenuItem(menu, XRCID("cd_settings"), _("Settings..."), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);

    pluginsMenu->Append(wxID_ANY, _("CodeDesigner"), menu);

    m_mgr->GetTheApp()->Connect(XRCID("cd_settings"), wxEVT_COMMAND_MENU_SELECTED,
                                wxCommandEventHandler(CodeDesigner::OnSettings), NULL, this);
}

void CodeDesigner::HookPopupMenu(wxMenu* menu, MenuType type)
{
    if(type == MenuTypeEditor) {
        // TODO::Append items for the editor context menu
    } else if(type == MenuTypeFileExplorer) {
        // TODO::Append items for the file explorer context menu
    } else if(type == MenuTypeFileView_Workspace) {
        // TODO::Append items for the file view / workspace context menu
    } else if(type == MenuTypeFileView_Project) {
        // TODO::Append items for the file view/Project context menu
    } else if(type == MenuTypeFileView_Folder) {
        // TODO::Append items for the file view/Virtual folder context menu
        if(!menu->FindItem(XRCID("cd_popup"))) {
            m_separatorItem = menu->AppendSeparator();
            menu->Append(XRCID("cd_popup"), _("CodeDesigner"), CreatePopupMenu());
        }
    } else if(type == MenuTypeFileView_File) {
        // TODO::Append items for the file view/file context menu
        if(m_addFileMenu) {
            wxMenuItem* item =
                new wxMenuItem(menu, XRCID("cd_open"), _("Open with CodeDesigner..."), wxEmptyString, wxITEM_NORMAL);

            menu->PrependSeparator();
            menu->Prepend(item);

            m_addFileMenu = false;
        }
    }
}

void CodeDesigner::UnHookPopupMenu(wxMenu* menu, MenuType type)
{
    if(type == MenuTypeEditor) {
        // TODO::Unhook items for the editor context menu
    } else if(type == MenuTypeFileExplorer) {
        // TODO::Unhook  items for the file explorer context menu
    } else if(type == MenuTypeFileView_Workspace) {
        // TODO::Unhook  items for the file view / workspace context menu
    } else if(type == MenuTypeFileView_Project) {
        // TODO::Unhook  items for the file view/Project context menu
    } else if(type == MenuTypeFileView_Folder) {
        // TODO::Unhook  items for the file view/Virtual folder context menu
    } else if(type == MenuTypeFileView_File) {
        // TODO::Unhook  items for the file view/file context menu
    }
}

void CodeDesigner::UnPlug()
{
    // TODO:: perform the unplug action for this plugin
}

void CodeDesigner::OnSettings(wxCommandEvent& e)
{
    CodeDesignerDlg dlg(m_mgr->GetTheApp()->GetTopWindow());

    ConfCodeDesigner data;
    m_mgr->GetConfigTool()->ReadObject(wxT("CodeDesigner"), &data);

    dlg.SetPort(data.GetPort());
    dlg.SetPath(data.GetPath());

    if(dlg.ShowModal() == wxID_OK) {
        bool fRestartServer = false;

        if(data.GetPort() != dlg.GetPort()) fRestartServer = true;

        data.SetPort(dlg.GetPort());
        data.SetPath(dlg.GetPath());

        m_mgr->GetConfigTool()->WriteObject(wxT("CodeDesigner"), &data);

        if(fRestartServer) StartServer();
    }
}

wxMenu* CodeDesigner::CreatePopupMenu()
{
    // Create the popup menu for the file explorer
    // The only menu that we are interseted is the file explorer menu
    wxMenu* menu = new wxMenu();
    wxMenuItem* item(NULL);

    item = new wxMenuItem(menu, XRCID("cd_new_simplestatechart"), _("New simple state chart..."), wxEmptyString,
                          wxITEM_NORMAL);
    item->SetBitmap(wxBitmap(SimpleStateChartDiag_xpm));
    menu->Append(item);

    item = new wxMenuItem(menu, XRCID("cd_new_hierarchicalstatechart"), _("New hierarchical state chart..."),
                          wxEmptyString, wxITEM_NORMAL);
    item->SetBitmap(wxBitmap(StateChartDiag_xpm));
    menu->Append(item);

    item = new wxMenuItem(menu, XRCID("cd_new_classdiagram"), _("New class diagram..."), wxEmptyString, wxITEM_NORMAL);
    item->SetBitmap(wxBitmap(ClassDiag_xpm));
    menu->Append(item);

    return menu;
}

void CodeDesigner::OnOpenFile(wxCommandEvent& e)
{
    wxString* fn = (wxString*)e.GetClientData();
    if(fn) {
        // launch it with the default application
        wxFileName fullpath(*fn);
        if(fullpath.GetExt().MakeLower() != wxT("cdp")) {
            e.Skip();
            return;
        }
#ifdef __WXGTK__
        // Under Linux, use xdg-open
        wxString cmd;
        cmd << wxT("/bin/sh -c 'xdg-open \"") << fullpath.GetFullPath() << wxT("\"' 2> /dev/null");
        wxExecute(cmd);
        return;
#else

        wxMimeTypesManager* mgr = wxTheMimeTypesManager;
        wxFileType* type = mgr->GetFileTypeFromExtension(fullpath.GetExt());
        if(type) {
            wxString cmd = type->GetOpenCommand(fullpath.GetFullPath());
            delete type;

            if(cmd.IsEmpty() == false) {
                wxExecute(cmd);
                return;
            }
        }
#endif
    }

    // we failed, call event.Skip()
    e.Skip();
}

void CodeDesigner::OnOpenWithCD(wxCommandEvent& e)
{
    // get the file name
    TreeItemInfo item = m_mgr->GetSelectedTreeItemInfo(TreeFileView);
    if(item.m_item.IsOk() && item.m_itemType == ProjectItem::TypeFile) {
        if(item.m_fileName.GetExt() == wxT("cdp")) {
            DoLaunchCD(item.m_fileName.GetFullPath());
        } else {
            wxMessageBox(_("Please select a 'cdp' (CodeDesigner Project) file only"), _("CodeLite"),
                         wxOK | wxCENTER | wxICON_INFORMATION);
            return;
        }
    }
}

void CodeDesigner::DoLaunchCD(const wxString& file)
{
    wxString cdpath = GetCDPath();

    if(cdpath.IsEmpty()) {
        wxMessageBox(_("Failed to launch CodeDesigner, no path specified\nPlease set CodeDesigner path from Plugins -> "
                       "CodeDesigner -> Settings..."),
                     _("CodeLite"), wxOK | wxCENTER | wxICON_WARNING);
        return;
    }

    wxString cmd = wxT("$(cd) $(cd_project)");
    cmd.Replace(wxT("$(cd)"), cdpath);
    cmd.Replace(wxT("$(cd_project)"), wxString::Format(wxT("\"%s\""), file.c_str()));

#ifdef __WXMSW__
    wxExecute(cmd, wxEXEC_ASYNC);
#else
    WrapInShell(cmd);
    CreateAsyncProcess(this, cmd, IProcessCreateWithHiddenConsole);
#endif
}

wxString CodeDesigner::GetCDPath()
{
    // Launch wxFB
    ConfCodeDesigner confData;
    m_mgr->GetConfigTool()->ReadObject(wxT("CodeDesigner"), &confData);
    wxString cdpath = confData.GetPath();

#ifdef __WXGTK__
    if(cdpath.IsEmpty()) {
        // try to locate the file at '/usr/bin' or '/usr/local/bin'
        if(wxFileName::FileExists(wxT("/usr/local/bin/codedesigner"))) {
            cdpath = wxT("/usr/local/bin/codedesigner");
        } else if(wxFileName::FileExists(wxT("/usr/bin/codedesigner"))) {
            cdpath = wxT("/usr/bin/codedesigner");
        }
    }
#endif
    return cdpath;
}

void CodeDesigner::OnUpdateOpenWithCD(wxUpdateUIEvent& e)
{
    TreeItemInfo item = m_mgr->GetSelectedTreeItemInfo(TreeFileView);
    if(item.m_item.IsOk() && item.m_itemType == ProjectItem::TypeFile) {
        e.Enable(item.m_fileName.GetExt() == wxT("cdp"));
    }
}

void CodeDesigner::OnCDTerminated(wxCommandEvent& e)
{
    ProcessEventData* ped = (ProcessEventData*)e.GetClientData();
    if(ped) {
        if(ped->GetProcess()) delete ped->GetProcess();
        delete ped;
    }
}

void CodeDesigner::OnNewCD(wxCommandEvent& e)
{
    CodeDesignerItemDlg dlg(m_mgr->GetTheApp()->GetTopWindow(), m_mgr, false);
    dlg.SetTitle(_("New Class Diagram"));

    CDItemInfo info;
    info.m_PackageName = wxT("Classes");

    dlg.SetCDInfo(info);
    if(dlg.ShowModal() == wxID_OK) {
        info = dlg.GetCDInfo();
        info.m_Type = cdtClassDiagram;

        DoCreateCDProject(info);
    }
}

void CodeDesigner::OnNewHSCH(wxCommandEvent& e)
{
    CodeDesignerItemDlg dlg(m_mgr->GetTheApp()->GetTopWindow(), m_mgr, true);
    dlg.SetTitle(_("New Hierarchical State Chart"));

    CDItemInfo info;
    info.m_PackageName = wxT("State charts");

    dlg.SetCDInfo(info);
    if(dlg.ShowModal() == wxID_OK) {
        info = dlg.GetCDInfo();
        info.m_Type = cdtHierarchicalStateChart;

        DoCreateCDProject(info);
    }
}

void CodeDesigner::OnNewSCH(wxCommandEvent& e)
{
    CodeDesignerItemDlg dlg(m_mgr->GetTheApp()->GetTopWindow(), m_mgr, true);
    dlg.SetTitle(_("New Simple State Chart"));

    CDItemInfo info;
    info.m_PackageName = wxT("State charts");

    dlg.SetCDInfo(info);
    if(dlg.ShowModal() == wxID_OK) {
        info = dlg.GetCDInfo();
        info.m_Type = cdtStateChart;

        DoCreateCDProject(info);
    }
}

void CodeDesigner::DoCreateCDProject(CDItemInfo& info)
{
    // add new virtual folder to the selected virtual directory
    wxString codedesignerVD;
    codedesignerVD = info.m_TreeFolderName.BeforeFirst(wxT(':'));

    m_mgr->CreateVirtualDirectory(codedesignerVD, wxT("codedesigner"));
    wxString templateFile(m_mgr->GetInstallDirectory() + wxT("/templates/codedesigner/"));
    // wxString
    // templateFile(wxT("/home/michal/Src/Projekty/CodeLite_dev/CLInstall/share/codelite/templates/codedesigner/"));

    switch(info.m_Type) {
    default:
    case cdtClassDiagram:
        templateFile << wxT("CDTemplate.cdp");
        break;
    case cdtStateChart:
        if(info.m_UseMain)
            templateFile << wxT("SCHTemplateWithMain.cdp");
        else
            templateFile << wxT("SCHTemplate.cdp");
        break;
    case cdtHierarchicalStateChart:
        if(info.m_UseMain)
            templateFile << wxT("HSCHTemplateWithMain.cdp");
        else
            templateFile << wxT("HSCHTemplate.cdp");
        break;
    }

    wxFileName tmplFile(templateFile);
    if(!tmplFile.FileExists()) {
        wxMessageBox(wxString::Format(_("Can't find CodeDesigner template file '%s'"), tmplFile.GetFullPath().c_str()),
                     _("CodeLite"), wxOK | wxCENTER | wxICON_WARNING);
        return;
    }

    // place the files under the VD's project owner
    wxString err_msg;
    wxString project = info.m_TreeFolderName.BeforeFirst(wxT(':'));
    ProjectPtr proj = m_mgr->GetWorkspace()->FindProjectByName(project, err_msg);
    if(proj) {
        wxString files_path = proj->GetFileName().GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME);
        // copy the file to here
        wxFileName cdpFile(files_path, info.m_DiagramName.Lower() + wxT(".cdp"));
        if(!wxCopyFile(tmplFile.GetFullPath(), cdpFile.GetFullPath())) {
            wxMessageBox(wxString::Format(_("Failed to copy template file to '%s'"), cdpFile.GetFullPath().c_str()),
                         _("CodeLite"), wxOK | wxCENTER | wxICON_WARNING);
            return;
        }
        // open the file, and replace expand its macros
        wxString content;
        if(!ReadFileWithConversion(cdpFile.GetFullPath().c_str(), content)) {
            wxMessageBox(wxString::Format(_("Failed to read file '%s'"), cdpFile.GetFullPath().c_str()), _("CodeLite"),
                         wxOK | wxCENTER | wxICON_WARNING);
            return;
        }

        wxString fileName = info.m_DiagramName.Lower();

        content.Replace(wxT("$(BaseFileName)"), fileName);
        content.Replace(wxT("$(CIFileName)"), fileName + wxT("_ci"));
        content.Replace(wxT("$(PackageName)"), info.m_PackageName);
        content.Replace(wxT("$(DiagramName)"), info.m_DiagramName);
        content.Replace(wxT("$(Language)"), info.m_Language);

        if(!WriteFileWithBackup(cdpFile.GetFullPath().c_str(), content, false)) {
            wxMessageBox(wxString::Format(_("Failed to write file '%s'"), cdpFile.GetFullPath().c_str()), _("CodeLite"),
                         wxOK | wxCENTER | wxICON_WARNING);
            return;
        }

        // add the file to the project
        wxArrayString paths;
        paths.Add(cdpFile.GetFullPath());
        m_mgr->AddFilesToVirtualFolder(project + wxT(":codedesigner"), paths);

        if(info.m_GenerateCode) {
            // first we launch CodeDesigner with the -g flag set
            wxString genFileCmd;
            genFileCmd << GetCDPath() << wxT(" -g ") << cdpFile.GetFullPath();

            wxArrayString dummy;
            ProcUtils::SafeExecuteCommand(genFileCmd, dummy);

            wxFileName cFile(cdpFile.GetPath(), fileName + wxT(".c"));
            wxFileName cppFile(cdpFile.GetPath(), fileName + wxT(".cpp"));
            wxFileName cCiFile(cdpFile.GetPath(), fileName + wxT("_ci.c"));
            wxFileName cppCiFile(cdpFile.GetPath(), fileName + wxT("_ci.cpp"));
            wxFileName headerFile(cdpFile.GetPath(), fileName + wxT(".h"));
            wxFileName headerCiFile(cdpFile.GetPath(), fileName + wxT("_ci.h"));
            wxFileName pyFile(cdpFile.GetPath(), fileName + wxT(".py"));
            wxFileName pyCiFile(cdpFile.GetPath(), fileName + wxT("_ci.py"));

            paths.Clear();

            if(cFile.FileExists()) paths.Add(cFile.GetFullPath());
            if(cppFile.FileExists()) paths.Add(cppFile.GetFullPath());
            if(headerFile.FileExists()) paths.Add(headerFile.GetFullPath());
            if(cCiFile.FileExists()) paths.Add(cCiFile.GetFullPath());
            if(cppCiFile.FileExists()) paths.Add(cppCiFile.GetFullPath());
            if(headerCiFile.FileExists()) paths.Add(headerCiFile.GetFullPath());
            if(pyFile.FileExists()) paths.Add(pyFile.GetFullPath());
            if(pyCiFile.FileExists()) paths.Add(pyCiFile.GetFullPath());

            if(paths.GetCount()) {
                m_mgr->AddFilesToVirtualFolder(info.m_TreeFolderName, paths);
            }

            RetagWorkspace();
        }

        DoLaunchCD(cdpFile.GetFullPath());
    }
}

void CodeDesigner::StartServer()
{
    ConfCodeDesigner data;
    m_mgr->GetConfigTool()->ReadObject(wxT("CodeDesigner"), &data);

    delete m_Server;

    m_Server = new CDServer();
    if(!m_Server->Create(data.GetPort())) {
        delete m_Server;
        m_Server = NULL;

        // wxMessageBox( _("Unable to create CodeDesigner plugin's IPC server. Please, check communication port value"),
        // _("CodeLite"), wxOK | wxICON_WARNING );
    }
}

void CodeDesigner::RetagWorkspace()
{
    wxCommandEvent e(wxEVT_COMMAND_MENU_SELECTED, XRCID("retag_workspace"));
    m_mgr->GetTheApp()->GetTopWindow()->GetEventHandler()->AddPendingEvent(e);
}

bool CodeDesigner::AddFilesToWorkspace(wxArrayString& paths, CDConnection* connection)
{
    // get tree folder if not specified
    if(connection->GetTreeFolder().IsEmpty()) {
        m_mgr->GetTheApp()->GetTopWindow()->Raise();

        VirtualDirectorySelector dlg(m_mgr->GetTheApp()->GetTopWindow(), m_mgr->GetWorkspace(), wxT(""));
        dlg.SetTitle(_("Insert generated files into..."));

        if(dlg.ShowModal() == wxID_OK) {
            connection->SetTreeFolder(dlg.GetVirtualDirectoryPath());
        } else
            return false;
    }

    // check whether given files are already included in the project
    wxArrayString arrFilesToAdd;

    wxString err_msg;
    wxString project = connection->GetTreeFolder().BeforeFirst(wxT(':'));
    ProjectPtr proj = m_mgr->GetWorkspace()->FindProjectByName(project, err_msg);
    if(proj) {
        std::vector<wxFileName> vctPresentFiles;
        proj->GetFiles(vctPresentFiles, true);

        for(size_t i = 0; i < paths.GetCount(); ++i) {
            wxFileName file(paths[i]);
            if(std::find(vctPresentFiles.begin(), vctPresentFiles.end(), file) == vctPresentFiles.end()) {
                arrFilesToAdd.Add(paths[i]);
            }
        }
    }

    // update CodeLite workspace
    if(!arrFilesToAdd.IsEmpty()) m_mgr->AddFilesToVirtualFolder(connection->GetTreeFolder(), arrFilesToAdd);

    RetagWorkspace();

    return true;
}

void CodeDesigner::OnPokeData(wxCommandEvent& e)
{
    wxArrayString arrFiles;
    wxStringTokenizer tokens(e.GetString(), wxT("\n"), wxTOKEN_STRTOK);

    while(tokens.HasMoreTokens()) {
        arrFiles.Add(tokens.GetNextToken());
    }

    AddFilesToWorkspace(arrFiles, (CDConnection*)e.GetClientData());
}

// IPC /////////////////////////////////////////////////////////////////////////

CDConnection::CDConnection(CDServer* server) { m_Server = server; }

CDConnection::~CDConnection() { Disconnect(); }

bool CDConnection::OnDisconnect()
{
    m_Server->Disconnect(this);
    return true;
}

bool CDConnection::OnPoke(const wxString& topic, const wxString& item, wxChar* data, int size, wxIPCFormat format)
{
    if(topic == TOPIC) {
        if(item == wxT("ADD FILES")) {
            wxCommandEvent e(wxEVT_IPC_POKE, wxID_ANY);
            e.SetString(wxString(data));
            e.SetClientData(this);

            wxPostEvent(thePlugin, e);
        }

        return true;
    } else
        return false;
}

CDServer::CDServer() { m_Connections.DeleteContents(true); }

CDServer::~CDServer() { m_Connections.Clear(); }

wxConnectionBase* CDServer::OnAcceptConnection(const wxString& topic)
{
    if(topic == TOPIC) {
        CDConnection* connection = new CDConnection(this);
        m_Connections.Append(connection);

        return connection;
    } else
        return NULL;
}

void CDServer::Disconnect(CDConnection* connection)
{
    if(connection) {
        m_Connections.DeleteObject(connection);
    }
}
