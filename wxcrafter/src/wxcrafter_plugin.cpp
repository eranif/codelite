#include "wxcrafter_plugin.h"

#include "AboutDlg.h"
#include "ColoursAndFontsManager.h"
#include "DefineCustomControlWizard.h"
#include "DeleteCustomControlDlg.h"
#include "EditCustomControlDlg.h"
#include "EventsEditorDlg.h"
#include "MessageDlg.h"
#include "MyComboBoxXmlHandler.h"
#include "MyRearrangeListXmlHandler.h"
#include "NewFormWizard.h"
#include "VirtualDirectorySelectorDlg.h"
#include "allocator_mgr.h"
#include "clKeyboardManager.h"
#include "clStatusBar.h"
#include "cl_command_event.h"
#include "codelite_events.h"
#include "ctags_manager.h"
#include "editor_config.h"
#include "entry.h"
#include "event_notifier.h"
#include "functions_parser.h"
#include "globals.h"
#include "gui.h"
#include "import_from_wxFB.h"
#include "import_from_wxSmith.h"
#include "import_from_xrc.h"
#include "myxh_auibook.h"
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
#include "wxcSettingsDlg.h"
#include "wxcTreeView.h"
#include "wxc_bitmap_code_generator.h"
#include "wxc_settings.h"
#include "wxgui_bitmaploader.h"
#include "wxgui_globals.h"
#include "wxgui_helpers.h"
#include "wxguicraft_main_view.h"

#include <wx/choicdlg.h>
#include <wx/dataview.h>
#include <wx/module.h>
#include <wx/msgdlg.h>
#include <wx/propgrid/propgrid.h>
#include <wx/stc/stc.h>
#include <wx/xrc/xmlres.h>

static wxCrafterPlugin* thePlugin = NULL;

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == 0) {
        thePlugin = new wxCrafterPlugin(manager, false);
    }
    return thePlugin;
}

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
    , m_mainPanel(NULL)
    , m_allEditorsClosing(false)
    , m_addFileMenu(true)
    , m_useFrame(true)
    , m_mainFrame(NULL)
    , m_serverMode(serverMode)
{
    Allocator::Initialize();
    Allocator::Instance()->SetPlugin(this);

#if !STANDALONE_BUILD
    /// Initialize wxPG only in plugin mode
    wxPGInitResourceModule();

// Start the network thread
#endif

    // Initialize all image handlers known to us (that aren't already loaded)
    if(wxImage::FindHandler(wxBITMAP_TYPE_PNG) == 0) {
        wxImage::AddHandler(new wxPNGHandler);
    }
    if(wxImage::FindHandler(wxBITMAP_TYPE_JPEG) == 0) {
        wxImage::AddHandler(new wxJPEGHandler);
    }
    if(wxImage::FindHandler(wxBITMAP_TYPE_GIF) == 0) {
        wxImage::AddHandler(new wxGIFHandler);
    }
    if(wxImage::FindHandler(wxBITMAP_TYPE_BMP) == 0) {
        wxImage::AddHandler(new wxBMPHandler);
    }
    if(wxImage::FindHandler(wxBITMAP_TYPE_ICO) == 0) {
        wxImage::AddHandler(new wxICOHandler);
    }
    if(wxImage::FindHandler(wxBITMAP_TYPE_ANI) == 0) {
        wxImage::AddHandler(new wxANIHandler);
    }
    if(wxImage::FindHandler(wxBITMAP_TYPE_CUR) == 0) {
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
    EventNotifier::Get()->Connect(wxEVT_SHOW_WXCRAFTER_DESIGNER, wxCommandEventHandler(wxCrafterPlugin::OnShowDesigner),
                                  NULL, this);
    EventNotifier::Get()->Connect(wxEVT_DESIGNER_DELETED, wxCommandEventHandler(wxCrafterPlugin::OnDesignerDelete),
                                  NULL, this);
    EventNotifier::Get()->Connect(wxEVT_BITMAP_CODE_GENERATION_DONE,
                                  wxCommandEventHandler(wxCrafterPlugin::OnBitmapCodeGenerationCompleted), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_TREE_ITEM_FILE_ACTIVATED, clCommandEventHandler(wxCrafterPlugin::OnOpenFile),
                                  NULL, this);
    EventNotifier::Get()->Connect(wxEVT_NOTIFY_PAGE_CLOSING, wxNotifyEventHandler(wxCrafterPlugin::OnPageClosing), NULL,
                                  this);
    EventNotifier::Get()->Connect(wxEVT_CMD_WXCRAFTER_PROJECT_MODIFIED,
                                  wxCommandEventHandler(wxCrafterPlugin::OnProjectModified), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_CMD_WXCRAFTER_PROJECT_SYNCHED,
                                  wxCommandEventHandler(wxCrafterPlugin::OnProjectSynched), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_CMD_PAGE_CHANGED, wxCommandEventHandler(wxCrafterPlugin::OnPageChanged), NULL,
                                  this);
    EventNotifier::Get()->Connect(wxEVT_PREVIEW_CTRL_SELECTED,
                                  wxCommandEventHandler(wxCrafterPlugin::OnDesignerItemSelected), NULL, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &wxCrafterPlugin::OnWorkspaceClosed, this);
    EventNotifier::Get()->Connect(wxEVT_BUILD_STARTING, wxCommandEventHandler(wxCrafterPlugin::OnBuildStarting), NULL,
                                  this);
    EventNotifier::Get()->Connect(wxEVT_ALL_EDITORS_CLOSING,
                                  wxCommandEventHandler(wxCrafterPlugin::OnAllEditorsClosing), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_ALL_EDITORS_CLOSED, wxCommandEventHandler(wxCrafterPlugin::OnAllEditorsClosed),
                                  NULL, this);
    EventNotifier::Get()->Connect(XRCID("save_file"), wxCommandEventHandler(wxCrafterPlugin::OnSave), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_SAVE_ALL_EDITORS, clCommandEventHandler(wxCrafterPlugin::OnSaveAll), NULL,
                                  this);
    EventNotifier::Get()->Connect(wxEVT_WXGUI_PROJECT_LOADED, wxCommandEventHandler(wxCrafterPlugin::OnProjectLoaded),
                                  NULL, this);

    EventNotifier::Get()->Bind(wxEVT_CONTEXT_MENU_FILE, &wxCrafterPlugin::OnFileContextMenu, this);
    EventNotifier::Get()->Bind(wxEVT_CONTEXT_MENU_VIRTUAL_FOLDER, &wxCrafterPlugin::OnVirtualFolderContextMenu, this);
    EventNotifier::Get()->Bind(wxEVT_CONTEXT_MENU_PROJECT, &wxCrafterPlugin::OnProjectContextMenu, this);
    EventNotifier::Get()->Bind(wxEVT_PAGE_MODIFIED_UPDATE_UI, &wxCrafterPlugin::OnSaveUI, this);
    // Menu events
    wxTheApp->Connect(XRCID("wxc_settings"), wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(wxCrafterPlugin::OnSettings), NULL, this);
    wxTheApp->Connect(XRCID("open_wxcp_project"), wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(wxCrafterPlugin::OnOpenProject), NULL, this);
    wxTheApp->Connect(XRCID("close_wxcp_project"), wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(wxCrafterPlugin::OnCloseProject), NULL, this);
    wxTheApp->Connect(XRCID("save_wxcp_project"), wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(wxCrafterPlugin::OnSaveProject), NULL, this);
    wxTheApp->Connect(XRCID("import_wxFB_project"), wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(wxCrafterPlugin::OnImportwxFBProject), NULL, this);
    wxTheApp->Connect(XRCID("import_wxSmith_project"), wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(wxCrafterPlugin::OnImportwxSmith), NULL, this);
    wxTheApp->Connect(XRCID("import_XRC_project"), wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(wxCrafterPlugin::OnImportXRC), NULL, this);
    wxTheApp->Connect(XRCID("define_custom_controls"), wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(wxCrafterPlugin::OnDefineCustomControls), NULL, this);
    wxTheApp->Connect(XRCID("edit_custom_controls"), wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(wxCrafterPlugin::OnEditCustomControls), NULL, this);
    wxTheApp->Connect(XRCID("delete_custom_controls"), wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(wxCrafterPlugin::OnDeleteCustomControls), NULL, this);
    wxTheApp->Connect(XRCID("wxcp_about"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(wxCrafterPlugin::OnAbout),
                      NULL, this);
    wxTheApp->Connect(XRCID("open_wxcp_project"), wxEVT_UPDATE_UI,
                      wxUpdateUIEventHandler(wxCrafterPlugin::OnOpenProjectUI), NULL, this);
    wxTheApp->Connect(XRCID("close_wxcp_project"), wxEVT_UPDATE_UI,
                      wxUpdateUIEventHandler(wxCrafterPlugin::OnCloseProjectUI), NULL, this);
    wxTheApp->Connect(XRCID("save_wxcp_project"), wxEVT_UPDATE_UI,
                      wxUpdateUIEventHandler(wxCrafterPlugin::OnSaveProjectUI), NULL, this);
    wxTheApp->Connect(XRCID("wxcp_new_form"), wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(wxCrafterPlugin::OnNewForm), NULL, this);
    wxTheApp->Bind(wxEVT_MENU, &wxCrafterPlugin::OnReGenerateForProject, this, XRCID("wxcp_generate_all_project"));
    wxTheApp->Connect(XRCID("ID_SHOW_DESIGNER"), wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(wxCrafterPlugin::OnShowDesigner), NULL, (wxEvtHandler*)this);

#if !STANDALONE_BUILD
    m_mgr->GetWorkspacePaneNotebook()->Bind(wxEVT_BOOK_PAGE_CHANGED, &wxCrafterPlugin::OnWorkspaceTabSelected, this);
    clKeyboardManager::Get()->AddAccelerator("ID_SHOW_DESIGNER", _("wxCrafter"), _("Show the designer"),
                                             "Ctrl-Shift-F12");
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
    EventNotifier::Get()->Disconnect(wxEVT_DESIGNER_DELETED, wxCommandEventHandler(wxCrafterPlugin::OnDesignerDelete),
                                     NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_SHOW_WXCRAFTER_DESIGNER,
                                     wxCommandEventHandler(wxCrafterPlugin::OnShowDesigner), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_INIT_DONE, wxCommandEventHandler(wxCrafterPlugin::OnInitDone), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_BITMAP_CODE_GENERATION_DONE,
                                     wxCommandEventHandler(wxCrafterPlugin::OnBitmapCodeGenerationCompleted), NULL,
                                     this);
    EventNotifier::Get()->Disconnect(wxEVT_TREE_ITEM_FILE_ACTIVATED, clCommandEventHandler(wxCrafterPlugin::OnOpenFile),
                                     NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_NOTIFY_PAGE_CLOSING, wxNotifyEventHandler(wxCrafterPlugin::OnPageClosing),
                                     NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_CMD_WXCRAFTER_PROJECT_MODIFIED,
                                     wxCommandEventHandler(wxCrafterPlugin::OnProjectModified), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_CMD_WXCRAFTER_PROJECT_SYNCHED,
                                     wxCommandEventHandler(wxCrafterPlugin::OnProjectSynched), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_CMD_PAGE_CHANGED, wxCommandEventHandler(wxCrafterPlugin::OnPageChanged),
                                     NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_PREVIEW_CTRL_SELECTED,
                                     wxCommandEventHandler(wxCrafterPlugin::OnDesignerItemSelected), NULL, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &wxCrafterPlugin::OnWorkspaceClosed, this);
    EventNotifier::Get()->Disconnect(wxEVT_BUILD_STARTING, wxCommandEventHandler(wxCrafterPlugin::OnBuildStarting),
                                     NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_ALL_EDITORS_CLOSING,
                                     wxCommandEventHandler(wxCrafterPlugin::OnAllEditorsClosing), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_ALL_EDITORS_CLOSED,
                                     wxCommandEventHandler(wxCrafterPlugin::OnAllEditorsClosed), NULL, this);
    EventNotifier::Get()->Disconnect(XRCID("save_file"), wxCommandEventHandler(wxCrafterPlugin::OnSave), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_SAVE_ALL_EDITORS, clCommandEventHandler(wxCrafterPlugin::OnSaveAll), NULL,
                                     this);
    EventNotifier::Get()->Disconnect(wxEVT_WXGUI_PROJECT_LOADED,
                                     wxCommandEventHandler(wxCrafterPlugin::OnProjectLoaded), NULL, this);

    EventNotifier::Get()->Unbind(wxEVT_CONTEXT_MENU_FILE, &wxCrafterPlugin::OnFileContextMenu, this);
    EventNotifier::Get()->Unbind(wxEVT_CONTEXT_MENU_VIRTUAL_FOLDER, &wxCrafterPlugin::OnVirtualFolderContextMenu, this);
    EventNotifier::Get()->Unbind(wxEVT_CONTEXT_MENU_PROJECT, &wxCrafterPlugin::OnProjectContextMenu, this);

    // Menu events
    wxTheApp->Disconnect(XRCID("open_wxcp_project"), wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(wxCrafterPlugin::OnOpenProject), NULL, this);
    wxTheApp->Disconnect(XRCID("close_wxcp_project"), wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(wxCrafterPlugin::OnCloseProject), NULL, this);
    wxTheApp->Disconnect(XRCID("save_wxcp_project"), wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(wxCrafterPlugin::OnSaveProject), NULL, this);
    wxTheApp->Disconnect(XRCID("import_wxFB_project"), wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(wxCrafterPlugin::OnImportwxFBProject), NULL, this);
    wxTheApp->Disconnect(XRCID("import_wxSmith_project"), wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(wxCrafterPlugin::OnImportwxSmith), NULL, this);
    wxTheApp->Disconnect(XRCID("import_XRC_project"), wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(wxCrafterPlugin::OnImportXRC), NULL, this);
    wxTheApp->Disconnect(XRCID("define_custom_controls"), wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(wxCrafterPlugin::OnDefineCustomControls), NULL, this);
    wxTheApp->Disconnect(XRCID("edit_custom_controls"), wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(wxCrafterPlugin::OnEditCustomControls), NULL, this);
    wxTheApp->Disconnect(XRCID("delete_custom_controls"), wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(wxCrafterPlugin::OnDeleteCustomControls), NULL, this);
    // Connect the events to us
    wxTheApp->Disconnect(XRCID("ID_SHOW_DESIGNER"), wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(wxCrafterPlugin::OnShowDesigner), NULL, (wxEvtHandler*)this);

    wxTheApp->Disconnect(XRCID("open_wxcp_project"), wxEVT_UPDATE_UI,
                         wxUpdateUIEventHandler(wxCrafterPlugin::OnOpenProjectUI), NULL, this);
    wxTheApp->Disconnect(XRCID("close_wxcp_project"), wxEVT_UPDATE_UI,
                         wxUpdateUIEventHandler(wxCrafterPlugin::OnCloseProjectUI), NULL, this);
    wxTheApp->Disconnect(XRCID("save_wxcp_project"), wxEVT_UPDATE_UI,
                         wxUpdateUIEventHandler(wxCrafterPlugin::OnSaveProjectUI), NULL, this);
#if !STANDALONE_BUILD
    m_mgr->GetWorkspacePaneNotebook()->Unbind(wxEVT_BOOK_PAGE_CHANGED, &wxCrafterPlugin::OnWorkspaceTabSelected, this);
#endif
    wxTheApp->Disconnect(XRCID("wxcp_new_form"), wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(wxCrafterPlugin::OnNewForm), NULL, this);

    // before this plugin is un-plugged we must remove the tab we added
    if(IsTabMode()) {

#if !STANDALONE_BUILD
        for(size_t i = 0; i < m_mgr->GetWorkspacePaneNotebook()->GetPageCount(); i++) {
            if(m_treeView == m_mgr->GetWorkspacePaneNotebook()->GetPage(i)) {
                m_mgr->GetWorkspacePaneNotebook()->RemovePage(i);
                break;
            }
        }
        m_treeView->Destroy();
#endif

    } else {
        m_mainFrame->Destroy();
        m_treeView = NULL;
    }

    wxXmlResource::Get()->ClearHandlers();
    // Why is wxModule::CleanUpModules(); commented out?
    // We comment this or m_mainFrame->Destroy(); will crash (it destroys wxPG class which relies on a
    // wxModule::Register)
    // We can:
    // 1. use wxDELETE(m_mainFrame) and uncomment the line below
    // 2. use m_mainFrame->Destroy() and comment the line below
    // we choose the second option
    // wxModule::CleanUpModules();
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

void wxCrafterPlugin::OnDesignerDelete(wxCommandEvent& e)
{
    e.Skip();
    m_mainPanel = NULL;
}

void wxCrafterPlugin::OnPageClosing(wxNotifyEvent& e)
{
    if(!IsTabMode()) {
        e.Skip();
        return;
    }

    wxWindow* win = reinterpret_cast<wxWindow*>(e.GetClientData());
    if(win && win == m_mainPanel) {
        if(wxcEditManager::Get().IsDirty()) {

            wxString msg;
            msg << _("wxCrafter project is modified\nDo you want to save your changes?");

            int rc = ::wxMessageBox(msg, _("wxCrafter"), wxYES_NO | wxCANCEL | wxCENTER);
            switch(rc) {
            case wxYES: {
                m_treeView->CloseProject(true);
                e.Skip();
                break;
            }
            case wxNO:
                m_treeView->CloseProject(false);
                e.Skip();
                break;

            case wxCANCEL:
                e.Veto();
                break;
            }
        } else {
            m_treeView->CloseProject(false);
        }

    } else {
        e.Skip();
    }
}

void wxCrafterPlugin::OnBitmapCodeGenerationCompleted(wxCommandEvent& e)
{
    if(wxcProjectMetadata::Get().GetGenerateCPPCode()) {
        // First, notify codelite to reload all modified files that are opened (so codelite will not prompt us about
        // them)
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED_NOPROMPT);
        EventNotifier::Get()->TopFrame()->GetEventHandler()->AddPendingEvent(evt);

        // Next, compile a list of all generated files and add them to codelite
        wxFileName bitmapSourceFile(e.GetString());
        wxCrafter::MakeAbsToProject(bitmapSourceFile);

        // Base header/cpp
        wxFileName headerFile = wxcProjectMetadata::Get().BaseHeaderFile();
        wxCrafter::MakeAbsToProject(headerFile);

        wxFileName sourceFile = wxcProjectMetadata::Get().BaseCppFile();
        wxCrafter::MakeAbsToProject(sourceFile);

        // And the derived classes
        wxFileName derivedHeader = wxcProjectMetadata::Get().GetGeneratedHeader();
        wxCrafter::MakeAbsToProject(derivedHeader);

        wxFileName derivedSource = wxcProjectMetadata::Get().GetGeneratedSource();
        wxCrafter::MakeAbsToProject(derivedSource);

        // XRC file
        wxFileName xrcFile;
        if(wxcProjectMetadata::Get().GetGenerateXRC()) {
            xrcFile = wxcProjectMetadata::Get().GetXrcFileName();
            wxCrafter::MakeAbsToProject(xrcFile);
        }

        // Notify codelite to retagg the workspace
        std::vector<wxFileName> filesToRetag;
        filesToRetag.push_back(headerFile);
        filesToRetag.push_back(sourceFile);
        filesToRetag.push_back(derivedHeader);
        filesToRetag.push_back(derivedSource);
        filesToRetag.push_back(bitmapSourceFile);

        // Prepare a list of additional files generated by wxC (this can happen for custom model for wxDataViewCtrl)
        wxArrayString additionalFiles;
        const wxStringMap_t& af = wxcProjectMetadata::Get().GetAdditionalFiles();
        wxStringMap_t::const_iterator itr = af.begin();
        for(; itr != af.end(); ++itr) {
            wxFileName f(wxcProjectMetadata::Get().GetGeneratedHeader());
            f.SetFullName(itr->first);
            wxCrafter::MakeAbsToProject(f);
            additionalFiles.Add(f.GetFullPath());
            filesToRetag.push_back(f);
        }

        m_generatedClassInfo.Clear();
        m_generatedClassInfo.classname = wxcProjectMetadata::Get().GetGeneratedClassName();
        m_generatedClassInfo.derivedHeader = derivedHeader;
        m_generatedClassInfo.derivedSource = derivedSource;

#if !STANDALONE_BUILD
        wxArrayString filesToAdd;
        std::set<wxString> uniqueFiles;
        wxString sourceFilesVD;
        if(clCxxWorkspaceST::Get()->IsOpen()) {
            wxString vd = wxcProjectMetadata::Get().GetVirtualFolder();

            wxString projectName;
            vd.Trim().Trim(false);
            if(vd.IsEmpty()) {
                // We got no Virtual Folder to place the files to
                // Search for resource file
                wxFileName projectFile(wxcProjectMetadata::Get().GetProjectFile());
                projectName = m_mgr->GetProjectNameByFile(projectFile.GetFullPath());

            } else {
                projectName = vd.BeforeFirst(':');
            }

            projectName.Trim().Trim(false);
            if(projectName.IsEmpty()) {
                return;
            }

            wxString errMsg;
            ProjectPtr project = clCxxWorkspaceST::Get()->FindProjectByName(projectName, errMsg);

            if(!project) {
                wxString msg;
                msg << _("Could not find target project: '") << projectName << _("'\n");
                msg << _("Make sure that the virtual folder entry is valid (it must exist)\n");
                msg << _("Virtual Folder provided was: '") << vd << _("'");
                ::wxMessageBox(msg, _("wxCrafter"), wxOK | wxICON_ERROR);
                return;
            }

            std::vector<wxFileName> projectFiles;
            project->GetFilesAsVectorOfFileName(projectFiles);
            uniqueFiles = wxCrafter::VectorToSet(projectFiles);

            wxString baseFilesVD, bitmapFilesVD, xrcVD;
            sourceFilesVD << wxcProjectMetadata::Get().GetVirtualFolder();
            sourceFilesVD.Trim().Trim(false);

            if(sourceFilesVD.IsEmpty()) {
                sourceFilesVD << projectName << ":src";
            }

            baseFilesVD << projectName << ":wxcrafter:base";
            bitmapFilesVD << projectName << ":wxcrafter:resources";
            xrcVD << projectName << ":wxcrafter:XRC";

            //////////////////////////////////////////////////////////////
            // Add the bitmap resource files
            //////////////////////////////////////////////////////////////
            if(uniqueFiles.find(bitmapSourceFile.GetFullPath()) == uniqueFiles.end()) {
                filesToAdd.Add(bitmapSourceFile.GetFullPath());
            }

            if(filesToAdd.IsEmpty() == false) {
                DoCreateVirtualFolder(bitmapFilesVD);
                m_mgr->AddFilesToVirtualFolder(bitmapFilesVD, filesToAdd);
            }

            //////////////////////////////////////////////////////////////
            // Add the XRC files
            //////////////////////////////////////////////////////////////
            filesToAdd.Clear();
            if(xrcFile.IsOk() && uniqueFiles.find(xrcFile.GetFullPath()) == uniqueFiles.end()) {
                filesToAdd.Add(xrcFile.GetFullPath());
            }

            if(filesToAdd.IsEmpty() == false) {
                DoCreateVirtualFolder(xrcVD);
                m_mgr->AddFilesToVirtualFolder(xrcVD, filesToAdd);
            }

            //////////////////////////////////////////////////////////////
            // Add the base classes files
            //////////////////////////////////////////////////////////////
            filesToAdd.Clear();
            if(uniqueFiles.find(headerFile.GetFullPath()) == uniqueFiles.end()) {
                filesToAdd.Add(headerFile.GetFullPath());
            }
            if(uniqueFiles.find(sourceFile.GetFullPath()) == uniqueFiles.end()) {
                filesToAdd.Add(sourceFile.GetFullPath());
            }

            // The additional files are added to the 'base'
            for(size_t i = 0; i < additionalFiles.GetCount(); ++i) {
                if(uniqueFiles.find(additionalFiles.Item(i)) == uniqueFiles.end()) {
                    filesToAdd.Add(additionalFiles.Item(i));
                }
            }

            if(filesToAdd.IsEmpty() == false) {
                DoCreateVirtualFolder(baseFilesVD);
                m_mgr->AddFilesToVirtualFolder(baseFilesVD, filesToAdd);
            }
        }
#endif

        //////////////////////////////////////////////////////////////
        // Add the derived classes files
        //////////////////////////////////////////////////////////////

        // Not all top level windows wants subclassing..
        if(derivedHeader.IsOk() && !derivedHeader.GetName().IsEmpty()) {
#if !STANDALONE_BUILD
            if(clCxxWorkspaceST::Get()->IsOpen()) {
                filesToAdd.Clear();
                if(uniqueFiles.find(derivedHeader.GetFullPath()) == uniqueFiles.end()) {
                    filesToAdd.Add(derivedHeader.GetFullPath());
                }

                if(uniqueFiles.find(derivedSource.GetFullPath()) == uniqueFiles.end()) {
                    filesToAdd.Add(derivedSource.GetFullPath());
                }

                if(filesToAdd.IsEmpty() == false) {
                    DoCreateVirtualFolder(sourceFilesVD);
                    m_mgr->AddFilesToVirtualFolder(sourceFilesVD, filesToAdd);
                }
            }
#endif
            DoUpdateDerivedClassEventHandlers();
        }
    }

    wxString msg;
    msg << _("wxCrafter: code generation completed successfully!");
    wxCrafter::SetStatusMessage(msg);

    // Restart ctagsd
    clLanguageServerEvent restart_event{ wxEVT_LSP_RESTART };
    restart_event.SetLspName("ctagsd");
    EventNotifier::Get()->ProcessEvent(restart_event);
}

wxMenu* wxCrafterPlugin::DoCreateFolderMenu()
{
    // Create the popup menu for the file explorer
    // The only menu that we are interseted is the file explorer menu
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
    int type = e.GetInt();

#if STANDALONE_BUILD
    if(!wxcProjectMetadata::Get().IsLoaded()) {
        ::wxMessageBox(_("You must create a project before you can add new forms"), _("wxCrafter"),
                       wxOK | wxCENTER | wxICON_WARNING);
        return;
    }
#endif

    // Check to see if we already got a wxCrafter.wxcp file
    NewFormWizard wizard(wxCrafter::TopFrame(), m_mgr, type);
    if(wizard.RunWizard(wizard.GetFirstPage())) {
        NewFormDetails details = wizard.GetFormDetails();
        DoGenerateCode(details);

        wxString label("new ");
        switch(type) {
        case ID_WXDIALOG:
            label << "dialog";
            break;
        case ID_WXFRAME:
            label << "frame";
            break;
        case ID_WXWIZARD:
            label << "wizard";
            break;
        case ID_WXPANEL_TOPLEVEL:
            label << "panel";
            break;
        case ID_WXPOPUPWINDOW:
            label << "popup window";
            break;
        case ID_WXIMAGELIST:
            label << "image list";
            break;
        default:
            label << "top-level item";
        }
        wxcEditManager::Get().PushState(label);
    }
}

void wxCrafterPlugin::DoGenerateCode(const NewFormDetails& fd)
{
    wxCrafter::ResourceLoader rl;
    wxString errMsg;

    wxFileName wxcpFile = wxFileName(fd.wxcpFile);
    if(!wxcpFile.FileExists()) {
        wxFFile fp(wxcpFile.GetFullPath(), "w+b");
        if(fp.IsOpened()) {
            fp.Close();

        } else {
            wxString msg;
            msg << _("Could not create resource file '") << wxcpFile.GetFullPath() << "'";
            ::wxMessageBox(msg, _("wxCrafter"), wxOK | wxICON_WARNING | wxCENTRE);
            return;
        }
    }

#if !STANDALONE_BUILD
    // If we have a workspace opened, handle the virtual folder thing here
    if(clCxxWorkspaceST::Get()->IsOpen()) {
        // Step 1:
        // Check if we already got a virtual folder named 'wxcrafter'
        wxString projectName = fd.virtualFolder.BeforeFirst(':');
        ProjectPtr project = clCxxWorkspaceST::Get()->FindProjectByName(projectName, errMsg);
        if(!project) {
            ::wxMessageBox(errMsg, _("wxCrafter"), wxOK | wxICON_WARNING | wxCENTRE);
            return;
        }

        wxString vdFullPath;
        vdFullPath << projectName;

        // Check if already got a file with this name in the project
        wxStringSet_t files;
        wxCrafter::GetProjectFiles(project->GetName(), files);

        if(!files.count(wxcpFile.GetFullPath())) {

            if(!m_mgr->CreateVirtualDirectory(vdFullPath, "wxcrafter")) {
                ::wxMessageBox(_("Could not create virtual folder 'wxcrafter'"), _("wxCrafter"),
                               wxOK | wxICON_WARNING | wxCENTRE);
                return;
            }

            // Add the resource file to the newly created virtual folder
            wxArrayString f;
            f.Add(wxcpFile.GetFullPath());
            vdFullPath << ":"
                       << "wxcrafter";
            m_mgr->AddFilesToVirtualFolder(vdFullPath, f);
        }
    }
#endif

    // At this point, wxcpFile contains the fullpath the
    // wxCrafter project, load it
    DoShowDesigner();
    m_treeView->LoadProject(wxcpFile.GetFullPath());
    m_treeView->AddForm(fd);

    // Notify about file system changes here
    clFileSystemEvent eventFilesGenerate(wxEVT_FILE_CREATED);
    eventFilesGenerate.GetPaths().Add(wxcpFile.GetFullPath());
    EventNotifier::Get()->QueueEvent(eventFilesGenerate.Clone());

    // And finally, select the wxCrafter tab in the 'Workspace' view
    if(IsTabMode()) {
        DoSelectWorkspaceTab();

    } else {
        wxCommandEvent evtShowDesigner(wxEVT_SHOW_WXCRAFTER_DESIGNER);
        EventNotifier::Get()->ProcessEvent(evtShowDesigner);
    }
}

bool wxCrafterPlugin::DoShowDesigner(bool createIfNotExist)
{
    if(!m_mgr) {
        return false;
    }

    m_mainFrame->DisplayDesigner();
    return false;
}

bool wxCrafterPlugin::DoCreateVirtualFolder(const wxString& vdFullPath)
{
    CHECK_POINTER_RET_FALSE(m_mgr);
    wxString name = vdFullPath.AfterLast(':');
    wxString parent = vdFullPath.BeforeLast(':');
    return m_mgr->CreateVirtualDirectory(parent, name);
}

void wxCrafterPlugin::OnOpenFile(clCommandEvent& e)
{
    e.Skip();
    wxFileName fullpath(e.GetFileName());
    if(fullpath.GetExt() == "wxcp") {
        e.Skip(false);
        DoLoadWxcProject(fullpath);
    }
}

void wxCrafterPlugin::DoSelectWorkspaceTab()
{

    CHECK_POINTER(m_mgr);
    if(IsTabMode()) {
        // And finally, select the wxCrafter tab in the 'Workspace' view
        Notebook* book = m_mgr->GetWorkspacePaneNotebook();
        for(size_t i = 0; i < book->GetPageCount(); i++) {
            if(book->GetPage(i) == m_treeView) {
                book->SetSelection(i);
                break;
            }
        }
    }
}

void wxCrafterPlugin::OnProjectModified(wxCommandEvent& e)
{
    e.Skip();
    CHECK_POINTER(m_mainPanel);
    CHECK_POINTER(m_mgr);

    wxString title = m_mgr->GetPageTitle(m_mainPanel);
    if(!title.StartsWith("*")) {
        title.Prepend("*");
        m_mgr->SetPageTitle(m_mainPanel, title);
    }
}

void wxCrafterPlugin::OnProjectSynched(wxCommandEvent& e)
{
    e.Skip();
    CHECK_POINTER(m_mainPanel);
    CHECK_POINTER(m_mgr);

    wxString title = m_mgr->GetPageTitle(m_mainPanel);
    if(title.StartsWith("*")) {
        title.Remove(0, 1);
        m_mgr->SetPageTitle(m_mainPanel, title);
    }
}

void wxCrafterPlugin::OnPageChanged(wxCommandEvent& e)
{
    e.Skip();
    if(!m_allEditorsClosing) {
        wxWindow* win = reinterpret_cast<wxWindow*>(e.GetClientData());
        if(win && (win == m_mainPanel)) {
            DoSelectWorkspaceTab();
            if(wxcProjectMetadata::Get().IsLoaded()) {
                CallAfter(&wxCrafterPlugin::UpdateFileNameInStatusBar);
            }
        }
    }
}

void wxCrafterPlugin::UpdateFileNameInStatusBar() {}

void wxCrafterPlugin::OnWorkspaceTabSelected(wxBookCtrlEvent& e)
{
    e.Skip();
    CHECK_POINTER(m_mgr);
    wxWindow* page = m_mgr->GetWorkspacePaneNotebook()->GetPage(e.GetSelection());
    if(page == m_treeView) {
        DoShowDesigner(false);
    }
}

void wxCrafterPlugin::OnDesignerItemSelected(wxCommandEvent& e)
{
    e.Skip();
    DoSelectWorkspaceTab();
}

void wxCrafterPlugin::OnWorkspaceClosed(clWorkspaceEvent& e)
{
    e.Skip();
    m_treeView->CloseProject(false);
}

void wxCrafterPlugin::DoUpdateDerivedClassEventHandlers()
{
    wxString headerContent, sourceContent;
    IEditor *headerEditor(NULL), *sourceEditor(NULL);

    if(!DoReadFileContentAndPrompt(m_generatedClassInfo.derivedHeader, headerContent, &headerEditor)) {
        return;
    }

    if(!DoReadFileContentAndPrompt(m_generatedClassInfo.derivedSource, sourceContent, &sourceEditor)) {
        return;
    }

    CHECK_POINTER(m_mainPanel);

    // Parse and collect all functions declared in the header file
    wxcWidget* topLevelWin = m_mainPanel->GetActiveTopLevelWin();
    CHECK_POINTER(topLevelWin);

    wxcWidget::Map_t connectedEvents = topLevelWin->GetConnectedEventsRecrusively();
    FunctionsParser parser(connectedEvents, m_generatedClassInfo.classname, headerContent);
    parser.Parse();

    wxString decl, impl;
    const wxcWidget::Map_t& newEvents = parser.GetAllEvents();
    if(newEvents.empty()) {
        return;
    }

    wxcWidget::Map_t::const_iterator iter = newEvents.begin();
    for(; iter != newEvents.end(); ++iter) {
        impl << iter->second.GetFunctionImpl(m_generatedClassInfo.classname);
        decl << iter->second.GetFunctionDecl();
    }

    // Insert the functions declarations
    if(TagsManagerST::Get()->InsertFunctionDecl(m_generatedClassInfo.classname, decl, headerContent, 1)) {
        if(wxcSettings::Get().HasFlag(wxcSettings::FORMAT_INHERITED_FILES)) {
            // Format the string
            wxCrafter::FormatString(headerContent, m_generatedClassInfo.derivedHeader);
        }

        // Write the resulting string
        DoWriteFileContent(m_generatedClassInfo.derivedHeader, headerContent, headerEditor);
        wxCrafter::NotifyFileSaved(m_generatedClassInfo.derivedHeader);
    }

    // Insert the functions definitions at the end of the file buffer
    if(!sourceContent.EndsWith("\n")) {
        sourceContent << "\n";
    }
    sourceContent << impl;

    // Format the source file
    if(wxcSettings::Get().HasFlag(wxcSettings::FORMAT_INHERITED_FILES)) {
        wxCrafter::FormatString(headerContent, m_generatedClassInfo.derivedSource);
    }

    DoWriteFileContent(m_generatedClassInfo.derivedSource, sourceContent, sourceEditor);
    wxCrafter::NotifyFileSaved(m_generatedClassInfo.derivedSource);
}

bool wxCrafterPlugin::DoReadFileContentAndPrompt(const wxFileName& fn, wxString& content, IEditor** editor)
{
    *editor = NULL;

    // Check to see if we got the file opened in the editor
    if(m_mgr) {
        *editor = m_mgr->FindEditor(fn.GetFullPath());
        if(*editor) {
            content = (*editor)->GetTextRange(0, (*editor)->GetLength());
            return true;
        }
    }

    // Read the header file content from the disk
    if(!wxCrafter::ReadFileContent(fn.GetFullPath(), content)) {
        wxString msg;
        msg << _("Error while reading file content: '") << fn.GetFullPath() << _("'.\n")
            << _("Could not read file content");
        ::wxMessageBox(msg, _("wxCrafter"), wxICON_WARNING | wxOK | wxCENTER, EventNotifier::Get()->TopFrame());
        return false;
    }
    return true;
}

void wxCrafterPlugin::OnBuildStarting(wxCommandEvent& e)
{
    e.Skip(); // Important...
}

void wxCrafterPlugin::OnAllEditorsClosing(wxCommandEvent& e)
{
    e.Skip();
    if(IsTabMode()) {
        m_allEditorsClosing = true;
        m_treeView->CloseProject(true);
    }
}

void wxCrafterPlugin::OnAllEditorsClosed(wxCommandEvent& e)
{
    e.Skip();
    if(IsTabMode()) {
        m_allEditorsClosing = false;
    }
}

void wxCrafterPlugin::OnDebugStarting(clDebugEvent& e)
{
    if(!m_mainPanel) {
        e.Skip();
        return;
    }

    if(!m_mgr) {
        e.Skip();
        return;
    }

    if(IsTabMode() && m_mgr->GetActivePage() == m_mainPanel) {

        // Show the preview instead
        wxCommandEvent showPreviewEvent(wxEVT_COMMAND_MENU_SELECTED, ID_SHOW_PREVIEW);
        m_mainPanel->GetEventHandler()->ProcessEvent(showPreviewEvent);

    } else {
        e.Skip();
    }
}

void wxCrafterPlugin::DoWriteFileContent(const wxFileName& fn, const wxString& content, IEditor* editor)
{
    if(editor) {
        editor->GetCtrl()->SetText(content);

    } else {

        // Write the file content from the disk
        wxCrafter::WriteFile(fn, content, true);
    }
}

void wxCrafterPlugin::OnSave(wxCommandEvent& e)
{
    CHECK_POINTER(m_mgr);
    if(IsTabMode() && m_mainPanel && m_mgr->GetActivePage() == m_mainPanel) {
        m_treeView->SaveProject();

    } else {
        e.Skip();
    }
}

bool wxCrafterPlugin::IsMainViewActive()
{
    return true; // IIUC this function was protecting against outside events being caught when in Tabbed mode and a
                 // different tab was active
    if(!m_mgr) {
        return true;
    } else {
        return IsTabMode() && m_mainPanel && m_mgr->GetActivePage() == m_mainPanel;
    }
}

// plugin menu
void wxCrafterPlugin::OnCloseProject(wxCommandEvent& e)
{
    CHECK_POINTER(m_mgr);
    wxUnusedVar(e);
    if(IsTabMode()) {
        m_mgr->ClosePage(_("[wxCrafter]"));
    }
    // m_treeView->CloseProject(true);
}

void wxCrafterPlugin::OnCloseProjectUI(wxUpdateUIEvent& e) { e.Enable(wxcProjectMetadata::Get().IsLoaded()); }

void wxCrafterPlugin::OnOpenProject(wxCommandEvent& e)
{
    wxUnusedVar(e);
    m_treeView->LoadProject(wxFileName());
}

void wxCrafterPlugin::OnOpenProjectUI(wxUpdateUIEvent& e) { e.Enable(wxcProjectMetadata::Get().IsLoaded() == false); }

void wxCrafterPlugin::OnSaveProject(wxCommandEvent& e)
{
    wxUnusedVar(e);
    m_treeView->SaveProject();
}

void wxCrafterPlugin::OnSaveProjectUI(wxUpdateUIEvent& e) { e.Enable(wxcEditManager::Get().IsDirty()); }

void wxCrafterPlugin::DoLoadAfterImport(ImportDlg::ImportFileData& data)
{
    if(m_mgr && data.addToProject && !data.virtualFolder.IsEmpty()) {
        wxArrayString filesToAdd;
        filesToAdd.Add(data.wxcpFilename.GetFullPath());
        if(DoCreateVirtualFolder(data.virtualFolder)) {
            m_mgr->AddFilesToVirtualFolder(data.virtualFolder, filesToAdd);
        }
    }

    if(data.loadWhenDone) {
        m_treeView->LoadProject(data.wxcpFilename);
    }

    // do it using event, orelse the main framw will steal the focus
    wxCommandEvent evtShowDesigner(wxEVT_SHOW_WXCRAFTER_DESIGNER);
    EventNotifier::Get()->AddPendingEvent(evtShowDesigner);
}

void wxCrafterPlugin::OnImportwxFBProject(wxCommandEvent& e)
{
    wxUnusedVar(e);
    DoImportFB();
}

void wxCrafterPlugin::OnImportXRC(wxCommandEvent& e)
{
    ImportDlg::ImportFileData data;
    ImportFromXrc import(wxCrafter::TopFrame());
    if(import.ImportProject(data)) {
        DoLoadAfterImport(data);
    }
}

void wxCrafterPlugin::OnImportwxSmith(wxCommandEvent& e)
{
    ImportDlg::ImportFileData data;
    ImportFromwxSmith import(wxCrafter::TopFrame());
    if(import.ImportProject(data)) {
        DoLoadAfterImport(data);
    }
}

void wxCrafterPlugin::OnDefineCustomControls(wxCommandEvent& e)
{
    DefineCustomControlWizard wiz(NULL);
    if(wiz.RunWizard(wiz.GetFirstPage())) {
        CustomControlTemplate controlData = wiz.GetControl();
        wxcSettings::Get().RegisterCustomControl(controlData);
        wxcSettings::Get().Save();
    }
}
void wxCrafterPlugin::OnEditCustomControls(wxCommandEvent& e)
{
    EditCustomControlDlg dlg(NULL);
    dlg.ShowModal();

    // Once this dialog is saved, we should:
    // 1) Refresh  the view
    wxCommandEvent evt(wxEVT_PROPERTIES_MODIFIED);
    EventNotifier::Get()->AddPendingEvent(evt);
}

void wxCrafterPlugin::OnDeleteCustomControls(wxCommandEvent& e)
{
    DeleteCustomControlDlg dlg(NULL);
    dlg.ShowModal();

    // Once this dialog is saved, we should:
    // 1) Refresh  the view
    wxCommandEvent evt(wxEVT_PROPERTIES_MODIFIED);
    EventNotifier::Get()->AddPendingEvent(evt);
}

void wxCrafterPlugin::OnAbout(wxCommandEvent& e)
{
    wxcAboutDlg dlg(NULL);
    dlg.ShowModal();
}

void wxCrafterPlugin::DoImportFB(const wxString& filename)
{
    ImportDlg::ImportFileData data;
    ImportFromwxFB import(wxCrafter::TopFrame());
    if(import.ImportProject(data, filename)) {
        DoLoadAfterImport(data);
    }
}
void wxCrafterPlugin::OnSettings(wxCommandEvent& e)
{
    wxcSettingsDlg dlg(NULL);
    dlg.ShowModal();

    if(dlg.IsRestartRequired()) {
        ::wxMessageBox(_("In order for the change to take place, you need to restart codelite"), _("wxCrafter"));
    }
}

void wxCrafterPlugin::DoInitDone(wxObject* obj)
{
    wxUnusedVar(obj);
#if !STANDALONE_BUILD
    wxToolBar* mainToolbar = EventNotifier::Get()->TopFrame()->GetToolBar();
    if(mainToolbar) {
        int toolHeight = mainToolbar->GetToolBitmapSize().GetHeight();

        wxCrafter::ResourceLoader rl;
        wxBitmap bmp = rl.Bitmap(toolHeight == 24 ? "wxc-logo-24" : "wxc-logo-16");
        wxTheApp->Connect(XRCID("ID_SHOW_DESIGNER"), wxEVT_COMMAND_MENU_SELECTED,
                          wxCommandEventHandler(wxCrafterPlugin::OnShowDesigner), NULL, (wxEvtHandler*)this);
    }

#endif

#if STANDALONE_BUILD
    /// explicitly load the lexers
    ColoursAndFontsManager::Get().Load();
#endif

    m_mainFrame = new MainFrame(nullptr, m_serverMode);

    m_treeView = new wxcTreeView(m_mainFrame->GetTreeParent(), this);
    m_mainFrame->Add(m_treeView);

    m_mainPanel = new GUICraftMainPanel(m_mainFrame->GetDesignerParent(), this, m_treeView->GetTree());
    m_mainFrame->Add(m_mainPanel);
    m_mainFrame->Layout();
    wxCrafter::SetTopFrame(m_mainFrame);
}

void wxCrafterPlugin::OnProjectLoaded(wxCommandEvent& e) { e.Skip(); }

void wxCrafterPlugin::OnSaveAll(clCommandEvent& e)
{
    e.Skip();
    if(wxcProjectMetadata::Get().IsLoaded()) {
        m_treeView->SaveProject();
    }
}

void wxCrafterPlugin::OnFileContextMenu(clContextMenuEvent& event)
{
    event.Skip();
    const wxArrayString& files = event.GetStrings();
    if(files.GetCount() == 1) {
        m_selectedFile = files.Item(0);
        if(m_selectedFile.GetExt() == "wxcp") {
            // a wxCrafter file
            event.GetMenu()->PrependSeparator();
            event.GetMenu()->Prepend(new wxMenuItem(event.GetMenu(), XRCID("open_wxC_project_from_context_menu"),
                                                    _("Open with wxCrafter..."), wxEmptyString, wxITEM_NORMAL));
            event.GetMenu()->Bind(wxEVT_COMMAND_MENU_SELECTED, &wxCrafterPlugin::OnOpenWxcpProject, this,
                                  XRCID("open_wxC_project_from_context_menu"));
        } else if(m_selectedFile.GetExt() == "fbp") {
            // form builder file
            event.GetMenu()->PrependSeparator();
            event.GetMenu()->Prepend(new wxMenuItem(event.GetMenu(), XRCID("import_wxFB_project_from_context_menu"),
                                                    _("Import with wxCrafter..."), wxEmptyString, wxITEM_NORMAL));
            event.GetMenu()->Bind(wxEVT_COMMAND_MENU_SELECTED, &wxCrafterPlugin::OnImportFBProject, this,
                                  XRCID("import_wxFB_project_from_context_menu"));
        } else if(m_selectedFile.GetExt() == "wxs") {
            event.GetMenu()->PrependSeparator();
            event.GetMenu()->Prepend(new wxMenuItem(event.GetMenu(), XRCID("import_wxSmith_project"),
                                                    _("Import with wxCrafter..."), wxEmptyString, wxITEM_NORMAL));
            event.GetMenu()->Bind(wxEVT_COMMAND_MENU_SELECTED, &wxCrafterPlugin::OnImportwxSmithProject, this,
                                  XRCID("import_wxSmith_project"));
        }
    }
}

void wxCrafterPlugin::OnImportFBProject(wxCommandEvent& event) { DoImportFB(m_selectedFile.GetFullPath()); }
void wxCrafterPlugin::OnOpenWxcpProject(wxCommandEvent& event) { DoLoadWxcProject(m_selectedFile); }

void wxCrafterPlugin::DoLoadWxcProject(const wxFileName& filename)
{
    DoShowDesigner();
    m_treeView->LoadProject(filename);
    DoSelectWorkspaceTab();

    if(!IsTabMode()) {
        wxCommandEvent evtShowDesigner(wxEVT_COMMAND_MENU_SELECTED, XRCID("ID_SHOW_DESIGNER"));
        m_mainFrame->GetEventHandler()->AddPendingEvent(evtShowDesigner);
    }
}

void wxCrafterPlugin::OnImportwxSmithProject(wxCommandEvent& event)
{
    ImportDlg::ImportFileData data;
    ImportFromwxSmith import(wxCrafter::TopFrame());
    if(import.ImportProject(data, m_selectedFile.GetFullPath())) {
        DoLoadAfterImport(data);
    }
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
    if(clGetManager()->GetWorkspace() && clGetManager()->GetWorkspace()->IsOpen()) {
        wxStringSet_t all_files;
        wxArrayString projects;
        ProjectPtr activeProject = clGetManager()->GetSelectedProject();
        if(!activeProject) {
            return;
        }
        wxCrafter::GetProjectFiles(activeProject->GetName(), all_files);

        // Filter out and keep only wxcp files
        std::for_each(all_files.begin(), all_files.end(), [&](const wxString& file) {
            if(FileExtManager::GetType(file) == FileExtManager::TypeWxCrafter) {
                wxcpFiles.Add(file);
            }
        });

        if(wxcpFiles.IsEmpty()) {
            ::wxMessageBox(_("This project does not contain any wxCrafter files"), "wxCrafter");
            return;
        }

        // Ensure that we have a designer
        if(DoShowDesigner()) {}

        // Now generate the code
        m_mainPanel->BatchGenerate(wxcpFiles);
    }
}

void wxCrafterPlugin::OnToggleView(clCommandEvent& event)
{
    if(event.GetString() != _("wxCrafter")) {
        event.Skip();
        return;
    }
#if 0
    if(event.IsSelected()) {
        // show it
        wxcImages images;
        m_mgr->GetWorkspacePaneNotebook()->AddPage(m_treeView, _("wxCrafter"), false, images.Bitmap("wxc_icon"));
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
    if(win == m_mainPanel) {
        event.Skip(false);
        event.SetAnswer(wxcEditManager::Get().IsDirty());
    }
}
