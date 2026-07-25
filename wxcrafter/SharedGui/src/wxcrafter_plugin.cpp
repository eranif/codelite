#include "wxcrafter_plugin.h"

#include "ColoursAndFontsManager.h"
#include "MyComboBoxXmlHandler.h"
#include "MyRearrangeListXmlHandler.h"
#include "UI/NewFormWizard.h"
#include "UI/wxcTreeView.h"
#include "UI/wxguicraft_main_view.h"
#include "allocator_mgr.h"
#include "cl_command_event.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "myxh_auimgr.h"
#include "myxh_auitoolb.h"
#include "myxh_cmdlinkbtn.h"
#include "myxh_dataview.h"
#include "myxh_dlg.h"
#include "myxh_dvlistctrl.h"
#include "myxh_dvtreectrl.h"
#include "myxh_frame.h"
#include "myxh_glcanvas.h"
#include "myxh_grid.h"
#include "myxh_infobar.h"
#include "myxh_listc.h"
#include "myxh_mediactrl.h"
#include "myxh_panel.h"
#include "myxh_propgrid.h"
#include "myxh_ribbon.h"
#include "myxh_richtext.h"
#include "myxh_searchctrl.h"
#include "myxh_simplebook.h"
#include "myxh_stc.h"
#include "myxh_textctrl.h"
#include "myxh_toolbk.h"
#include "myxh_treebk.h"
#include "myxh_treelist.h"
#include "myxh_webview.h"
#include "plugin.h"
#include "project.h"
#include "workspace.h"
#include "wxc_project_metadata.h"
#include "wxc_settings.h"
#include "wxgui_bitmaploader.h"
#include "wxgui_helpers.h"

#include <wx/ffile.h>
#include <wx/msgdlg.h>
#include <wx/xrc/xmlres.h>

#if !STANDALONE_BUILD
#include "Keyboard/clKeyboardManager.h"
#endif

namespace
{
wxStringSet_t GetProjectFiles(const wxString& projectName)
{
    ProjectPtr p = clCxxWorkspaceST::Get()->GetProject(projectName);
    if (!p) {
        return {};
    }
    const Project::FilesMap_t& filesMap = p->GetFiles();
    wxStringSet_t files;
    files.reserve(filesMap.size());
    for (const auto& [filename, _] : filesMap) {
        files.insert(filename);
    }
    return files;
}
} // namespace

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager) { return new wxCrafterPlugin(manager, false); }

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor("Eran");
    info.SetName("wxcrafter");
    info.SetDescription(_("wxWidgets GUI Designer"));
    info.SetVersion("v2.4");
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

wxCrafterPlugin::wxCrafterPlugin(IManager* manager, bool serverMode)
    : IPlugin(manager)
    , m_mainFrame(NULL)
    , m_serverMode(serverMode)
{
#if !STANDALONE_BUILD
    /// Initialize wxPG only in plugin mode
    wxPGInitResourceModule();

// Start the network thread
#endif

    // Initialize all image handlers known to us (that aren't already loaded)
    if (wxImage::FindHandler(wxBITMAP_TYPE_PNG) == 0) {
        wxImage::AddHandler(new wxPNGHandler);
    }
    if (wxImage::FindHandler(wxBITMAP_TYPE_JPEG) == 0) {
        wxImage::AddHandler(new wxJPEGHandler);
    }
    if (wxImage::FindHandler(wxBITMAP_TYPE_GIF) == 0) {
        wxImage::AddHandler(new wxGIFHandler);
    }
    if (wxImage::FindHandler(wxBITMAP_TYPE_BMP) == 0) {
        wxImage::AddHandler(new wxBMPHandler);
    }
    if (wxImage::FindHandler(wxBITMAP_TYPE_ICO) == 0) {
        wxImage::AddHandler(new wxICOHandler);
    }
    if (wxImage::FindHandler(wxBITMAP_TYPE_ANI) == 0) {
        wxImage::AddHandler(new wxANIHandler);
    }
    if (wxImage::FindHandler(wxBITMAP_TYPE_CUR) == 0) {
        wxImage::AddHandler(new wxCURHandler);
    }

    wxXmlResource::Get()->ClearHandlers();

    // +++++++++++---------------------------------------------
    // Custom XML resource handlers
    // +++++++++++---------------------------------------------

    wxXmlResource::Get()->AddHandler(new MYwxTreebookXmlHandler);
    wxXmlResource::Get()->AddHandler(new MyWxRichTextCtrlXmlHandler);
    wxXmlResource::Get()->AddHandler(new MyWxGridXmlHandler);
    wxXmlResource::Get()->AddHandler(new MyWxSearchCtrlXmlHandler);
    wxXmlResource::Get()->AddHandler(new MYwxToolbookXmlHandler);
    wxXmlResource::Get()->AddHandler(new MYwxListCtrlXmlHandler);
    wxXmlResource::Get()->AddHandler(new MyWxSimplebookXmlHandler);
    wxXmlResource::Get()->AddHandler(new MyWxAuiToolBarXmlHandler);
    wxXmlResource::Get()->AddHandler(new MyWxStcXmlHandler);
    wxXmlResource::Get()->AddHandler(new MyWxDataViewListCtrlHandler);
    wxXmlResource::Get()->AddHandler(new MyWxDataViewTreeCtrlHandler);
    wxXmlResource::Get()->AddHandler(new MyWxDataViewCtrlHandler);
    wxXmlResource::Get()->AddHandler(new MyWxInfoBarCtrlHandler);
    wxXmlResource::Get()->AddHandler(new MyWxWebViewXmlHandler);
    wxXmlResource::Get()->AddHandler(new MyWxGLCanvasXmlHandler);
    wxXmlResource::Get()->AddHandler(new MyWxMediaCtrlXmlHandler);
    wxXmlResource::Get()->AddHandler(new wxMyFrameXmlHandler);
    wxXmlResource::Get()->AddHandler(new MyWxPanelXmlHandler);
    wxXmlResource::Get()->AddHandler(new MyWxAuiManagerXmlHandler);
    wxXmlResource::Get()->AddHandler(new MyWxDialogXmlHandler);
    wxXmlResource::Get()->AddHandler(new MyWxPropGridXmlHandler);
    wxXmlResource::Get()->AddHandler(new MyWxRibbonXmlHandler);
    wxXmlResource::Get()->AddHandler(new MyWxCommandLinkButtonXmlHandler);
    wxXmlResource::Get()->AddHandler(new MyTreeListCtrl);
    wxXmlResource::Get()->AddHandler(new MyTextCtrlXrcHandler);
    wxXmlResource::Get()->AddHandler(new MyComboBoxXmlHandler);
    wxXmlResource::Get()->AddHandler(new MyRearrangeListXmlHandler);
    wxXmlResource::Get()->InitAllHandlers();

    // wxCrafter::ResourceLoader bmps;
    m_longName = _("wxWidgets GUI Designer");
    m_shortName = "wxCrafter";

// will be initialized in the OnInitDone()
#if STANDALONE_BUILD
    DoInitDone();
#endif

    EventNotifier::Get()->Bind(wxEVT_SHOW_WORKSPACE_TAB, &wxCrafterPlugin::OnToggleView, this);
    EventNotifier::Get()->Connect(wxEVT_INIT_DONE, wxCommandEventHandler(wxCrafterPlugin::OnInitDone), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_SHOW_WXCRAFTER_DESIGNER, wxCommandEventHandler(wxCrafterPlugin::OnShowDesigner), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_TREE_ITEM_FILE_ACTIVATED, clCommandEventHandler(wxCrafterPlugin::OnOpenFile), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_CMD_WXCRAFTER_PROJECT_MODIFIED, wxCommandEventHandler(wxCrafterPlugin::OnProjectModified), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_CMD_WXCRAFTER_PROJECT_SYNCHED, wxCommandEventHandler(wxCrafterPlugin::OnProjectSynched), NULL, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &wxCrafterPlugin::OnWorkspaceClosed, this);
    EventNotifier::Get()->Connect(XRCID("save_file"), wxCommandEventHandler(wxCrafterPlugin::OnSave), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_SAVE_ALL_EDITORS, clCommandEventHandler(wxCrafterPlugin::OnSaveAll), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_WXGUI_PROJECT_LOADED, wxCommandEventHandler(wxCrafterPlugin::OnProjectLoaded), NULL, this);

    EventNotifier::Get()->Bind(wxEVT_CONTEXT_MENU_FILE, &wxCrafterPlugin::OnFileContextMenu, this);
    EventNotifier::Get()->Bind(wxEVT_CONTEXT_MENU_VIRTUAL_FOLDER, &wxCrafterPlugin::OnVirtualFolderContextMenu, this);
    EventNotifier::Get()->Bind(wxEVT_CONTEXT_MENU_PROJECT, &wxCrafterPlugin::OnProjectContextMenu, this);
    EventNotifier::Get()->Bind(wxEVT_PAGE_MODIFIED_UPDATE_UI, &wxCrafterPlugin::OnSaveUI, this);
    // Menu events
    wxTheApp->Connect(XRCID("open_wxcp_project"),
                      wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(wxCrafterPlugin::OnOpenProject),
                      NULL,
                      this);
    wxTheApp->Connect(XRCID("close_wxcp_project"),
                      wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(wxCrafterPlugin::OnCloseProject),
                      NULL,
                      this);
    wxTheApp->Connect(XRCID("save_wxcp_project"),
                      wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(wxCrafterPlugin::OnSaveProject),
                      NULL,
                      this);
    wxTheApp->Connect(XRCID("define_custom_controls"),
                      wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(wxCrafterPlugin::OnDefineCustomControls),
                      NULL,
                      this);
    wxTheApp->Connect(XRCID("edit_custom_controls"),
                      wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(wxCrafterPlugin::OnEditCustomControls),
                      NULL,
                      this);
    wxTheApp->Connect(XRCID("delete_custom_controls"),
                      wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(wxCrafterPlugin::OnDeleteCustomControls),
                      NULL,
                      this);
    wxTheApp->Connect(
        XRCID("wxcp_about"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(wxCrafterPlugin::OnAbout), NULL, this);
    wxTheApp->Connect(XRCID("open_wxcp_project"),
                      wxEVT_UPDATE_UI,
                      wxUpdateUIEventHandler(wxCrafterPlugin::OnOpenProjectUI),
                      NULL,
                      this);
    wxTheApp->Connect(XRCID("close_wxcp_project"),
                      wxEVT_UPDATE_UI,
                      wxUpdateUIEventHandler(wxCrafterPlugin::OnCloseProjectUI),
                      NULL,
                      this);
    wxTheApp->Connect(XRCID("save_wxcp_project"),
                      wxEVT_UPDATE_UI,
                      wxUpdateUIEventHandler(wxCrafterPlugin::OnSaveProjectUI),
                      NULL,
                      this);
    wxTheApp->Connect(XRCID("wxcp_new_form"),
                      wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(wxCrafterPlugin::OnNewForm),
                      NULL,
                      this);
    wxTheApp->Bind(wxEVT_MENU, &wxCrafterPlugin::OnReGenerateForProject, this, XRCID("wxcp_generate_all_project"));
    wxTheApp->Connect(XRCID("ID_SHOW_DESIGNER"),
                      wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(wxCrafterPlugin::OnShowDesigner),
                      NULL,
                      (wxEvtHandler*)this);

#if !STANDALONE_BUILD
    clKeyboardManager::Get()->AddAccelerator(
        "ID_SHOW_DESIGNER", _("wxCrafter"), _("Show the designer"), "Ctrl-Shift-F12");
#endif
}

wxCrafterPlugin::~wxCrafterPlugin()
{
    Allocator::Release();
    wxcSettings::Get().Save();
}

void wxCrafterPlugin::UnPlug()
{
    EventNotifier::Get()->Unbind(wxEVT_PAGE_MODIFIED_UPDATE_UI, &wxCrafterPlugin::OnSaveUI, this);
    EventNotifier::Get()->Unbind(wxEVT_SHOW_WORKSPACE_TAB, &wxCrafterPlugin::OnToggleView, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_SHOW_WXCRAFTER_DESIGNER, wxCommandEventHandler(wxCrafterPlugin::OnShowDesigner), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_INIT_DONE, wxCommandEventHandler(wxCrafterPlugin::OnInitDone), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_TREE_ITEM_FILE_ACTIVATED, clCommandEventHandler(wxCrafterPlugin::OnOpenFile), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_CMD_WXCRAFTER_PROJECT_MODIFIED, wxCommandEventHandler(wxCrafterPlugin::OnProjectModified), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_CMD_WXCRAFTER_PROJECT_SYNCHED, wxCommandEventHandler(wxCrafterPlugin::OnProjectSynched), NULL, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &wxCrafterPlugin::OnWorkspaceClosed, this);
    EventNotifier::Get()->Disconnect(XRCID("save_file"), wxCommandEventHandler(wxCrafterPlugin::OnSave), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_SAVE_ALL_EDITORS, clCommandEventHandler(wxCrafterPlugin::OnSaveAll), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_WXGUI_PROJECT_LOADED, wxCommandEventHandler(wxCrafterPlugin::OnProjectLoaded), NULL, this);

    EventNotifier::Get()->Unbind(wxEVT_CONTEXT_MENU_FILE, &wxCrafterPlugin::OnFileContextMenu, this);
    EventNotifier::Get()->Unbind(wxEVT_CONTEXT_MENU_VIRTUAL_FOLDER, &wxCrafterPlugin::OnVirtualFolderContextMenu, this);
    EventNotifier::Get()->Unbind(wxEVT_CONTEXT_MENU_PROJECT, &wxCrafterPlugin::OnProjectContextMenu, this);

    // Menu events
    wxTheApp->Disconnect(XRCID("open_wxcp_project"),
                         wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(wxCrafterPlugin::OnOpenProject),
                         NULL,
                         this);
    wxTheApp->Disconnect(XRCID("close_wxcp_project"),
                         wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(wxCrafterPlugin::OnCloseProject),
                         NULL,
                         this);
    wxTheApp->Disconnect(XRCID("save_wxcp_project"),
                         wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(wxCrafterPlugin::OnSaveProject),
                         NULL,
                         this);
    wxTheApp->Disconnect(XRCID("define_custom_controls"),
                         wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(wxCrafterPlugin::OnDefineCustomControls),
                         NULL,
                         this);
    wxTheApp->Disconnect(XRCID("edit_custom_controls"),
                         wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(wxCrafterPlugin::OnEditCustomControls),
                         NULL,
                         this);
    wxTheApp->Disconnect(XRCID("delete_custom_controls"),
                         wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(wxCrafterPlugin::OnDeleteCustomControls),
                         NULL,
                         this);
    // Connect the events to us
    wxTheApp->Disconnect(XRCID("ID_SHOW_DESIGNER"),
                         wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(wxCrafterPlugin::OnShowDesigner),
                         NULL,
                         (wxEvtHandler*)this);

    wxTheApp->Disconnect(XRCID("open_wxcp_project"),
                         wxEVT_UPDATE_UI,
                         wxUpdateUIEventHandler(wxCrafterPlugin::OnOpenProjectUI),
                         NULL,
                         this);
    wxTheApp->Disconnect(XRCID("close_wxcp_project"),
                         wxEVT_UPDATE_UI,
                         wxUpdateUIEventHandler(wxCrafterPlugin::OnCloseProjectUI),
                         NULL,
                         this);
    wxTheApp->Disconnect(XRCID("save_wxcp_project"),
                         wxEVT_UPDATE_UI,
                         wxUpdateUIEventHandler(wxCrafterPlugin::OnSaveProjectUI),
                         NULL,
                         this);
    wxTheApp->Disconnect(XRCID("wxcp_new_form"),
                         wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(wxCrafterPlugin::OnNewForm),
                         NULL,
                         this);

    m_mainFrame->Destroy();
    wxXmlResource::Get()->ClearHandlers();
}

void wxCrafterPlugin::CreateToolBar(clToolBarGeneric* toolbar) { wxUnusedVar(toolbar); }

void wxCrafterPlugin::CreatePluginMenu(wxMenu* pluginsMenu)
{
    wxUnusedVar(pluginsMenu);
    wxMenu* menu = new wxMenu();
    menu->Append(XRCID("open_wxcp_project"), _("Open wxCrafter project..."));
    menu->Append(XRCID("close_wxcp_project"), _("Close wxCrafter project"));
    menu->Append(XRCID("save_wxcp_project"), _("Save project"));
    menu->AppendSeparator();
    menu->Append(XRCID("import_wxFB_project"), _("Import a wxFormBuilder project"));
    menu->Append(XRCID("import_wxSmith_project"), _("Import a wxSmith file"));
    menu->Append(XRCID("import_XRC_project"), _("Import from XRC"));
    menu->AppendSeparator();
    menu->Append(XRCID("define_custom_controls"), _("Define custom control..."));
    menu->Append(XRCID("edit_custom_controls"), _("Edit custom controls..."));
    menu->Append(XRCID("delete_custom_controls"), _("Delete custom control..."));
    menu->AppendSeparator();
    menu->Append(XRCID("ID_SHOW_DESIGNER"), _("Show the designer"), _("Show the designer"));
    menu->AppendSeparator();
#if STANDALONE_BUILD
    menu->Append(XRCID("wxcp_about"), _("About..."));
#endif
    menu->Append(XRCID("wxc_settings"), _("Settings..."));
    pluginsMenu->Append(wxID_ANY, _("wxCrafter"), menu);
}

void wxCrafterPlugin::OnInitDone(wxCommandEvent& e)
{
    e.Skip();
    DoInitDone(NULL);
}

void wxCrafterPlugin::OnShowDesigner(wxCommandEvent& e) { DoShowDesigner(); }

wxMenu* wxCrafterPlugin::DoCreateFolderMenu()
{
    // Create the popup menu for the file explorer
    // The only menu that we are interested is the file explorer menu
    wxMenu* menu = new wxMenu();
    wxMenuItem* item(NULL);

    wxCrafter::ResourceLoader bmps;
    item = new wxMenuItem(menu, XRCID("wxcp_new_form"), _("Add wxWidgets UI Form..."), wxEmptyString, wxITEM_NORMAL);
    item->SetBitmap(bmps.Bitmap("new-form"));
    menu->Append(item);

    return menu;
}

void wxCrafterPlugin::OnNewForm(wxCommandEvent& e)
{
    m_mainFrame->OpenNewFormWizard(e.GetInt());
}

void wxCrafterPlugin::DoShowDesigner()
{
    if (!m_mgr) {
        return;
    }

    m_mainFrame->DisplayDesigner();
}

void wxCrafterPlugin::OnOpenFile(clCommandEvent& e)
{
    e.Skip();
    wxFileName fullpath(e.GetFileName());
    if (fullpath.GetExt() == "wxcp") {
        e.Skip(false);
        DoLoadWxcProject(fullpath);
    }
}

void wxCrafterPlugin::OnProjectModified(wxCommandEvent& e)
{
    e.Skip();
    CHECK_POINTER(m_mainFrame->GetWxcView());
    CHECK_POINTER(m_mgr);

    wxString title = m_mgr->GetPageTitle(m_mainFrame->GetWxcView());
    if (!title.StartsWith("*")) {
        title.Prepend("*");
        m_mgr->SetPageTitle(m_mainFrame->GetWxcView(), title);
    }
}

void wxCrafterPlugin::OnProjectSynched(wxCommandEvent& e)
{
    e.Skip();
    CHECK_POINTER(m_mainFrame->GetWxcView());
    CHECK_POINTER(m_mgr);

    wxString title = m_mgr->GetPageTitle(m_mainFrame->GetWxcView());
    if (title.StartsWith("*")) {
        title.Remove(0, 1);
        m_mgr->SetPageTitle(m_mainFrame->GetWxcView(), title);
    }
}

void wxCrafterPlugin::OnWorkspaceClosed(clWorkspaceEvent& e)
{
    e.Skip();
    m_mainFrame->CloseProject(false);
}

void wxCrafterPlugin::OnSave(wxCommandEvent& e)
{
    CHECK_POINTER(m_mgr);
    e.Skip();
}

// plugin menu
void wxCrafterPlugin::OnCloseProject(wxCommandEvent& e)
{
    CHECK_POINTER(m_mgr);
    wxUnusedVar(e);
}

void wxCrafterPlugin::OnCloseProjectUI(wxUpdateUIEvent& e) { e.Enable(wxcProjectMetadata::Get().IsLoaded()); }

void wxCrafterPlugin::OnOpenProject(wxCommandEvent&)
{
    m_mainFrame->LoadProject(wxFileName());
}

void wxCrafterPlugin::OnOpenProjectUI(wxUpdateUIEvent& e) { e.Enable(wxcProjectMetadata::Get().IsLoaded() == false); }

void wxCrafterPlugin::OnSaveProject(wxCommandEvent&)
{
    m_mainFrame->SaveProject();
}

void wxCrafterPlugin::OnSaveProjectUI(wxUpdateUIEvent& e) { e.Enable(wxcEditManager::Get().IsDirty()); }

void wxCrafterPlugin::OnImportFBProject(wxCommandEvent&)
{
    m_mainFrame->OpenWxFormBuilderImporterDialog(m_selectedFile.GetFullPath());
}

void wxCrafterPlugin::OnImportwxSmithProject(wxCommandEvent&)
{
    m_mainFrame->OpenWxSmithImporterDialog(m_selectedFile.GetFullPath());
}

void wxCrafterPlugin::OnDefineCustomControls(wxCommandEvent&)
{
    m_mainFrame->OpenDefineCustomControlWizard();
}

void wxCrafterPlugin::OnEditCustomControls(wxCommandEvent&)
{
    m_mainFrame->OpenEditCustomControlDialog();
}

void wxCrafterPlugin::OnDeleteCustomControls(wxCommandEvent&)
{
    m_mainFrame->OpenDeleteCustomControlDialog();
}

void wxCrafterPlugin::OnAbout(wxCommandEvent& e)
{
    m_mainFrame->OpenAboutDialog();
}

void wxCrafterPlugin::OnSettings(wxCommandEvent&)
{
    m_mainFrame->OpenSettingsDialog();
}

void wxCrafterPlugin::DoInitDone(wxObject* obj)
{
    wxUnusedVar(obj);
#if !STANDALONE_BUILD
    wxToolBar* mainToolbar = EventNotifier::Get()->TopFrame()->GetToolBar();
    if (mainToolbar) {
        int toolHeight = mainToolbar->GetToolBitmapSize().GetHeight();

        wxCrafter::ResourceLoader rl;
        wxBitmap bmp = rl.Bitmap(toolHeight == 24 ? "wxc-logo-24" : "wxc-logo-16");
        wxTheApp->Connect(XRCID("ID_SHOW_DESIGNER"),
                          wxEVT_COMMAND_MENU_SELECTED,
                          wxCommandEventHandler(wxCrafterPlugin::OnShowDesigner),
                          NULL,
                          (wxEvtHandler*)this);
    }

#endif

#if STANDALONE_BUILD
    /// explicitly load the lexers
    ColoursAndFontsManager::Get().Load();
#endif

    m_mainFrame = new MainFrame(EventNotifier::Get()->TopFrame(), m_serverMode, m_mgr);

    wxCrafter::SetTopFrame(m_mainFrame);
}

void wxCrafterPlugin::OnProjectLoaded(wxCommandEvent& e) { e.Skip(); }

void wxCrafterPlugin::OnSaveAll(clCommandEvent& e)
{
    e.Skip();
    if (wxcProjectMetadata::Get().IsLoaded()) {
        m_mainFrame->SaveProject();
    }
}

void wxCrafterPlugin::OnFileContextMenu(clContextMenuEvent& event)
{
    event.Skip();
    const wxArrayString& files = event.GetStrings();
    if (files.GetCount() == 1) {
        m_selectedFile = files.Item(0);
        if (m_selectedFile.GetExt() == "wxcp") {
            // a wxCrafter file
            event.GetMenu()->PrependSeparator();
            event.GetMenu()->Prepend(new wxMenuItem(event.GetMenu(),
                                                    XRCID("open_wxC_project_from_context_menu"),
                                                    _("Open with wxCrafter..."),
                                                    wxEmptyString,
                                                    wxITEM_NORMAL));
            event.GetMenu()->Bind(wxEVT_COMMAND_MENU_SELECTED,
                                  &wxCrafterPlugin::OnOpenWxcpProject,
                                  this,
                                  XRCID("open_wxC_project_from_context_menu"));
        } else if (m_selectedFile.GetExt() == "fbp") {
            // form builder file
            event.GetMenu()->PrependSeparator();
            event.GetMenu()->Prepend(new wxMenuItem(event.GetMenu(),
                                                    XRCID("import_wxFB_project_from_context_menu"),
                                                    _("Import with wxCrafter..."),
                                                    wxEmptyString,
                                                    wxITEM_NORMAL));
            event.GetMenu()->Bind(wxEVT_COMMAND_MENU_SELECTED,
                                  &wxCrafterPlugin::OnImportFBProject,
                                  this,
                                  XRCID("import_wxFB_project_from_context_menu"));
        } else if (m_selectedFile.GetExt() == "wxs") {
            event.GetMenu()->PrependSeparator();
            event.GetMenu()->Prepend(new wxMenuItem(event.GetMenu(),
                                                    XRCID("import_wxSmith_project"),
                                                    _("Import with wxCrafter..."),
                                                    wxEmptyString,
                                                    wxITEM_NORMAL));
            event.GetMenu()->Bind(wxEVT_COMMAND_MENU_SELECTED,
                                  &wxCrafterPlugin::OnImportwxSmithProject,
                                  this,
                                  XRCID("import_wxSmith_project"));
        }
    }
}

void wxCrafterPlugin::OnOpenWxcpProject(wxCommandEvent& event) { DoLoadWxcProject(m_selectedFile); }

void wxCrafterPlugin::DoLoadWxcProject(const wxFileName& filename)
{
    DoShowDesigner();
    m_mainFrame->LoadProject(filename);

    wxCommandEvent evtShowDesigner(wxEVT_COMMAND_MENU_SELECTED, XRCID("ID_SHOW_DESIGNER"));
    m_mainFrame->GetEventHandler()->AddPendingEvent(evtShowDesigner);
}

void wxCrafterPlugin::OnVirtualFolderContextMenu(clContextMenuEvent& event)
{
    event.Skip();
    event.GetMenu()->AppendSeparator();
    event.GetMenu()->Append(wxID_ANY, "wxCrafter", DoCreateFolderMenu());
}

wxMenu* wxCrafterPlugin::DoProjectMenu()
{
    wxMenu* menu = new wxMenu();
    menu->Append(XRCID("wxcp_generate_all_project"), _("Re-generate code for project"));
    return menu;
}

void wxCrafterPlugin::OnProjectContextMenu(clContextMenuEvent& event)
{
    event.Skip();
    event.GetMenu()->Append(wxID_ANY, "wxCrafter", DoProjectMenu());
}

void wxCrafterPlugin::OnReGenerateForProject(wxCommandEvent& e)
{
    wxArrayString wxcpFiles;
    if (m_mgr->GetWorkspace() && m_mgr->GetWorkspace()->IsOpen()) {
        wxArrayString projects;
        ProjectPtr activeProject = m_mgr->GetSelectedProject();
        if (!activeProject) {
            return;
        }
        wxStringSet_t all_files = GetProjectFiles(activeProject->GetName());

        // Filter out and keep only wxcp files
        for (const wxString& file : all_files) {
            if (FileExtManager::GetType(file) == FileExtManager::TypeWxCrafter) {
                wxcpFiles.Add(file);
            }
        }

        if (wxcpFiles.IsEmpty()) {
            ::wxMessageBox(_("This project does not contain any wxCrafter files"), "wxCrafter");
            return;
        }

        // Ensure that we have a designer
        DoShowDesigner();

        // Now generate the code
        m_mainFrame->GetWxcView()->BatchGenerate(wxcpFiles);
    }
}

void wxCrafterPlugin::OnToggleView(clCommandEvent& event)
{
    if (event.GetString() != _("wxCrafter")) {
        event.Skip();
        return;
    }
#if 0
    if(event.IsSelected()) {
        // show it
        wxcImages images;
        m_mgr->GetWorkspacePaneNotebook()->AddPage(m_mainFrame->GetTreeView(), _("wxCrafter"), false, images.Bitmap("wxc_icon"));
    } else {
        int where = m_mgr->GetWorkspacePaneNotebook()->GetPageIndex(_("wxCrafter"));
        if(where != wxNOT_FOUND) {
            m_mgr->GetWorkspacePaneNotebook()->RemovePage(where);
        }
    }
#endif
}

void wxCrafterPlugin::OnSaveUI(clCommandEvent& event)
{
    event.Skip();
    wxWindow* win = (wxWindow*)event.GetClientData();
    if (win == m_mainFrame->GetWxcView()) {
        event.Skip(false);
        event.SetAnswer(wxcEditManager::Get().IsDirty());
    }
}
