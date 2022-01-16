#include "wxguicraft_main_view.h"

#include "AuiToolBarTopLevel.h"
#include "EventsEditorDlg.h"
#include "ToolBoxPanel.h"
#include "allocator_mgr.h"
#include "attribute_style.h"
#include "button_wrapper.h"
#include "cl_command_event.h"
#include "ctags_manager.h"
#include "custom_control_wrapper.h"
#include "designer_panel.h"
#include "dialog_wrapper.h"
#include "duplicateTLWdlg.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileextmanager.h"
#include "fileutils.h"
#include "flexgridsizer_wrapper.h"
#include "frame_wrapper.h"
#include "globals.h"
#include "myxh_grid.h"
#include "myxh_listc.h"
#include "myxh_richtext.h"
#include "myxh_searchctrl.h"
#include "myxh_toolbk.h"
#include "myxh_treebk.h"
#include "notebook_base_wrapper.h"
#include "notebook_page_wrapper.h"
#include "panel_wrapper_top_level.h"
#include "plugin.h"
#include "popup_window_preview.h"
#include "popup_window_wrapper.h"
#include "preview_dialog.h"
#include "preview_frame.h"
#include "preview_panel.h"
#include "preview_wizard.h"
#include "ribbon_bar_wrapper.h"
#include "ribbon_page_wrapper.h"
#include "tool_bar_item_wrapper.h"
#include "wizard_wrapper.h"
#include "wxcTreeView.h"
#include "wxc_bitmap_code_generator.h"
#include "wxc_edit_manager.h"
#include "wxc_settings.h"
#include "wxcrafter_plugin.h"
#include "wxgui_bitmaploader.h"
#include "wxgui_helpers.h"
#include "xy_pair.h"

#include <algorithm>
#include <set>
#include <wx/app.h>
#include <wx/busyinfo.h>
#include <wx/ffile.h>
#include <wx/filedlg.h>
#include <wx/imaggif.h>
#include <wx/imaglist.h>
#include <wx/infobar.h>
#include <wx/log.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/sstream.h>
#include <wx/textdlg.h>
#include <wx/utils.h>
#include <wx/wupdlock.h>
#include <wx/xrc/xmlres.h>
#if defined(__WXOSX__)
#include <wx/osx/tooltip.h>
#endif

GUICraftMainPanel* GUICraftMainPanel::m_MainPanel = NULL;

static bool bManualSelection = false;

#define MENU_ENTRY(id, label, bmpname)              \
    menuItem = new wxMenuItem(&menu, id, label);    \
    menuItem->SetBitmap(bmpLoader.Bitmap(bmpname)); \
    menu.Append(menuItem);

#define MENU_SEPARATOR() menu.AppendSeparator();

BEGIN_EVENT_TABLE(GUICraftMainPanel, GUICraftMainPanelBase)

EVT_MENU(ID_GENERATE_CODE, GUICraftMainPanel::OnGenerateCode)
EVT_COMMAND_RANGE(ID_FIRST_CONTROL, ID_LAST_CONTROL - 1, wxEVT_COMMAND_BUTTON_CLICKED, GUICraftMainPanel::OnNewControl)
EVT_COMMAND_RANGE(ID_CHANGE_SIZER_FIRST, ID_CHANGE_SIZER_LAST - 1, wxEVT_COMMAND_MENU_SELECTED,
                  GUICraftMainPanel::OnChangeSizerType)
EVT_COMMAND_RANGE(ID_INSERT_INTO_SIZER_FIRST, ID_INSERT_INTO_SIZER_LAST - 1, wxEVT_COMMAND_MENU_SELECTED,
                  GUICraftMainPanel::OnInsertIntoSizer)
EVT_COMMAND(ID_WXCUSTOMCONTROL, wxEVT_COMMAND_BUTTON_CLICKED, GUICraftMainPanel::OnNewCustomControlMenu)
EVT_MENU_RANGE(ID_FIRST_CONTROL, ID_LAST_CONTROL - 1, GUICraftMainPanel::OnNewControl)
EVT_MENU(ID_DELETE_NODE, GUICraftMainPanel::OnDelete)
EVT_MENU_RANGE(ID_MOVE_NODE_UP, ID_MOVE_NODE_INTO_SIBLING, GUICraftMainPanel::OnMoveItem)
EVT_MENU(ID_SHOW_PREVIEW, GUICraftMainPanel::OnShowPreview)
EVT_MENU(ID_CANCEL_PREVIEW, GUICraftMainPanel::OnCancelPreview)
EVT_TOOL(ID_SAVE_WXGUI_PROJECT, GUICraftMainPanel::OnSaveProject)
EVT_AUITOOLBAR_TOOL_DROPDOWN(ID_GENERATE_CODE, GUICraftMainPanel::OnGenerateCodeMenu)
EVT_MENU(XRCID("GenerateCPP"), GUICraftMainPanel::OnCodeGenerationTypeChanged)
EVT_MENU(XRCID("GenerateXRC"), GUICraftMainPanel::OnCodeGenerationTypeChanged)
EVT_TOOL_RANGE(ID_TOOL_ALIGN_LEFT, ID_TOOL_WXEXPAND, GUICraftMainPanel::OnSizerTool)
EVT_TOOL(ID_TOOL_PROP1, GUICraftMainPanel::OnSetSizerProp1)

EVT_MENU(ID_EVENT_EDITOR, GUICraftMainPanel::OnEventEditor)
#if STANDALONE_BUILD
EVT_UPDATE_UI(ID_EVENT_EDITOR, GUICraftMainPanel::OnEventEditorUI)
#endif
EVT_MENU(ID_EVENT_EDITOR_COMMON, GUICraftMainPanel::OnEventEditorCommon)

// IMPORTANT: this entry MUST be last!
EVT_MENU(wxID_ANY, GUICraftMainPanel::OnMenuItemClicked)

// -----------------------------------------------------------
// UpdateUI events
// -----------------------------------------------------------
EVT_UPDATE_UI(ID_SAVE_WXGUI_PROJECT, GUICraftMainPanel::OnSaveProjectUI)
EVT_UPDATE_UI(ID_GENERATE_CODE, GUICraftMainPanel::OnGenerateCodeUI)
EVT_UPDATE_UI_RANGE(ID_MOVE_NODE_UP, ID_MOVE_NODE_INTO_SIBLING, GUICraftMainPanel::OnMoveItemUI)
EVT_UPDATE_UI(ID_DELETE_NODE, GUICraftMainPanel::OnDeleteUI)
EVT_UPDATE_UI(ID_CANCEL_PREVIEW, GUICraftMainPanel::OnCancelPreviewUI)
EVT_UPDATE_UI(ID_TOOL_PROP1, GUICraftMainPanel::OnSetSizerProp1UI)
EVT_UPDATE_UI_RANGE(ID_TOOL_ALIGN_LEFT, ID_TOOL_WXEXPAND, GUICraftMainPanel::OnSizerToolUI)
EVT_UPDATE_UI(ID_SHOW_PREVIEW, GUICraftMainPanel::OnShowPreviewUI)
EVT_UPDATE_UI(wxID_ANY, GUICraftMainPanel::OnMenuItemUI)

EVT_MENU(ID_CUT, GUICraftMainPanel::OnCut)
EVT_MENU(ID_PASTE, GUICraftMainPanel::OnPaste)
EVT_MENU(ID_DUPLICATE, GUICraftMainPanel::OnDuplicate)
EVT_MENU(ID_COPY, GUICraftMainPanel::OnCopy)
EVT_MENU(ID_RENAME, GUICraftMainPanel::OnRename)
END_EVENT_TABLE()

const wxEventType wxEVT_UPDATE_PREVIEW = wxNewEventType();
const wxEventType wxEVT_UPDATE_EVENTSEDITORPANE = wxNewEventType();
wxDEFINE_EVENT(wxEVT_EVENTS_PROPERTIES_UPDATED, wxCommandEvent); // Must do it this way as it's used in a Bind()
const wxEventType wxEVT_PROPERTIES_MODIFIED = wxNewEventType();
const wxEventType wxEVT_PROJECT_METADATA_MODIFIED = wxNewEventType();
const wxEventType wxEVT_WXGUI_PROJECT_CLOSED = wxNewEventType();
const wxEventType wxEVT_WXGUI_PROJECT_SAVED = wxNewEventType();
const wxEventType wxEVT_WXGUI_PROJECT_LOADED = wxNewEventType();
const wxEventType wxEVT_CLOSE_PREVIEW = wxNewEventType();
const wxEventType wxEVT_PREVIEW_CLOSED = wxNewEventType();
const wxEventType wxEVT_DESIGNER_DELETED = wxNewEventType();
const wxEventType wxEVT_WXC_CMD_GENERATE_CODE = wxNewEventType();
wxDEFINE_EVENT(wxEVT_WXC_CMD_BATCH_GENERATE_CODE, wxCommandEvent);
const wxEventType wxEVT_WXC_OPEN_PROJECT = wxNewEventType();
const wxEventType wxEVT_WXC_SAVE_PROJECT = XRCID("wxEVT_WXC_SAVE_PROJECT");
const wxEventType wxEVT_WXC_CLOSE_PROJECT = wxNewEventType();
const wxEventType wxEVT_REFRESH_DESIGNER = wxNewEventType();
const wxEventType wxEVT_TREE_ITEM_SELECTED = wxNewEventType();
const wxEventType wxEVT_SHOW_CONTEXT_MENU = wxNewEventType();
const wxEventType wxEVT_DELETE_CONTROL = wxNewEventType();
const wxEventType wxEVT_REFRESH_PROPERTIES_VIEW = wxNewEventType();
const wxEventType wxEVT_WXC_PROJECT_LOADED = wxNewEventType();
const wxEventType wxEVT_WXC_SELECT_TREE_TLW = wxNewEventType();
const wxEventType wxEVT_WXC_CODE_PREVIEW_PAGE_CHANGED = wxNewEventType();

GUICraftMainPanel::GUICraftMainPanel(wxWindow* parent, wxCrafterPlugin* plugin, clTreeCtrl* treeView)
    : GUICraftMainPanelBase(parent)
    , m_clipboardItem(NULL)
    , m_previewAlive(false)
    , m_treeControls(treeView)
    , m_plugin(plugin)
{
#if defined(__WXMSW__) || defined(__WXOSX__)
    wxToolTip::SetDelay(100);
#endif

#if STANDALONE_BUILD
    FileLogger::OpenLog("wxcrafter.log", FileLogger::Dbg);
#endif

    wxcSettings::Get().Load();
    wxCrafter::ResourceLoader bmps;
    wxLog::EnableLogging(false);

    m_panelToolBox->GetSizer()->Add(new ToolBoxPanel(m_panelToolBox, this), 0, wxEXPAND);

    m_toolbar->AddTool(ID_GENERATE_CODE, _("Generate Code"), bmps.Bitmap("generate-code"), _("Generate Code"));
    m_toolbar->AddSeparator();
    m_toolbar->AddTool(ID_SHOW_PREVIEW, _("Show Preview..."), bmps.Bitmap("preview"), _("Show Preview"));
    m_toolbar->AddTool(ID_CANCEL_PREVIEW, _("Close Preview..."), bmps.Bitmap("cancel"), _("Close Preview"));
    m_toolbar->AddSeparator();

    m_toolbar->AddTool(ID_DELETE_NODE, _("Delete Item"), bmps.Bitmap("delete"), _("Delete Item"));
    m_toolbar->AddSeparator();
    m_toolbar->AddTool(ID_TOOL_ALIGN_LEFT, _("Align Left"), bmps.Bitmap("align-left"), _("Align Left"), wxITEM_CHECK);
    m_toolbar->AddTool(ID_TOOL_ALIGN_HCENTER, _("Align Center"), bmps.Bitmap("align-center-horizontal"),
                       _("Align Center Horizontally"), wxITEM_CHECK);
    m_toolbar->AddTool(ID_TOOL_ALIGN_RIGHT, _("Align Right"), bmps.Bitmap("align-right"), _("Align Right"),
                       wxITEM_CHECK);
    m_toolbar->AddSeparator();

    m_toolbar->AddTool(ID_TOOL_ALIGN_TOP, _("Align Top"), bmps.Bitmap("align-top"), _("Align Top"), wxITEM_CHECK);
    m_toolbar->AddTool(ID_TOOL_ALIGN_VCENTER, _("Align Middle"), bmps.Bitmap("align-center-vertical"),
                       _("Align Center Vertically"), wxITEM_CHECK);
    m_toolbar->AddTool(ID_TOOL_ALIGN_BOTTOM, _("Align Bottom"), bmps.Bitmap("align-bottom"), _("Align Bottom"),
                       wxITEM_CHECK);
    m_toolbar->AddSeparator();

    m_toolbar->AddTool(ID_TOOL_BORDER_ALL, _("All Borders"), bmps.Bitmap("wxall"), _("All Borders"), wxITEM_CHECK);
    m_toolbar->AddTool(ID_TOOL_BORDER_LEFT, _("Left Border"), bmps.Bitmap("border-left"), _("Left Border"),
                       wxITEM_CHECK);
    m_toolbar->AddTool(ID_TOOL_BORDER_TOP, _("Top Border"), bmps.Bitmap("border-top"), _("Top Border"), wxITEM_CHECK);
    m_toolbar->AddTool(ID_TOOL_BORDER_RIGHT, _("Right Border"), bmps.Bitmap("border-right"), _("Right Border"),
                       wxITEM_CHECK);
    m_toolbar->AddTool(ID_TOOL_BORDER_BOTTOM, _("Bottom Border"), bmps.Bitmap("border-bottom"), _("Bottom Border"),
                       wxITEM_CHECK);

    m_toolbar->AddSeparator();
    m_toolbar->AddTool(ID_TOOL_PROP1, _("Set Sizer Proportion to 1"), bmps.Bitmap("proportion-1"),
                       _("Set Sizer Proportion to 1"), wxITEM_CHECK);
    m_toolbar->AddTool(ID_TOOL_WXEXPAND, _("Expand Item"), bmps.Bitmap("expand"), _("Expand Item"), wxITEM_CHECK);
    m_toolbar->AddSeparator();

    m_toolbar->AddTool(ID_MOVE_NODE_UP, _("Move Up"), bmps.Bitmap("move-up"), _("Move Up"));
    m_toolbar->AddTool(ID_MOVE_NODE_DOWN, _("Move Down"), bmps.Bitmap("move-down"), _("Move Down"));
    m_toolbar->AddTool(ID_MOVE_NODE_INTO_SIZER, _("Move Left"), bmps.Bitmap("move-left"),
                       _("Move Left into parent sizer"));
    m_toolbar->AddTool(ID_MOVE_NODE_INTO_SIBLING, _("Move Right"), bmps.Bitmap("move-upper-right"),
                       _("Move Right into a sibling sizer"));

    wxAuiToolBarItem* item = m_toolbar->FindTool(ID_GENERATE_CODE);
    item->SetHasDropDown(true);

    m_toolbar->Realize();

    m_propertiesPage = new PropertiesSheet(m_panelProperties);
    m_panelProperties->GetSizer()->Add(m_propertiesPage, 1, wxEXPAND);
    m_panelProperties->GetSizer()->Layout();
    EventNotifier::Get()->Bind(wxEVT_WXC_CMD_BATCH_GENERATE_CODE, &GUICraftMainPanel::OnBatchGenerateCode, this);
    EventNotifier::Get()->Connect(wxEVT_PROPERTIES_MODIFIED,
                                  wxCommandEventHandler(GUICraftMainPanel::OnPropertyChanged), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_PREVIEW_CTRL_SELECTED,
                                  wxCommandEventHandler(GUICraftMainPanel::OnPreviewItemSelected), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_WXC_SELECT_TREE_TLW,
                                  wxCommandEventHandler(GUICraftMainPanel::OnSelectToplevelItem), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_PREVIEW_BOOKPAGE_SELECTED,
                                  wxCommandEventHandler(GUICraftMainPanel::OnBookPageSelected), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_PREVIEW_RIBBON_PAGE_SELECTED,
                                  wxCommandEventHandler(GUICraftMainPanel::OnRibbonPageSelected), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_PREVIEW_CLOSED, wxCommandEventHandler(GUICraftMainPanel::OnPreviewClosed), NULL,
                                  this);
    EventNotifier::Get()->Connect(wxEVT_WXC_OPEN_PROJECT, wxCommandEventHandler(GUICraftMainPanel::OnOpenProject), NULL,
                                  this);
    EventNotifier::Get()->Connect(wxEVT_WXC_SAVE_PROJECT, wxCommandEventHandler(GUICraftMainPanel::OnSaveProject), NULL,
                                  this);
    EventNotifier::Get()->Connect(wxEVT_WXC_CLOSE_PROJECT, wxCommandEventHandler(GUICraftMainPanel::OnCloseProject),
                                  NULL, this);
    EventNotifier::Get()->Connect(wxEVT_WXC_CMD_GENERATE_CODE, wxCommandEventHandler(GUICraftMainPanel::OnGenerateCode),
                                  NULL, this);
    EventNotifier::Get()->Connect(wxEVT_REFRESH_DESIGNER, wxCommandEventHandler(GUICraftMainPanel::OnRefreshView), NULL,
                                  this);
    EventNotifier::Get()->Connect(wxEVT_PREVIEW_BAR_SELECTED,
                                  wxCommandEventHandler(GUICraftMainPanel::OnBarItemSelected), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_SHOW_CONTEXT_MENU, wxCommandEventHandler(GUICraftMainPanel::OnShowContextMenu),
                                  NULL, this);
    EventNotifier::Get()->Connect(wxEVT_DELETE_CONTROL, wxCommandEventHandler(GUICraftMainPanel::OnDelete), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_UPDATE_PREVIEW, wxCommandEventHandler(GUICraftMainPanel::OnUpdatePreview), NULL,
                                  this);
    EventNotifier::Get()->Bind(wxEVT_FINDBAR_ABOUT_TO_SHOW, &GUICraftMainPanel::OnFindBar, this);
    EventNotifier::Get()->Connect(wxEVT_REFRESH_PROPERTIES_VIEW,
                                  wxCommandEventHandler(GUICraftMainPanel::OnRefreshPropertiesView), NULL, this);
    m_MainPanel = this;

    m_treeControls->Connect(wxEVT_COMMAND_TREE_BEGIN_DRAG, wxTreeEventHandler(GUICraftMainPanel::OnBeginDrag), NULL,
                            this);
    m_treeControls->Connect(wxEVT_COMMAND_TREE_END_DRAG, wxTreeEventHandler(GUICraftMainPanel::OnEndDrag), NULL, this);
    m_treeControls->Connect(wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler(GUICraftMainPanel::OnMenu), NULL, this);
    m_treeControls->Connect(wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK,
                            wxTreeEventHandler(GUICraftMainPanel::OnItemRightClick), NULL, this);
    m_treeControls->Connect(wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler(GUICraftMainPanel::OnItemSelected), NULL,
                            this);
    m_treeControls->Connect(ID_DELETE_NODE, wxEVT_COMMAND_MENU_SELECTED,
                            wxCommandEventHandler(GUICraftMainPanel::OnDelete), NULL,
                            this); // Translated from a DEL keypress

    wxTheApp->Connect(wxID_UNDO, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(GUICraftMainPanel::OnUndo), NULL,
                      this);
    wxTheApp->Connect(wxID_REDO, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(GUICraftMainPanel::OnRedo), NULL,
                      this);
    wxTheApp->Connect(wxID_UNDO, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(GUICraftMainPanel::OnUndoUI), NULL, this);
    wxTheApp->Connect(wxID_REDO, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(GUICraftMainPanel::OnRedoUI), NULL, this);
    wxTheApp->Connect(XRCID("label_current_state"), wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(GUICraftMainPanel::OnLabelCurrentState), NULL, this);
    wxTheApp->Connect(wxEVT_MULTIPLE_UNREDO, wxCommandEventHandler(GUICraftMainPanel::OnLoadCurrentState), NULL, this);
    this->Connect(ID_FORM_TYPE, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(GUICraftMainPanel::OnNewFormUI), NULL, this);

    int sashPos = wxcSettings::Get().GetSashPosition();
    if(sashPos != wxNOT_FOUND) {
        m_mainSplitter->SetSashPosition(sashPos);
    }

    // Use codelite's coloring settings
    LexerConf::Ptr_t cppLexer = EditorConfigST::Get()->GetLexer("C++");
    if(cppLexer) {
        cppLexer->Apply(m_textCtrlCppSource);
        cppLexer->Apply(m_textCtrlHeaderSource);
        m_textCtrlCppSource->SetProperty("lexer.cpp.track.preprocessor", "0");
        m_textCtrlCppSource->SetProperty("lexer.cpp.update.preprocessor", "0");

        m_textCtrlHeaderSource->SetProperty("lexer.cpp.track.preprocessor", "0");
        m_textCtrlHeaderSource->SetProperty("lexer.cpp.update.preprocessor", "0");
    }

    LexerConf::Ptr_t xmlLexer = EditorConfigST::Get()->GetLexer("XML");
    if(xmlLexer) {
        xmlLexer->Apply(m_textCtrlXrc);
    }

    wxTheApp->Connect(XRCID("save_file"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(GUICraftMainPanel::OnSaveProjectUI),
                      NULL, this);

    ::wxPGPropertyBooleanUseCheckbox(m_pgMgrAuiProperties->GetGrid());
    ::wxPGPropertyBooleanUseCheckbox(m_pgMgrSizerFlags->GetGrid());
}

GUICraftMainPanel::~GUICraftMainPanel()
{
    // Notify the find-bar to clear any pointer referenced by it
    // that is belonged to this view
    DoDismissFindBar();

    wxDELETE(m_clipboardItem);

    wxcSettings::Get().SetSashPosition(m_mainSplitter->GetSashPosition());
    wxcSettings::Get().Save();

    EventNotifier::Get()->Disconnect(wxEVT_PROPERTIES_MODIFIED,
                                     wxCommandEventHandler(GUICraftMainPanel::OnPropertyChanged), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_PREVIEW_CTRL_SELECTED,
                                     wxCommandEventHandler(GUICraftMainPanel::OnPreviewItemSelected), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_PREVIEW_BOOKPAGE_SELECTED,
                                     wxCommandEventHandler(GUICraftMainPanel::OnBookPageSelected), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_PREVIEW_RIBBON_PAGE_SELECTED,
                                     wxCommandEventHandler(GUICraftMainPanel::OnRibbonPageSelected), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_PREVIEW_CLOSED, wxCommandEventHandler(GUICraftMainPanel::OnPreviewClosed),
                                     NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_WXC_OPEN_PROJECT, wxCommandEventHandler(GUICraftMainPanel::OnOpenProject),
                                     NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_WXC_SAVE_PROJECT, wxCommandEventHandler(GUICraftMainPanel::OnSaveProject),
                                     NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_WXC_CMD_GENERATE_CODE,
                                     wxCommandEventHandler(GUICraftMainPanel::OnGenerateCode), NULL, this);
    EventNotifier::Get()->Unbind(wxEVT_WXC_CMD_BATCH_GENERATE_CODE, &GUICraftMainPanel::OnBatchGenerateCode, this);
    EventNotifier::Get()->Disconnect(wxEVT_REFRESH_DESIGNER, wxCommandEventHandler(GUICraftMainPanel::OnRefreshView),
                                     NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_WXC_CLOSE_PROJECT, wxCommandEventHandler(GUICraftMainPanel::OnCloseProject),
                                     NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_PREVIEW_BAR_SELECTED,
                                     wxCommandEventHandler(GUICraftMainPanel::OnBarItemSelected), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_SHOW_CONTEXT_MENU,
                                     wxCommandEventHandler(GUICraftMainPanel::OnShowContextMenu), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_DELETE_CONTROL, wxCommandEventHandler(GUICraftMainPanel::OnDelete), NULL,
                                     this);
    EventNotifier::Get()->Disconnect(wxEVT_UPDATE_PREVIEW, wxCommandEventHandler(GUICraftMainPanel::OnUpdatePreview),
                                     NULL, this);
    EventNotifier::Get()->Unbind(wxEVT_FINDBAR_ABOUT_TO_SHOW, &GUICraftMainPanel::OnFindBar, this);
    EventNotifier::Get()->Disconnect(wxEVT_REFRESH_PROPERTIES_VIEW,
                                     wxCommandEventHandler(GUICraftMainPanel::OnRefreshPropertiesView), NULL, this);

    // Only disconnect the events in Tabbed mode,
    // in the "frame" mode, the main panel and the tree view have the same
    // parent
    if(false) {
        m_treeControls->Disconnect(wxEVT_COMMAND_TREE_BEGIN_DRAG, wxTreeEventHandler(GUICraftMainPanel::OnBeginDrag),
                                   NULL, this);
        m_treeControls->Disconnect(wxEVT_COMMAND_TREE_END_DRAG, wxTreeEventHandler(GUICraftMainPanel::OnEndDrag), NULL,
                                   this);
        m_treeControls->Disconnect(wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler(GUICraftMainPanel::OnMenu), NULL,
                                   this);
        m_treeControls->Disconnect(wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK,
                                   wxTreeEventHandler(GUICraftMainPanel::OnItemRightClick), NULL, this);
        m_treeControls->Disconnect(wxEVT_COMMAND_TREE_SEL_CHANGED,
                                   wxTreeEventHandler(GUICraftMainPanel::OnItemSelected), NULL, this);
        m_treeControls->Disconnect(ID_DELETE_NODE, wxEVT_COMMAND_MENU_SELECTED,
                                   wxCommandEventHandler(GUICraftMainPanel::OnDelete), NULL, this);
        wxTheApp->Disconnect(ID_COPY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(GUICraftMainPanel::OnCopy),
                             NULL, this);
        wxTheApp->Disconnect(ID_CUT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(GUICraftMainPanel::OnCut), NULL,
                             this);
        wxTheApp->Disconnect(ID_PASTE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(GUICraftMainPanel::OnPaste),
                             NULL, this);
    }

    wxTheApp->Disconnect(wxID_UNDO, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(GUICraftMainPanel::OnUndo), NULL,
                         this);
    wxTheApp->Disconnect(wxID_REDO, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(GUICraftMainPanel::OnRedo), NULL,
                         this);
    wxTheApp->Disconnect(wxID_UNDO, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(GUICraftMainPanel::OnUndoUI), NULL, this);
    wxTheApp->Disconnect(wxID_REDO, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(GUICraftMainPanel::OnRedoUI), NULL, this);
    wxTheApp->Disconnect(XRCID("label_current_state"), wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(GUICraftMainPanel::OnLabelCurrentState), NULL, this);
    wxTheApp->Disconnect(wxEVT_MULTIPLE_UNREDO, wxCommandEventHandler(GUICraftMainPanel::OnLoadCurrentState), NULL,
                         this);
    wxTheApp->Disconnect(XRCID("save_file"), wxEVT_UPDATE_UI,
                         wxUpdateUIEventHandler(GUICraftMainPanel::OnSaveProjectUI), NULL, this);

    m_treeControls = NULL;
    m_MainPanel = NULL;

    wxCommandEvent deleteEvent(wxEVT_DESIGNER_DELETED);
    EventNotifier::Get()->AddPendingEvent(deleteEvent);
}

void GUICraftMainPanel::Clear()
{
#ifdef __WXMSW__
    wxWindowUpdateLocker locker(this);
#endif
    // Clear all the data
    m_propertiesPage->Construct(NULL);
    m_sizerFlags.Construct(m_pgMgrSizerFlags->GetGrid(), NULL);
    m_styles.Clear(m_pgMgrStyles->GetGrid());

    m_treeControls->DeleteAllItems();
    m_treeControls->AddRoot(_("wxCrafter Project"), 0, 0);
    wxcProjectMetadata::Get().Reset();

    // Notify all controls to perform cleanup
    wxCommandEvent evt(wxEVT_WXGUI_PROJECT_CLOSED);
    EventNotifier::Get()->AddPendingEvent(evt);

    // Close any opened preview
    wxCommandEvent closePreviewEvt(wxEVT_CLOSE_PREVIEW);
    EventNotifier::Get()->AddPendingEvent(closePreviewEvt);
}

void GUICraftMainPanel::OnNewFormUI(wxUpdateUIEvent& event) { event.Enable(wxcProjectMetadata::Get().IsLoaded()); }

void GUICraftMainPanel::OnMenu(wxTreeEvent& event)
{
    wxMenu menu;
    GUICraftItemData* data = GetSelItemData();
    Allocator::Instance()->PrepareMenu(menu, data ? data->m_wxcWidget : NULL);
    PopupMenu(&menu);
}

void GUICraftMainPanel::OnShowContextMenu(wxCommandEvent& e)
{
    GUICraftItemData* data = GetSelItemData();

    wxString title;
    if(data && data->m_wxcWidget) {
        title = data->m_wxcWidget->GetName();
    }
    wxMenu menu(title.IsEmpty() ? "" : title);
    Allocator::Instance()->PrepareMenu(menu, data ? data->m_wxcWidget : NULL);
    PopupMenu(&menu);
}

void GUICraftMainPanel::OnItemSelected(wxTreeEvent& event)
{
    event.Skip();
    wxUnusedVar(event);
    GUICraftItemData* data = GetSelItemData();

    if(!data) {
        // Clear all properties
        DoUpdatPropertiesFlags(NULL);
        m_propertiesPage->Construct(NULL);
        m_auiPaneInfo.Construct(m_pgMgrAuiProperties->GetGrid(), NULL);

        m_propertiesPage->ConstructProjectSettings();
    }

    CHECK_POINTER(data);
    CHECK_POINTER(data->m_wxcWidget);

    DoUpdateNotebookSelection(m_treeControls->GetSelection());
    if(!bManualSelection) {
        NotifyPreviewChanged();
    }
    DoUpdatePropertiesView();

    wxCommandEvent eventTreeItemSeleted(wxEVT_TREE_ITEM_SELECTED);
    eventTreeItemSeleted.SetString(data->m_wxcWidget->GetName());
    EventNotifier::Get()->AddPendingEvent(eventTreeItemSeleted);
}

void GUICraftMainPanel::DoUpdatePropertiesView()
{
    GUICraftItemData* data = GetSelItemData();
    if(!data) {
        // Clear all properties
        DoUpdatPropertiesFlags(NULL);
        m_propertiesPage->Construct(NULL);
        m_auiPaneInfo.Construct(m_pgMgrAuiProperties->GetGrid(), NULL);
        m_propertiesPage->ConstructProjectSettings();
    }

    CHECK_POINTER(data);
    CHECK_POINTER(data->m_wxcWidget);
    DoUpdatPropertiesFlags(data->m_wxcWidget);
    m_propertiesPage->Construct(data->m_wxcWidget);
}

GUICraftItemData* GUICraftMainPanel::GetSelItemData()
{
    wxTreeItemId itemId = m_treeControls->GetSelection();
    if(itemId.IsOk() == false) {
        return NULL;
    }

    GUICraftItemData* data = dynamic_cast<GUICraftItemData*>(m_treeControls->GetItemData(itemId));
    return data;
}

void GUICraftMainPanel::OnSizerForTopLevelWinUI(wxUpdateUIEvent& e)
{
    GUICraftItemData* data = GetSelItemData();
    CHECK_POINTER(data);

    e.Enable((data->m_wxcWidget->IsSizer()) || (!data->m_wxcWidget->IsSizer() && !data->m_wxcWidget->HasMainSizer()));
}

void GUICraftMainPanel::OnShowPreview(wxCommandEvent& e)
{
    wxTreeItemId topLevelItem = DoGetTopLevelTreeItem();
    CHECK_TREEITEM(topLevelItem);

    GUICraftItemData* data = dynamic_cast<GUICraftItemData*>(m_treeControls->GetItemData(topLevelItem));
    CHECK_POINTER(data);

    wxWindow* parent = GetTopFrame();
    FrameWrapper* fw = dynamic_cast<FrameWrapper*>(data->m_wxcWidget);
    if(fw) {
        PreviewFrame* frame = new PreviewFrame(parent, *fw);
        m_previewAlive = true;
        frame->Show();
    }

    PanelWrapperTopLevel* pw = dynamic_cast<PanelWrapperTopLevel*>(data->m_wxcWidget);
    if(pw) {
        PreviewPanel* frame = new PreviewPanel(parent, *pw);
        m_previewAlive = true;
        frame->Show();
    }

    AuiToolBarTopLevelWrapper* aui = dynamic_cast<AuiToolBarTopLevelWrapper*>(data->m_wxcWidget);
    if(aui) {
        PreviewPanel* frame = new PreviewPanel(parent, *aui);
        m_previewAlive = true;
        frame->Show();
    }

    DialogWrapper* dw = dynamic_cast<DialogWrapper*>(data->m_wxcWidget);
    if(dw) {
        // Show the dialog
        PreviewDialog* dlg = new PreviewDialog(parent, *dw);
        m_previewAlive = true;
        dlg->Show();
    }

    WizardWrapper* ww = dynamic_cast<WizardWrapper*>(data->m_wxcWidget);
    if(ww) {
        // Show the dialog
        PreviewWizard dlg(parent, *ww);
        m_previewAlive = true;
        dlg.Run();
    }

    PopupWindowWrapper* pop = dynamic_cast<PopupWindowWrapper*>(data->m_wxcWidget);
    if(pop) {
        PopupWindowPreview* frame = new PopupWindowPreview(parent, *pop);
        m_previewAlive = true;
        frame->Show();
    }
}

void GUICraftMainPanel::OnItemRightClick(wxTreeEvent& event)
{
    event.Skip();
    m_treeControls->SelectItem(event.GetItem());
}

void GUICraftMainPanel::DoUpdatPropertiesFlags(wxcWidget* data)
{
    DoUpdateSizerFlags(data);
    DoUpdateStyleFlags(data);
}

void GUICraftMainPanel::DoUpdateStyleFlags(wxcWidget* data) { m_styles.Construct(m_pgMgrStyles->GetGrid(), data); }

void GUICraftMainPanel::DoUpdateSizerFlags(wxcWidget* data)
{
    m_auiPaneInfo.Construct(m_pgMgrAuiProperties->GetGrid(), data);
    m_sizerFlags.Construct(m_pgMgrSizerFlags->GetGrid(), data);

    if(!data) {
        DoShowPropertiesPage(m_panelAuiPaneInfo, "wxAuiPaneInfo", false);
        DoShowPropertiesPage(m_panelSizerFlags, _("Sizer Flags"), false);

    } else {
        int cursel = m_notebook2->GetSelection();
        if(data->IsAuiPane()) {
            DoShowPropertiesPage(m_panelAuiPaneInfo, "wxAuiPaneInfo", true);
            DoShowPropertiesPage(m_panelSizerFlags, _("Sizer Flags"), false);

        } else {
            DoShowPropertiesPage(m_panelAuiPaneInfo, "wxAuiPaneInfo", false);
            DoShowPropertiesPage(m_panelSizerFlags, _("Sizer Flags"), true);
        }
#ifndef __WXGTK__
        m_notebook2->SetSelection(cursel);
#endif
    }
}

void GUICraftMainPanel::NotifyPreviewChanged(int eventType)
{

    GUICraftItemData* data = GetSelItemData();
    CHECK_POINTER(data);
    CHECK_POINTER(data->m_wxcWidget);

    // If the selected item is notebook page, make sure we select this page again after the update
    NotebookPageWrapper* bookPage = dynamic_cast<NotebookPageWrapper*>(data->m_wxcWidget);
    if(bookPage) {
        NotebookBaseWrapper* book = dynamic_cast<NotebookBaseWrapper*>(bookPage->GetParent());
        if(book) {
            book->SetSelection(bookPage);
        }
    }

    wxcWidget* parent = data->m_wxcWidget;
    while(parent && !parent->IsTopWindow()) {
        parent = parent->GetParent();
    }

    CHECK_POINTER(parent);

    wxCommandEvent evt(eventType);
    TopLevelWinWrapper* topWin = dynamic_cast<TopLevelWinWrapper*>(parent);
    if(topWin) {
        evt.SetString(topWin->DesignerXRC(false));
        evt.SetInt(topWin->GetType());
        EventNotifier::Get()->AddPendingEvent(evt);
    }

    // Also, notify the deisgner to mark the new selection
    wxCommandEvent eventTreeItemSeleted(wxEVT_TREE_ITEM_SELECTED);
    eventTreeItemSeleted.SetString(data->m_wxcWidget->GetName());
    EventNotifier::Get()->AddPendingEvent(eventTreeItemSeleted);
}

void GUICraftMainPanel::OnPropertyChanged(wxCommandEvent& e)
{
    e.Skip();
    // Special case, before we test for valid pointers: a metadata alteration
    GUICraftItemData* itemData = GetSelItemData();
    CHECK_POINTER(itemData);
    CHECK_POINTER(itemData->m_wxcWidget);
    wxTreeItemId item;
    wxcWidget* modifiedWidget = reinterpret_cast<wxcWidget*>(e.GetClientData());
    if(modifiedWidget && modifiedWidget != itemData->m_wxcWidget) {
        // Locate the proper wxTreeItemId to update
        item = DoFindItemByWxcWidget(modifiedWidget, m_treeControls->GetRootItem());

    } else {
        item = m_treeControls->GetSelection();
        if(item.IsOk()) {
            GUICraftItemData* data = dynamic_cast<GUICraftItemData*>(m_treeControls->GetItemData(item));
            if(data) {
                modifiedWidget = dynamic_cast<wxcWidget*>(data->m_wxcWidget);
            }
        }
    }

    wxString newname = e.GetString();
    if(item.IsOk() && !newname.IsEmpty() && (newname != m_treeControls->GetItemText(item))) {
        m_treeControls->SetItemText(item, newname);
    }

    // Special case: An auitoolbar item changing kind
    if(modifiedWidget && modifiedWidget->IsParentAuiToolbar()) {
        ToolBarItemWrapper* auiItem = dynamic_cast<ToolBarItemWrapper*>(modifiedWidget);
        if(auiItem) {
            auiItem->UpdateRegisteredEventsIfNeeded(); // Switch the event between wxEVT_COMMAND_MENU_SELECTED and
                                                       // wxEVT_COMMAND_AUITOOLBAR_TOOL_DROPDOWN
            auiItem->OnPropertiesUpdated();
            DoUpdatePropertiesView();

            wxCrafter::TOOL_TYPE toolType = wxCrafter::GetToolType(auiItem->PropertyString(PROP_KIND));
            if(toolType == wxCrafter::TOOL_TYPE_DROPDOWN && auiItem->PropertyString(PROP_DROPDOWN_MENU) == "1") {
                // We're a dropdown item, and the user wants to construct its menu inside wxC
                if(auiItem->GetChildren().empty()) {
                    wxcWidget* menu = Allocator::Instance()->Create(ID_WXMENU);
                    menu->SetParent(auiItem);
                    DoInsertControl(menu, auiItem, Allocator::INSERT_CHILD,
                                    Allocator::Instance()->GetImageId(ID_WXMENU));
                }
            } else if(!auiItem->GetChildren().empty()) {
                // There's a redundant menu, either because user switched to DIY, or because it's no longer a dropdown
                wxTreeItemIdValue cookie;
                wxTreeItemId menuitem = m_treeControls->GetFirstChild(item, cookie);
                if(menuitem.IsOk()) {
                    DoUnsetItemData(menuitem);
                    delete auiItem->GetChildren().front();
                    m_treeControls->DeleteChildren(menuitem);
                    m_treeControls->Delete(menuitem);
                }
            }

            // I'm not certain this is still needed, but it, and its associated code, seems like a good idea anyway
            wxCommandEvent evt(wxEVT_UPDATE_EVENTSEDITORPANE);
            EventNotifier::Get()->AddPendingEvent(evt); // Keep the events view in sync
        }
    }
    NotifyPreviewChanged();
}

void GUICraftMainPanel::OnNewControl(wxCommandEvent& e)
{
    if(e.GetId() == ID_WXCUSTOMCONTROL) {
        // Custom Controls are handled differently
        e.Skip();
        return;
    }

    GUICraftItemData* data = GetSelItemData();
    int imgId = Allocator::Instance()->GetImageId(e.GetId());

    bool isTopLevel = (e.GetId() == ID_WXDIALOG || e.GetId() == ID_WXFRAME || e.GetId() == ID_WXWIZARD ||
                       e.GetId() == ID_WXPANEL_TOPLEVEL || e.GetId() == ID_WXPOPUPWINDOW ||
                       e.GetId() == ID_WXIMAGELIST || e.GetId() == ID_WXAUITOOLBARTOPLEVEL);
    if(isTopLevel) {

        // Use the Wizard instead of adding the form directly
        wxCommandEvent createFormEvent(wxEVT_COMMAND_MENU_SELECTED, XRCID("wxcp_new_form"));
        createFormEvent.SetInt(e.GetId());
        wxTheApp->AddPendingEvent(createFormEvent);

    } else if(data && data->m_wxcWidget) {
        // Non toplevel window
        wxcWidget* control = Allocator::Instance()->Create(e.GetId());
        CHECK_POINTER(control);

        int insertType;

        // Special handling for treebook
        if(e.GetId() == ID_WXTREEBOOK_SUB_PAGE) {
            insertType = Allocator::INSERT_CHILD;

        } else if(e.GetId() == ID_WXPGPROPERTY_SUB) {
            insertType = Allocator::INSERT_CHILD;

        } else if(e.GetId() == ID_WXSPACER) {
            // A spacer has no proportion by default, but this one's supposed to stretch
            control->SizerItem().SetProportion(1);
            insertType =
                Allocator::Instance()->GetInsertionType(control->GetType(), data->m_wxcWidget->GetType(), false);

        } else {
            insertType =
                Allocator::Instance()->GetInsertionType(control->GetType(), data->m_wxcWidget->GetType(), true);
        }
        DoInsertControl(control, data->m_wxcWidget, insertType, imgId);
    }
}

void GUICraftMainPanel::OnStyleChanged(wxPropertyGridEvent& event)
{
    GUICraftItemData* itemData = GetSelItemData();
    CHECK_POINTER(itemData);

    wxVariant v = event.GetPropertyValue();
    wxString asString = v.GetString();
    // WrapperBase::MapStyles_t flags = DoGetStylesFromCheckList(m_checkListStyles);
    // itemData->m_wxcWidget->SetStyles(flags);
    // wxcProjectMetadata::Get().SetIsModified(true);
    NotifyPreviewChanged();
}

void GUICraftMainPanel::OnGenerateCode(wxCommandEvent& e)
{
    wxUnusedVar(e);
    DoGenerateCode(false);
}

bool GUICraftMainPanel::GenerateCppOutput(wxString& cpp, wxString& header, wxArrayString& headers,
                                          wxStringMap_t& additionalFiles, size_t flags) const
{
    // If onlySelection, just produce output for the currently selected tree branch. By default do everything
    wxTreeItemId start, item;
    wxTreeItemId selectedItem = DoGetTopLevelTreeItem();
    if((flags & kGenCodeSelectionOnly) && selectedItem.IsOk()) {
        start = selectedItem;
    } else {
        start = m_treeControls->GetRootItem();
    }
    wxCHECK(start.IsOk(), "Invalid tree root");

    wxTreeItemIdValue cookie;
    if(start == m_treeControls->GetRootItem()) {
        item = m_treeControls->GetFirstChild(start, cookie);
        flags &= ~kGenCodeSelectionOnly; // in case onlySelection was asked for, but selectedItem wasn't Ok

    } else {
        item = start;
    }

    while(item.IsOk()) {
        GUICraftItemData* data = dynamic_cast<GUICraftItemData*>(m_treeControls->GetItemData(item));
        if(data) {
            wxcWidget* wb = dynamic_cast<wxcWidget*>(data->m_wxcWidget);
            if(wb) {
                TopLevelWinWrapper* tl = dynamic_cast<TopLevelWinWrapper*>(wb);
                if(tl) {
                    tl->GenerateCode(wxcProjectMetadata::Get(), !(flags & kGenCodeForPreview),
                                     (flags & kGenCodeSelectionOnly) || (item != selectedItem), // output base only
                                     cpp, header, headers, additionalFiles);
                    if(flags & kGenCodeSelectionOnly) {
                        break; // We only want a single tlw, so don't loop
                    }
                }
            }
        }
        item = m_treeControls->GetNextChild(m_treeControls->GetRootItem(), cookie);
    }
    return true;
}

void GUICraftMainPanel::GenerateXrcOutput(wxString& output, size_t flags) const
{
    // If onlySelection, just produce output for the currently selected tree branch. By default do everything
    wxTreeItemId start, item;
    wxTreeItemId sel = DoGetTopLevelTreeItem();
    if((flags & kGenCodeSelectionOnly) && sel.IsOk()) {
        start = sel;
    } else {
        start = m_treeControls->GetRootItem();
    }
    wxCHECK_RET(start.IsOk(), "Invalid tree root");

    wxTreeItemIdValue cookie;
    if(start == m_treeControls->GetRootItem()) {
        item = m_treeControls->GetFirstChild(start, cookie);
        flags &= ~kGenCodeSelectionOnly; // in case onlySelection was asked for, but sel wasn't Ok

    } else {
        item = start;
    }

    while(item.IsOk()) {
        GUICraftItemData* data = dynamic_cast<GUICraftItemData*>(m_treeControls->GetItemData(item));
        if(data) {
            wxcWidget* wb = dynamic_cast<wxcWidget*>(data->m_wxcWidget);
            if(wb) {
                TopLevelWinWrapper* tl = dynamic_cast<TopLevelWinWrapper*>(wb);
                if(tl) {
                    wxString text;
                    tl->ToXRC(text, wxcWidget::XRC_LIVE);
                    output << text;
                    if(flags & kGenCodeSelectionOnly) {
                        break; // We only want a single tlw, so don't loop
                    }
                }
            }
        }
        item = m_treeControls->GetNextChild(m_treeControls->GetRootItem(), cookie);
    }
}

void GUICraftMainPanel::OnSizerTool(wxCommandEvent& e)
{
    GUICraftItemData* itemData = GetSelItemData();
    CHECK_POINTER(itemData);
    CHECK_POINTER(itemData->m_wxcWidget);

    wxString bitname = GetStyleFromGuiID(e.GetId());
    if(bitname.IsEmpty()) {
        return;
    }

    itemData->m_wxcWidget->EnableSizerFlag(bitname, e.IsChecked());

    if(bitname == "wxALL") {
        itemData->m_wxcWidget->EnableSizerFlag("wxLEFT", e.IsChecked());
        itemData->m_wxcWidget->EnableSizerFlag("wxRIGHT", e.IsChecked());
        itemData->m_wxcWidget->EnableSizerFlag("wxTOP", e.IsChecked());
        itemData->m_wxcWidget->EnableSizerFlag("wxBOTTOM", e.IsChecked());
    }

    wxcEditManager::Get().PushState("sizer flags change");
    DoUpdatPropertiesFlags(itemData->m_wxcWidget);
    NotifyPreviewChanged();
}

void GUICraftMainPanel::OnSizerToolUI(wxUpdateUIEvent& e)
{
    if(!wxcProjectMetadata::Get().IsLoaded()) {
        e.Enable(false);
        return;
    }

    GUICraftItemData* itemData = GetSelItemData();
    if(!itemData || !itemData->m_wxcWidget || !itemData->m_wxcWidget->IsSizerItem()) {
        e.Enable(false);
        e.Check(false);
        return;
    }

    e.Enable(true);
    wxString bitname = GetStyleFromGuiID(e.GetId());
    if(bitname.IsEmpty()) {
        e.Enable(false);
        return;
    }

    if(bitname == "wxALL") {
        bool previous = itemData->m_wxcWidget->GetSizerFlags().Item("wxALL").is_set;
        bool current = itemData->m_wxcWidget->GetSizerFlags().Item("wxLEFT").is_set &&
                       itemData->m_wxcWidget->GetSizerFlags().Item("wxRIGHT").is_set &&
                       itemData->m_wxcWidget->GetSizerFlags().Item("wxTOP").is_set &&
                       itemData->m_wxcWidget->GetSizerFlags().Item("wxBOTTOM").is_set;

        e.Check(current);
        if(current != previous) {
            itemData->m_wxcWidget->EnableSizerFlag("wxALL", current);
            DoUpdatPropertiesFlags(itemData->m_wxcWidget);
            NotifyPreviewChanged();
        }
        return;
    }

    if(itemData->m_wxcWidget->GetSizerFlags().Contains(bitname) &&
       itemData->m_wxcWidget->GetSizerFlags().Item(bitname).is_set) {
        e.Check(true);

    } else {
        e.Check(false);
    }

    m_sizerFlags.DoUpdateUI(m_pgMgrSizerFlags->GetGrid(), e); // This will do updateui for the alignment tools
}

wxString GUICraftMainPanel::GetStyleFromGuiID(int guiId) const
{
    switch(guiId) {
    case ID_TOOL_ALIGN_LEFT:
        return STRINGFY(wxALIGN_LEFT);

    case ID_TOOL_ALIGN_HCENTER:
        return STRINGFY(wxALIGN_CENTER_HORIZONTAL);

    case ID_TOOL_ALIGN_RIGHT:
        return STRINGFY(wxALIGN_RIGHT);

    case ID_TOOL_ALIGN_TOP:
        return STRINGFY(wxALIGN_TOP);

    case ID_TOOL_ALIGN_VCENTER:
        return STRINGFY(wxALIGN_CENTER_VERTICAL);

    case ID_TOOL_ALIGN_BOTTOM:
        return STRINGFY(wxALIGN_BOTTOM);

    case ID_TOOL_BORDER_ALL:
        return STRINGFY(wxALL);

    case ID_TOOL_BORDER_LEFT:
        return STRINGFY(wxLEFT);

    case ID_TOOL_BORDER_TOP:
        return STRINGFY(wxTOP);

    case ID_TOOL_BORDER_RIGHT:
        return STRINGFY(wxRIGHT);

    case ID_TOOL_BORDER_BOTTOM:
        return STRINGFY(wxBOTTOM);

    case ID_TOOL_WXEXPAND:
        return STRINGFY(wxEXPAND);

    default:
        return "";
    }
}

JSONElement GUICraftMainPanel::ToJSON(const wxTreeItemId& fromItem)
{
    JSONElement json = JSONElement::createArray("windows");
    wxTreeItemIdValue cookie;
    wxTreeItemId rootItem = fromItem.IsOk() ? fromItem : m_treeControls->GetRootItem();
    wxTreeItemId child = m_treeControls->GetFirstChild(rootItem, cookie);
    while(child.IsOk()) {

        GUICraftItemData* itemData = dynamic_cast<GUICraftItemData*>(m_treeControls->GetItemData(child));
        if(itemData && itemData->m_wxcWidget) {
            JSONElement obj = JSONElement::createObject();
            itemData->m_wxcWidget->FixPaths(
                wxcProjectMetadata::Get().GetProjectPath()); // Fix abs paths to fit the new project file
            itemData->m_wxcWidget->Serialize(obj);
            json.arrayAppend(obj);
        }

        child = m_treeControls->GetNextChild(rootItem, cookie);
    }
    DoUpdatePropertiesView();
    return json;
}

void GUICraftMainPanel::OnSaveProject(wxCommandEvent& e)
{
    wxArrayString customControls = GetCustomControlsUsed();
    if(wxcProjectMetadata::Get().GetProjectFile().IsEmpty()) {
        wxString path =
            wxFileSelector(_("Save as"), wxEmptyString, _("my_gui.wxcp"), "wxCrafter Project File(*.wxcp)|*.wxcp",
                           wxFileSelectorDefaultWildcardStr, wxFD_SAVE | wxFD_CHANGE_DIR | wxFD_OVERWRITE_PROMPT);
        if(path.IsEmpty()) {
            return;
        }

        wxcProjectMetadata::Get().SetProjectFile(path);
    }
    wxcProjectMetadata::Get().SetObjCounter(wxcWidget::GetObjCounter());

    JSONRoot root(cJSON_Object);

    // If we dont have bitmap function assigned to this project, create new one
    if(wxcProjectMetadata::Get().GetBitmapFunction().IsEmpty()) {
        wxcProjectMetadata::Get().GenerateBitmapFunctionName();
    }

    JSONElement metadata = wxcProjectMetadata::Get().ToJSON();
    wxcProjectMetadata::Get().AppendCustomControlsJSON(customControls, metadata);
    root.toElement().append(metadata);

    wxFFile fp(wxcProjectMetadata::Get().GetProjectFile(), "w+b");
    if(fp.IsOpened()) {
        root.toElement().append(ToJSON());

        fp.Write(root.toElement().format(), wxConvUTF8);
        fp.Close();

        // Notify that this project was saved
        wxCommandEvent eventSave(wxEVT_WXGUI_PROJECT_SAVED);
        EventNotifier::Get()->AddPendingEvent(eventSave);

    } else {

        wxString msg;
        msg << _("Error occurred while opening file '") << wxcProjectMetadata::Get().GetProjectFile()
            << _("' for writing");
        wxMessageBox(msg, "wxCrafter", wxICON_ERROR | wxOK | wxCENTER);
    }
}

void GUICraftMainPanel::OnOpenProject(wxCommandEvent& e)
{
    wxcSettings::Get().SetInitCompleted(true);
    if(!e.GetString().IsEmpty() && wxcProjectMetadata::Get().IsLoaded() &&
       e.GetString() == wxcProjectMetadata::Get().GetProjectFile()) {
        // the currently loaded file is the same as this one
        // do nothing
        if(!IsShown()) {
            Show();
        }
        Raise();
        return;
    }

    if(wxcProjectMetadata::Get().IsLoaded() && wxcEditManager::Get().IsDirty()) {
        if(::wxMessageBox(_("Current file has been modified\nContinue?"), "wxCrafter", wxYES_NO | wxCANCEL | wxCENTER,
                          wxCrafter::TopFrame()) != wxYES) {
            return;
        }
    }

    wxString path = e.GetString();
    if(path.IsEmpty()) {
        // User request
        path = wxFileSelector(_("Open wxCrafter project"), wxEmptyString, wxEmptyString, wxEmptyString,
                              "wxCrafter Project File(*.wxcp)|*.wxcp", wxFD_OPEN);

    } else {
        // Progrematically
        wxFileName curfile = wxcProjectMetadata::Get().GetProjectFile();
        wxFileName newfile = path;

        wxString p1, p2;
        p1 = curfile.GetFullPath();
        p2 = path;
        if(curfile == path) {
            return;
        }
    }

    if(path.IsEmpty() == false) {

        /// update the history list
        wxArrayString history = wxcSettings::Get().GetHistory();
        if(history.Index(path) == wxNOT_FOUND) {
            history.Add(path);
            wxcSettings::Get().SetHistory(history);
            wxcSettings::Get().Save();
        }

        wxFileName fn(path);
        wxFFile fp(fn.GetFullPath(), "r+b");
        if(fp.IsOpened()) {
            wxString content;
            fp.ReadAll(&content, wxConvUTF8);
            fp.Close();

            // Update the root item
            wxString rootText;
            rootText << fn.GetFullPath();
            m_treeControls->SetItemText(m_treeControls->GetRootItem(), rootText);
            LoadProject(fn, content);

            // The next line shouldn't be necessary, as it's done when the wxEVT_WXC_PROJECT_LOADED event is handled in
            // wxcEditManager
            // However, for unknown reasons, a heisenbug sometimes makes that event not arrive, which means the initial
            // state is
            // never initialised; this prevents the first Undo from being undoable. So, until the bug is squashed:
            wxcEditManager::Get().SaveInitialState(CurrentState());

            // notify about project load
            wxCommandEvent evtProjectLoaded(wxEVT_WXC_PROJECT_LOADED);
            evtProjectLoaded.SetString(fn.GetFullPath());
            EventNotifier::Get()->AddPendingEvent(evtProjectLoaded);
        }
    }
}

void GUICraftMainPanel::LoadProject(const wxFileName& fn, const wxString& fileContent, bool lightLoad)
{
    bool deleteAllItems = false;
    wxFileName project_file = fn;
    JSONRoot json(fileContent);
    if(!lightLoad) {
        Clear();

        // Read the metadata first
        wxcProjectMetadata::Get().SetProjectFile(fn.GetFullPath());
        wxcProjectMetadata::Get().FromJSON(json.toElement().namedObject("metadata"));
        wxcProjectMetadata::Get().UpdatePaths();

    } else {
        deleteAllItems = true;
    }

    // Read the top-level windows
    JSONElement windows = json.toElement().namedObject("windows");
    int nCount = windows.arraySize();

#ifdef __WXMSW__
    wxWindowUpdateLocker locker(m_treeControls);
#endif
    if(deleteAllItems) {
        m_treeControls->DeleteAllItems();
        m_treeControls->AddRoot(_("wxCrafter Project"), 0, 0);
        project_file = wxcProjectMetadata::Get().GetProjectFile();
        wxcProjectMetadata::Get().FromJSON(
            json.toElement().namedObject("metadata")); // Needed for Redo() to work correctly
        wxcProjectMetadata::Get().UpdatePaths();
    }

    wxTreeItemId dummy;
    for(int i = 0; i < nCount; i++) {
        wxcWidget* wrapper = Allocator::Instance()->CreateWrapperFromJSON(windows.arrayItem(i));
        DoBuildTree(dummy, wrapper, m_treeControls->GetRootItem());
    }

    wxTreeItemIdValue cookie;
    wxTreeItemId rootItem = m_treeControls->GetRootItem();
    wxTreeItemId firstChild = m_treeControls->GetFirstChild(rootItem, cookie);
    if(firstChild.IsOk()) {
        m_treeControls->SelectItem(firstChild);
    }
    NotifyPreviewChanged(wxEVT_WXGUI_PROJECT_LOADED);

    wxString rootText = project_file.IsOk() ? project_file.GetFullPath() : _("wxCrafter Project");

    m_treeControls->SetItemText(m_treeControls->GetRootItem(), rootText);
    wxcWidget::SetObjCounter(wxcProjectMetadata::Get().GetObjCounter());

    // Expand the top level root item
    if(rootItem.IsOk() && m_treeControls->ItemHasChildren(rootItem)) {
        m_treeControls->Expand(rootItem);
    }
}

void GUICraftMainPanel::DoBuildTree(wxTreeItemId& itemToSelect, wxcWidget* wrapper, const wxTreeItemId& parent,
                                    const wxTreeItemId& beforeItem, bool insertBefore)
{
    int imgId = Allocator::Instance()->GetImageId(wrapper->GetType());
    wxTreeItemId item;

    if(beforeItem.IsOk()) {

        wxTreeItemId insertionItem = beforeItem;
        if(insertBefore) {
            insertionItem = m_treeControls->GetPrevSibling(beforeItem);
            if(insertionItem.IsOk() == false) {
                insertionItem = parent;
            }
        }

        item = m_treeControls->InsertItem(parent, insertionItem, wrapper->GetName(), imgId, imgId,
                                          new GUICraftItemData(wrapper));
        if(itemToSelect.IsOk() == false) {
            itemToSelect = item;
        }

    } else {

        item = m_treeControls->AppendItem(parent, wrapper->GetName(), imgId, imgId, new GUICraftItemData(wrapper));
        if(itemToSelect.IsOk() == false) {
            itemToSelect = item;
        }
    }

    wxcWidget::List_t::const_iterator iter = wrapper->GetChildren().begin();
    for(; iter != wrapper->GetChildren().end(); ++iter) {
        DoBuildTree(itemToSelect, (*iter), item);
    }
}

void GUICraftMainPanel::DoUnsetItemData(const wxTreeItemId& item)
{
    if(item.IsOk()) {

        // Remove the WrapperBase item associated with this item-data
        // by setting it to NULL
        GUICraftItemData* itemData = dynamic_cast<GUICraftItemData*>(m_treeControls->GetItemData(item));
        if(itemData) {
            itemData->m_wxcWidget = NULL;
        }

        if(m_treeControls->ItemHasChildren(item)) {
            wxTreeItemIdValue cookie;
            wxTreeItemId child = m_treeControls->GetFirstChild(item, cookie);
            while(child.IsOk()) {
                DoUnsetItemData(child);
                child = m_treeControls->GetNextChild(item, cookie);
            }
        }
    }
}

void GUICraftMainPanel::DoMoveToplevelWindow(wxcWidget* tlw, int direction)
{
    CHECK_POINTER(tlw);
    wxTreeItemId treeItem = m_treeControls->GetSelection();
    CHECK_TREEITEM(treeItem);
    wxTreeItemId root = m_treeControls->GetRootItem();
    CHECK_TREEITEM(root);
    wxTreeItemId target;
    bool doPrepend(false);

    if(direction == ID_MOVE_NODE_DOWN) {
        target = m_treeControls->GetNextSibling(treeItem);
        CHECK_TREEITEM(target);

    } else {
        // There's no InsertBefore(), so we need to go up 2, then InsertAfter()
        target = m_treeControls->GetPrevSibling(treeItem);
        CHECK_TREEITEM(target);
        wxTreeItemId previous = m_treeControls->GetPrevSibling(target);
        if(previous.IsOk()) {
            target = previous;
        } else {
            // The exception: we can't go up by 2 if the second item is moving above the first. So:
            doPrepend = true;
        }
    }
    int image = m_treeControls->GetItemImage(treeItem);
    wxString text = m_treeControls->GetItemText(treeItem);

#ifdef __WXMSW__
    wxWindowUpdateLocker locker(m_treeControls);
#endif

    DoUnsetItemData(treeItem);
    m_treeControls->Delete(treeItem);

    wxTreeItemId insertedItem;
    if(!doPrepend) {
        insertedItem = m_treeControls->InsertItem(root, target, text, image, image, new GUICraftItemData(tlw));
    } else {
        insertedItem = m_treeControls->AppendItem(root, text, image, image, new GUICraftItemData(tlw));
    }

    CHECK_TREEITEM(insertedItem);

    // Restore the parent tree item data
    wxTreeItemId dummy;
    const wxcWidget::List_t& list = tlw->GetChildren();
    wxcWidget::List_t::const_iterator iter = list.begin();
    for(; iter != list.end(); ++iter) {
        DoBuildTree(dummy, *iter, insertedItem);
    }

    m_treeControls->EnsureVisible(insertedItem);
    m_treeControls->SelectItem(insertedItem);

    wxcEditManager::Get().PushState("top-level window move");
    DoRefresh(wxEVT_UPDATE_PREVIEW);
}

void GUICraftMainPanel::OnMoveItem(wxCommandEvent& e)
{
    GUICraftItemData* itemData = GetSelItemData();
    if(!itemData || !itemData->m_wxcWidget) {
        return;
    } else if(!itemData->m_wxcWidget->GetParent()) {
        return DoMoveToplevelWindow(itemData->m_wxcWidget, e.GetId());
    }

    wxTreeItemId treeItem, treeItemParent;
    treeItem = m_treeControls->GetSelection();
    CHECK_TREEITEM(treeItem);

    treeItemParent = m_treeControls->GetItemParent(treeItem);
    CHECK_TREEITEM(treeItemParent);

    wxString itemName = itemData->m_wxcWidget->GetName();
    wxcWidget* nodeToMove = itemData->m_wxcWidget;
    CHECK_POINTER(nodeToMove->GetParent());
    wxcWidget* subTree(NULL);

    switch(e.GetId()) {
    case ID_MOVE_NODE_DOWN:
        nodeToMove->MoveDown();
        subTree = nodeToMove->GetParent();
        break;
    case ID_MOVE_NODE_UP:
        nodeToMove->MoveUp();
        subTree = nodeToMove->GetParent();
        break;
    case ID_MOVE_NODE_INTO_SIZER:
        treeItemParent = m_treeControls->GetItemParent(treeItemParent); // We need to go back a generation this time
        CHECK_TREEITEM(treeItemParent);
        {
            wxcWidget* grandparent = nodeToMove->GetParent()->GetParent();
            wxCHECK_RET(grandparent && grandparent->IsSizer(), "UpdateUI failure: No grandparent sizer");
            nodeToMove->Reparent(grandparent);
            subTree = grandparent;
        }
        break;
    case ID_MOVE_NODE_INTO_SIBLING:
        wxcWidget* siblingSizer = nodeToMove->GetAdjacentSiblingSizer();
        wxCHECK_RET(siblingSizer, "UpdateUI failure: No adjacent sibling sizer");
        nodeToMove->Reparent(siblingSizer);
        subTree = siblingSizer->GetParent();
    }
    CHECK_POINTER(subTree);

#ifdef __WXMSW__
    wxWindowUpdateLocker locker(m_treeControls);
#endif

    DoUnsetItemData(treeItemParent);
    m_treeControls->DeleteChildren(treeItemParent);

    // Restore the parent tree item data
    wxTreeItemId dummy;
    m_treeControls->SetItemData(treeItemParent, new GUICraftItemData(subTree));
    const wxcWidget::List_t& list = subTree->GetChildren();
    wxcWidget::List_t::const_iterator iter = list.begin();
    for(; iter != list.end(); ++iter) {
        DoBuildTree(dummy, *iter, treeItemParent);
    }

    wxTreeItemId where;
    DoFindName(treeItemParent, itemName, where);
    if(where.IsOk()) {
        m_treeControls->EnsureVisible(where);
        m_treeControls->SelectItem(where);
    }

    wxcEditManager::Get().PushState("move");
    DoRefresh(wxEVT_UPDATE_PREVIEW);
}

void GUICraftMainPanel::OnDelete(wxCommandEvent& e)
{
    GUICraftItemData* data = GetSelItemData();
    if(data) {
        int eventType = wxEVT_UPDATE_PREVIEW;
        if(data->m_wxcWidget->IsTopWindow()) {
            // Notify the preview that we are about to delete the entire preview page
            NotifyPreviewChanged(wxEVT_WXGUI_PROJECT_CLOSED);
            eventType = wxEVT_WXGUI_PROJECT_LOADED;
        }

        wxTreeItemId newSel = DoFindBestSelection(m_treeControls->GetSelection());

        m_treeControls->DeleteChildren(m_treeControls->GetSelection());
        m_treeControls->Delete(m_treeControls->GetSelection());
        if(newSel.IsOk()) {
            m_treeControls->SelectItem(newSel);
        }
        CallAfter(&GUICraftMainPanel::DoRefresh, eventType);
        m_treeControls->CallAfter(&clTreeCtrl::SetFocus); // Without this, focus will unexpectedly appear elsewhere,
                                                          // e.g. on a propertygrid style
        wxcEditManager::Get().PushState("deletion");
    }
}

void GUICraftMainPanel::DoRefresh(int eventType)
{
    DoUpdatePropertiesView();
    NotifyPreviewChanged(eventType);
}

void GUICraftMainPanel::ExpandToItem(const wxString& name, wxTreeItemId& searchFromItem)
{
    if(!searchFromItem.IsOk()) {
        searchFromItem = m_treeControls->GetRootItem();
    }

    wxTreeItemId item;
    DoFindName(searchFromItem, name, item);
    if(item.IsOk()) {
        m_treeControls->Expand(item);
        m_treeControls->EnsureVisible(item);
        m_treeControls->SelectItem(item);
    }
}

void GUICraftMainPanel::DoFindName(const wxTreeItemId& parent, const wxString& name, wxTreeItemId& item)
{
    if(item.IsOk()) {
        return;
    }

    if(parent.IsOk() == false) {
        return;
    }

    if(m_treeControls->GetItemText(parent) == name) {
        item = parent;
        return;
    }

    if(m_treeControls->ItemHasChildren(parent)) {
        wxTreeItemIdValue cookie;
        wxTreeItemId child = m_treeControls->GetFirstChild(parent, cookie);
        while(child.IsOk()) {
            DoFindName(child, name, item);
            if(item.IsOk()) {
                return;
            }
            child = m_treeControls->GetNextChild(parent, cookie);
        }
    }
}

void GUICraftMainPanel::OnMoveItemUI(wxUpdateUIEvent& e)
{
    e.Enable(false); // Disable by default here; it simplifies all the 'if' statements

    GUICraftItemData* d = GetSelItemData();
    CHECK_POINTER(d);
    wxcWidget* control = d->m_wxcWidget;
    CHECK_POINTER(control);

    if(e.GetId() == ID_MOVE_NODE_DOWN) {
        if(control->GetParent()) {
            e.Enable(control->CanMoveDown());
        } else if(control->IsTopWindow()) {
            wxTreeItemId item;
            DoFindName(m_treeControls->GetRootItem(), control->GetName(), item);
            if(item.IsOk()) {
                e.Enable(m_treeControls->GetNextSibling(item).IsOk());
            }
        }

    } else if(e.GetId() == ID_MOVE_NODE_UP) {
        if(control->GetParent()) {
            e.Enable(control->CanMoveUp());
        } else if(control->IsTopWindow()) {
            wxTreeItemId item;
            DoFindName(m_treeControls->GetRootItem(), control->GetName(), item);
            if(item.IsOk()) {
                e.Enable(m_treeControls->GetPrevSibling(item).IsOk());
            }
        }
    } else if(e.GetId() == ID_MOVE_NODE_INTO_SIZER) {
        if(!control->IsSizerItem()) {
            return;
        }
        wxcWidget* parent = control->GetParent();
        CHECK_POINTER(parent);
        if(parent->IsSizer()) {
            wxcWidget* grandparent = parent->GetParent();
            e.Enable(grandparent && grandparent->IsSizer());
        }
    } else if(e.GetId() == ID_MOVE_NODE_INTO_SIBLING) {
        if(!control->IsSizerItem()) {
            return;
        }
        if(control->CanMoveRight()) {
            bool isAbove;
            control->GetAdjacentSiblingSizer(&isAbove); // Which icon should we use? upper-right or lower-right?
            wxCrafter::ResourceLoader bmps;
            if(isAbove) {
                m_toolbar->SetToolBitmap(ID_MOVE_NODE_INTO_SIBLING, bmps.Bitmap("move-upper-right"));
            } else {
                m_toolbar->SetToolBitmap(ID_MOVE_NODE_INTO_SIBLING, bmps.Bitmap("move-lower-right"));
            }

            e.Enable(true);
        }
    }
}

void GUICraftMainPanel::OnSetSizerProp1(wxCommandEvent& e)
{
    GUICraftItemData* d = GetSelItemData();
    CHECK_POINTER(d);
    CHECK_POINTER(d->m_wxcWidget);

    if(e.IsChecked()) {
        d->m_wxcWidget->SizerItem().SetProportion(1);
        m_sizerFlags.Construct(m_pgMgrSizerFlags->GetGrid(), d->m_wxcWidget);

    } else {
        d->m_wxcWidget->SizerItem().SetProportion(0);
        m_sizerFlags.Construct(m_pgMgrSizerFlags->GetGrid(), d->m_wxcWidget);
    }

    wxcEditManager::Get().PushState("sizer proportion change");
    NotifyPreviewChanged();
}

void GUICraftMainPanel::OnSetSizerProp1UI(wxUpdateUIEvent& e)
{
    GUICraftItemData* d = GetSelItemData();
    if(!d || !d->m_wxcWidget || !d->m_wxcWidget->IsSizerItem()) {
        e.Check(false);
        e.Enable(false);
        return;

    } else {
        e.Enable(wxcProjectMetadata::Get().IsLoaded());
        e.Check(d->m_wxcWidget->SizerItem().GetProportion() == 1);
    }
}

wxTreeItemId GUICraftMainPanel::DoGetTopLevelTreeItem() const
{
    wxTreeItemId item = m_treeControls->GetSelection();
    if(!item.IsOk()) {
        return wxTreeItemId();
    }

    wxTreeItemId parent = item;

    while(true) {

        wxTreeItemId tmpParent = m_treeControls->GetItemParent(parent);
        if(tmpParent.IsOk() && tmpParent != m_treeControls->GetRootItem()) {
            parent = tmpParent;

        } else {
            break;
        }
    }
    return parent;
}

void GUICraftMainPanel::OnMenuItemClicked(wxCommandEvent& e)
{
    GUICraftItemData* itemData = GetSelItemData();
    if(!itemData || !itemData->m_wxcWidget) {
        e.Skip();
        return;
    }

    if(Allocator::GetCommonEvents().Exists(e.GetId())) {
        ConnectDetails eventDetails = Allocator::GetCommonEvents().Item(e.GetId());
        itemData->m_wxcWidget->AddEvent(eventDetails);

    } else if(itemData->m_wxcWidget->GetControlEvents().Exists(e.GetId())) {
        ConnectDetails eventDetails = itemData->m_wxcWidget->GetControlEvents().Item(e.GetId());
        itemData->m_wxcWidget->AddEvent(eventDetails);

    } else {
        e.Skip();
    }
}

void GUICraftMainPanel::OnMenuItemUI(wxUpdateUIEvent& e)
{
    GUICraftItemData* itemData = GetSelItemData();
    CHECK_POINTER(itemData);
    CHECK_POINTER(itemData->m_wxcWidget);

    if(Allocator::GetCommonEvents().Exists(e.GetId())) {
        ConnectDetails eventDetails = Allocator::GetCommonEvents().Item(e.GetId());
        e.Check(itemData->m_wxcWidget->HasEvent(eventDetails.GetEventName()));

    } else if(itemData->m_wxcWidget->GetControlEvents().Exists(e.GetId())) {
        ConnectDetails eventDetails = itemData->m_wxcWidget->GetControlEvents().Item(e.GetId());
        e.Check(itemData->m_wxcWidget->HasEvent(eventDetails.GetEventName()));

    } else {
        e.Skip();
    }
}

void GUICraftMainPanel::OnDeleteUI(wxUpdateUIEvent& e)
{
    GUICraftItemData* itemData = GetSelItemData();
    e.Enable(itemData);
}

void GUICraftMainPanel::OnEventEditor(wxCommandEvent& e) { wxUnusedVar(e); }

void GUICraftMainPanel::OnEventEditorCommon(wxCommandEvent& e) { wxUnusedVar(e); }

wxTreeItemId GUICraftMainPanel::DoFindBestSelection(const wxTreeItemId& item)
{
    if(item.IsOk() == false) {
        return wxTreeItemId();
    }

    wxTreeItemId newSel = m_treeControls->GetPrevSibling(item);
    if(newSel.IsOk() == false) {
        newSel = m_treeControls->GetNextSibling(item);
        if(newSel.IsOk() == false && m_treeControls->GetItemParent(item)) {
            newSel = m_treeControls->GetItemParent(item);
        }
    }
    return newSel;
}

bool GUICraftMainPanel::DoUpdateNotebookSelection(const wxTreeItemId& item)
{
    if(!item.IsOk()) {
        return false;
    }

    bool dirty = false;
    GUICraftItemData* itemData = dynamic_cast<GUICraftItemData*>(m_treeControls->GetItemData(item));
    CHECK_POINTER_RET_FALSE(itemData);
    CHECK_POINTER_RET_FALSE(itemData->m_wxcWidget);

    wxcWidget* wrapper = itemData->m_wxcWidget;
    NotebookPageWrapper* page = NULL;
    RibbonPageWrapper* ribbonPage = NULL;
    while(wrapper) {
        page = dynamic_cast<NotebookPageWrapper*>(wrapper);
        if(page) {
            break;
        }
        wrapper = wrapper->GetParent();
    }

    if(!page) {
        wrapper = itemData->m_wxcWidget;
        while(wrapper) {
            ribbonPage = dynamic_cast<RibbonPageWrapper*>(wrapper);
            if(ribbonPage) {
                break;
            }
            wrapper = wrapper->GetParent();
        }
    }

    if(!page && !ribbonPage) {
        return false;
    }

    if(page) {
        // Page is an actual notebook page, get the notebook that it represents
        NotebookBaseWrapper* book = page->GetNotebook();
        CHECK_POINTER_RET_FALSE(book);

        // Get the selection
        dirty = (book->GetSelection() != page);

        // Mark the page as selected (it will unselect all other pages)
        book->SetSelection(page);
    } else {
        // Page is an actual notebook page, get the notebook that it represents
        RibbonBarWrapper* book = dynamic_cast<RibbonBarWrapper*>(ribbonPage->GetParent());
        CHECK_POINTER_RET_FALSE(book);

        // Get the selection
        dirty = (book->GetSelection() != ribbonPage);

        // Mark the page as selected (it will unselect all other pages)
        book->SetSelection(ribbonPage);
    }
    return dirty;
}

void GUICraftMainPanel::OnPreviewItemSelected(wxCommandEvent& e)
{
    e.Skip();
    wxString controlName = e.GetString();
    wxTreeItemId item = DoGetTopLevelTreeItem();
    if(item.IsOk()) {
        wxTreeItemId sel;
        DoFindName(item, controlName, sel);
        if(sel.IsOk()) {
            m_treeControls->EnsureVisible(sel);
            bManualSelection = true;
            m_treeControls->SelectItem(sel);
            bManualSelection = false;
            DoUpdatePropertiesView();
        }
    }
}

void GUICraftMainPanel::OnSelectToplevelItem(wxCommandEvent& e)
{
    wxTreeItemId item = DoGetTopLevelTreeItem();
    if(item.IsOk()) {
        bManualSelection = true;
        m_treeControls->EnsureVisible(item);
        m_treeControls->SelectItem(item);
        bManualSelection = false;
        DoUpdatePropertiesView();
    }
}

void GUICraftMainPanel::OnBookPageSelected(wxCommandEvent& e)
{
    wxTreeItemId item;
    DoFindName(DoGetTopLevelTreeItem(), e.GetString(), item);
    if(item.IsOk()) {
        GUICraftItemData* d = dynamic_cast<GUICraftItemData*>(m_treeControls->GetItemData(item));
        CHECK_POINTER(d);
        CHECK_POINTER(d->m_wxcWidget);

        NotebookPageWrapper* page = dynamic_cast<NotebookPageWrapper*>(d->m_wxcWidget);
        CHECK_POINTER(page);

        NotebookBaseWrapper* book = page->GetNotebook();
        CHECK_POINTER(book);

        book->SetSelection(page);
        bManualSelection = true;
        m_treeControls->SelectItem(item);
        bManualSelection = false;
        m_treeControls->EnsureVisible(item);
    }
}

void GUICraftMainPanel::OnRibbonPageSelected(wxCommandEvent& e)
{
    wxTreeItemId item;
    DoFindName(DoGetTopLevelTreeItem(), e.GetString(), item);
    if(item.IsOk()) {
        GUICraftItemData* d = dynamic_cast<GUICraftItemData*>(m_treeControls->GetItemData(item));
        CHECK_POINTER(d);
        CHECK_POINTER(d->m_wxcWidget);

        RibbonPageWrapper* page = dynamic_cast<RibbonPageWrapper*>(d->m_wxcWidget);
        CHECK_POINTER(page);

        RibbonBarWrapper* ribbonbar = dynamic_cast<RibbonBarWrapper*>(page->GetParent());
        CHECK_POINTER(ribbonbar);

        ribbonbar->SetSelection(page);
        bManualSelection = true;
        m_treeControls->SelectItem(item);
        bManualSelection = false;
        m_treeControls->EnsureVisible(item);
    }
}

void GUICraftMainPanel::OnCut(wxCommandEvent& e)
{
    if(!m_plugin->IsMainViewActive()) {
        e.Skip(); // This event is nothing to do with us
        return;
    }

    if(!IsTreeViewSelected()) {
        e.Skip(); // The treeview isn't selected, so don't cut its selection!
        return;
    }

    GUICraftItemData* itemData = GetSelItemData();
    CHECK_POINTER(itemData);
    CHECK_POINTER(itemData->m_wxcWidget);

    wxDELETE(m_clipboardItem);

    m_clipboardItem = itemData->m_wxcWidget;
    m_clipboardItem->SetCopyReason(wxcWidget::CR_Cut);

    wxWindowUpdateLocker locker(m_treeControls);

    // This step is needed so when we delete the GUI subtree, the
    // metadata kept on each node is not deleted
    wxTreeItemId item = m_treeControls->GetSelection();
    DoUnsetItemData(item);

    // disconnect this item from the its parent
    m_clipboardItem->RemoveFromParent();

    if(m_treeControls->ItemHasChildren(item)) {
        m_treeControls->DeleteChildren(item);
    }
    m_treeControls->Delete(item);

    NotifyPreviewChanged();

    // Without this, focus will unexpectedly appear elsewhere, e.g. on a propertygrid style. And for Cut, it still does;
    // Copy/Paste/Del work OK though :/
    m_treeControls->SetFocus();
    wxcEditManager::Get().PushState("cut");
}

void GUICraftMainPanel::OnCopy(wxCommandEvent& e)
{
    if(!m_plugin->IsMainViewActive()) {
        e.Skip(); // This event is nothing to do with us
        return;
    }

    if(!IsTreeViewSelected()) {
        e.Skip(); // The treeview isn't selected, so don't copy its selection unnecessarily
        return;
    }

    GUICraftItemData* itemData = GetSelItemData();
    CHECK_POINTER(itemData);
    CHECK_POINTER(itemData->m_wxcWidget);

    // Delete any old instance
    wxDELETE(m_clipboardItem);
    std::set<wxString> unused;
    m_clipboardItem = itemData->m_wxcWidget->Copy((DuplicatingOptions)(DO_renameNone | DO_copyEventsToo), unused);
    m_clipboardItem->SetCopyReason(wxcWidget::CR_Copy);

    m_treeControls->SetFocus(); // Without this, focus will unexpectedly appear elsewhere, e.g. on a propertygrid style
}

void GUICraftMainPanel::OnPaste(wxCommandEvent& e)
{
    if(!m_plugin->IsMainViewActive()) {
        e.Skip(); // This event is nothing to do with us
        return;
    }

    if(!IsTreeViewSelected()) {
        e.Skip(); // The treeview isn't selected, so don't make an unintented paste into it
        return;
    }

    CHECK_POINTER(m_clipboardItem);

    wxcWidget *source, *target;
    wxcWidget* destinationItem = NULL;
    wxcWidget* destTLW = NULL;
    GUICraftItemData* sel = GetSelItemData();

    if(!sel) {
        // pasting on the root item
        target = NULL;

    } else {
        CHECK_POINTER(sel->m_wxcWidget);

        destinationItem = sel->m_wxcWidget;

        // Check if we can paste it here
        if(!Allocator::Instance()->CanPaste(m_clipboardItem, destinationItem)) {
            wxMessageBox(_("Can't paste it here"));
            return;
        }
        target = destinationItem;
    }

    bool differentTLW(false);
    wxString newname, newinheritedname, newfilename;
    int options(DO_renameAllChildren);

    DuplicateTLWDlg dlg(this);
    if(m_clipboardItem->IsTopWindow()) {
        while(true) {
            if(dlg.ShowModal() != wxID_OK) {
                return; // Presumably the user meant cancel the whole process
            }
            newname = dlg.GetBaseName();
            wxTreeItemId match;
            DoFindName(m_treeControls->GetRootItem(), newname, match);
            if(match.IsOk()) {
                if(wxMessageBox(_("This name is already in use. Try again?"), _("wxCrafter"),
                                wxYES_NO | wxICON_QUESTION, this) == wxYES) {
                    continue;
                } else {
                    return;
                }
            }
            break;
        }
        newinheritedname = dlg.GetInheritedName();
        newfilename = dlg.GetFilename();
    } else if(target) {
        // try to locate an item with this name inside the destination TLW
        // If we got a match - force a rename
        destTLW = target->GetTopLevel();
        CHECK_POINTER(destTLW);
        // FindChildByName() can't tell if an idiot user is trying to paste m_foo into a different TLW that already
        // contains an m_foo.
        // So compare not only the names, but also the addresses
        const wxcWidget* sameNameChild = destTLW->FindChildByName(m_clipboardItem->GetName());
        differentTLW = (sameNameChild == NULL) || (sameNameChild != m_clipboardItem);
    }

    if(m_clipboardItem->IsTopWindow() || differentTLW) {
        if(wxcSettings::Get().HasFlag(wxcSettings::DUPLICATE_KEEPS_ALL_NAMES)) {
            options = DO_renameNone;
        } else if(wxcSettings::Get().HasFlag(wxcSettings::DUPLICATE_KEEPS_USERSET_NAMES)) {
            options = DO_renameAllChildrenExceptUsernamed;
        }
        options |= (wxcSettings::Get().HasFlag(wxcSettings::DUPLICATE_EVENTHANDLERS_TOO) ? DO_copyEventsToo : 0);
    }

    std::set<wxString> existingNames;
    if(destTLW) {
        destTLW->StoreNames(existingNames);
    }
    // Don't place the actual item, but rather a copy of it
    source = m_clipboardItem->Copy((DuplicatingOptions)options, existingNames, newname, newinheritedname, newfilename);

    DoPasteOrDuplicate(source, target, false);
}

void GUICraftMainPanel::OnDuplicate(wxCommandEvent& e)
{
    wxcWidget *source, *target;
    wxcWidget* destinationItem = NULL;
    GUICraftItemData* sel = GetSelItemData();

    if(!sel) {
        // pasting on the root item
        target = NULL;

    } else {
        CHECK_POINTER(sel->m_wxcWidget);

        destinationItem = sel->m_wxcWidget;

        // Check if we can paste it here
        if(!Allocator::Instance()->CanPaste(destinationItem, destinationItem)) {
            wxMessageBox(_("Can't paste it here"));
            return;
        }
        target = destinationItem;
    }

    wxString newname, newinheritedname, newfilename;
    int options(DO_renameAllChildren);

    // We're duplicating, not pasting, so only TLWs can possibly retain old names for child-controls
    if(destinationItem->IsTopWindow()) {

        DuplicateTLWDlg dlg(this);
        while(true) {
            if(dlg.ShowModal() != wxID_OK) {
                return; // Presumably the user meant cancel the whole process
            }
            newname = dlg.GetBaseName();
            wxTreeItemId match;
            DoFindName(m_treeControls->GetRootItem(), newname, match);
            if(match.IsOk()) {
                if(wxMessageBox(_("This name is already in use. Try again?"), _("wxCrafter"),
                                wxYES_NO | wxICON_QUESTION, this) == wxYES) {
                    continue;
                } else {
                    return;
                }
            }
            break;
        }
        newinheritedname = dlg.GetInheritedName();
        newfilename = dlg.GetFilename();

        if(wxcSettings::Get().HasFlag(wxcSettings::DUPLICATE_KEEPS_ALL_NAMES)) {
            options = DO_renameNone;
        } else if(wxcSettings::Get().HasFlag(wxcSettings::DUPLICATE_KEEPS_USERSET_NAMES)) {
            options = DO_renameAllChildrenExceptUsernamed;
        }
    }

    // There's nothing to stop us honouring the user's event-copying preference, even if it's _not_ a TLW
    options |= (wxcSettings::Get().HasFlag(wxcSettings::DUPLICATE_EVENTHANDLERS_TOO) ? DO_copyEventsToo : 0);

    std::set<wxString> unused;
    source = destinationItem->Copy((DuplicatingOptions)options, unused, newname, newinheritedname, newfilename);

    DoPasteOrDuplicate(source, target, true);
}

void GUICraftMainPanel::DoPasteOrDuplicate(wxcWidget* source, wxcWidget* target, bool duplicating)
{
    wxWindowUpdateLocker locker(m_treeControls);

    if((!target && source->IsTopWindow()) || // pasting on the root item a top level window
       (source->IsTopWindow() && target->IsTopWindow())) {
        // top level window copy/paste
        DoAppendItem(wxTreeItemId(), m_treeControls->GetRootItem(), source);

    } else {

        int insertType = Allocator::Instance()->GetInsertionType(source->GetType(), target->GetType(), true);
        if(duplicating && (insertType == Allocator::INSERT_CHILD) &&
           (wxcWidget::GetWidgetType(target->GetType()) == TYPE_SIZER) && target->IsSizerItem()) {
            // If we're duplicating a (child) sizer, don't insert the dup into the original; make it a sibling
            insertType = Allocator::INSERT_SIBLING;
        }

        if(insertType == Allocator::INSERT_SIBLING) {
            DoInsertBefore(wxTreeItemId(), m_treeControls->GetSelection(), source);

        } else if(insertType == Allocator::INSERT_CHILD) {

            // trying to paste a wxAUI manager on a control with wxAUI manager?
            if(target->IsAuiManaged() && source->GetType() == ID_WXAUIMANAGER) {
                wxMessageBox(_("Only one wxAui Manager is allowed"), "wxCrafter", wxOK | wxICON_WARNING | wxCENTER);
                wxDELETE(source);
                return;
            }

            DoAppendItem(wxTreeItemId(), m_treeControls->GetSelection(), source);

        } else if(insertType == Allocator::INSERT_MAIN_SIZER) {
            if(target->HasMainSizer()) {
                wxDELETE(source);
                wxMessageBox(_("Can't insert this item here\nThere is already a main sizer"));
                return;

            } else if(target->IsAuiManaged()) {
                wxDELETE(source);
                wxMessageBox(_("Can't insert this item here\nThis item is managed by wxAUI"));
                return;
            }
            DoAppendItem(wxTreeItemId(), m_treeControls->GetSelection(), source);
        }
    }
    NotifyPreviewChanged();

    m_treeControls->SetFocus(); // Without this, focus will unexpectedly appear elsewhere, e.g. on a propertygrid style

    wxcEditManager::Get().PushState(duplicating ? "duplication" : "paste");
}

void GUICraftMainPanel::OnBeginDrag(wxTreeEvent& event)
{
    // event.Skip();
    m_draggedItem = m_treeControls->GetSelection();
    if(m_draggedItem.IsOk()) {
        event.Allow();
    }
}

void GUICraftMainPanel::OnEndDrag(wxTreeEvent& event)
{
    wxTreeItemId itemDst = event.GetItem();
    if(!itemDst.IsOk() || !m_draggedItem.IsOk()) {
        return;
    }

    wxcWidget *source, *target;
    GUICraftItemData* guiTargetItem = dynamic_cast<GUICraftItemData*>(m_treeControls->GetItemData(itemDst));
    CHECK_POINTER(guiTargetItem);
    CHECK_POINTER(guiTargetItem->m_wxcWidget);
    target = guiTargetItem->m_wxcWidget;

    GUICraftItemData* guiSourceItem = dynamic_cast<GUICraftItemData*>(m_treeControls->GetItemData(m_draggedItem));
    CHECK_POINTER(guiSourceItem);
    CHECK_POINTER(guiSourceItem->m_wxcWidget);
    source = guiSourceItem->m_wxcWidget;

    // source and targets are the same?
    if(source == target) {
        return;
    }

    // target is child of source?
    if(target->IsDirectOrIndirectChildOf(source)) {
        return;
    }

    // Can we paste the item to the new location?
    if(!Allocator::Instance()->CanPaste(source, target)) {
        return;
    }

    // We need to check for potential name-clashes. This is most likely with Paste, but may happen with Move between
    // TLWs
    wxcWidget* destTLW = target->GetTopLevel();
    CHECK_POINTER(destTLW);
    std::set<wxString> existingNames;
    destTLW->StoreNames(existingNames);

    bool Pasting = wxGetKeyState(WXK_CONTROL);

    int options(DO_renameAllChildren);

    // FindChildByName() can't tell if an idiot user is trying to paste m_foo into a different TLW that already contains
    // an m_foo.
    // So compare not only the names, but also the addresses
    const wxcWidget* sameNameChild = destTLW->FindChildByName(source->GetName());
    bool differentTLW = (sameNameChild == NULL) || (sameNameChild != source);

    if(Pasting) {
        if(source->IsTopWindow() || differentTLW) {
            if(wxcSettings::Get().HasFlag(wxcSettings::DUPLICATE_KEEPS_ALL_NAMES)) {
                options = DO_renameNone;
            } else if(wxcSettings::Get().HasFlag(wxcSettings::DUPLICATE_KEEPS_USERSET_NAMES)) {
                options = DO_renameAllChildrenExceptUsernamed;
            }
        }
    } else {
        options = DO_renameNone;
    }

    if(Pasting) {
        options |= (wxcSettings::Get().HasFlag(wxcSettings::DUPLICATE_EVENTHANDLERS_TOO) ? DO_copyEventsToo : 0);
    } else {
        // If we're moving, always retain any eventhandlers
        options |= DO_copyEventsToo;
    }

    wxTreeItemId itemSrc;
    if(Pasting) {
        // If this is a Paste rather than a Move, we need to copy the source
        source = source->Copy((DuplicatingOptions)options, existingNames);

    } else {
        if(!source->IsTopWindow() && differentTLW) {
            // Even if it's a Move, we still may need rename to avoid a name-clash
            // So, though it's a bit clunky, Copy() the source, then steal any new name from the copy
            wxcWidget* clone = source->Copy((DuplicatingOptions)options, existingNames);
            if(source->GetName() != clone->GetName()) {
                source->SetName(clone->GetName());
            }

            wxDELETE(clone);
        }

        itemSrc = m_draggedItem;
    }

    wxWindowUpdateLocker locker(m_treeControls);

    int insertType = Allocator::Instance()->GetInsertionType(source->GetType(), target->GetType(), true);
    if(insertType == Allocator::INSERT_SIBLING) {
        // We want to insert where the marker is, so pass 'false' for insert-after
        // (That means we can't insert above the top item: there's no place for the marker to go except onto the
        // containing sizer,
        // so the item gets appended to the that instead. However the top item can always be moved down...)
        DoInsertBefore(itemSrc, itemDst, source, false);

    } else if(insertType == Allocator::INSERT_CHILD || insertType == Allocator::INSERT_MAIN_SIZER) {
        DoAppendItem(itemSrc, itemDst, source);
    }

    NotifyPreviewChanged();
    wxcEditManager::Get().PushState(Pasting ? "paste" : "move");
}

void GUICraftMainPanel::DoAppendItem(const wxTreeItemId& sourceItem, const wxTreeItemId& targetItem,
                                     wxcWidget* sourceItemData)
{
    wxcWidget *source, *target = NULL;
    GUICraftItemData* guiTargetItem = dynamic_cast<GUICraftItemData*>(m_treeControls->GetItemData(targetItem));

    if(guiTargetItem && guiTargetItem->m_wxcWidget) {
        target = guiTargetItem->m_wxcWidget;
    }

    if(sourceItem.IsOk()) {
        GUICraftItemData* guiSourceItem = dynamic_cast<GUICraftItemData*>(m_treeControls->GetItemData(sourceItem));
        CHECK_POINTER(guiSourceItem);
        CHECK_POINTER(guiSourceItem->m_wxcWidget);
        source = guiSourceItem->m_wxcWidget;
        source->RemoveFromParent();

        // Disconnect the dragged item from its parent
        DoUnsetItemData(sourceItem);

        // Delete the UI of the dragged item
        if(m_treeControls->ItemHasChildren(sourceItem)) {
            m_treeControls->DeleteChildren(sourceItem);
        }
        m_treeControls->Delete(sourceItem);

    } else {
        source = sourceItemData;
        CHECK_POINTER(source);
    }

    // Paste it as a child of the target item
    if(target) {
        // the only case where "target" is NULL is when a top level item is being appended
        // in this case it has no one to be added to as a child
        target->AddChild(source);
    }

    wxTreeItemId selection;
    DoBuildTree(selection, source, targetItem);
    if(selection.IsOk()) {
        m_treeControls->SelectItem(selection);
        m_treeControls->EnsureVisible(selection);
    }
}

void GUICraftMainPanel::DoInsertBefore(const wxTreeItemId& sourceItem, const wxTreeItemId& targetItem,
                                       wxcWidget* sourceItemData, bool insertBefore)
{
    wxcWidget *source, *target;
    GUICraftItemData* guiTargetItem = dynamic_cast<GUICraftItemData*>(m_treeControls->GetItemData(targetItem));
    CHECK_POINTER(guiTargetItem);
    CHECK_POINTER(guiTargetItem->m_wxcWidget);
    target = guiTargetItem->m_wxcWidget;

    CHECK_POINTER(target->GetParent());

    if(sourceItem.IsOk()) {
        GUICraftItemData* guiSourceItem = dynamic_cast<GUICraftItemData*>(m_treeControls->GetItemData(sourceItem));
        CHECK_POINTER(guiSourceItem);
        CHECK_POINTER(guiSourceItem->m_wxcWidget);
        source = guiSourceItem->m_wxcWidget;

        // Check if we can perform the insert action
        if(!DoCheckInsert(source, target)) {
            return;
        }

        // Disconnect the dragged item from its parent
        source->RemoveFromParent();
        DoUnsetItemData(sourceItem);

        // Delete the UI of the dragged item
        if(m_treeControls->ItemHasChildren(sourceItem)) {
            m_treeControls->DeleteChildren(sourceItem);
        }
        m_treeControls->Delete(sourceItem);

    } else {
        source = sourceItemData;
        CHECK_POINTER(source);

        // Check if we can perform the insert action
        if(!DoCheckInsert(source, target)) {
            return;
        }
    }

    // Paste it as a child of the target item
    if(insertBefore) {
        target->GetParent()->InsertBefore(source, target);

    } else {
        target->GetParent()->InsertAfter(source, target);
    }

    wxTreeItemId selection;
    DoBuildTree(selection, source, m_treeControls->GetItemParent(targetItem), targetItem, insertBefore);

    if(selection.IsOk()) {
        m_treeControls->SelectItem(selection);
        m_treeControls->EnsureVisible(selection);
    }
}

void GUICraftMainPanel::OnCancelPreview(wxCommandEvent& e)
{
    wxCommandEvent closePreviewEvt(wxEVT_CLOSE_PREVIEW);
    EventNotifier::Get()->ProcessEvent(closePreviewEvt);
}

void GUICraftMainPanel::OnCancelPreviewUI(wxUpdateUIEvent& e)
{
    e.Enable(wxcProjectMetadata::Get().IsLoaded() && m_previewAlive);
}

void GUICraftMainPanel::OnPreviewClosed(wxCommandEvent& e)
{
    e.Skip();
    m_previewAlive = false;
}

void GUICraftMainPanel::OnShowPreviewUI(wxUpdateUIEvent& e)
{
    wxcWidget* tlw = GetActiveTopLevelWin();
    e.Enable(wxcProjectMetadata::Get().IsLoaded() && !m_previewAlive && tlw && tlw->GetType() != ID_WXIMAGELIST);
}

wxcWidget* GUICraftMainPanel::GetActiveWizardPage() const
{
    wxTreeItemId topLevel = DoGetTopLevelTreeItem();
    if(topLevel.IsOk() == false) {
        return NULL;
    }

    wxTreeItemId sel = m_treeControls->GetSelection();
    if(sel.IsOk() == false) {
        return NULL;
    }

    // Check that this top level is indeed a wxWizard...
    GUICraftItemData* topLevelItemData = dynamic_cast<GUICraftItemData*>(m_treeControls->GetItemData(topLevel));
    CHECK_POINTER_RET_NULL(topLevelItemData);
    CHECK_POINTER_RET_NULL(topLevelItemData->m_wxcWidget);
    if(topLevelItemData->m_wxcWidget->GetType() != ID_WXWIZARD) {
        return NULL;
    }

    GUICraftItemData* itemData = dynamic_cast<GUICraftItemData*>(m_treeControls->GetItemData(sel));
    CHECK_POINTER_RET_NULL(itemData);
    CHECK_POINTER_RET_NULL(itemData->m_wxcWidget);

    wxcWidget* p = itemData->m_wxcWidget;
    while(p && p->GetType() != ID_WXWIZARDPAGE) {
        p = p->GetParent();
    }

    if(p && p->GetType() == ID_WXWIZARDPAGE) {
        return p;

    } else if(!p && m_treeControls->ItemHasChildren(topLevel)) {
        wxTreeItemIdValue cookie;
        GUICraftItemData* itemData = dynamic_cast<GUICraftItemData*>(
            m_treeControls->GetItemData(m_treeControls->GetFirstChild(topLevel, cookie)));
        CHECK_POINTER_RET_NULL(itemData);
        return itemData->m_wxcWidget;
    }
    return NULL;
}

void GUICraftMainPanel::OnRefreshView(wxCommandEvent& e)
{
    e.Skip();
    DoRefresh(wxEVT_UPDATE_PREVIEW);
}

void GUICraftMainPanel::OnCloseProject(wxCommandEvent& e)
{
    e.Skip();
    if(wxcEditManager::Get().IsDirty()) {
        if(::wxMessageBox(_("Current file has been modified\nClose anyway?"), "wxCrafter",
                          wxYES_NO | wxCANCEL | wxCENTER, wxCrafter::TopFrame()) != wxYES) {
            return;
        }
    }
    Clear();

    wxCommandEvent refreshView(wxEVT_UPDATE_PREVIEW);
    EventNotifier::Get()->AddPendingEvent(refreshView);
}

void GUICraftMainPanel::OnGenerateCodeUI(wxUpdateUIEvent& e) { e.Enable(wxcProjectMetadata::Get().IsLoaded()); }

void GUICraftMainPanel::OnSaveProjectUI(wxUpdateUIEvent& e)
{
    e.Enable(wxcProjectMetadata::Get().IsLoaded() && wxcEditManager::Get().IsDirty());
}

void GUICraftMainPanel::OnBarItemSelected(wxCommandEvent& e)
{
    wxTreeItemId topLevel = DoGetTopLevelTreeItem();

    wxcWidget* tlid = DoGetItemData(topLevel);
    CHECK_POINTER(tlid);

    wxString parentname = e.GetString().BeforeFirst(wxT(':'));

    const wxcWidget* parnt = NULL;
    if(parentname == "TOOL_BAR_ID") {
        // direct toolbar child of the main frame
        parnt = tlid->FindFirstDirectChildOfType(ID_WXTOOLBAR);

    } else if(parentname == "MENU_BAR_ID") {
        // direct toolbar child of the main frame
        parnt = tlid->FindFirstDirectChildOfType(ID_WXMENUBAR);

    } else {
        parnt = tlid->FindChildByName(parentname);
    }
    CHECK_POINTER(parnt);

    wxString toolname = e.GetString().AfterFirst(wxT(':'));
    const wxcWidget::List_t& tools = parnt->GetChildren();
    wxcWidget::List_t::const_iterator menuIter = tools.begin();
    for(; menuIter != tools.end(); ++menuIter) {
        if((*menuIter)->PropertyString(PROP_LABEL) == toolname) {

            // Select this item
            wxCommandEvent evt(wxEVT_PREVIEW_CTRL_SELECTED);
            evt.SetString((*menuIter)->GetName());
            EventNotifier::Get()->AddPendingEvent(evt);

            break;
        }
    }
}

wxcWidget* GUICraftMainPanel::DoGetItemData(const wxTreeItemId& item) const
{
    if(!item.IsOk()) {
        return NULL;
    }

    GUICraftItemData* id = dynamic_cast<GUICraftItemData*>(m_treeControls->GetItemData(item));
    CHECK_POINTER_RET_NULL(id);
    return id->m_wxcWidget;
}

///////////////////////////////////////////////////////////
// Edit
///////////////////////////////////////////////////////////

void GUICraftMainPanel::OnRedo(wxCommandEvent& e)
{
    if(!m_plugin->IsTabMode() && !(m_plugin->GetMainFrame() && m_plugin->GetMainFrame()->IsShown())) {
        e.Skip();
        return; // Otherwise, in non-tabbed mode, there'll be contamination between us and clMainFrame
    }

    if(IsPropertyGridPropertySelected()) {
        e.Skip();
        return; // Otherwise Ctrl-Y in the EventsTableListView may do unwanted things to the tree
    }

    if(wxcEditManager::Get().CanRedo()) {

        State::Ptr_t state = wxcEditManager::Get().Redo();
        if(state) {
            LoadProject(wxFileName(), state->project_json, true);
            DoSelectItemByName(state->selection, state->parentTLW);
        }

    } else {
        e.Skip();
    }
}

void GUICraftMainPanel::OnRedoUI(wxUpdateUIEvent& e) { e.Enable(wxcEditManager::Get().CanRedo()); }

void GUICraftMainPanel::OnUndo(wxCommandEvent& e)
{
    if(!m_plugin->IsTabMode() && !(m_plugin->GetMainFrame() && m_plugin->GetMainFrame()->IsShown())) {
        e.Skip();
        return; // Otherwise, in non-tabbed mode, there'll be contamination between us and clMainFrame
    }

    if(IsPropertyGridPropertySelected()) {
        e.Skip();
        return; // Otherwise Ctrl-Z in the EventsTableListView may do unwanted things to the tree
    }

    if(wxcEditManager::Get().CanUndo()) {

        State::Ptr_t state = wxcEditManager::Get().Undo();
        if(state) {
            LoadProject(wxFileName(), state->project_json, true);
            DoSelectItemByName(state->selection, state->parentTLW);
        }

    } else {
        e.Skip();
    }
}

void GUICraftMainPanel::OnUndoUI(wxUpdateUIEvent& e) { e.Enable(wxcEditManager::Get().CanUndo()); }

void GUICraftMainPanel::OnLabelCurrentState(wxCommandEvent& event)
{
    if(!m_plugin->IsTabMode() && !(m_plugin->GetMainFrame() && m_plugin->GetMainFrame()->IsShown())) {
        event.Skip();
        return; // Otherwise, in non-tabbed mode, there'll be contamination between us and clMainFrame
    }

    // wxString label = wxGetTextFromUser("What would you like to call the current state?", "Label current state", "",
    // this);
    // if (!label.empty()) {
    //    wxcEditManager::Get().SetUserLabel(label);
    //}
}

void GUICraftMainPanel::OnEditMenuOpened(wxMenuEvent& event)
{
    if(m_plugin->IsTabMode() && !m_plugin->IsMainViewActive()) {
        event.Skip(); // This event is nothing to do with us
        return;
    }
    // wxcEditManager::Get().PrepareLabelledStatesMenu(event.GetMenu());
}

void GUICraftMainPanel::OnLoadCurrentState(
    wxCommandEvent& WXUNUSED(event)) // Called after a multiple unredo has changed the GetCurrentState() value
{
    State::Ptr_t state = wxcEditManager::Get().GetCurrentState();
    if(!state) {
        return;
    }
    LoadProject(wxFileName(), state->project_json, true);
    DoSelectItemByName(state->selection, state->parentTLW);
}

State::Ptr_t GUICraftMainPanel::CurrentState()
{
    JSONRoot root(cJSON_Object);

    // If we dont have bitmap function assigned to this project, create new one
    if(wxcProjectMetadata::Get().GetBitmapFunction().IsEmpty()) {
        wxcProjectMetadata::Get().GenerateBitmapFunctionName();
    }

    // Update the counter info; this will otherwise be wrong after e.g. a duplication
    wxcProjectMetadata::Get().SetObjCounter(wxcWidget::GetObjCounter());

    JSONElement metadata = wxcProjectMetadata::Get().ToJSON();
    wxcProjectMetadata::Get().AppendCustomControlsJSON(GetCustomControlsUsed(), metadata);
    root.toElement().append(metadata);
    root.toElement().append(ToJSON(wxTreeItemId()));

    State::Ptr_t state(new State);
    state->project_json = root.toElement().format();

    wxTreeItemId item = m_treeControls->GetSelection();
    if(item.IsOk()) {
        state->selection = m_treeControls->GetItemText(item);
        GUICraftItemData* control = dynamic_cast<GUICraftItemData*>(m_treeControls->GetItemData(item));
        if(control && control->m_wxcWidget) {
            state->parentTLW = control->m_wxcWidget->GetTopLevel()->GetName();
        }
    }
    return state;
}

void GUICraftMainPanel::DoSelectItemByName(const wxString& name, const wxString& parentTLW)
{
    if(name.IsEmpty()) {
        return;
    }

    wxTreeItemId startpoint = m_treeControls->GetRootItem();
    if(!parentTLW.IsEmpty()) {
        wxTreeItemId sp;
        DoFindName(startpoint, parentTLW, sp);
        if(sp.IsOk()) {
            startpoint = sp;
        }
    }

    wxTreeItemId where;
    DoFindName(startpoint, name, where);
    if(where.IsOk()) {
        m_treeControls->SelectItem(where);
        m_treeControls->EnsureVisible(where);

        wxCommandEvent eventTreeItemSeleted(wxEVT_TREE_ITEM_SELECTED);
        eventTreeItemSeleted.SetString(name);
        EventNotifier::Get()->AddPendingEvent(eventTreeItemSeleted);
    }
}

bool GUICraftMainPanel::IsTreeViewSelected() const { return wxWindow::FindFocus() == m_treeControls; }

bool GUICraftMainPanel::IsPropertyGridPropertySelected() const
{
    // This function tests if the parent of an item is a propertygrid. The idea is to avoid
    // e.g. Ctrl-Z affecting the tree when the user was actually trying to edit a property
    wxWindow* win = wxWindow::FindFocus();
    if(!win) {
        return false;
    }

    return dynamic_cast<wxPropertyGrid*>(win->GetParent()) != NULL;
}

wxcWidget* GUICraftMainPanel::GetActiveTopLevelWin() const
{
    wxTreeItemId topLevel = DoGetTopLevelTreeItem();
    if(topLevel.IsOk() == false) {
        return NULL;
    }

    GUICraftItemData* topLevelItemData = dynamic_cast<GUICraftItemData*>(m_treeControls->GetItemData(topLevel));
    CHECK_POINTER_RET_NULL(topLevelItemData);
    return topLevelItemData->m_wxcWidget;
}

void GUICraftMainPanel::OnGenerateCodeMenu(wxAuiToolBarEvent& e)
{
    if(e.IsDropDownClicked()) {
        wxMenu menu(_("Code Generation"));
        wxMenuItem* item = menu.AppendCheckItem(XRCID("GenerateCPP"), _("Generate C++ code"));
        item->Check(wxcProjectMetadata::Get().GetGenerateCPPCode());
        item = menu.AppendCheckItem(XRCID("GenerateXRC"), _("Generate XRC"));
        item->Check(wxcProjectMetadata::Get().GetGenerateXRC());

        wxPoint pt = e.GetItemRect().GetBottomLeft();
        pt.y++;
        m_toolbar->PopupMenu(&menu, pt);

    } else {
        wxCommandEvent dummy;
        OnGenerateCode(dummy);
    }
}

void GUICraftMainPanel::OnCodeGenerationTypeChanged(wxCommandEvent& event)
{
    if(event.GetId() == XRCID("GenerateCPP")) {
        wxcProjectMetadata::Get().SetGenerateCPPCode(event.IsChecked());
    }
    if(event.GetId() == XRCID("GenerateXRC")) {
        wxcProjectMetadata::Get().SetGenerateXRC(event.IsChecked());
    }
}

void GUICraftMainPanel::OnNewCustomControlMenu(wxCommandEvent& e)
{
    wxMenu menu(_("Choose a control"));
    const CustomControlTemplateMap_t& controls = wxcSettings::Get().GetTemplateClasses();
    CustomControlTemplateMap_t::const_iterator iter = controls.begin();

    if(controls.empty() == false) {
        for(; iter != controls.end(); ++iter) {
            menu.Append(iter->second.GetControlId(), iter->first);
            menu.Connect(iter->second.GetControlId(), wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(GUICraftMainPanel::OnNewCustomControl), NULL, this);
        }
        menu.AppendSeparator();
    }
    menu.Append(XRCID("define_custom_controls"), _("Define custom control..."));

    wxButton* button = dynamic_cast<wxButton*>(e.GetEventObject());
    if(button) {
        wxPoint pt = button->GetScreenPosition();
        pt.y += button->GetSize().GetHeight();
        pt = ScreenToClient(pt);
        PopupMenu(&menu, pt);
    }
}

void GUICraftMainPanel::OnNewCustomControl(wxCommandEvent& e)
{
    int ctrlId = e.GetId();
    AddCustomControl(ctrlId);
}

void GUICraftMainPanel::DoInsertControl(wxcWidget* control, wxcWidget* parent, int insertType, int imgId)
{
    if(control->GetType() == ID_WXAUIMANAGER && parent->IsAuiManaged()) {
        wxDELETE(control);
        wxMessageBox(_("Only one wxAui Manager is allowed"), "wxCrafter", wxOK | wxICON_WARNING | wxCENTER);
        return;
    }

    if(control->GetType() == ID_WXAUIMANAGER && parent->HasMainSizer()) {
        wxDELETE(control);
        wxMessageBox(_("wxAui Manager can not be placed onto a control with a main sizer"), "wxCrafter",
                     wxOK | wxICON_WARNING | wxCENTER);
        return;
    }

    if(parent->GetType() == ID_WXRIBBONPANEL && !parent->GetChildren().empty() &&
       insertType == Allocator::INSERT_CHILD) {
        wxDELETE(control);
        wxMessageBox(_("wxRibbonPanel can only have one direct child"), "wxCrafter", wxOK | wxICON_WARNING | wxCENTER);
        return;
    }

    if(insertType == Allocator::INSERT_SIBLING) {
        DoInsertBefore(wxTreeItemId(), m_treeControls->GetSelection(), control, false);

    } else if(insertType == Allocator::INSERT_CHILD || insertType == Allocator::INSERT_MAIN_SIZER) {

        // We need to place a main sizer, check that we dont have one already...
        if(parent->HasMainSizer() && insertType == Allocator::INSERT_MAIN_SIZER) {
            wxDELETE(control);
            wxMessageBox(_("Can't insert this item here\nThere is already a main sizer"));
            return;

        } else if(parent->IsAuiManaged() && insertType == Allocator::INSERT_MAIN_SIZER) {
            wxDELETE(control);
            wxMessageBox(_("A Main Sizer can not be placed into a wxAui managed window"), "wxCrafter",
                         wxOK | wxICON_WARNING | wxCENTER);
            return;
        }

        parent->AddChild(control);
        wxTreeItemId item = m_treeControls->AppendItem(m_treeControls->GetSelection(), control->GetName(), imgId, imgId,
                                                       new GUICraftItemData(control));
        m_treeControls->SelectItem(item);
    } else {
        wxDELETE(control);
        wxMessageBox(_("Can't insert this item here"));
        return;
    }

    DoRefresh(wxEVT_UPDATE_PREVIEW);
    wxcEditManager::Get().PushState("insertion");
}

size_t GUICraftMainPanel::DoFindPropertiesPage(wxWindow* win)
{
    // locate the page index
    size_t pageIdx = wxString::npos;
    for(size_t i = 0; i < m_notebook2->GetPageCount(); ++i) {
        if(win == m_notebook2->GetPage(i)) {
            pageIdx = i;
            break;
        }
    }
    return pageIdx;
}

void GUICraftMainPanel::DoShowPropertiesPage(wxWindow* win, const wxString& text, bool show)
{
    size_t where = DoFindPropertiesPage(win);
    if(show && where == wxString::npos) {
        m_notebook2->AddPage(win, text);

    } else if(!show && where != wxString::npos) {
        m_notebook2->RemovePage(where);
    }
}

bool GUICraftMainPanel::DoCheckInsert(wxcWidget* source, wxcWidget* target) const
{
    return Allocator::Instance()->CanPaste(source, target);
}

wxArrayString GUICraftMainPanel::GetCustomControlsUsed() const
{
    wxArrayString customControls;
    wxTreeItemIdValue cookie;
    wxTreeItemId rootItem = m_treeControls->GetRootItem();
    wxTreeItemId child = m_treeControls->GetFirstChild(rootItem, cookie);
    while(child.IsOk()) {

        GUICraftItemData* itemData = dynamic_cast<GUICraftItemData*>(m_treeControls->GetItemData(child));
        if(itemData && itemData->m_wxcWidget) {
            itemData->m_wxcWidget->GetCustomControlsName(customControls);
        }
        child = m_treeControls->GetNextChild(rootItem, cookie);
    }
    return customControls;
}

void GUICraftMainPanel::OnPageChanged(wxBookCtrlEvent& event)
{
    event.Skip();
    DoUpdateCppPreview();
    DoDismissFindBar();
}

void GUICraftMainPanel::OnUpdatePreview(wxCommandEvent& e)
{
    e.Skip();
    DoUpdateCppPreview();
}

void GUICraftMainPanel::DoUpdateCppPreview()
{
    // Notify about code editor {de}selected
    wxCommandEvent event(wxEVT_WXC_CODE_PREVIEW_PAGE_CHANGED);
    int sel = m_mainBook->GetSelection();
    if(sel == 1) { // C++
        event.SetClientData(m_notebookCpp->GetSelection() == 0 ? m_textCtrlCppSource : m_textCtrlHeaderSource);
        m_mainBook->CppPageSelected(m_textCtrlCppSource, m_textCtrlHeaderSource);

    } else if(sel == 2) { // XRC
        m_mainBook->XrcPageSelected(m_textCtrlXrc);
        event.SetClientData(m_textCtrlXrc);

    } else {
        event.SetClientData(NULL);
    }
    EventNotifier::Get()->AddPendingEvent(event);
}

void GUICraftMainPanel::OnFindBar(clFindEvent& e)
{
    e.Skip();
    if(IsShown()) {
        int sel = m_mainBook->GetSelection();
        if(sel == 1) { // C++
            e.Skip(false);
            sel = m_notebookCpp->GetSelection();
            e.SetCtrl(sel == 0 ? m_textCtrlCppSource : m_textCtrlHeaderSource);

        } else if(sel == 2) { // XRC
            e.Skip(false);
            e.SetCtrl(m_textCtrlXrc);
        }
    }
}

void GUICraftMainPanel::OnCppBookPageChanged(wxBookCtrlEvent& event)
{
    event.Skip();
    // Force the user to re-open the 'find-bar'
    DoDismissFindBar();
}

void GUICraftMainPanel::DoDismissFindBar()
{
    // Notify the find-bar to clear any pointer referenced by it
    // that is belonged to this view
    clFindEvent evtFindBar(wxEVT_FINDBAR_RELEASE_EDITOR);
    evtFindBar.SetCtrl(m_textCtrlCppSource);
    EventNotifier::Get()->ProcessEvent(evtFindBar);

    evtFindBar.SetCtrl(m_textCtrlHeaderSource);
    EventNotifier::Get()->ProcessEvent(evtFindBar);

    evtFindBar.SetCtrl(m_textCtrlXrc);
    EventNotifier::Get()->ProcessEvent(evtFindBar);
}

void GUICraftMainPanel::OnRename(wxCommandEvent& event)
{
    if(m_treeControls->GetSelection().IsOk()) {
        GUICraftItemData* itemData =
            dynamic_cast<GUICraftItemData*>(m_treeControls->GetItemData(m_treeControls->GetSelection()));
        CHECK_POINTER(itemData);
        CHECK_POINTER(itemData->m_wxcWidget);

        wxString msg;
        msg << _("Rename '") << itemData->m_wxcWidget->GetName() << _("'");
        wxString new_name = ::wxGetTextFromUser(_("Enter the new name:"), msg, itemData->m_wxcWidget->GetName());
        if(new_name.IsEmpty()) {
            return;
        }

        itemData->m_wxcWidget->SetName(new_name);
        m_treeControls->SetItemText(m_treeControls->GetSelection(), new_name);
        DoUpdatePropertiesView();
        wxcEditManager::Get().PushState("rename");
    }
}

void GUICraftMainPanel::OnRefreshPropertiesView(wxCommandEvent& e)
{
    e.Skip();
    DoUpdatePropertiesView();
}

wxTreeItemId GUICraftMainPanel::DoFindItemByWxcWidget(wxcWidget* widget, const wxTreeItemId& item) const
{
    GUICraftItemData* itemData = dynamic_cast<GUICraftItemData*>(m_treeControls->GetItemData(item));
    if(itemData && itemData->m_wxcWidget == widget) {
        return item;
    }

    if(!m_treeControls->HasChildren(item)) {
        return wxTreeItemId();
    }

    wxTreeItemIdValue cookie;
    wxTreeItemId child = m_treeControls->GetFirstChild(item, cookie);
    while(child.IsOk()) {

        GUICraftItemData* itemData = dynamic_cast<GUICraftItemData*>(m_treeControls->GetItemData(child));
        if(itemData && itemData->m_wxcWidget) {
            wxTreeItemId matchedItem = DoFindItemByWxcWidget(widget, child);
            if(matchedItem.IsOk()) {
                return matchedItem;
            }
        }
        child = m_treeControls->GetNextChild(item, cookie);
    }
    return wxTreeItemId();
}

void GUICraftMainPanel::AddCustomControl(int controlId)
{
    CustomControlTemplate templateInfo = wxcSettings::Get().FindByControlId(controlId);
    if(templateInfo.IsValid() == false) {
        return;
    }

    GUICraftItemData* data = GetSelItemData();
    CHECK_POINTER(data);
    CHECK_POINTER(data->m_wxcWidget);

    // Allocate the new control
    CustomControlWrapper* control =
        dynamic_cast<CustomControlWrapper*>(Allocator::Instance()->Create(ID_WXCUSTOMCONTROL));
    CHECK_POINTER(control);
    control->SetTemplInfoName(templateInfo.GetClassName());

    int insertType = Allocator::Instance()->GetInsertionType(control->GetType(), data->m_wxcWidget->GetType(), false);
    int imgId = Allocator::Instance()->GetImageId(ID_WXCUSTOMCONTROL);
    DoInsertControl(control, data->m_wxcWidget, insertType, imgId);
}

wxWindow* GUICraftMainPanel::GetTopFrame() const
{
    wxWindow* top = wxCrafter::TopFrame();
    if(!m_plugin->IsTabMode() && m_plugin->GetMainFrame() && m_plugin->GetMainFrame()->IsShown()) {
        top = m_plugin->GetMainFrame();
    } else if(m_plugin->IsTabMode()) {
        top = wxDynamicCast(top, wxFrame);
    }

    return top;
}

void GUICraftMainPanel::OnChangeSizerType(wxCommandEvent& e) { DoChangeOrInsertIntoSizer(e.GetId()); }

void GUICraftMainPanel::OnInsertIntoSizer(wxCommandEvent& e) { DoChangeOrInsertIntoSizer(e.GetId()); }

void GUICraftMainPanel::DoChangeOrInsertIntoSizer(int id)
{
    GUICraftItemData* data = GetSelItemData();
    wxTreeItemId itemId = m_treeControls->GetSelection();
    CHECK_POINTER(data);
    CHECK_POINTER(data->m_wxcWidget);
    CHECK_TREEITEM(itemId);

    int widgetType = wxNOT_FOUND;
    switch(id) {
    case ID_CHANGE_SIZER_BOXSIZER:
    case ID_INSERT_INTO_SIZER_BOXSIZER:
        widgetType = ID_WXBOXSIZER;
        break;
    case ID_CHANGE_SIZER_FLEXGRIDSIZER:
    case ID_INSERT_INTO_SIZER_FLEXGRIDSIZER:
        widgetType = ID_WXFLEXGRIDSIZER;
        break;
    case ID_CHANGE_SIZER_GRIDSIZER:
    case ID_INSERT_INTO_SIZER_GRIDSIZER:
        widgetType = ID_WXGRIDSIZER;
        break;
    case ID_CHANGE_SIZER_GRIDBAGSIZER:
    case ID_INSERT_INTO_SIZER_GRIDBAGSIZER:
        widgetType = ID_WXGRIDBAGSIZER;
        break;
    case ID_CHANGE_SIZER_STATICBOXSIZER:
    case ID_INSERT_INTO_SIZER_STATICBOXSIZER:
        widgetType = ID_WXSTATICBOXSIZER;
        break;
    }

    if(widgetType == wxNOT_FOUND) {
        return;
    }

    wxString label;
    wxcWidget* newWidget = Allocator::Instance()->Create(widgetType);

    if(id < ID_CHANGE_SIZER_LAST) { // Changing the sizer type
        wxcWidget::ReplaceWidget(data->m_wxcWidget, newWidget);
        // delete the old widget
        wxDELETE(data->m_wxcWidget);

        // replace the pointers
        data->m_wxcWidget = newWidget;

        // Visual fixes:
        // - Update the name on the tree view
        // - Update the icon
        m_treeControls->SetItemText(itemId, data->m_wxcWidget->GetName());
        int imgId = Allocator::Instance()->GetImageId(data->m_wxcWidget->GetType());
        m_treeControls->SetItemImage(itemId, imgId, wxTreeItemIcon_Expanded);
        m_treeControls->SetItemImage(itemId, imgId, wxTreeItemIcon_Selected);
        m_treeControls->SetItemImage(itemId, imgId, wxTreeItemIcon_Normal);

        label = "sizer type change";

    } else if(id < ID_INSERT_INTO_SIZER_LAST) { // Inserting into a new sizer
        wxString widgetName = m_treeControls->GetItemText(itemId);
        wxcWidget* parentWidget = data->m_wxcWidget->GetParent();
        CHECK_POINTER(parentWidget);

        wxcWidget::InsertWidgetInto(data->m_wxcWidget, newWidget);

#ifdef __WXMSW__
        wxWindowUpdateLocker locker(m_treeControls);
#endif
        wxTreeItemId treeItemParent = m_treeControls->GetItemParent(itemId);
        CHECK_TREEITEM(itemId);
        DoUnsetItemData(treeItemParent);
        m_treeControls->DeleteChildren(treeItemParent);

        wxTreeItemId dummy;
        m_treeControls->SetItemData(treeItemParent, new GUICraftItemData(parentWidget));
        const wxcWidget::List_t& list = parentWidget->GetChildren();
        wxcWidget::List_t::const_iterator iter = list.begin();
        for(; iter != list.end(); ++iter) {
            DoBuildTree(dummy, *iter, treeItemParent);
        }

        ExpandToItem(widgetName, treeItemParent);

        label = "insertion into sizer";
    }

    m_treeControls->Refresh();

    DoRefresh(wxEVT_UPDATE_PREVIEW);
    wxcEditManager::Get().PushState(label);
}

wxStyledTextCtrl* GUICraftMainPanel::GetPreviewEditor() const
{
    if(m_mainBook->GetSelection() == 1) {
        // C++ tab is selected
        wxStyledTextCtrl* stc = m_notebookCpp->GetSelection() == 0 ? m_textCtrlCppSource : m_textCtrlHeaderSource;
        return stc;
    }
    return NULL;
}

void GUICraftMainPanel::OnEventEditorUI(wxUpdateUIEvent& e) { e.Enable(true); }

void GUICraftMainPanel::OnStylesChanged(wxPropertyGridEvent& event)
{
    m_styles.Changed(m_pgMgrStyles->GetGrid(), event);
}
void GUICraftMainPanel::OnSizerFlagsChanged(wxPropertyGridEvent& event)
{
    m_sizerFlags.Changed(m_pgMgrSizerFlags->GetGrid(), event);
}
void GUICraftMainPanel::OnSizerFlagsUpdateUI(wxUpdateUIEvent& event)
{
    m_sizerFlags.DoUpdateUI(m_pgMgrSizerFlags->GetGrid(), event);
}

void GUICraftMainPanel::OnAuiPaneInfoChanged(wxPropertyGridEvent& event)
{
    m_auiPaneInfo.Changed(m_pgMgrAuiProperties->GetGrid(), event);
}

void GUICraftMainPanel::DoGenerateCode(bool silent)
{
    // Show the nag dialog if needed
    if(!silent) {
        wxcSettings::Get().ShowNagDialogIfNeeded();
    }

    if(!wxcProjectMetadata::Get().GetGenerateCPPCode() && !wxcProjectMetadata::Get().GetGenerateXRC()) {
        if(silent) {
            return;
        }
        wxString msg;
        msg << _("You need to enable at least one of 'Generate C++ code' and 'Generate XRC'");
        ::wxMessageBox(msg, "wxCrafter", wxOK | wxCENTER | wxICON_WARNING, wxCrafter::TopFrame());
        return;
    }

    wxFileName outputDir(wxcProjectMetadata::Get().GetGeneratedFilesDir(), "");
    wxCrafter::MakeAbsToProject(outputDir);
    if(!outputDir.DirExists()) {
        if(silent) {
            return;
        }
        wxString msg;
        msg << _("Please set the base classes generated files output directory\nThis can be done by selecting the root "
                 "item of the tree and edit the properties");
        ::wxMessageBox(msg, "wxCrafter", wxOK | wxCENTER | wxICON_WARNING, wxCrafter::TopFrame());
        return;
    }

    if(wxcProjectMetadata::Get().GetProjectFile().IsEmpty()) {
        if(silent) {
            return;
        }
        wxString msg;
        msg << _("You must save the project before generating code");
        ::wxMessageBox(msg, "wxCrafter", wxOK | wxCENTER | wxICON_WARNING, wxCrafter::TopFrame());
        return;
    }

    // Always save the project when generating code
    // even if its not modified
    wxCommandEvent dummy;
    OnSaveProject(dummy);

    // Loop over the top level windows and generate their base classes
    wxArrayString headers;
    wxString baseCpp;
    wxString baseHeader;
    wxString blockGuard;

    wxString autoGenComment;
    autoGenComment << "//////////////////////////////////////////////////////////////////////"
                   << "\n";
    autoGenComment << "// This file was auto-generated by codelite's wxCrafter Plugin\n";
    autoGenComment << "// wxCrafter project file: " << wxcProjectMetadata::Get().GetProjectFileName().GetFullName()
                   << "\n";
    autoGenComment << "// Do not modify this file by hand!\n";
    autoGenComment << "//////////////////////////////////////////////////////////////////////"
                   << "\n\n";

    wxFileName projectFile(wxcProjectMetadata::Get().GetProjectFile());
    wxCrafter::MakeAbsToProject(projectFile);

    wxFileName headerFile = wxcProjectMetadata::Get().BaseHeaderFile();
    wxCrafter::MakeAbsToProject(headerFile);

    wxFileName sourceFile = wxcProjectMetadata::Get().BaseCppFile();
    wxCrafter::MakeAbsToProject(sourceFile);

    blockGuard = "_"; // Ensure that the blockguard does not start with a number
    wxArrayString dirs = projectFile.GetDirs();
    if(!dirs.IsEmpty()) {
        if(dirs.size() > 2) {
            blockGuard << dirs.Item(dirs.size() - 2) << "_";
        }
        blockGuard << dirs.Last() << "_";
    }
    blockGuard << projectFile.GetName();
    blockGuard.Replace("-", "_");
    blockGuard.Replace(".", "_");
    blockGuard.Replace("+", "_");
    blockGuard.Replace(":", "_");
    blockGuard << "_BASE_CLASSES";
    blockGuard << "_" << wxcProjectMetadata::Get().GetHeaderFileExt();
    blockGuard.MakeUpper();

    // Before we start traversing the tree, clear the bitmap code generator content
    wxcCodeGeneratorHelper::Get().Clear();
    wxcProjectMetadata::Get().ClearAggregatedData();
    wxStringMap_t additionalFiles;

    GenerateCppOutput(baseCpp, baseHeader, headers, additionalFiles);

    wxcProjectMetadata::Get().SetAdditionalFiles(additionalFiles);
    if(wxcProjectMetadata::Get().GetGenerateCPPCode() && (baseCpp.IsEmpty() == false)) {

        // Write the base cpp/header
        headers = wxCrafter::MakeUnique(headers);

        wxString prefix;
        prefix << autoGenComment;
        prefix << "#ifndef " << blockGuard << "\n";
        prefix << "#define " << blockGuard << "\n\n";
        prefix << "// clang-format off\n";
        prefix << wxCrafter::Join(headers, "\n") << "\n";

        // wxPersistence support
        prefix << wxCrafter::WX29_BLOCK_START();
        prefix << "#include <wx/persist.h>\n";
        prefix << "#include <wx/persist/toplevel.h>\n";
        prefix << "#include <wx/persist/bookctrl.h>\n";
        prefix << "#include <wx/persist/treebook.h>\n";
        prefix << "#endif\n";

        prefix << "\n";
        prefix << "#ifdef WXC_FROM_DIP\n";
        prefix << "#undef WXC_FROM_DIP\n";
        prefix << "#endif\n";
        prefix << wxCrafter::WX31_BLOCK_START();
        prefix << "#define WXC_FROM_DIP(x) wxWindow::FromDIP(x, NULL)\n";
        prefix << "#else\n";
        prefix << "#define WXC_FROM_DIP(x) x\n";
        prefix << "#endif\n\n";

        // Prepare the project additional include files
        wxString projectIncludes;
        const wxArrayString& includes = wxcProjectMetadata::Get().GetIncludeFiles();
        for(size_t i = 0; i < includes.GetCount(); i++) {
            projectIncludes << "#include " << wxCrafter::AddQuotes(includes.Item(i)) << "\n";
        }
        prefix << projectIncludes;
        prefix << "// clang-format on\n";

        baseHeader.Prepend(prefix);
        baseHeader.Append("#endif\n");

        if(wxCrafter::IsTheSame(baseHeader, headerFile) == false) {
            wxCrafter::WriteFile(headerFile, baseHeader, true);
            wxCrafter::FormatFile(headerFile);
            wxCrafter::NotifyFileSaved(headerFile);
        }

        wxString cppPrefix;
        cppPrefix << autoGenComment;
        cppPrefix << "#include \"" << headerFile.GetFullName() << "\"\n";
        cppPrefix << projectIncludes << "\n\n";

        cppPrefix << "// Declare the bitmap loading function\n";
        cppPrefix << wxcCodeGeneratorHelper::Get().GenerateExternCode() << "\n"; // Add the bitmap extern code
        cppPrefix << "static bool bBitmapLoaded = false;\n\n";

        baseCpp.Prepend(cppPrefix);
        if(wxCrafter::IsTheSame(baseCpp, sourceFile) == false) {
            wxCrafter::WriteFile(sourceFile, baseCpp, true);
            wxCrafter::FormatFile(sourceFile);
            wxCrafter::NotifyFileSaved(sourceFile);
        }
    }

    if(wxcProjectMetadata::Get().GetGenerateCPPCode() && !additionalFiles.empty()) {
        wxStringMap_t::const_iterator itr = additionalFiles.begin();
        for(; itr != additionalFiles.end(); ++itr) {
            wxFileName additionalFile = wxcProjectMetadata::Get().BaseHeaderFile();
            additionalFile.SetFullName(itr->first);
            wxCrafter::MakeAbsToProject(additionalFile);

            if(wxCrafter::IsTheSame(itr->second, additionalFile) == false) {
                wxCrafter::WriteFile(additionalFile, itr->second, true);
                wxCrafter::FormatFile(additionalFile);
                wxCrafter::NotifyFileSaved(additionalFile);
            }
        }
    }

    // Export the XRC output if required
    if(wxcProjectMetadata::Get().GetGenerateXRC()) {
        wxString xrcFilePath = wxcProjectMetadata::Get().GetXrcFileName();
        if(!xrcFilePath.empty()) {
            wxString XrcOutput;
            GenerateXrcOutput(XrcOutput);
            if(!XrcOutput.empty()) {
                TopLevelWinWrapper::WrapXRC(XrcOutput);
                // Format the output and make it human readable
                wxStringInputStream str(XrcOutput);
                wxStringOutputStream out;
                wxXmlDocument doc(str);
                if(!doc.Save(out)) {
                    wxMessageBox(XrcOutput);
                }

                wxFileName fnXrcFilePath(xrcFilePath);
                wxCrafter::MakeAbsToProject(fnXrcFilePath);
                wxCrafter::WriteFile(fnXrcFilePath.GetFullPath(), out.GetString(), true);
                wxCrafter::NotifyFileSaved(fnXrcFilePath);
            }
        }
    }

    // And finally, generate the Bitmap resource file
    wxcCodeGeneratorHelper::Get().CreateXRC();
}

void GUICraftMainPanel::BatchGenerate(const wxArrayString& files)
{
    // SHow the nag dialog if needed
    wxcSettings::Get().ShowNagDialogIfNeeded();

    wxArrayString projectsGenerated;
    wxArrayString wxcpFiles;

#ifdef STANDALONE_BUILD
    wxUnusedVar(files);
    wxFileDialog openFileDialog(this, _("Select wxCrafter files:"), "", "", "wxCrafter Project Files (*.wxcp)|*.wxcp",
                                wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE);
    if(openFileDialog.ShowModal() != wxID_OK) {
        return;
    }
    openFileDialog.GetPaths(wxcpFiles);
#else
    wxcpFiles = files;
#endif

    if(wxcpFiles.IsEmpty()) {
        return;
    }
    if(wxcProjectMetadata::Get().IsLoaded()) {
        ::wxMessageBox(_("Please close the current wxCrafter project before batch generating code"), "wxCrafter",
                       wxOK | wxCENTER | wxICON_ERROR);
        return;
    }

    {
        wxBusyCursor bc;
        wxBusyInfo bi(_("Generating Code..."));

        // Lock the UI
        wxWindowUpdateLocker locker(EventNotifier::Get()->TopFrame());

        for(size_t i = 0; i < wxcpFiles.size(); ++i) {
            wxString fileContent;
            if(FileUtils::ReadFileContent(wxcpFiles.Item(i), fileContent)) {
                LoadProject(wxcpFiles.Item(i), fileContent);
                DoGenerateCode(true);
                projectsGenerated.Add(wxcpFiles.Item(i));
            }
        }

        // Close any project
        if(wxcProjectMetadata::Get().IsLoaded()) {
            wxCommandEvent e;
            OnCloseProject(e);
        }
    }
    if(!projectsGenerated.IsEmpty()) {
        wxString message;
        message << _("Generated Code For the following projects:\n") << wxImplode(projectsGenerated, "\n");
        ::wxMessageBox(message, "wxCrafter");
    }
}

void GUICraftMainPanel::OnBatchGenerateCode(wxCommandEvent& e)
{
    wxUnusedVar(e);
#ifdef STANDALONE_BUILD
    BatchGenerate(wxArrayString());
#endif
}
