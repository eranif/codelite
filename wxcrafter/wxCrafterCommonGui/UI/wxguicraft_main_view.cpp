#include "UI/wxguicraft_main_view.h"

#include "AuiToolBarTopLevel.h"
#include "JSON.h"
#include "Preview/menu_bar.h"
#include "Preview/popup_window_preview.h"
#include "Preview/preview_dialog.h"
#include "Preview/preview_frame.h"
#include "Preview/preview_panel.h"
#include "Preview/preview_wizard.h"
#include "StringUtils.h"
#include "UI/PropertiesView/properties_sheet.h"
#include "UI/ToolBoxPanel.h"
#include "allocator_mgr.h"
#include "cl_command_event.h"
#include "controls/Containers/notebook_base_wrapper.h"
#include "controls/Containers/notebook_page_wrapper.h"
#include "controls/RibbonBar/ribbon_bar_wrapper.h"
#include "controls/RibbonBar/ribbon_page_wrapper.h"
#include "controls/menu_toolbar/tool_bar_item_wrapper.h"
#include "custom_control_wrapper.h"
#include "dialog_wrapper.h"
#include "duplicateTLWdlg.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileutils.h"
#include "frame_wrapper.h"
#include "globals.h"
#include "panel_wrapper_top_level.h"
#include "popup_window_wrapper.h"
#include "wizard_wrapper.h"
#include "wxc_bitmap_code_generator.h"
#include "wxc_edit_manager.h"
#include "wxc_settings.h"
#include "wxcrafter_plugin.h"
#include "wxgui_bitmaploader.h"
#include "wxgui_helpers.h"

#include <optional>
#include <set>
#include <wx/app.h>
#include <wx/busyinfo.h>
#include <wx/ffile.h>
#include <wx/filedlg.h>
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

namespace
{
std::optional<wxString> FindConfigEntry(wxFileName projectFile)
{
    wxFileName keyfn{projectFile};
    auto key = keyfn.GetFullPath().ToStdString(wxConvUTF8);
    using json = nlohmann::ordered_json;
    projectFile.SetFullName(".wxcgen.json");
    wxString content;
    while (projectFile.GetDirCount()) {
        if (projectFile.FileExists()) {
            if (!FileUtils::ReadFileContent(projectFile, content, wxConvUTF8)) {
                return std::nullopt;
            }
            try {
                auto j = json::parse(content.ToStdString(wxConvUTF8));
                if (j.contains("output-directory") && j["output-directory"].is_object() &&
                    j["output-directory"].contains(key) && j["output-directory"][key].is_string()) {
                    auto outputDir = wxString::FromUTF8(j["output-directory"][key].get<std::string>());
                    return outputDir;
                }
            } catch (...) {
                return std::nullopt;
            }
        }
        projectFile.RemoveLastDir();
    }
    return std::nullopt;
}

struct OutputDirectoryLocker {
    wxString m_oldPath;
    bool m_enabled{false};
    OutputDirectoryLocker(const wxString& path)
    {
        if (!path.empty()) {
            m_oldPath = wxcProjectMetadata::Get().GetGeneratedFilesDir();
            wxcProjectMetadata::Get().SetGeneratedFilesDir(path);
            m_enabled = true;
        }
    }

    ~OutputDirectoryLocker()
    {
        if (m_enabled)
            wxcProjectMetadata::Get().SetGeneratedFilesDir(m_oldPath);
    }
};

} // namespace

static bool bManualSelection = false;

enum {
    ID_CHANGE_SIZER_TYPE = 3000,
    ID_CHANGE_SIZER_FIRST,
    ID_CHANGE_SIZER_BOXSIZER = ID_CHANGE_SIZER_FIRST,
    ID_CHANGE_SIZER_STATICBOXSIZER,
    ID_CHANGE_SIZER_FLEXGRIDSIZER,
    ID_CHANGE_SIZER_GRIDSIZER,
    ID_CHANGE_SIZER_GRIDBAGSIZER,
    ID_CHANGE_SIZER_LAST
};

enum {
    ID_INSERT_INTO_SIZER_TYPE = 3100,
    ID_INSERT_INTO_SIZER_FIRST,
    ID_INSERT_INTO_SIZER_BOXSIZER = ID_INSERT_INTO_SIZER_FIRST,
    ID_INSERT_INTO_SIZER_STATICBOXSIZER,
    ID_INSERT_INTO_SIZER_FLEXGRIDSIZER,
    ID_INSERT_INTO_SIZER_GRIDSIZER,
    ID_INSERT_INTO_SIZER_GRIDBAGSIZER,
    ID_INSERT_INTO_SIZER_LAST
};

#define MENU_ENTRY(id, label, bmpname)              \
    menuItem = new wxMenuItem(&menu, id, label);    \
    menuItem->SetBitmap(bmpLoader.Bitmap(bmpname)); \
    menu.Append(menuItem);

#define MENU_SEPARATOR() menu.AppendSeparator();

#define MENU_ENTRY_PTR(id, label, bmpname)          \
    menuItem = new wxMenuItem(menu, id, label);     \
    menuItem->SetBitmap(bmpLoader.Bitmap(bmpname)); \
    menu->Append(menuItem);

#define MENU_SEPARATOR_PTR() menu->AppendSeparator();

BEGIN_EVENT_TABLE(GUICraftMainPanel, GUICraftMainPanelBase)

EVT_MENU(ID_GENERATE_CODE, GUICraftMainPanel::OnGenerateCode)
EVT_COMMAND_RANGE(ID_FIRST_CONTROL, ID_LAST_CONTROL - 1, wxEVT_COMMAND_BUTTON_CLICKED, GUICraftMainPanel::OnNewControl)
EVT_COMMAND_RANGE(ID_CHANGE_SIZER_FIRST,
                  ID_CHANGE_SIZER_LAST - 1,
                  wxEVT_COMMAND_MENU_SELECTED,
                  GUICraftMainPanel::OnChangeSizerType)
EVT_COMMAND_RANGE(ID_INSERT_INTO_SIZER_FIRST,
                  ID_INSERT_INTO_SIZER_LAST - 1,
                  wxEVT_COMMAND_MENU_SELECTED,
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

namespace
{
#if !STANDALONE_BUILD
void NotifyFileSaved(const wxFileName& fn) { EventNotifier::Get()->PostFileSavedEvent(fn.GetFullPath()); }
#endif

//-------------------------------------------------------------
const FLAGS_t __ONE__ = 1;

constexpr FLAGS_t MT_TOP_LEVEL = __ONE__ << 1;               // Submenu
constexpr FLAGS_t MT_SIZERS = __ONE__ << 2;                  // Submenu
constexpr FLAGS_t MT_CONTROLS = __ONE__ << 3;                // Submenu
constexpr FLAGS_t MT_CONTAINERS = __ONE__ << 4;              // Submenu
constexpr FLAGS_t MT_NOTEBOOK_PAGES = __ONE__ << 5;          // Top
constexpr FLAGS_t MT_SPLITTERWIN_PAGES = __ONE__ << 6;       // Top
constexpr FLAGS_t MT_LIST_CTRL_COLUMNS = __ONE__ << 7;       // Top
constexpr FLAGS_t MT_PROJECT = __ONE__ << 8;                 // Top
constexpr FLAGS_t MT_COMMON_MENU = __ONE__ << 9;             // Top
constexpr FLAGS_t MT_EVENTS = __ONE__ << 10;                 // Top
constexpr FLAGS_t MT_PREVIEW_CODE = __ONE__ << 11;           // Top
constexpr FLAGS_t MT_EDIT = __ONE__ << 12;                   // Top
constexpr FLAGS_t MT_WIZARDPAGE = __ONE__ << 13;             // Top
constexpr FLAGS_t MT_MENUBAR = __ONE__ << 14;                // Top
constexpr FLAGS_t MT_MENU = __ONE__ << 15;                   // Top
constexpr FLAGS_t MT_CONTROL_EVENTS = __ONE__ << 16;         // Top
constexpr FLAGS_t MT_TOOLBAR = __ONE__ << 17;                // Top
constexpr FLAGS_t MT_DV_LIST_CTRL_COL = __ONE__ << 18;       // Top
constexpr FLAGS_t MT_COLLPANE = __ONE__ << 19;               // Top
constexpr FLAGS_t MT_INFOBAR = __ONE__ << 20;                // Top
constexpr FLAGS_t MT_IMGLIST = __ONE__ << 21;                // Top
constexpr FLAGS_t MT_TIMER = __ONE__ << 22;                  // Top
constexpr FLAGS_t MT_AUIMGR = __ONE__ << 23;                 // Top
constexpr FLAGS_t MT_PASTE = __ONE__ << 24;                  // Top
constexpr FLAGS_t MT_PG_PROPERTY = __ONE__ << 25;            // Top
constexpr FLAGS_t MT_PG_MANAGER = __ONE__ << 26;             // Top
constexpr FLAGS_t MT_RIBBON = __ONE__ << 27;                 // Top
constexpr FLAGS_t MT_RIBBON_PAGE = __ONE__ << 28;            // Top
constexpr FLAGS_t MT_RIBBON_BUTTON_BAR = __ONE__ << 29;      // Top
constexpr FLAGS_t MT_RIBBON_PANEL = __ONE__ << 30;           // Top
constexpr FLAGS_t MT_RIBBON_TOOL_BAR = __ONE__ << 31;        // Top
constexpr FLAGS_t MT_RIBBON_GALLERY = __ONE__ << 32;         // Top
constexpr FLAGS_t MT_SIZERS_TYPE = __ONE__ << 33;            // Top
constexpr FLAGS_t MT_GRID = __ONE__ << 34;                   // Top
constexpr FLAGS_t MT_TREE_LIST_CTRL_COLUMNS = __ONE__ << 35; // Top
constexpr FLAGS_t MT_TASKBARICON = __ONE__ << 36;            // Top
constexpr FLAGS_t MT_INSERT_INTO_SIZER = __ONE__ << 37;      // Top

FLAGS_t DoGetValidMenus(const wxcWidget* item)
{
    // ADD_NEW_CONTROL
    if (!item) {
        return MT_PROJECT | MT_TOP_LEVEL | MT_PASTE;
    }
    FLAGS_t menuflags = 0;
    switch (item->GetType()) {
    case ID_WXRIBBONGALLERYITME:
        return MT_COMMON_MENU | MT_EDIT;
    case ID_WXRIBBONGALLERY:
        return MT_RIBBON_GALLERY | MT_EVENTS | MT_COMMON_MENU | MT_EDIT;
    case ID_WXRIBBONBUTTONBAR:
        return MT_RIBBON_BUTTON_BAR | MT_EVENTS | MT_COMMON_MENU | MT_EDIT;
    case ID_WXRIBBONTOOLBAR:
        return MT_RIBBON_TOOL_BAR | MT_EVENTS | MT_COMMON_MENU | MT_EDIT;
    case ID_WXRIBBONPANEL:
        return MT_SIZERS | MT_RIBBON_PANEL | MT_EVENTS | MT_COMMON_MENU | MT_EDIT;
    case ID_WXRIBBONPAGE:
        return MT_RIBBON_PAGE | MT_EVENTS | MT_COMMON_MENU | MT_EDIT;
    case ID_WXRIBBONBAR:
        return MT_RIBBON | MT_EVENTS | MT_COMMON_MENU | MT_EDIT;
    case ID_WXINFOBAR:
        return MT_INFOBAR | MT_EVENTS | MT_COMMON_MENU | MT_EDIT;
    case ID_WXIMAGELIST:
        return MT_IMGLIST | MT_COMMON_MENU | MT_EDIT;
    case ID_WXCOLLAPSIBLEPANE:
        return MT_COLLPANE | MT_EVENTS | MT_COMMON_MENU | MT_EDIT | MT_INSERT_INTO_SIZER;
    case ID_WXCOLLAPSIBLEPANE_PANE:
        return MT_SIZERS | MT_EVENTS | MT_COMMON_MENU | MT_EDIT;
    case ID_WXAUITOOLBARTOPLEVEL:
    case ID_WXAUITOOLBAR:
    case ID_WXTOOLBAR:
        return MT_EVENTS | MT_COMMON_MENU | MT_EDIT | MT_TOOLBAR;
    case ID_WXMENUITEM:
    case ID_WXTOOLBARITEM:
        return MT_CONTROL_EVENTS | MT_COMMON_MENU | MT_EDIT;
    case ID_WXTOOLBARITEM_SEPARATOR:
    case ID_WXTOOLBARITEM_STRETCHSPACE:
    case ID_WXAUITOOLBARLABEL:
    case ID_WXAUITOOLBARITEM_SPACE:
    case ID_WXAUITOOLBARITEM_STRETCHSPACE:
        return MT_COMMON_MENU | MT_EDIT;
    case ID_WXMENUBAR:
        return MT_COMMON_MENU | MT_MENUBAR | MT_EDIT;
    case ID_WXMENU:
    case ID_WXSUBMENU:
        return MT_COMMON_MENU | MT_MENU | MT_EDIT;
    case ID_WXWIZARD:
        return MT_WIZARDPAGE | MT_EVENTS | MT_COMMON_MENU | MT_PREVIEW_CODE | MT_EDIT;
    case ID_WXFRAME:
    case ID_WXDIALOG:
        menuflags |= MT_TASKBARICON;
        [[fallthrough]];
    case ID_WXPANEL_TOPLEVEL:
    case ID_WXPOPUPWINDOW:
        if (!item->HasMainSizer() && !item->IsAuiManaged()) {
            menuflags |= MT_SIZERS | MT_AUIMGR;
        }
        menuflags |= MT_TIMER | MT_EVENTS | MT_COMMON_MENU | MT_EDIT | MT_PREVIEW_CODE;
        return menuflags;
    case ID_WXSPACER:
        return MT_COMMON_MENU | MT_EDIT | MT_INSERT_INTO_SIZER;
    case ID_WXBOXSIZER:
    case ID_WXFLEXGRIDSIZER:
    case ID_WXSTATICBOXSIZER:
    case ID_WXGRIDSIZER:
    case ID_WXGRIDBAGSIZER:
        return MT_SIZERS_TYPE | MT_SIZERS | MT_CONTROLS | MT_CONTAINERS | MT_COMMON_MENU | MT_EDIT |
               MT_INSERT_INTO_SIZER;
    case ID_WXPROPERTYGRIDMANAGER:
        return MT_EVENTS | MT_PG_MANAGER | MT_COMMON_MENU | MT_EDIT | MT_INSERT_INTO_SIZER;
    case ID_WXPGPROPERTY:
        return MT_PG_PROPERTY | MT_COMMON_MENU | MT_EDIT;
    case ID_WXTREELISTCTRL:
        return MT_EVENTS | MT_TREE_LIST_CTRL_COLUMNS | MT_COMMON_MENU | MT_EDIT | MT_INSERT_INTO_SIZER;
    case ID_WXLISTCTRL:
        return MT_EVENTS | MT_LIST_CTRL_COLUMNS | MT_COMMON_MENU | MT_EDIT | MT_INSERT_INTO_SIZER;
    case ID_WXDATAVIEWLISTCTRL:
    case ID_WXDATAVIEWTREELISTCTRL:
        return MT_EVENTS | MT_DV_LIST_CTRL_COL | MT_COMMON_MENU | MT_EDIT | MT_INSERT_INTO_SIZER;
    case ID_WXNOTEBOOK:
    case ID_WXLISTBOOK:
    case ID_WXCHOICEBOOK:
    case ID_WXTOOLBOOK:
    case ID_WXTREEBOOK:
    case ID_WXAUINOTEBOOK:
    case ID_WXSIMPLEBOOK:
        return MT_NOTEBOOK_PAGES | MT_EVENTS | MT_COMMON_MENU | MT_EDIT | MT_INSERT_INTO_SIZER;
    case ID_WXSPLITTERWINDOW:
        return MT_EVENTS | MT_SPLITTERWIN_PAGES | MT_COMMON_MENU | MT_EDIT | MT_INSERT_INTO_SIZER;
    case ID_WXPANEL:
    case ID_WXSCROLLEDWIN:
        menuflags |= MT_INSERT_INTO_SIZER;
        [[fallthrough]];
    case ID_WXPANEL_NOTEBOOK_PAGE:
    case ID_WXSPLITTERWINDOW_PAGE:
    case ID_WXWIZARDPAGE:
        menuflags |= MT_SIZERS | MT_EVENTS | MT_COMMON_MENU | MT_EDIT;
        {
            // wxTreebook pages are allowed to have notebook pages as their direct children
            const NotebookPageWrapper* page = dynamic_cast<const NotebookPageWrapper*>(item);
            if (page && page->IsTreebookPage()) {
                menuflags |= MT_NOTEBOOK_PAGES;
            }
        }
        return menuflags;
    case ID_WXRIBBONTOOLSEPARATOR:
        return MT_COMMON_MENU | MT_EDIT;
    case ID_WXGRIDCOL:
    case ID_WXGRIDROW:
        return MT_COMMON_MENU | MT_EDIT;
    case ID_WXGRID:
        return MT_GRID | MT_EVENTS | MT_COMMON_MENU | MT_EDIT | MT_INSERT_INTO_SIZER;
    case ID_WXAUIMANAGER:
        return MT_EVENTS | MT_COMMON_MENU | MT_EDIT;
    default:
        return MT_EVENTS | MT_COMMON_MENU | MT_EDIT | MT_INSERT_INTO_SIZER;
    }
}

wxMenu* CreateContainersMenu()
{
    wxMenu* menu = new wxMenu;
    wxCrafter::ResourceLoader bmpLoader;
    wxMenuItem* menuItem = nullptr;

    MENU_ENTRY_PTR(ID_WXPANEL, wxT("wxPanel"), wxT("wxpanel"));
    MENU_ENTRY_PTR(ID_WXSCROLLEDWIN, wxT("wxScrolledWindow"), wxT("wxscrolledwindow"));
    MENU_ENTRY_PTR(ID_WXSPLITTERWINDOW, wxT("wxSplitterWindow"), wxT("wxsplitterwindow"));
    MENU_ENTRY_PTR(ID_WXWIZARDPAGE, wxT("wxWizardSimplePage"), wxT("wxwizardpage"));
    MENU_SEPARATOR_PTR();
    MENU_ENTRY_PTR(ID_WXGLCANVAS, wxT("wxGLCanvas"), wxT("wxglcanvas"));
    MENU_SEPARATOR_PTR();
    MENU_ENTRY_PTR(ID_WXCOLLAPSIBLEPANE, "wxCollapsiblePane", "wxcollapsiblepane");
    MENU_ENTRY_PTR(ID_WXCOLLAPSIBLEPANE_PANE, "wxCollapsiblePane Window", "wxpanel");

    MENU_SEPARATOR_PTR();
    MENU_ENTRY_PTR(ID_WXNOTEBOOK, wxT("wxNotebook"), wxT("wxnotebook"));
    MENU_ENTRY_PTR(ID_WXTOOLBOOK, wxT("wxToolbook"), wxT("wxtoolbook"));
    MENU_ENTRY_PTR(ID_WXLISTBOOK, wxT("wxListbook"), wxT("wxlistbook"));
    MENU_ENTRY_PTR(ID_WXCHOICEBOOK, wxT("wxChoicebook"), wxT("wxchoicebook"));
    MENU_ENTRY_PTR(ID_WXTREEBOOK, wxT("wxTreebook"), wxT("wxtreebook"));
    MENU_ENTRY_PTR(ID_WXAUINOTEBOOK, wxT("wxAuiNotebook"), wxT("wxauinotebook"));
    MENU_ENTRY_PTR(ID_WXSIMPLEBOOK, wxT("wxSimplebook"), wxT("wxsimplebook"));
    // ADD_NEW_CONTROL

    return menu;
}

wxMenu* CreateControlsMenu()
{
    wxCrafter::ResourceLoader bmpLoader;

    wxMenu* menu = new wxMenu;
    wxMenuItem* menuItem = nullptr;

    MENU_ENTRY_PTR(ID_WXBUTTON, wxT("wxButton"), wxT("wxbutton"));
    MENU_ENTRY_PTR(ID_WXBITMAPBUTTON, wxT("wxBitmapButton"), wxT("wxbitmapbutton"));
    MENU_ENTRY_PTR(ID_WXTOGGLEBUTTON, wxT("wxToggleButton"), wxT("wxtogglebutton"));
    MENU_ENTRY_PTR(ID_WXBITMAPTOGGLEBUTTON, wxT("wxBitmapToggleButton"), wxT("wxbitmaptogglebutton"));
    MENU_ENTRY_PTR(ID_WXSPINCTRL, wxT("wxSpinCtrl"), wxT("wxspinctrl"));
    MENU_ENTRY_PTR(ID_WXSPINBUTTON, wxT("wxSpinButton"), wxT("wxspinbutton"));
    MENU_ENTRY_PTR(ID_WXHYPERLINK, wxT("wxHyperlinkCtrl"), wxT("wxhyperlink"));
    MENU_ENTRY_PTR(ID_WXCOMMANDLINKBUTTON, wxT("wxCommandLinkButton"), wxT("wxcommandlinkbutton"));

    MENU_SEPARATOR_PTR();

    MENU_ENTRY_PTR(ID_WXCHECKBOX, wxT("wxCheckBox"), wxT("wxcheckbox"));
    MENU_ENTRY_PTR(ID_WXRADIOBOX, wxT("wxRadioBox"), wxT("wxradiobox"));
    MENU_ENTRY_PTR(ID_WXRADIOBUTTON, wxT("wxRadioButton"), wxT("wxradiobutton"));
    MENU_SEPARATOR_PTR();

    MENU_ENTRY_PTR(ID_WXSTATICTEXT, wxT("wxStaticText"), wxT("wxstatictext"));
    MENU_ENTRY_PTR(ID_WXSTATICLINE, wxT("wxStaticLine"), wxT("wxstaticline"));
    MENU_ENTRY_PTR(ID_WXTEXTCTRL, wxT("wxTextCtrl"), wxT("wxtextctrl"));
    MENU_ENTRY_PTR(ID_WXRICHTEXT, wxT("wxRichTextCtrl"), wxT("wxrichtextctrl"));
    MENU_ENTRY_PTR(ID_WXSTC, wxT("wxStyledTextCtrl"), wxT("stc"));
    MENU_SEPARATOR_PTR();

    MENU_ENTRY_PTR(ID_WXSTATICBITMAP, wxT("wxStaticBitmap"), wxT("wxbitmap"));
    MENU_SEPARATOR_PTR();

    MENU_ENTRY_PTR(ID_WXCOMBOBOX, wxT("wxComboBox"), wxT("wxcombobox"));
    MENU_ENTRY_PTR(ID_WXCHOICE, wxT("wxChoice"), wxT("wxchoice"));
    MENU_ENTRY_PTR(ID_WXBITMAPCOMBOBOX, wxT("wxBitmapComboBox"), wxT("wxbitmapcombobox"));
    MENU_SEPARATOR_PTR();

    MENU_ENTRY_PTR(ID_WXLISTBOX, wxT("wxListBox"), wxT("wxlistbox"));
    MENU_ENTRY_PTR(ID_WXSIMPLEHTMLLISTBOX, wxT("wxSimpleHtmlListBox"), wxT("wxhtmllistbox"));
    MENU_ENTRY_PTR(ID_WXCHECKLISTBOX, wxT("wxCheckListBox"), wxT("wxchecklistbox"));
    MENU_ENTRY_PTR(ID_WXREARRANGELIST, wxT("wxRearrangeList"), wxT("wxrearrangelist"));
    MENU_ENTRY_PTR(ID_WXLISTCTRL, wxT("wxListCtrl"), wxT("wxlistctrl"));
    MENU_ENTRY_PTR(ID_WXTREELISTCTRL, wxT("wxTreeListCtrl"), wxT("wxtreelistctrl"));
    MENU_ENTRY_PTR(ID_WXTREECTRL, wxT("wxTreeCtrl"), wxT("wxtreectrl"));
    MENU_ENTRY_PTR(ID_WXGRID, wxT("wxGrid"), wxT("wxgrid"));
    MENU_ENTRY_PTR(ID_WXDATAVIEWLISTCTRL, wxT("wxDataViewListCtrl"), "wxdataviewlistctrl");
    MENU_ENTRY_PTR(ID_WXDATAVIEWTREECTRL, wxT("wxDataViewTreeCtrl"), "wxdataviewtreectrl");
    MENU_ENTRY_PTR(ID_WXDATAVIEWTREELISTCTRL, wxT("wxDataViewCtrl (with tree-list custom model)"), "wxdataviewctrl");
    MENU_SEPARATOR_PTR();

    MENU_ENTRY_PTR(ID_WXSLIDER, wxT("wxSlider"), wxT("wxslider"));
    MENU_ENTRY_PTR(ID_WXSCROLLBAR, wxT("wxScrollBar"), wxT("wxscrollbar"));
    MENU_ENTRY_PTR(ID_WXGAUGE, wxT("wxGauge"), wxT("wxgauge"));
    MENU_SEPARATOR_PTR();

    MENU_ENTRY_PTR(ID_WXHTMLWIN, wxT("wxHtmlWindow"), wxT("wxhtmlwindow"));
    MENU_ENTRY_PTR(ID_WXWEBVIEW, "wxWebView", "wxwebview");
    MENU_ENTRY_PTR(ID_WXBANNERWINDOW, wxT("wxBannerWindow"), wxT("wxbannerwindow"));
    MENU_ENTRY_PTR(ID_WXINFOBAR, "wxInfoBar", "wxinfobar");
    MENU_ENTRY_PTR(ID_WXMEDIACTRL, "wxMediaCtrl", "wxmediactrl-16");
    MENU_ENTRY_PTR(ID_WXANIMATIONCTRL, "wxAnimationCtrl", "wxanimationctrl");
    MENU_ENTRY_PTR(ID_WXACTIVITYINDICATOR, "wxActivityIndicator", "wxactivityindicator");
    MENU_ENTRY_PTR(ID_WXTIMEPICKERCTRL, "wxTimePickerCtrl", "wxtimepickerctrl");
    MENU_SEPARATOR_PTR();

    MENU_ENTRY_PTR(ID_WXSEARCHCTRL, wxT("wxSearchCtrl"), wxT("wxsearchctrl"));
    MENU_ENTRY_PTR(ID_WXCOLORPICKER, wxT("wxColourPickerCtrl"), wxT("wxcolourpicker"));
    MENU_ENTRY_PTR(ID_WXFONTPICKER, wxT("wxFontPickerCtrl"), wxT("wxfontpickerctrl"));
    MENU_ENTRY_PTR(ID_WXFILEPICKER, wxT("wxFilePickerCtrl"), wxT("wxfilepickerctrl"));
    MENU_ENTRY_PTR(ID_WXDIRPICKER, wxT("wxDirPickerCtrl"), wxT("wxdirpickerctrl"));
    MENU_ENTRY_PTR(ID_WXDATEPICKER, wxT("wxDatePickerCtrl"), wxT("wxdatepickerctrl"));
    MENU_ENTRY_PTR(ID_WXCALEDARCTRL, wxT("wxCalendarCtrl"), wxT("wxcalendarctrl"));
    MENU_ENTRY_PTR(ID_WXGENERICDIRCTRL, wxT("wxGenericDirCtrl"), wxT("wxgenericdirctrl"));
    // ADD_NEW_CONTROL
    return menu;
}

wxMenu* CreateInsertIntoSizerMenu()
{
    wxMenu* menu = new wxMenu;
    wxMenuItem* menuItem = nullptr;
    wxCrafter::ResourceLoader bmpLoader;
    MENU_ENTRY_PTR(ID_INSERT_INTO_SIZER_BOXSIZER, "wxBoxSizer", "wxboxsizer_v");
    MENU_ENTRY_PTR(ID_INSERT_INTO_SIZER_STATICBOXSIZER, "wxStaticBoxSizer", "wxstaticboxsizer");
    MENU_ENTRY_PTR(ID_INSERT_INTO_SIZER_FLEXGRIDSIZER, "wxFlexGridSizer", "wxflexgridsizer");
    MENU_ENTRY_PTR(ID_INSERT_INTO_SIZER_GRIDSIZER, "wxGridSizer", "wxgridsizer");
    MENU_ENTRY_PTR(ID_INSERT_INTO_SIZER_GRIDBAGSIZER, "wxGridBagSizer", "wxgridbagsizer");
    return menu;
}

wxMenu* CreateSizerTypeMenu()
{
    wxMenu* menu = new wxMenu;
    wxMenuItem* menuItem = nullptr;
    wxCrafter::ResourceLoader bmpLoader;
    MENU_ENTRY_PTR(ID_CHANGE_SIZER_BOXSIZER, "wxBoxSizer", "wxboxsizer_v");
    MENU_ENTRY_PTR(ID_CHANGE_SIZER_STATICBOXSIZER, "wxStaticBoxSizer", "wxstaticboxsizer");
    MENU_ENTRY_PTR(ID_CHANGE_SIZER_FLEXGRIDSIZER, "wxFlexGridSizer", "wxflexgridsizer");
    MENU_ENTRY_PTR(ID_CHANGE_SIZER_GRIDSIZER, "wxGridSizer", "wxgridsizer");
    MENU_ENTRY_PTR(ID_CHANGE_SIZER_GRIDBAGSIZER, "wxGridBagSizer", "wxgridbagsizer");
    return menu;
}

wxMenu* CreateSizersMenu()
{
    wxMenu* menu = new wxMenu;
    wxMenuItem* menuItem = nullptr;
    wxCrafter::ResourceLoader bmpLoader;
    MENU_ENTRY_PTR(ID_WXBOXSIZER, wxT("wxBoxSizer"), wxT("wxboxsizer_v"));
    MENU_ENTRY_PTR(ID_WXSTATICBOXSIZER, wxT("wxStaticBoxSizer"), wxT("wxstaticboxsizer"));
    MENU_ENTRY_PTR(ID_WXFLEXGRIDSIZER, wxT("wxFlexGridSizer"), wxT("wxflexgridsizer"));
    MENU_ENTRY_PTR(ID_WXGRIDSIZER, wxT("wxGridSizer"), wxT("wxgridsizer"));
    MENU_ENTRY_PTR(ID_WXGRIDBAGSIZER, wxT("wxGridBagSizer"), wxT("wxgridbagsizer"));
    MENU_SEPARATOR_PTR();
    MENU_ENTRY_PTR(ID_WXSTDDLGBUTTONSIZER, wxT("wxStdDialogButtonSizer"), wxT("stddlgbuttonsizer"));
    MENU_ENTRY_PTR(ID_WXSTDBUTTON, _("Standard wxButton"), wxT("wxbutton"));
    MENU_SEPARATOR_PTR();
    MENU_ENTRY_PTR(ID_WXSPACER, _("Spacer"), wxT("spacer"));
    // ADD_NEW_CONTROL
    return menu;
}

wxMenu* CreateTopLevelMenu()
{
    wxMenu* menu = new wxMenu;
    wxMenuItem* menuItem = nullptr;
    wxCrafter::ResourceLoader bmpLoader;

    MENU_ENTRY_PTR(ID_WXFRAME, _("New wxFrame"), wxT("wxframe"));
    MENU_ENTRY_PTR(ID_WXDIALOG, _("New wxDialog"), wxT("wxdialog"));
    MENU_ENTRY_PTR(ID_WXWIZARD, _("New wxWizard"), wxT("wxwizard"));
    MENU_ENTRY_PTR(ID_WXPANEL_TOPLEVEL, _("New wxPanel"), wxT("wxpanel"));
    MENU_ENTRY_PTR(ID_WXAUITOOLBARTOPLEVEL, _("wxAuiToolBar"), wxT("wxauitoolbar"));
    MENU_SEPARATOR_PTR();
    MENU_ENTRY_PTR(ID_WXIMAGELIST, _("New wxImageList"), wxT("wximglist"));
    MENU_ENTRY_PTR(ID_WXPOPUPWINDOW, _("New wxPopupWindow"), wxT("wxpopupwindow"));

    // ADD_NEW_CONTROL
    return menu;
}

void PrepareMenu(wxMenu& menu, const wxcWidget* item)
{
    // ADD_NEW_CONTROL
    bool isChildOfTreeBook = false;
    if (item) {
        const wxcWidget* parent = item->GetParent();
        while (parent) {
            if (parent->GetType() == ID_WXTREEBOOK) {
                isChildOfTreeBook = true;
                break;
            }
            parent = parent->GetParent();
        }
    }

    const FLAGS_t flags = DoGetValidMenus(item);
    if (flags & MT_PG_MANAGER) {
        menu.Append(ID_WXPGPROPERTY, _("Add wxPGProperty"));
    }
    if (flags & MT_PG_PROPERTY) {
        menu.Append(ID_WXPGPROPERTY_SUB, _("Add wxPGProperty Child"));
    }
    if (flags & MT_INFOBAR) {
        menu.Append(ID_WXINFOBARBUTTON, _("Add Button"));
    }

    if (flags & MT_IMGLIST) {
        menu.Append(ID_WXBITMAP, _("Add Bitmap"));
    }

    if (flags & MT_GRID) {
        menu.Append(ID_WXGRIDCOL, _("Add Column"));
        menu.Append(ID_WXGRIDROW, _("Add Row"));
    }

    if (flags & MT_COLLPANE) {
        menu.Append(ID_WXCOLLAPSIBLEPANE_PANE, _("Add Pane"));
    }

    if (flags & MT_TOOLBAR) {
        menu.Append(ID_WXTOOLBARITEM, _("Add Tool"));
        menu.Append(wxID_ANY, _("Add Control"), CreateControlsMenu());
        if (item->GetType() == ID_WXAUITOOLBAR || item->GetType() == ID_WXAUITOOLBARTOPLEVEL) {
            menu.Append(ID_WXAUITOOLBARLABEL, _("Add Label"));
            menu.Append(ID_WXAUITOOLBARITEM_SPACE, _("Add Spacer"));
            menu.Append(ID_WXAUITOOLBARITEM_STRETCHSPACE, _("Add Stretch Spacer"));
        } else {
            menu.Append(ID_WXTOOLBARITEM_STRETCHSPACE, _("Add Stretch Spacer"));
        }

        menu.Append(ID_WXTOOLBARITEM_SEPARATOR, _("Add Separator"));
    }

    if (flags & MT_MENUBAR) {
        menu.Append(ID_WXMENU, _("Add wxMenu"));
    }

    if (flags & MT_MENU) {
        menu.Append(ID_WXMENUITEM, _("Add Menu Item"));
        menu.Append(ID_WXSUBMENU, _("Add Sub Menu"));
    }

    if (flags & MT_TOP_LEVEL) {
#if STANDALONE_BUILD
        menu.Append(wxID_NEW, _("New Project..."));
#endif
        menu.Append(ID_FORM_TYPE, _("Add Form"), CreateTopLevelMenu());
    }

    if (flags & MT_WIZARDPAGE) {
        menu.Append(ID_WXWIZARDPAGE, _("Add Page"));
    }

    if (flags & MT_SIZERS) {
        menu.Append(wxID_ANY, _("Add Sizer"), CreateSizersMenu());
    }

    if (flags & MT_INSERT_INTO_SIZER) {
        menu.Append(wxID_ANY, _("Insert into new Sizer"), CreateInsertIntoSizerMenu());
    }

    if (flags & MT_SIZERS_TYPE) {
        menu.Append(wxID_ANY, _("Change wxSizer Type"), CreateSizerTypeMenu());
        menu.AppendSeparator();
    }

    if (flags & MT_AUIMGR) {
        menu.Append(ID_WXAUIMANAGER, _("Add wxAuiManager"));
    }

    if (flags & MT_CONTAINERS) {
        menu.Append(wxID_ANY, _("Add Container"), CreateContainersMenu());
    }

    if (flags & MT_CONTROLS) {
        menu.Append(wxID_ANY, _("Add Control"), CreateControlsMenu());
    }

    if (flags & MT_RIBBON) {
        menu.Append(ID_WXRIBBONPAGE, _("Add wxRibbonPage"));
    }

    if (flags & MT_RIBBON_PAGE) {
        menu.Append(ID_WXRIBBONPANEL, _("Add wxRibbonPanel"));
    }

    if (flags & MT_RIBBON_PANEL) {
        menu.Append(ID_WXRIBBONBUTTONBAR, _("Add wxRibbonButtonBar"));
        menu.Append(ID_WXRIBBONTOOLBAR, _("Add wxRibbonToolBar"));
        menu.Append(ID_WXRIBBONGALLERY, _("Add wxRibbonGallery"));
    }

    if (flags & MT_RIBBON_BUTTON_BAR) {
        menu.Append(ID_WXRIBBONBUTTON, _("Add Button"));
        menu.Append(ID_WXRIBBONDROPDOWNBUTTON, _("Add Dropdown Button"));
        menu.Append(ID_WXRIBBONHYBRIDBUTTON, _("Add Hybrid Button"));
        menu.Append(ID_WXRIBBONTOGGLEBUTTON, _("Add Toggle Button"));
    }

    if (flags & MT_RIBBON_TOOL_BAR) {
        menu.Append(ID_WXRIBBONTOOL, _("Add Tool"));
        menu.Append(ID_WXRIBBONDROPDOWNTOOL, _("Add Dropdown Tool"));
        menu.Append(ID_WXRIBBONHYBRIDTOOL, _("Add Hybrid Tool"));
        menu.Append(ID_WXRIBBONTOGGLETOOL, _("Add Toggle Tool"));
    }
    if (flags & MT_RIBBON_GALLERY) {
        menu.Append(ID_WXRIBBONGALLERYITME, _("Add Gallery Item"));
    }

    if (flags & MT_NOTEBOOK_PAGES) {
        menu.Append(ID_WXPANEL_NOTEBOOK_PAGE, _("Add Notebook Page"));
        if (isChildOfTreeBook) {
            menu.Append(ID_WXTREEBOOK_SUB_PAGE, _("Add Sub Page"));
        }
    }

    if (flags & MT_SPLITTERWIN_PAGES) {
        menu.Append(ID_WXSPLITTERWINDOW_PAGE, _("Add Panel"), _("Add Panel"));
    }

    if (flags & MT_DV_LIST_CTRL_COL) {
        menu.Append(ID_WXDATAVIEWCOL, _("Add Column"));
    }

    if (flags & MT_LIST_CTRL_COLUMNS) {
        menu.Append(ID_WXLISTCTRL_COL, _("Add List Column"));
    }

    if (flags & MT_TREE_LIST_CTRL_COLUMNS) {
        menu.Append(ID_WXTREELISTCTRLCOL, _("Add Column"));
    }

    if (flags & MT_EVENTS) {
        // the events pane is now always visible
    }

    if (flags & MT_CONTROL_EVENTS) {
        // the events pane is now always visible
    }

    if (flags & MT_PROJECT) {
        if (menu.GetMenuItemCount() != 0) {
            menu.AppendSeparator();
        }
        menu.Append(ID_SAVE_WXGUI_PROJECT, _("Save"));
    }

    if (flags & MT_PREVIEW_CODE) {
        if (menu.GetMenuItemCount() != 0) {
            menu.AppendSeparator();
        }
        menu.Append(ID_GENERATE_CODE, _("Generate code..."));
    }

    if (flags & MT_TIMER) {
        if (menu.GetMenuItemCount() != 0) {
            menu.AppendSeparator();
        }
        menu.Append(ID_WXTIMER, _("Add wxTimer"));
    }

    if (flags & MT_TASKBARICON) {
        if (menu.GetMenuItemCount() != 0) {
            menu.AppendSeparator();
        }
        menu.Append(ID_WXTASKBARICON, _("Add wxTaskBarIcon"));
    }

    if (flags & MT_EDIT) {
        if (menu.GetMenuItemCount() != 0) {
            menu.AppendSeparator();
        }
        menu.Append(ID_RENAME, _("Rename..."));
        menu.AppendSeparator();
        menu.Append(ID_COPY, _("Copy"));
        menu.Append(ID_CUT, _("Cut"));
        menu.Append(ID_PASTE, _("Paste"));
        menu.Append(ID_DUPLICATE, _("Duplicate"));
    }

    if (flags & MT_PASTE) {
        menu.Append(ID_PASTE, _("Paste"));
    }
    // Last, add the common menu
    if (flags & MT_COMMON_MENU) {
        if (menu.GetMenuItemCount() != 0) {
            menu.AppendSeparator();
        }
        menu.Append(ID_MOVE_NODE_UP, _("Move Up"));
        menu.Append(ID_MOVE_NODE_DOWN, _("Move Down"));
        menu.Append(ID_MOVE_NODE_INTO_SIZER, _("Move Left into Higher Sizer"));
        menu.Append(ID_MOVE_NODE_INTO_SIBLING, _("Move Right into Sibling Sizer"));
        menu.AppendSeparator();
        menu.Append(ID_DELETE_NODE, _("Delete"));
    }

    if (item && item->GetType() == ID_WXSTDDLGBUTTONSIZER) {
        menu.PrependSeparator();
        menu.Prepend(ID_WXSTDBUTTON, _("Add Button"));
    }
}
} // namespace

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
    m_toolbar->AddTool(ID_TOOL_ALIGN_HCENTER,
                       _("Align Center"),
                       bmps.Bitmap("align-center-horizontal"),
                       _("Align Center Horizontally"),
                       wxITEM_CHECK);
    m_toolbar->AddTool(
        ID_TOOL_ALIGN_RIGHT, _("Align Right"), bmps.Bitmap("align-right"), _("Align Right"), wxITEM_CHECK);
    m_toolbar->AddSeparator();

    m_toolbar->AddTool(ID_TOOL_ALIGN_TOP, _("Align Top"), bmps.Bitmap("align-top"), _("Align Top"), wxITEM_CHECK);
    m_toolbar->AddTool(ID_TOOL_ALIGN_VCENTER,
                       _("Align Middle"),
                       bmps.Bitmap("align-center-vertical"),
                       _("Align Center Vertically"),
                       wxITEM_CHECK);
    m_toolbar->AddTool(
        ID_TOOL_ALIGN_BOTTOM, _("Align Bottom"), bmps.Bitmap("align-bottom"), _("Align Bottom"), wxITEM_CHECK);
    m_toolbar->AddSeparator();

    m_toolbar->AddTool(ID_TOOL_BORDER_ALL, _("All Borders"), bmps.Bitmap("wxall"), _("All Borders"), wxITEM_CHECK);
    m_toolbar->AddTool(
        ID_TOOL_BORDER_LEFT, _("Left Border"), bmps.Bitmap("border-left"), _("Left Border"), wxITEM_CHECK);
    m_toolbar->AddTool(ID_TOOL_BORDER_TOP, _("Top Border"), bmps.Bitmap("border-top"), _("Top Border"), wxITEM_CHECK);
    m_toolbar->AddTool(
        ID_TOOL_BORDER_RIGHT, _("Right Border"), bmps.Bitmap("border-right"), _("Right Border"), wxITEM_CHECK);
    m_toolbar->AddTool(
        ID_TOOL_BORDER_BOTTOM, _("Bottom Border"), bmps.Bitmap("border-bottom"), _("Bottom Border"), wxITEM_CHECK);

    m_toolbar->AddSeparator();
    m_toolbar->AddTool(ID_TOOL_PROP1,
                       _("Set Sizer Proportion to 1"),
                       bmps.Bitmap("proportion-1"),
                       _("Set Sizer Proportion to 1"),
                       wxITEM_CHECK);
    m_toolbar->AddTool(ID_TOOL_WXEXPAND, _("Expand Item"), bmps.Bitmap("expand"), _("Expand Item"), wxITEM_CHECK);
    m_toolbar->AddSeparator();

    m_toolbar->AddTool(ID_MOVE_NODE_UP, _("Move Up"), bmps.Bitmap("move-up"), _("Move Up"));
    m_toolbar->AddTool(ID_MOVE_NODE_DOWN, _("Move Down"), bmps.Bitmap("move-down"), _("Move Down"));
    m_toolbar->AddTool(
        ID_MOVE_NODE_INTO_SIZER, _("Move Left"), bmps.Bitmap("move-left"), _("Move Left into parent sizer"));
    m_toolbar->AddTool(ID_MOVE_NODE_INTO_SIBLING,
                       _("Move Right"),
                       bmps.Bitmap("move-upper-right"),
                       _("Move Right into a sibling sizer"));

    wxAuiToolBarItem* item = m_toolbar->FindTool(ID_GENERATE_CODE);
    item->SetHasDropDown(true);

    m_toolbar->Realize();

    m_propertiesPage = new PropertiesSheet(m_panelProperties);
    m_panelProperties->GetSizer()->Add(m_propertiesPage, 1, wxEXPAND);
    m_panelProperties->GetSizer()->Layout();
    EventNotifier::Get()->Bind(wxEVT_WXC_CMD_BATCH_GENERATE_CODE, &GUICraftMainPanel::OnBatchGenerateCode, this);
    EventNotifier::Get()->Connect(
        wxEVT_PROPERTIES_MODIFIED, wxCommandEventHandler(GUICraftMainPanel::OnPropertyChanged), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_PREVIEW_CTRL_SELECTED, wxCommandEventHandler(GUICraftMainPanel::OnPreviewItemSelected), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_WXC_SELECT_TREE_TLW, wxCommandEventHandler(GUICraftMainPanel::OnSelectToplevelItem), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_PREVIEW_BOOKPAGE_SELECTED, wxCommandEventHandler(GUICraftMainPanel::OnBookPageSelected), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_PREVIEW_RIBBON_PAGE_SELECTED, wxCommandEventHandler(GUICraftMainPanel::OnRibbonPageSelected), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_PREVIEW_CLOSED, wxCommandEventHandler(GUICraftMainPanel::OnPreviewClosed), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_WXC_OPEN_PROJECT, wxCommandEventHandler(GUICraftMainPanel::OnOpenProject), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_WXC_SAVE_PROJECT, wxCommandEventHandler(GUICraftMainPanel::OnSaveProject), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_WXC_CLOSE_PROJECT, wxCommandEventHandler(GUICraftMainPanel::OnCloseProject), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_WXC_CMD_GENERATE_CODE, wxCommandEventHandler(GUICraftMainPanel::OnGenerateCode), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_REFRESH_DESIGNER, wxCommandEventHandler(GUICraftMainPanel::OnRefreshView), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_PREVIEW_BAR_SELECTED, wxCommandEventHandler(GUICraftMainPanel::OnBarItemSelected), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_SHOW_CONTEXT_MENU, wxCommandEventHandler(GUICraftMainPanel::OnShowContextMenu), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_DELETE_CONTROL, wxCommandEventHandler(GUICraftMainPanel::OnDelete), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_UPDATE_PREVIEW, wxCommandEventHandler(GUICraftMainPanel::OnUpdatePreview), NULL, this);
    EventNotifier::Get()->Bind(wxEVT_FINDBAR_ABOUT_TO_SHOW, &GUICraftMainPanel::OnFindBar, this);
    EventNotifier::Get()->Connect(
        wxEVT_REFRESH_PROPERTIES_VIEW, wxCommandEventHandler(GUICraftMainPanel::OnRefreshPropertiesView), NULL, this);
    m_MainPanel = this;

    m_treeControls->Connect(
        wxEVT_COMMAND_TREE_BEGIN_DRAG, wxTreeEventHandler(GUICraftMainPanel::OnBeginDrag), NULL, this);
    m_treeControls->Connect(wxEVT_COMMAND_TREE_END_DRAG, wxTreeEventHandler(GUICraftMainPanel::OnEndDrag), NULL, this);
    m_treeControls->Connect(wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler(GUICraftMainPanel::OnMenu), NULL, this);
    m_treeControls->Connect(
        wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, wxTreeEventHandler(GUICraftMainPanel::OnItemRightClick), NULL, this);
    m_treeControls->Connect(
        wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler(GUICraftMainPanel::OnItemSelected), NULL, this);
    m_treeControls->Connect(ID_DELETE_NODE,
                            wxEVT_COMMAND_MENU_SELECTED,
                            wxCommandEventHandler(GUICraftMainPanel::OnDelete),
                            NULL,
                            this); // Translated from a DEL keypress

    wxTheApp->Connect(
        wxID_UNDO, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(GUICraftMainPanel::OnUndo), NULL, this);
    wxTheApp->Connect(
        wxID_REDO, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(GUICraftMainPanel::OnRedo), NULL, this);
    wxTheApp->Connect(wxID_UNDO, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(GUICraftMainPanel::OnUndoUI), NULL, this);
    wxTheApp->Connect(wxID_REDO, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(GUICraftMainPanel::OnRedoUI), NULL, this);
    wxTheApp->Connect(XRCID("label_current_state"),
                      wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(GUICraftMainPanel::OnLabelCurrentState),
                      NULL,
                      this);
    wxTheApp->Connect(wxEVT_MULTIPLE_UNREDO, wxCommandEventHandler(GUICraftMainPanel::OnLoadCurrentState), NULL, this);
    this->Connect(ID_FORM_TYPE, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(GUICraftMainPanel::OnNewFormUI), NULL, this);

    int sashPos = wxcSettings::Get().GetSashPosition();
    if (sashPos != wxNOT_FOUND) {
        m_mainSplitter->SetSashPosition(sashPos);
    }

    // Use CodeLite's coloring settings
    LexerConf::Ptr_t cppLexer = EditorConfigST::Get()->GetLexer("C++");
    if (cppLexer) {
        cppLexer->Apply(m_textCtrlCppSource);
        cppLexer->Apply(m_textCtrlHeaderSource);
        m_textCtrlCppSource->SetProperty("lexer.cpp.track.preprocessor", "0");
        m_textCtrlCppSource->SetProperty("lexer.cpp.update.preprocessor", "0");

        m_textCtrlHeaderSource->SetProperty("lexer.cpp.track.preprocessor", "0");
        m_textCtrlHeaderSource->SetProperty("lexer.cpp.update.preprocessor", "0");
    }

    LexerConf::Ptr_t xmlLexer = EditorConfigST::Get()->GetLexer("XML");
    if (xmlLexer) {
        xmlLexer->Apply(m_textCtrlXrc);
    }

    wxTheApp->Connect(
        XRCID("save_file"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(GUICraftMainPanel::OnSaveProjectUI), NULL, this);

    ::wxPGPropertyBooleanUseCheckbox(m_pgMgrAuiProperties->GetGrid());
    ::wxPGPropertyBooleanUseCheckbox(m_pgMgrSizerFlags->GetGrid());
    m_textCtrlCppSource->Bind(wxEVT_SET_FOCUS, [](wxFocusEvent& e) {
        e.Skip();
        clCommandEvent focus_event{wxEVT_STC_GOT_FOCUS};
        EventNotifier::Get()->AddPendingEvent(focus_event);
    });

    m_textCtrlHeaderSource->Bind(wxEVT_SET_FOCUS, [](wxFocusEvent& e) {
        e.Skip();
        clCommandEvent focus_event{wxEVT_STC_GOT_FOCUS};
        EventNotifier::Get()->AddPendingEvent(focus_event);
    });
    m_textCtrlXrc->Bind(wxEVT_SET_FOCUS, [](wxFocusEvent& e) {
        e.Skip();
        clCommandEvent focus_event{wxEVT_STC_GOT_FOCUS};
        EventNotifier::Get()->AddPendingEvent(focus_event);
    });
}

GUICraftMainPanel::~GUICraftMainPanel()
{
    // Notify the find-bar to clear any pointer referenced by it
    // that is belonged to this view
    DoDismissFindBar();

    wxDELETE(m_clipboardItem);

    wxcSettings::Get().SetSashPosition(m_mainSplitter->GetSashPosition());
    wxcSettings::Get().Save();

    EventNotifier::Get()->Disconnect(
        wxEVT_PROPERTIES_MODIFIED, wxCommandEventHandler(GUICraftMainPanel::OnPropertyChanged), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_PREVIEW_CTRL_SELECTED, wxCommandEventHandler(GUICraftMainPanel::OnPreviewItemSelected), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_PREVIEW_BOOKPAGE_SELECTED, wxCommandEventHandler(GUICraftMainPanel::OnBookPageSelected), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_PREVIEW_RIBBON_PAGE_SELECTED, wxCommandEventHandler(GUICraftMainPanel::OnRibbonPageSelected), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_PREVIEW_CLOSED, wxCommandEventHandler(GUICraftMainPanel::OnPreviewClosed), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_WXC_OPEN_PROJECT, wxCommandEventHandler(GUICraftMainPanel::OnOpenProject), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_WXC_SAVE_PROJECT, wxCommandEventHandler(GUICraftMainPanel::OnSaveProject), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_WXC_CMD_GENERATE_CODE, wxCommandEventHandler(GUICraftMainPanel::OnGenerateCode), NULL, this);
    EventNotifier::Get()->Unbind(wxEVT_WXC_CMD_BATCH_GENERATE_CODE, &GUICraftMainPanel::OnBatchGenerateCode, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_REFRESH_DESIGNER, wxCommandEventHandler(GUICraftMainPanel::OnRefreshView), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_WXC_CLOSE_PROJECT, wxCommandEventHandler(GUICraftMainPanel::OnCloseProject), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_PREVIEW_BAR_SELECTED, wxCommandEventHandler(GUICraftMainPanel::OnBarItemSelected), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_SHOW_CONTEXT_MENU, wxCommandEventHandler(GUICraftMainPanel::OnShowContextMenu), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_DELETE_CONTROL, wxCommandEventHandler(GUICraftMainPanel::OnDelete), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_UPDATE_PREVIEW, wxCommandEventHandler(GUICraftMainPanel::OnUpdatePreview), NULL, this);
    EventNotifier::Get()->Unbind(wxEVT_FINDBAR_ABOUT_TO_SHOW, &GUICraftMainPanel::OnFindBar, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_REFRESH_PROPERTIES_VIEW, wxCommandEventHandler(GUICraftMainPanel::OnRefreshPropertiesView), NULL, this);

    // Only disconnect the events in Tabbed mode,
    // in the "frame" mode, the main panel and the tree view have the same
    // parent
    if (false) {
        m_treeControls->Disconnect(
            wxEVT_COMMAND_TREE_BEGIN_DRAG, wxTreeEventHandler(GUICraftMainPanel::OnBeginDrag), NULL, this);
        m_treeControls->Disconnect(
            wxEVT_COMMAND_TREE_END_DRAG, wxTreeEventHandler(GUICraftMainPanel::OnEndDrag), NULL, this);
        m_treeControls->Disconnect(
            wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler(GUICraftMainPanel::OnMenu), NULL, this);
        m_treeControls->Disconnect(
            wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, wxTreeEventHandler(GUICraftMainPanel::OnItemRightClick), NULL, this);
        m_treeControls->Disconnect(
            wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler(GUICraftMainPanel::OnItemSelected), NULL, this);
        m_treeControls->Disconnect(ID_DELETE_NODE,
                                   wxEVT_COMMAND_MENU_SELECTED,
                                   wxCommandEventHandler(GUICraftMainPanel::OnDelete),
                                   NULL,
                                   this);
        wxTheApp->Disconnect(
            ID_COPY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(GUICraftMainPanel::OnCopy), NULL, this);
        wxTheApp->Disconnect(
            ID_CUT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(GUICraftMainPanel::OnCut), NULL, this);
        wxTheApp->Disconnect(
            ID_PASTE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(GUICraftMainPanel::OnPaste), NULL, this);
    }

    wxTheApp->Disconnect(
        wxID_UNDO, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(GUICraftMainPanel::OnUndo), NULL, this);
    wxTheApp->Disconnect(
        wxID_REDO, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(GUICraftMainPanel::OnRedo), NULL, this);
    wxTheApp->Disconnect(wxID_UNDO, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(GUICraftMainPanel::OnUndoUI), NULL, this);
    wxTheApp->Disconnect(wxID_REDO, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(GUICraftMainPanel::OnRedoUI), NULL, this);
    wxTheApp->Disconnect(XRCID("label_current_state"),
                         wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(GUICraftMainPanel::OnLabelCurrentState),
                         NULL,
                         this);
    wxTheApp->Disconnect(
        wxEVT_MULTIPLE_UNREDO, wxCommandEventHandler(GUICraftMainPanel::OnLoadCurrentState), NULL, this);
    wxTheApp->Disconnect(
        XRCID("save_file"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(GUICraftMainPanel::OnSaveProjectUI), NULL, this);

    m_treeControls = NULL;
    m_MainPanel = NULL;
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
    PrepareMenu(menu, data ? data->m_wxcWidget : nullptr);
    PopupMenu(&menu);
}

void GUICraftMainPanel::OnShowContextMenu(wxCommandEvent& e)
{
    GUICraftItemData* data = GetSelItemData();

    wxString title;
    if (data && data->m_wxcWidget) {
        title = data->m_wxcWidget->GetName();
    }
    wxMenu menu(title.IsEmpty() ? "" : title);
    PrepareMenu(menu, data ? data->m_wxcWidget : nullptr);
    PopupMenu(&menu);
}

void GUICraftMainPanel::OnItemSelected(wxTreeEvent& event)
{
    event.Skip();
    wxUnusedVar(event);
    GUICraftItemData* data = GetSelItemData();

    if (!data) {
        // Clear all properties
        DoUpdatePropertiesFlags(nullptr);
        m_propertiesPage->Construct(nullptr);
        m_auiPaneInfo.Construct(m_pgMgrAuiProperties->GetGrid(), nullptr);

        m_propertiesPage->ConstructProjectSettings();
    }

    CHECK_POINTER(data);
    CHECK_POINTER(data->m_wxcWidget);

    DoUpdateNotebookSelection(m_treeControls->GetSelection());
    if (!bManualSelection) {
        NotifyPreviewChanged();
    }
    DoUpdatePropertiesView();

    wxCommandEvent eventTreeItemSelected(wxEVT_TREE_ITEM_SELECTED);
    eventTreeItemSelected.SetString(data->m_wxcWidget->GetName());
    EventNotifier::Get()->AddPendingEvent(eventTreeItemSelected);
}

void GUICraftMainPanel::DoUpdatePropertiesView()
{
    GUICraftItemData* data = GetSelItemData();
    if (!data) {
        // Clear all properties
        DoUpdatePropertiesFlags(nullptr);
        m_propertiesPage->Construct(nullptr);
        m_auiPaneInfo.Construct(m_pgMgrAuiProperties->GetGrid(), nullptr);
        m_propertiesPage->ConstructProjectSettings();
    }

    CHECK_POINTER(data);
    CHECK_POINTER(data->m_wxcWidget);
    DoUpdatePropertiesFlags(data->m_wxcWidget);
    m_propertiesPage->Construct(data->m_wxcWidget);
}

GUICraftItemData* GUICraftMainPanel::GetSelItemData()
{
    wxTreeItemId itemId = m_treeControls->GetSelection();
    if (itemId.IsOk() == false) {
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
    if (fw) {
        PreviewFrame* frame = new PreviewFrame(parent, *fw);
        m_previewAlive = true;
        frame->Show();
    }

    PanelWrapperTopLevel* pw = dynamic_cast<PanelWrapperTopLevel*>(data->m_wxcWidget);
    if (pw) {
        PreviewPanel* frame = new PreviewPanel(parent, *pw);
        m_previewAlive = true;
        frame->Show();
    }

    AuiToolBarTopLevelWrapper* aui = dynamic_cast<AuiToolBarTopLevelWrapper*>(data->m_wxcWidget);
    if (aui) {
        PreviewPanel* frame = new PreviewPanel(parent, *aui);
        m_previewAlive = true;
        frame->Show();
    }

    DialogWrapper* dw = dynamic_cast<DialogWrapper*>(data->m_wxcWidget);
    if (dw) {
        // Show the dialog
        PreviewDialog* dlg = new PreviewDialog(parent, *dw);
        m_previewAlive = true;
        dlg->Show();
    }

    WizardWrapper* ww = dynamic_cast<WizardWrapper*>(data->m_wxcWidget);
    if (ww) {
        // Show the dialog
        PreviewWizard dlg(parent, *ww);
        m_previewAlive = true;
        dlg.Run();
    }

    PopupWindowWrapper* pop = dynamic_cast<PopupWindowWrapper*>(data->m_wxcWidget);
    if (pop) {
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

void GUICraftMainPanel::DoUpdatePropertiesFlags(wxcWidget* data)
{
    DoUpdateSizerFlags(data);
    DoUpdateStyleFlags(data);
}

void GUICraftMainPanel::DoUpdateStyleFlags(wxcWidget* data) { m_styles.Construct(m_pgMgrStyles->GetGrid(), data); }

void GUICraftMainPanel::DoUpdateSizerFlags(wxcWidget* data)
{
    m_auiPaneInfo.Construct(m_pgMgrAuiProperties->GetGrid(), data);
    m_sizerFlags.Construct(m_pgMgrSizerFlags->GetGrid(), data);

    if (!data) {
        DoShowPropertiesPage(m_panelAuiPaneInfo, "wxAuiPaneInfo", false);
        DoShowPropertiesPage(m_panelSizerFlags, _("Sizer Flags"), false);

    } else {
        int cursel = m_notebook2->GetSelection();
        if (data->IsAuiPane()) {
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
    if (bookPage) {
        NotebookBaseWrapper* book = dynamic_cast<NotebookBaseWrapper*>(bookPage->GetParent());
        if (book) {
            book->SetSelection(bookPage);
        }
    }

    wxcWidget* parent = data->m_wxcWidget;
    while (parent && !parent->IsTopWindow()) {
        parent = parent->GetParent();
    }

    CHECK_POINTER(parent);

    wxCommandEvent evt(eventType);
    TopLevelWinWrapper* topWin = dynamic_cast<TopLevelWinWrapper*>(parent);
    if (topWin) {
        evt.SetString(topWin->DesignerXRC(false));
        evt.SetInt(topWin->GetType());
        EventNotifier::Get()->AddPendingEvent(evt);
    }

    // Also, notify the designer to mark the new selection
    wxCommandEvent eventTreeItemSelected(wxEVT_TREE_ITEM_SELECTED);
    eventTreeItemSelected.SetString(data->m_wxcWidget->GetName());
    EventNotifier::Get()->AddPendingEvent(eventTreeItemSelected);
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
    if (modifiedWidget && modifiedWidget != itemData->m_wxcWidget) {
        // Locate the proper wxTreeItemId to update
        item = DoFindItemByWxcWidget(modifiedWidget, m_treeControls->GetRootItem());

    } else {
        item = m_treeControls->GetSelection();
        if (item.IsOk()) {
            GUICraftItemData* data = dynamic_cast<GUICraftItemData*>(m_treeControls->GetItemData(item));
            if (data) {
                modifiedWidget = dynamic_cast<wxcWidget*>(data->m_wxcWidget);
            }
        }
    }

    wxString newname = e.GetString();
    if (item.IsOk() && !newname.IsEmpty() && (newname != m_treeControls->GetItemText(item))) {
        m_treeControls->SetItemText(item, newname);
    }

    // Special case: An auitoolbar item changing kind
    if (modifiedWidget && modifiedWidget->IsParentAuiToolbar()) {
        ToolBarItemWrapper* auiItem = dynamic_cast<ToolBarItemWrapper*>(modifiedWidget);
        if (auiItem) {
            auiItem->UpdateRegisteredEventsIfNeeded(); // Switch the event between wxEVT_COMMAND_MENU_SELECTED and
                                                       // wxEVT_COMMAND_AUITOOLBAR_TOOL_DROPDOWN
            auiItem->OnPropertiesUpdated();
            DoUpdatePropertiesView();

            wxCrafter::TOOL_TYPE toolType = wxCrafter::GetToolType(auiItem->PropertyString(PROP_KIND));
            if (toolType == wxCrafter::TOOL_TYPE_DROPDOWN && auiItem->PropertyString(PROP_DROPDOWN_MENU) == "1") {
                // We're a dropdown item, and the user wants to construct its menu inside wxC
                if (auiItem->GetChildren().empty()) {
                    wxcWidget* menu = wxcWidget::Create(ID_WXMENU);
                    menu->SetParent(auiItem);
                    DoInsertControl(
                        menu, auiItem, Allocator::INSERT_CHILD, Allocator::Instance()->GetImageId(ID_WXMENU));
                }
            } else if (!auiItem->GetChildren().empty()) {
                // There's a redundant menu, either because user switched to DIY, or because it's no longer a dropdown
                wxTreeItemIdValue cookie;
                wxTreeItemId menuitem = m_treeControls->GetFirstChild(item, cookie);
                if (menuitem.IsOk()) {
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
    if (e.GetId() == ID_WXCUSTOMCONTROL) {
        // Custom Controls are handled differently
        e.Skip();
        return;
    }

    GUICraftItemData* data = GetSelItemData();
    int imgId = Allocator::Instance()->GetImageId(e.GetId());

    bool isTopLevel = (e.GetId() == ID_WXDIALOG || e.GetId() == ID_WXFRAME || e.GetId() == ID_WXWIZARD ||
                       e.GetId() == ID_WXPANEL_TOPLEVEL || e.GetId() == ID_WXPOPUPWINDOW ||
                       e.GetId() == ID_WXIMAGELIST || e.GetId() == ID_WXAUITOOLBARTOPLEVEL);
    if (isTopLevel) {

        // Use the Wizard instead of adding the form directly
        wxCommandEvent createFormEvent(wxEVT_COMMAND_MENU_SELECTED, XRCID("wxcp_new_form"));
        createFormEvent.SetInt(e.GetId());
        wxTheApp->AddPendingEvent(createFormEvent);

    } else if (data && data->m_wxcWidget) {
        // Non toplevel window
        wxcWidget* control = wxcWidget::Create(e.GetId());
        CHECK_POINTER(control);

        int insertType;

        // Special handling for treebook
        if (e.GetId() == ID_WXTREEBOOK_SUB_PAGE) {
            insertType = Allocator::INSERT_CHILD;

        } else if (e.GetId() == ID_WXPGPROPERTY_SUB) {
            insertType = Allocator::INSERT_CHILD;

        } else if (e.GetId() == ID_WXSPACER) {
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
    DoGenerateCode(InteractionMode::ShowDialogs, SaveMode::SaveBeforeGeneration);
}

bool GUICraftMainPanel::GenerateCppOutput(
    wxString& cpp, wxString& header, wxArrayString& headers, wxStringMap_t& additionalFiles, size_t flags) const
{
    // If onlySelection, just produce output for the currently selected tree branch. By default do everything
    wxTreeItemId start, item;
    wxTreeItemId selectedItem = DoGetTopLevelTreeItem();
    if ((flags & kGenCodeSelectionOnly) && selectedItem.IsOk()) {
        start = selectedItem;
    } else {
        start = m_treeControls->GetRootItem();
    }
    wxCHECK(start.IsOk(), "Invalid tree root");

    wxTreeItemIdValue cookie;
    if (start == m_treeControls->GetRootItem()) {
        item = m_treeControls->GetFirstChild(start, cookie);
        flags &= ~kGenCodeSelectionOnly; // in case onlySelection was asked for, but selectedItem wasn't Ok

    } else {
        item = start;
    }

    while (item.IsOk()) {
        GUICraftItemData* data = dynamic_cast<GUICraftItemData*>(m_treeControls->GetItemData(item));
        if (data) {
            wxcWidget* wb = dynamic_cast<wxcWidget*>(data->m_wxcWidget);
            if (wb) {
                TopLevelWinWrapper* tl = dynamic_cast<TopLevelWinWrapper*>(wb);
                if (tl) {
                    tl->GenerateCode(wxcProjectMetadata::Get(),
                                     !(flags & kGenCodeForPreview),
                                     (flags & kGenCodeSelectionOnly) || (item != selectedItem), // output base only
                                     cpp,
                                     header,
                                     headers,
                                     additionalFiles);
                    if (flags & kGenCodeSelectionOnly) {
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
    if ((flags & kGenCodeSelectionOnly) && sel.IsOk()) {
        start = sel;
    } else {
        start = m_treeControls->GetRootItem();
    }
    wxCHECK_RET(start.IsOk(), "Invalid tree root");

    wxTreeItemIdValue cookie;
    if (start == m_treeControls->GetRootItem()) {
        item = m_treeControls->GetFirstChild(start, cookie);
        flags &= ~kGenCodeSelectionOnly; // in case onlySelection was asked for, but sel wasn't Ok

    } else {
        item = start;
    }

    while (item.IsOk()) {
        GUICraftItemData* data = dynamic_cast<GUICraftItemData*>(m_treeControls->GetItemData(item));
        if (data) {
            wxcWidget* wb = dynamic_cast<wxcWidget*>(data->m_wxcWidget);
            if (wb) {
                TopLevelWinWrapper* tl = dynamic_cast<TopLevelWinWrapper*>(wb);
                if (tl) {
                    wxString text;
                    tl->ToXRC(text, wxcWidget::XRC_LIVE);
                    output << text;
                    if (flags & kGenCodeSelectionOnly) {
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
    if (bitname.IsEmpty()) {
        return;
    }

    itemData->m_wxcWidget->EnableSizerFlag(bitname, e.IsChecked());

    if (bitname == "wxALL") {
        itemData->m_wxcWidget->EnableSizerFlag("wxLEFT", e.IsChecked());
        itemData->m_wxcWidget->EnableSizerFlag("wxRIGHT", e.IsChecked());
        itemData->m_wxcWidget->EnableSizerFlag("wxTOP", e.IsChecked());
        itemData->m_wxcWidget->EnableSizerFlag("wxBOTTOM", e.IsChecked());
    }

    wxcEditManager::Get().PushState("sizer flags change");
    DoUpdatePropertiesFlags(itemData->m_wxcWidget);
    NotifyPreviewChanged();
}

void GUICraftMainPanel::OnSizerToolUI(wxUpdateUIEvent& e)
{
    if (!wxcProjectMetadata::Get().IsLoaded()) {
        e.Enable(false);
        return;
    }

    GUICraftItemData* itemData = GetSelItemData();
    if (!itemData || !itemData->m_wxcWidget || !itemData->m_wxcWidget->IsSizerItem()) {
        e.Enable(false);
        e.Check(false);
        return;
    }

    e.Enable(true);
    wxString bitname = GetStyleFromGuiID(e.GetId());
    if (bitname.IsEmpty()) {
        e.Enable(false);
        return;
    }

    if (bitname == "wxALL") {
        bool previous = itemData->m_wxcWidget->GetSizerFlags().Item("wxALL").is_set;
        bool current = itemData->m_wxcWidget->GetSizerFlags().Item("wxLEFT").is_set &&
                       itemData->m_wxcWidget->GetSizerFlags().Item("wxRIGHT").is_set &&
                       itemData->m_wxcWidget->GetSizerFlags().Item("wxTOP").is_set &&
                       itemData->m_wxcWidget->GetSizerFlags().Item("wxBOTTOM").is_set;

        e.Check(current);
        if (current != previous) {
            itemData->m_wxcWidget->EnableSizerFlag("wxALL", current);
            DoUpdatePropertiesFlags(itemData->m_wxcWidget);
            NotifyPreviewChanged();
        }
        return;
    }

    if (itemData->m_wxcWidget->GetSizerFlags().Contains(bitname) &&
        itemData->m_wxcWidget->GetSizerFlags().Item(bitname).is_set) {
        e.Check(true);

    } else {
        e.Check(false);
    }

    m_sizerFlags.DoUpdateUI(m_pgMgrSizerFlags->GetGrid(), e); // This will do updateui for the alignment tools
}

wxString GUICraftMainPanel::GetStyleFromGuiID(int guiId) const
{
    switch (guiId) {
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

JSONItem GUICraftMainPanel::ToJSON(const wxTreeItemId& fromItem)
{
    JSONItem json = JSONItem::createArray();
    wxTreeItemIdValue cookie;
    wxTreeItemId rootItem = fromItem.IsOk() ? fromItem : m_treeControls->GetRootItem();
    wxTreeItemId child = m_treeControls->GetFirstChild(rootItem, cookie);
    while (child.IsOk()) {

        GUICraftItemData* itemData = dynamic_cast<GUICraftItemData*>(m_treeControls->GetItemData(child));
        if (itemData && itemData->m_wxcWidget) {
            JSONItem obj = JSONItem::createObject();
            itemData->m_wxcWidget->FixPaths(
                wxcProjectMetadata::Get().GetProjectPath()); // Fix abs paths to fit the new project file
            itemData->m_wxcWidget->Serialize(obj);
            json.arrayAppend(std::move(obj));
        }

        child = m_treeControls->GetNextChild(rootItem, cookie);
    }
    DoUpdatePropertiesView();
    return json;
}

void GUICraftMainPanel::OnSaveProject(wxCommandEvent& e)
{
    wxArrayString customControls = GetCustomControlsUsed();
    if (wxcProjectMetadata::Get().GetProjectFile().IsEmpty()) {
        wxString path = wxFileSelector(_("Save as"),
                                       wxEmptyString,
                                       _("my_gui.wxcp"),
                                       "wxCrafter Project File(*.wxcp)|*.wxcp",
                                       wxFileSelectorDefaultWildcardStr,
                                       wxFD_SAVE | wxFD_CHANGE_DIR | wxFD_OVERWRITE_PROMPT);
        if (path.IsEmpty()) {
            return;
        }

        wxcProjectMetadata::Get().SetProjectFile(path);
    }
    wxcProjectMetadata::Get().SetObjCounter(wxcWidget::GetObjCounter());

    JSON root(JsonType::Object);

    // If we don't have bitmap function assigned to this project, create new one
    if (wxcProjectMetadata::Get().GetBitmapFunction().IsEmpty()) {
        wxcProjectMetadata::Get().GenerateBitmapFunctionName();
    }

    JSONItem metadata = wxcProjectMetadata::Get().ToJSON();
    wxcProjectMetadata::Get().AppendCustomControlsJSON(customControls, metadata);
    root.toElement().addProperty("metadata", metadata);

    wxFFile fp(wxcProjectMetadata::Get().GetProjectFile(), "w+b");
    if (fp.IsOpened()) {
        root.toElement().addProperty("windows", ToJSON());

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
    if (!e.GetString().IsEmpty() && wxcProjectMetadata::Get().IsLoaded() &&
        e.GetString() == wxcProjectMetadata::Get().GetProjectFile()) {
        // the currently loaded file is the same as this one
        // do nothing
        if (!IsShown()) {
            Show();
        }
        Raise();
        return;
    }

    if (wxcProjectMetadata::Get().IsLoaded() && wxcEditManager::Get().IsDirty()) {
        if (::wxMessageBox(_("Current file has been modified\nContinue?"),
                           "wxCrafter",
                           wxYES_NO | wxCANCEL | wxCENTER,
                           wxCrafter::TopFrame()) != wxYES) {
            return;
        }
    }

    wxString path = e.GetString();
    if (path.IsEmpty()) {
        // User request
        path = wxFileSelector(_("Open wxCrafter project"),
                              wxEmptyString,
                              wxEmptyString,
                              wxEmptyString,
                              "wxCrafter Project File(*.wxcp)|*.wxcp",
                              wxFD_OPEN);

    } else {
        // Programmatically
        wxFileName curfile = wxcProjectMetadata::Get().GetProjectFile();
        wxFileName newfile = path;

        if (curfile == path) {
            return;
        }
    }

    if (path.IsEmpty() == false) {

        /// update the history list
        wxArrayString history = wxcSettings::Get().GetHistory();
        if (history.Index(path) == wxNOT_FOUND) {
            history.Add(path);
            wxcSettings::Get().SetHistory(history);
            wxcSettings::Get().Save();
        }

        wxFileName fn(path);
        wxFFile fp(fn.GetFullPath(), "r+b");
        if (fp.IsOpened()) {
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
    JSON json(fileContent);
    if (!lightLoad) {
        Clear();

        // Read the metadata first
        wxcProjectMetadata::Get().SetProjectFile(fn.GetFullPath());
        wxcProjectMetadata::Get().FromJSON(json.toElement().namedObject("metadata"));
        wxcProjectMetadata::Get().UpdatePaths();

    } else {
        deleteAllItems = true;
    }

    // Read the top-level windows
    JSONItem windows = json.toElement().namedObject("windows");
    int nCount = windows.arraySize();

#ifdef __WXMSW__
    wxWindowUpdateLocker locker(m_treeControls);
#endif
    if (deleteAllItems) {
        m_treeControls->DeleteAllItems();
        m_treeControls->AddRoot(_("wxCrafter Project"), 0, 0);
        project_file = wxcProjectMetadata::Get().GetProjectFile();
        wxcProjectMetadata::Get().FromJSON(
            json.toElement().namedObject("metadata")); // Needed for Redo() to work correctly
        wxcProjectMetadata::Get().UpdatePaths();
    }

    wxTreeItemId dummy;
    for (int i = 0; i < nCount; i++) {
        wxcWidget* wrapper = wxcWidget::CreateFromJSON(windows.arrayItem(i));
        DoBuildTree(dummy, wrapper, m_treeControls->GetRootItem());
    }

    wxTreeItemIdValue cookie;
    wxTreeItemId rootItem = m_treeControls->GetRootItem();
    wxTreeItemId firstChild = m_treeControls->GetFirstChild(rootItem, cookie);
    if (firstChild.IsOk()) {
        m_treeControls->SelectItem(firstChild);
    }
    NotifyPreviewChanged(wxEVT_WXGUI_PROJECT_LOADED);

    wxString rootText = project_file.IsOk() ? project_file.GetFullPath() : _("wxCrafter Project");

    m_treeControls->SetItemText(m_treeControls->GetRootItem(), rootText);
    wxcWidget::SetObjCounter(wxcProjectMetadata::Get().GetObjCounter());

    // Expand the top level root item
    if (rootItem.IsOk() && m_treeControls->ItemHasChildren(rootItem)) {
        m_treeControls->Expand(rootItem);
    }
}

void GUICraftMainPanel::DoBuildTree(wxTreeItemId& itemToSelect,
                                    wxcWidget* wrapper,
                                    const wxTreeItemId& parent,
                                    const wxTreeItemId& beforeItem,
                                    bool insertBefore)
{
    CHECK_PTR_RET(wrapper);
    int imgId = Allocator::Instance()->GetImageId(wrapper->GetType());
    wxTreeItemId item;

    if (beforeItem.IsOk()) {

        wxTreeItemId insertionItem = beforeItem;
        if (insertBefore) {
            insertionItem = m_treeControls->GetPrevSibling(beforeItem);
            if (insertionItem.IsOk() == false) {
                insertionItem = parent;
            }
        }

        item = m_treeControls->InsertItem(
            parent, insertionItem, wrapper->GetName(), imgId, imgId, new GUICraftItemData(wrapper));
        if (itemToSelect.IsOk() == false) {
            itemToSelect = item;
        }

    } else {

        item = m_treeControls->AppendItem(parent, wrapper->GetName(), imgId, imgId, new GUICraftItemData(wrapper));
        if (itemToSelect.IsOk() == false) {
            itemToSelect = item;
        }
    }

    for (auto child : wrapper->GetChildren()) {
        DoBuildTree(itemToSelect, child, item);
    }
}

void GUICraftMainPanel::DoUnsetItemData(const wxTreeItemId& item)
{
    if (item.IsOk()) {

        // Remove the WrapperBase item associated with this item-data
        // by setting it to NULL
        GUICraftItemData* itemData = dynamic_cast<GUICraftItemData*>(m_treeControls->GetItemData(item));
        if (itemData) {
            itemData->m_wxcWidget = NULL;
        }

        if (m_treeControls->ItemHasChildren(item)) {
            wxTreeItemIdValue cookie;
            wxTreeItemId child = m_treeControls->GetFirstChild(item, cookie);
            while (child.IsOk()) {
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

    if (direction == ID_MOVE_NODE_DOWN) {
        target = m_treeControls->GetNextSibling(treeItem);
        CHECK_TREEITEM(target);

    } else {
        // There's no InsertBefore(), so we need to go up 2, then InsertAfter()
        target = m_treeControls->GetPrevSibling(treeItem);
        CHECK_TREEITEM(target);
        wxTreeItemId previous = m_treeControls->GetPrevSibling(target);
        if (previous.IsOk()) {
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
    if (!doPrepend) {
        insertedItem = m_treeControls->InsertItem(root, target, text, image, image, new GUICraftItemData(tlw));
    } else {
        insertedItem = m_treeControls->AppendItem(root, text, image, image, new GUICraftItemData(tlw));
    }

    CHECK_TREEITEM(insertedItem);

    // Restore the parent tree item data
    wxTreeItemId dummy;
    for (auto child : tlw->GetChildren()) {
        DoBuildTree(dummy, child, insertedItem);
    }

    m_treeControls->EnsureVisible(insertedItem);
    m_treeControls->SelectItem(insertedItem);

    wxcEditManager::Get().PushState("top-level window move");
    DoRefresh(wxEVT_UPDATE_PREVIEW);
}

void GUICraftMainPanel::OnMoveItem(wxCommandEvent& e)
{
    GUICraftItemData* itemData = GetSelItemData();
    if (!itemData || !itemData->m_wxcWidget) {
        return;
    } else if (!itemData->m_wxcWidget->GetParent()) {
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

    switch (e.GetId()) {
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
    for (auto child : subTree->GetChildren()) {
        DoBuildTree(dummy, child, treeItemParent);
    }

    wxTreeItemId where;
    DoFindName(treeItemParent, itemName, where);
    if (where.IsOk()) {
        m_treeControls->EnsureVisible(where);
        m_treeControls->SelectItem(where);
    }

    wxcEditManager::Get().PushState("move");
    DoRefresh(wxEVT_UPDATE_PREVIEW);
}

void GUICraftMainPanel::OnDelete(wxCommandEvent& e)
{
    GUICraftItemData* data = GetSelItemData();
    if (data) {
        int eventType = wxEVT_UPDATE_PREVIEW;
        if (data->m_wxcWidget->IsTopWindow()) {
            // Notify the preview that we are about to delete the entire preview page
            NotifyPreviewChanged(wxEVT_WXGUI_PROJECT_CLOSED);
            eventType = wxEVT_WXGUI_PROJECT_LOADED;
        }

        wxTreeItemId newSel = DoFindBestSelection(m_treeControls->GetSelection());

        m_treeControls->DeleteChildren(m_treeControls->GetSelection());
        m_treeControls->Delete(m_treeControls->GetSelection());
        if (newSel.IsOk()) {
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
    if (!searchFromItem.IsOk()) {
        searchFromItem = m_treeControls->GetRootItem();
    }

    wxTreeItemId item;
    DoFindName(searchFromItem, name, item);
    if (item.IsOk()) {
        m_treeControls->Expand(item);
        m_treeControls->EnsureVisible(item);
        m_treeControls->SelectItem(item);
    }
}

void GUICraftMainPanel::DoFindName(const wxTreeItemId& parent, const wxString& name, wxTreeItemId& item)
{
    if (item.IsOk()) {
        return;
    }

    if (parent.IsOk() == false) {
        return;
    }

    if (m_treeControls->GetItemText(parent) == name) {
        item = parent;
        return;
    }

    if (m_treeControls->ItemHasChildren(parent)) {
        wxTreeItemIdValue cookie;
        wxTreeItemId child = m_treeControls->GetFirstChild(parent, cookie);
        while (child.IsOk()) {
            DoFindName(child, name, item);
            if (item.IsOk()) {
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

    if (e.GetId() == ID_MOVE_NODE_DOWN) {
        if (control->GetParent()) {
            e.Enable(control->CanMoveDown());
        } else if (control->IsTopWindow()) {
            wxTreeItemId item;
            DoFindName(m_treeControls->GetRootItem(), control->GetName(), item);
            if (item.IsOk()) {
                e.Enable(m_treeControls->GetNextSibling(item).IsOk());
            }
        }

    } else if (e.GetId() == ID_MOVE_NODE_UP) {
        if (control->GetParent()) {
            e.Enable(control->CanMoveUp());
        } else if (control->IsTopWindow()) {
            wxTreeItemId item;
            DoFindName(m_treeControls->GetRootItem(), control->GetName(), item);
            if (item.IsOk()) {
                e.Enable(m_treeControls->GetPrevSibling(item).IsOk());
            }
        }
    } else if (e.GetId() == ID_MOVE_NODE_INTO_SIZER) {
        if (!control->IsSizerItem()) {
            return;
        }
        wxcWidget* parent = control->GetParent();
        CHECK_POINTER(parent);
        if (parent->IsSizer()) {
            wxcWidget* grandparent = parent->GetParent();
            e.Enable(grandparent && grandparent->IsSizer());
        }
    } else if (e.GetId() == ID_MOVE_NODE_INTO_SIBLING) {
        if (!control->IsSizerItem()) {
            return;
        }
        if (control->CanMoveRight()) {
            bool isAbove;
            control->GetAdjacentSiblingSizer(&isAbove); // Which icon should we use? upper-right or lower-right?
            wxCrafter::ResourceLoader bmps;
            if (isAbove) {
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

    if (e.IsChecked()) {
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
    if (!d || !d->m_wxcWidget || !d->m_wxcWidget->IsSizerItem()) {
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
    if (!item.IsOk()) {
        return wxTreeItemId();
    }

    wxTreeItemId parent = item;

    while (true) {

        wxTreeItemId tmpParent = m_treeControls->GetItemParent(parent);
        if (tmpParent.IsOk() && tmpParent != m_treeControls->GetRootItem()) {
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
    if (!itemData || !itemData->m_wxcWidget) {
        e.Skip();
        return;
    }

    if (wxcWidget::GetCommonEvents().Exists(e.GetId())) {
        ConnectDetails eventDetails = wxcWidget::GetCommonEvents().Item(e.GetId());
        itemData->m_wxcWidget->AddEvent(eventDetails);

    } else if (itemData->m_wxcWidget->GetControlEvents().Exists(e.GetId())) {
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

    if (wxcWidget::GetCommonEvents().Exists(e.GetId())) {
        ConnectDetails eventDetails = wxcWidget::GetCommonEvents().Item(e.GetId());
        e.Check(itemData->m_wxcWidget->HasEvent(eventDetails.GetEventName()));

    } else if (itemData->m_wxcWidget->GetControlEvents().Exists(e.GetId())) {
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
    if (item.IsOk() == false) {
        return wxTreeItemId();
    }

    wxTreeItemId newSel = m_treeControls->GetPrevSibling(item);
    if (newSel.IsOk() == false) {
        newSel = m_treeControls->GetNextSibling(item);
        if (newSel.IsOk() == false && m_treeControls->GetItemParent(item)) {
            newSel = m_treeControls->GetItemParent(item);
        }
    }
    return newSel;
}

bool GUICraftMainPanel::DoUpdateNotebookSelection(const wxTreeItemId& item)
{
    if (!item.IsOk()) {
        return false;
    }

    bool dirty = false;
    GUICraftItemData* itemData = dynamic_cast<GUICraftItemData*>(m_treeControls->GetItemData(item));
    CHECK_POINTER_RET_FALSE(itemData);
    CHECK_POINTER_RET_FALSE(itemData->m_wxcWidget);

    wxcWidget* wrapper = itemData->m_wxcWidget;
    NotebookPageWrapper* page = NULL;
    RibbonPageWrapper* ribbonPage = NULL;
    while (wrapper) {
        page = dynamic_cast<NotebookPageWrapper*>(wrapper);
        if (page) {
            break;
        }
        wrapper = wrapper->GetParent();
    }

    if (!page) {
        wrapper = itemData->m_wxcWidget;
        while (wrapper) {
            ribbonPage = dynamic_cast<RibbonPageWrapper*>(wrapper);
            if (ribbonPage) {
                break;
            }
            wrapper = wrapper->GetParent();
        }
    }

    if (!page && !ribbonPage) {
        return false;
    }

    if (page) {
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
    if (item.IsOk()) {
        wxTreeItemId sel;
        DoFindName(item, controlName, sel);
        if (sel.IsOk()) {
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
    if (item.IsOk()) {
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
    if (item.IsOk()) {
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
    if (item.IsOk()) {
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
    if (!m_plugin->IsMainViewActive()) {
        e.Skip(); // This event is nothing to do with us
        return;
    }

    if (!IsTreeViewSelected()) {
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

    if (m_treeControls->ItemHasChildren(item)) {
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
    if (!m_plugin->IsMainViewActive()) {
        e.Skip(); // This event is nothing to do with us
        return;
    }

    if (!IsTreeViewSelected()) {
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
    if (!m_plugin->IsMainViewActive()) {
        e.Skip(); // This event is nothing to do with us
        return;
    }

    if (!IsTreeViewSelected()) {
        e.Skip(); // The treeview isn't selected, so don't make an unintended paste into it
        return;
    }

    CHECK_POINTER(m_clipboardItem);

    wxcWidget *source, *target;
    wxcWidget* destinationItem = NULL;
    wxcWidget* destTLW = NULL;
    GUICraftItemData* sel = GetSelItemData();

    if (!sel) {
        // pasting on the root item
        target = NULL;

    } else {
        CHECK_POINTER(sel->m_wxcWidget);

        destinationItem = sel->m_wxcWidget;

        // Check if we can paste it here
        if (!Allocator::Instance()->CanPaste(m_clipboardItem, destinationItem)) {
            wxMessageBox(_("Can't paste it here"));
            return;
        }
        target = destinationItem;
    }

    bool differentTLW(false);
    wxString newname, newinheritedname, newfilename;
    int options(DO_renameAllChildren);

    DuplicateTLWDlg dlg(this);
    if (m_clipboardItem->IsTopWindow()) {
        while (true) {
            if (dlg.ShowModal() != wxID_OK) {
                return; // Presumably the user meant cancel the whole process
            }
            newname = dlg.GetBaseName();
            wxTreeItemId match;
            DoFindName(m_treeControls->GetRootItem(), newname, match);
            if (match.IsOk()) {
                if (wxMessageBox(_("This name is already in use. Try again?"),
                                 _("wxCrafter"),
                                 wxYES_NO | wxICON_QUESTION,
                                 this) == wxYES) {
                    continue;
                } else {
                    return;
                }
            }
            break;
        }
        newinheritedname = dlg.GetInheritedName();
        newfilename = dlg.GetFilename();
    } else if (target) {
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

    if (m_clipboardItem->IsTopWindow() || differentTLW) {
        if (wxcSettings::Get().HasFlag(wxcSettings::DUPLICATE_KEEPS_ALL_NAMES)) {
            options = DO_renameNone;
        } else if (wxcSettings::Get().HasFlag(wxcSettings::DUPLICATE_KEEPS_USERSET_NAMES)) {
            options = DO_renameAllChildrenExceptUsernamed;
        }
        options |= (wxcSettings::Get().HasFlag(wxcSettings::DUPLICATE_EVENTHANDLERS_TOO) ? DO_copyEventsToo : 0);
    }

    std::set<wxString> existingNames;
    if (destTLW) {
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

    if (!sel) {
        // pasting on the root item
        target = NULL;

    } else {
        CHECK_POINTER(sel->m_wxcWidget);

        destinationItem = sel->m_wxcWidget;

        // Check if we can paste it here
        if (!Allocator::Instance()->CanPaste(destinationItem, destinationItem)) {
            wxMessageBox(_("Can't paste it here"));
            return;
        }
        target = destinationItem;
    }

    wxString newname, newinheritedname, newfilename;
    int options(DO_renameAllChildren);

    // We're duplicating, not pasting, so only TLWs can possibly retain old names for child-controls
    if (destinationItem->IsTopWindow()) {

        DuplicateTLWDlg dlg(this);
        while (true) {
            if (dlg.ShowModal() != wxID_OK) {
                return; // Presumably the user meant cancel the whole process
            }
            newname = dlg.GetBaseName();
            wxTreeItemId match;
            DoFindName(m_treeControls->GetRootItem(), newname, match);
            if (match.IsOk()) {
                if (wxMessageBox(_("This name is already in use. Try again?"),
                                 _("wxCrafter"),
                                 wxYES_NO | wxICON_QUESTION,
                                 this) == wxYES) {
                    continue;
                } else {
                    return;
                }
            }
            break;
        }
        newinheritedname = dlg.GetInheritedName();
        newfilename = dlg.GetFilename();

        if (wxcSettings::Get().HasFlag(wxcSettings::DUPLICATE_KEEPS_ALL_NAMES)) {
            options = DO_renameNone;
        } else if (wxcSettings::Get().HasFlag(wxcSettings::DUPLICATE_KEEPS_USERSET_NAMES)) {
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

    if ((!target && source->IsTopWindow()) || // pasting on the root item a top level window
        (source->IsTopWindow() && target->IsTopWindow())) {
        // top level window copy/paste
        DoAppendItem(wxTreeItemId(), m_treeControls->GetRootItem(), source);

    } else {

        int insertType = Allocator::Instance()->GetInsertionType(source->GetType(), target->GetType(), true);
        if (duplicating && (insertType == Allocator::INSERT_CHILD) &&
            (wxcWidget::GetWidgetType(target->GetType()) == TYPE_SIZER) && target->IsSizerItem()) {
            // If we're duplicating a (child) sizer, don't insert the dup into the original; make it a sibling
            insertType = Allocator::INSERT_SIBLING;
        }

        if (insertType == Allocator::INSERT_SIBLING) {
            DoInsertBefore(wxTreeItemId(), m_treeControls->GetSelection(), source);

        } else if (insertType == Allocator::INSERT_CHILD) {

            // trying to paste a wxAUI manager on a control with wxAUI manager?
            if (target->IsAuiManaged() && source->GetType() == ID_WXAUIMANAGER) {
                wxMessageBox(_("Only one wxAui Manager is allowed"), "wxCrafter", wxOK | wxICON_WARNING | wxCENTER);
                wxDELETE(source);
                return;
            }

            DoAppendItem(wxTreeItemId(), m_treeControls->GetSelection(), source);

        } else if (insertType == Allocator::INSERT_MAIN_SIZER) {
            if (target->HasMainSizer()) {
                wxDELETE(source);
                wxMessageBox(_("Can't insert this item here\nThere is already a main sizer"));
                return;

            } else if (target->IsAuiManaged()) {
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
    if (m_draggedItem.IsOk()) {
        event.Allow();
    }
}

void GUICraftMainPanel::OnEndDrag(wxTreeEvent& event)
{
    wxTreeItemId itemDst = event.GetItem();
    if (!itemDst.IsOk() || !m_draggedItem.IsOk()) {
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
    if (source == target) {
        return;
    }

    // target is child of source?
    if (target->IsDirectOrIndirectChildOf(source)) {
        return;
    }

    // Can we paste the item to the new location?
    if (!Allocator::Instance()->CanPaste(source, target)) {
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

    if (Pasting) {
        if (source->IsTopWindow() || differentTLW) {
            if (wxcSettings::Get().HasFlag(wxcSettings::DUPLICATE_KEEPS_ALL_NAMES)) {
                options = DO_renameNone;
            } else if (wxcSettings::Get().HasFlag(wxcSettings::DUPLICATE_KEEPS_USERSET_NAMES)) {
                options = DO_renameAllChildrenExceptUsernamed;
            }
        }
    } else {
        options = DO_renameNone;
    }

    if (Pasting) {
        options |= (wxcSettings::Get().HasFlag(wxcSettings::DUPLICATE_EVENTHANDLERS_TOO) ? DO_copyEventsToo : 0);
    } else {
        // If we're moving, always retain any eventhandlers
        options |= DO_copyEventsToo;
    }

    wxTreeItemId itemSrc;
    if (Pasting) {
        // If this is a Paste rather than a Move, we need to copy the source
        source = source->Copy((DuplicatingOptions)options, existingNames);

    } else {
        if (!source->IsTopWindow() && differentTLW) {
            // Even if it's a Move, we still may need rename to avoid a name-clash
            // So, though it's a bit clunky, Copy() the source, then steal any new name from the copy
            wxcWidget* clone = source->Copy((DuplicatingOptions)options, existingNames);
            if (source->GetName() != clone->GetName()) {
                source->SetName(clone->GetName());
            }

            wxDELETE(clone);
        }

        itemSrc = m_draggedItem;
    }

    wxWindowUpdateLocker locker(m_treeControls);

    int insertType = Allocator::Instance()->GetInsertionType(source->GetType(), target->GetType(), true);
    if (insertType == Allocator::INSERT_SIBLING) {
        // We want to insert where the marker is, so pass 'false' for insert-after
        // (That means we can't insert above the top item: there's no place for the marker to go except onto the
        // containing sizer,
        // so the item gets appended to the that instead. However the top item can always be moved down...)
        DoInsertBefore(itemSrc, itemDst, source, false);

    } else if (insertType == Allocator::INSERT_CHILD || insertType == Allocator::INSERT_MAIN_SIZER) {
        DoAppendItem(itemSrc, itemDst, source);
    }

    NotifyPreviewChanged();
    wxcEditManager::Get().PushState(Pasting ? "paste" : "move");
}

void GUICraftMainPanel::DoAppendItem(const wxTreeItemId& sourceItem,
                                     const wxTreeItemId& targetItem,
                                     wxcWidget* sourceItemData)
{
    wxcWidget *source, *target = NULL;
    GUICraftItemData* guiTargetItem = dynamic_cast<GUICraftItemData*>(m_treeControls->GetItemData(targetItem));

    if (guiTargetItem && guiTargetItem->m_wxcWidget) {
        target = guiTargetItem->m_wxcWidget;
    }

    if (sourceItem.IsOk()) {
        GUICraftItemData* guiSourceItem = dynamic_cast<GUICraftItemData*>(m_treeControls->GetItemData(sourceItem));
        CHECK_POINTER(guiSourceItem);
        CHECK_POINTER(guiSourceItem->m_wxcWidget);
        source = guiSourceItem->m_wxcWidget;
        source->RemoveFromParent();

        // Disconnect the dragged item from its parent
        DoUnsetItemData(sourceItem);

        // Delete the UI of the dragged item
        if (m_treeControls->ItemHasChildren(sourceItem)) {
            m_treeControls->DeleteChildren(sourceItem);
        }
        m_treeControls->Delete(sourceItem);

    } else {
        source = sourceItemData;
        CHECK_POINTER(source);
    }

    // Paste it as a child of the target item
    if (target) {
        // the only case where "target" is NULL is when a top level item is being appended
        // in this case it has no one to be added to as a child
        target->AddChild(source);
    }

    wxTreeItemId selection;
    DoBuildTree(selection, source, targetItem);
    if (selection.IsOk()) {
        m_treeControls->SelectItem(selection);
        m_treeControls->EnsureVisible(selection);
    }
}

void GUICraftMainPanel::DoInsertBefore(const wxTreeItemId& sourceItem,
                                       const wxTreeItemId& targetItem,
                                       wxcWidget* sourceItemData,
                                       bool insertBefore)
{
    wxcWidget *source, *target;
    GUICraftItemData* guiTargetItem = dynamic_cast<GUICraftItemData*>(m_treeControls->GetItemData(targetItem));
    CHECK_POINTER(guiTargetItem);
    CHECK_POINTER(guiTargetItem->m_wxcWidget);
    target = guiTargetItem->m_wxcWidget;

    CHECK_POINTER(target->GetParent());

    if (sourceItem.IsOk()) {
        GUICraftItemData* guiSourceItem = dynamic_cast<GUICraftItemData*>(m_treeControls->GetItemData(sourceItem));
        CHECK_POINTER(guiSourceItem);
        CHECK_POINTER(guiSourceItem->m_wxcWidget);
        source = guiSourceItem->m_wxcWidget;

        // Check if we can perform the insert action
        if (!DoCheckInsert(source, target)) {
            return;
        }

        // Disconnect the dragged item from its parent
        source->RemoveFromParent();
        DoUnsetItemData(sourceItem);

        // Delete the UI of the dragged item
        if (m_treeControls->ItemHasChildren(sourceItem)) {
            m_treeControls->DeleteChildren(sourceItem);
        }
        m_treeControls->Delete(sourceItem);

    } else {
        source = sourceItemData;
        CHECK_POINTER(source);

        // Check if we can perform the insert action
        if (!DoCheckInsert(source, target)) {
            return;
        }
    }

    // Paste it as a child of the target item
    if (insertBefore) {
        target->GetParent()->InsertBefore(source, target);

    } else {
        target->GetParent()->InsertAfter(source, target);
    }

    wxTreeItemId selection;
    DoBuildTree(selection, source, m_treeControls->GetItemParent(targetItem), targetItem, insertBefore);

    if (selection.IsOk()) {
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
    if (topLevel.IsOk() == false) {
        return NULL;
    }

    wxTreeItemId sel = m_treeControls->GetSelection();
    if (sel.IsOk() == false) {
        return NULL;
    }

    // Check that this top level is indeed a wxWizard...
    GUICraftItemData* topLevelItemData = dynamic_cast<GUICraftItemData*>(m_treeControls->GetItemData(topLevel));
    CHECK_POINTER_RET_NULL(topLevelItemData);
    CHECK_POINTER_RET_NULL(topLevelItemData->m_wxcWidget);
    if (topLevelItemData->m_wxcWidget->GetType() != ID_WXWIZARD) {
        return NULL;
    }

    GUICraftItemData* itemData = dynamic_cast<GUICraftItemData*>(m_treeControls->GetItemData(sel));
    CHECK_POINTER_RET_NULL(itemData);
    CHECK_POINTER_RET_NULL(itemData->m_wxcWidget);

    wxcWidget* p = itemData->m_wxcWidget;
    while (p && p->GetType() != ID_WXWIZARDPAGE) {
        p = p->GetParent();
    }

    if (p && p->GetType() == ID_WXWIZARDPAGE) {
        return p;

    } else if (!p && m_treeControls->ItemHasChildren(topLevel)) {
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
    if (wxcEditManager::Get().IsDirty()) {
        if (::wxMessageBox(_("Current file has been modified\nClose anyway?"),
                           "wxCrafter",
                           wxYES_NO | wxCANCEL | wxCENTER,
                           wxCrafter::TopFrame()) != wxYES) {
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

    wxString parentName = e.GetString().BeforeFirst(wxT(':'));

    const wxcWidget* parent = nullptr;
    if (parentName == "TOOL_BAR_ID") {
        // direct toolbar child of the main frame
        parent = tlid->FindFirstDirectChildOfType(ID_WXTOOLBAR);

    } else if (parentName == "MENU_BAR_ID") {
        // direct toolbar child of the main frame
        parent = tlid->FindFirstDirectChildOfType(ID_WXMENUBAR);

    } else {
        parent = tlid->FindChildByName(parentName);
    }
    CHECK_POINTER(parent);

    wxString toolName = e.GetString().AfterFirst(wxT(':'));
    const wxcWidget::List_t& tools = parent->GetChildren();
    for (auto child : tools) {
        if (child->PropertyString(PROP_LABEL) == toolName) {

            // Select this item
            wxCommandEvent evt(wxEVT_PREVIEW_CTRL_SELECTED);
            evt.SetString(child->GetName());
            EventNotifier::Get()->AddPendingEvent(evt);

            break;
        }
    }
}

wxcWidget* GUICraftMainPanel::DoGetItemData(const wxTreeItemId& item) const
{
    if (!item.IsOk()) {
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
    if (!(m_plugin->GetMainFrame() && m_plugin->GetMainFrame()->IsShown())) {
        e.Skip();
        return;
    }

    if (IsPropertyGridPropertySelected()) {
        e.Skip();
        return; // Otherwise Ctrl-Y in the EventsTableListView may do unwanted things to the tree
    }

    if (wxcEditManager::Get().CanRedo()) {

        State::Ptr_t state = wxcEditManager::Get().Redo();
        if (state) {
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
    if (!(m_plugin->GetMainFrame() && m_plugin->GetMainFrame()->IsShown())) {
        e.Skip();
        return;
    }

    if (IsPropertyGridPropertySelected()) {
        e.Skip();
        return; // Otherwise Ctrl-Z in the EventsTableListView may do unwanted things to the tree
    }

    if (wxcEditManager::Get().CanUndo()) {

        State::Ptr_t state = wxcEditManager::Get().Undo();
        if (state) {
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
    if (!(m_plugin->GetMainFrame() && m_plugin->GetMainFrame()->IsShown())) {
        event.Skip();
        return;
    }

    // wxString label = wxGetTextFromUser("What would you like to call the current state?", "Label current state", "",
    // this);
    // if (!label.empty()) {
    //    wxcEditManager::Get().SetUserLabel(label);
    //}
}

void GUICraftMainPanel::OnLoadCurrentState(
    wxCommandEvent& WXUNUSED(event)) // Called after a multiple unredo has changed the GetCurrentState() value
{
    State::Ptr_t state = wxcEditManager::Get().GetCurrentState();
    if (!state) {
        return;
    }
    LoadProject(wxFileName(), state->project_json, true);
    DoSelectItemByName(state->selection, state->parentTLW);
}

State::Ptr_t GUICraftMainPanel::CurrentState()
{
    JSON root(JsonType::Object);

    // If we don't have bitmap function assigned to this project, create new one
    if (wxcProjectMetadata::Get().GetBitmapFunction().IsEmpty()) {
        wxcProjectMetadata::Get().GenerateBitmapFunctionName();
    }

    // Update the counter info; this will otherwise be wrong after e.g. a duplication
    wxcProjectMetadata::Get().SetObjCounter(wxcWidget::GetObjCounter());

    JSONItem metadata = wxcProjectMetadata::Get().ToJSON();
    wxcProjectMetadata::Get().AppendCustomControlsJSON(GetCustomControlsUsed(), metadata);
    root.toElement().addProperty("metadata", metadata);
    root.toElement().addProperty("windows", ToJSON(wxTreeItemId()));

    State::Ptr_t state(new State);
    state->project_json = root.toElement().format();

    wxTreeItemId item = m_treeControls->GetSelection();
    if (item.IsOk()) {
        state->selection = m_treeControls->GetItemText(item);
        GUICraftItemData* control = dynamic_cast<GUICraftItemData*>(m_treeControls->GetItemData(item));
        if (control && control->m_wxcWidget) {
            state->parentTLW = control->m_wxcWidget->GetTopLevel()->GetName();
        }
    }
    return state;
}

void GUICraftMainPanel::DoSelectItemByName(const wxString& name, const wxString& parentTLW)
{
    if (name.IsEmpty()) {
        return;
    }

    wxTreeItemId startpoint = m_treeControls->GetRootItem();
    if (!parentTLW.IsEmpty()) {
        wxTreeItemId sp;
        DoFindName(startpoint, parentTLW, sp);
        if (sp.IsOk()) {
            startpoint = sp;
        }
    }

    wxTreeItemId where;
    DoFindName(startpoint, name, where);
    if (where.IsOk()) {
        m_treeControls->SelectItem(where);
        m_treeControls->EnsureVisible(where);

        wxCommandEvent eventTreeItemSelected(wxEVT_TREE_ITEM_SELECTED);
        eventTreeItemSelected.SetString(name);
        EventNotifier::Get()->AddPendingEvent(eventTreeItemSelected);
    }
}

bool GUICraftMainPanel::IsTreeViewSelected() const { return wxWindow::FindFocus() == m_treeControls; }

bool GUICraftMainPanel::IsPropertyGridPropertySelected() const
{
    // This function tests if the parent of an item is a propertygrid. The idea is to avoid
    // e.g. Ctrl-Z affecting the tree when the user was actually trying to edit a property
    wxWindow* win = wxWindow::FindFocus();
    if (!win) {
        return false;
    }

    return dynamic_cast<wxPropertyGrid*>(win->GetParent()) != NULL;
}

wxcWidget* GUICraftMainPanel::GetActiveTopLevelWin() const
{
    wxTreeItemId topLevel = DoGetTopLevelTreeItem();
    if (topLevel.IsOk() == false) {
        return NULL;
    }

    GUICraftItemData* topLevelItemData = dynamic_cast<GUICraftItemData*>(m_treeControls->GetItemData(topLevel));
    CHECK_POINTER_RET_NULL(topLevelItemData);
    return topLevelItemData->m_wxcWidget;
}

void GUICraftMainPanel::OnGenerateCodeMenu(wxAuiToolBarEvent& e)
{
    if (e.IsDropDownClicked()) {
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
    if (event.GetId() == XRCID("GenerateCPP")) {
        wxcProjectMetadata::Get().SetGenerateCPPCode(event.IsChecked());
    }
    if (event.GetId() == XRCID("GenerateXRC")) {
        wxcProjectMetadata::Get().SetGenerateXRC(event.IsChecked());
    }
}

void GUICraftMainPanel::OnNewCustomControlMenu(wxCommandEvent& e)
{
    wxMenu menu(_("Choose a control"));
    const CustomControlTemplateMap_t& controls = wxcSettings::Get().GetTemplateClasses();

    if (controls.empty() == false) {
        for (const auto& p : controls) {
            menu.Append(p.second.GetControlId(), p.first);
            menu.Connect(p.second.GetControlId(),
                         wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(GUICraftMainPanel::OnNewCustomControl),
                         NULL,
                         this);
        }
        menu.AppendSeparator();
    }
    menu.Append(XRCID("define_custom_controls"), _("Define custom control..."));

    wxButton* button = dynamic_cast<wxButton*>(e.GetEventObject());
    if (button) {
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
    if (control->GetType() == ID_WXAUIMANAGER && parent->IsAuiManaged()) {
        wxDELETE(control);
        wxMessageBox(_("Only one wxAui Manager is allowed"), "wxCrafter", wxOK | wxICON_WARNING | wxCENTER);
        return;
    }

    if (control->GetType() == ID_WXAUIMANAGER && parent->HasMainSizer()) {
        wxDELETE(control);
        wxMessageBox(_("wxAui Manager can not be placed onto a control with a main sizer"),
                     "wxCrafter",
                     wxOK | wxICON_WARNING | wxCENTER);
        return;
    }

    if (parent->GetType() == ID_WXRIBBONPANEL && !parent->GetChildren().empty() &&
        insertType == Allocator::INSERT_CHILD) {
        wxDELETE(control);
        wxMessageBox(_("wxRibbonPanel can only have one direct child"), "wxCrafter", wxOK | wxICON_WARNING | wxCENTER);
        return;
    }

    if (insertType == Allocator::INSERT_SIBLING) {
        DoInsertBefore(wxTreeItemId(), m_treeControls->GetSelection(), control, false);

    } else if (insertType == Allocator::INSERT_CHILD || insertType == Allocator::INSERT_MAIN_SIZER) {

        // We need to place a main sizer, check that we don't have one already...
        if (parent->HasMainSizer() && insertType == Allocator::INSERT_MAIN_SIZER) {
            wxDELETE(control);
            wxMessageBox(_("Can't insert this item here\nThere is already a main sizer"));
            return;

        } else if (parent->IsAuiManaged() && insertType == Allocator::INSERT_MAIN_SIZER) {
            wxDELETE(control);
            wxMessageBox(_("A Main Sizer can not be placed into a wxAui managed window"),
                         "wxCrafter",
                         wxOK | wxICON_WARNING | wxCENTER);
            return;
        }

        parent->AddChild(control);
        wxTreeItemId item = m_treeControls->AppendItem(
            m_treeControls->GetSelection(), control->GetName(), imgId, imgId, new GUICraftItemData(control));
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
    for (size_t i = 0; i < m_notebook2->GetPageCount(); ++i) {
        if (win == m_notebook2->GetPage(i)) {
            pageIdx = i;
            break;
        }
    }
    return pageIdx;
}

void GUICraftMainPanel::DoShowPropertiesPage(wxWindow* win, const wxString& text, bool show)
{
    size_t where = DoFindPropertiesPage(win);
    if (show && where == wxString::npos) {
        m_notebook2->AddPage(win, text);

    } else if (!show && where != wxString::npos) {
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
    while (child.IsOk()) {

        GUICraftItemData* itemData = dynamic_cast<GUICraftItemData*>(m_treeControls->GetItemData(child));
        if (itemData && itemData->m_wxcWidget) {
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
    if (sel == 1) { // C++
        event.SetClientData(m_notebookCpp->GetSelection() == 0 ? m_textCtrlCppSource : m_textCtrlHeaderSource);
        m_mainBook->CppPageSelected(m_textCtrlCppSource, m_textCtrlHeaderSource);

    } else if (sel == 2) { // XRC
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
    if (IsShown()) {
        int sel = m_mainBook->GetSelection();
        if (sel == 1) { // C++
            e.Skip(false);
            sel = m_notebookCpp->GetSelection();
            e.SetCtrl(sel == 0 ? m_textCtrlCppSource : m_textCtrlHeaderSource);

        } else if (sel == 2) { // XRC
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
    if (m_treeControls->GetSelection().IsOk()) {
        GUICraftItemData* itemData =
            dynamic_cast<GUICraftItemData*>(m_treeControls->GetItemData(m_treeControls->GetSelection()));
        CHECK_POINTER(itemData);
        CHECK_POINTER(itemData->m_wxcWidget);

        wxString msg = wxString::Format(_("Rename '%s'"), itemData->m_wxcWidget->GetName());
        wxString new_name = ::wxGetTextFromUser(_("Enter the new name:"), msg, itemData->m_wxcWidget->GetName(), this);
        if (new_name.empty()) {
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
    if (itemData && itemData->m_wxcWidget == widget) {
        return item;
    }

    if (!m_treeControls->HasChildren(item)) {
        return wxTreeItemId();
    }

    wxTreeItemIdValue cookie;
    wxTreeItemId child = m_treeControls->GetFirstChild(item, cookie);
    while (child.IsOk()) {

        GUICraftItemData* itemData = dynamic_cast<GUICraftItemData*>(m_treeControls->GetItemData(child));
        if (itemData && itemData->m_wxcWidget) {
            wxTreeItemId matchedItem = DoFindItemByWxcWidget(widget, child);
            if (matchedItem.IsOk()) {
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
    if (templateInfo.IsValid() == false) {
        return;
    }

    GUICraftItemData* data = GetSelItemData();
    CHECK_POINTER(data);
    CHECK_POINTER(data->m_wxcWidget);

    // Allocate the new control
    CustomControlWrapper* control = dynamic_cast<CustomControlWrapper*>(wxcWidget::Create(ID_WXCUSTOMCONTROL));
    CHECK_POINTER(control);
    control->SetTemplInfoName(templateInfo.GetClassName());

    int insertType = Allocator::Instance()->GetInsertionType(control->GetType(), data->m_wxcWidget->GetType(), false);
    int imgId = Allocator::Instance()->GetImageId(ID_WXCUSTOMCONTROL);
    DoInsertControl(control, data->m_wxcWidget, insertType, imgId);
}

wxWindow* GUICraftMainPanel::GetTopFrame() const
{
    wxWindow* top = wxCrafter::TopFrame();
    if (m_plugin->GetMainFrame() && m_plugin->GetMainFrame()->IsShown()) {
        top = m_plugin->GetMainFrame();
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
    switch (id) {
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

    if (widgetType == wxNOT_FOUND) {
        return;
    }

    wxString label;
    wxcWidget* newWidget = wxcWidget::Create(widgetType);

    if (id < ID_CHANGE_SIZER_LAST) { // Changing the sizer type
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

    } else if (id < ID_INSERT_INTO_SIZER_LAST) { // Inserting into a new sizer
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
        for (auto child : parentWidget->GetChildren()) {
            DoBuildTree(dummy, child, treeItemParent);
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
    if (m_mainBook->GetSelection() == 1) {
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

void GUICraftMainPanel::DoGenerateCode(InteractionMode interactionMode, SaveMode saveMode)
{
    if (!wxcProjectMetadata::Get().GetGenerateCPPCode() && !wxcProjectMetadata::Get().GetGenerateXRC()) {
        if (interactionMode == InteractionMode::HideDialogs) {
            return;
        }
        wxString msg;
        msg << _("You need to enable at least one of 'Generate C++ code' and 'Generate XRC'");
        ::wxMessageBox(msg, "wxCrafter", wxOK | wxCENTER | wxICON_WARNING, wxCrafter::TopFrame());
        return;
    }

    if (wxcProjectMetadata::Get().GetProjectFile().IsEmpty()) {
        if (interactionMode == InteractionMode::HideDialogs) {
            return;
        }
        wxString msg;
        msg << _("You must save the project before generating code");
        ::wxMessageBox(msg, "wxCrafter", wxOK | wxCENTER | wxICON_WARNING, wxCrafter::TopFrame());
        return;
    }

    if (saveMode == SaveMode::SaveBeforeGeneration) {
        // save the project when generating code even if its not modified
        wxCommandEvent dummy;
        OnSaveProject(dummy);
    }

    OutputDirectoryLocker locker{
        FindConfigEntry(wxcProjectMetadata::Get().GetProjectFileName()).value_or(wxEmptyString)};

    wxFileName outputDir(wxcProjectMetadata::Get().GetGeneratedFilesDir(), "");
    wxCrafter::MakeAbsToProject(outputDir);
    if (!outputDir.DirExists()) {
        if (interactionMode == InteractionMode::HideDialogs) {
            return;
        }
        wxString msg;
        msg << _("Please set the base classes generated files output directory\nThis can be done by selecting the root "
                 "item of the tree and edit the properties");
        ::wxMessageBox(msg, "wxCrafter", wxOK | wxCENTER | wxICON_WARNING, wxCrafter::TopFrame());
        return;
    }

    wxArrayString headers;
    wxString baseCpp;
    wxString baseHeader;

    wxString autoGenComment;
    autoGenComment << "//////////////////////////////////////////////////////////////////////\n";
    autoGenComment << "// This file was auto-generated by CodeLite's wxCrafter Plugin\n";
    autoGenComment << "// wxCrafter project file: " << wxcProjectMetadata::Get().GetProjectFileName().GetFullName()
                   << "\n";
    autoGenComment << "// Do not modify this file by hand!\n";
    autoGenComment << "//////////////////////////////////////////////////////////////////////\n\n";

    // Before we start traversing the tree, clear the bitmap code generator content
    wxcCodeGeneratorHelper::Get().Clear();
    wxcProjectMetadata::Get().ClearAggregatedData();
    wxStringMap_t additionalFiles;

    GenerateCppOutput(baseCpp, baseHeader, headers, additionalFiles);

    wxcProjectMetadata::Get().SetAdditionalFiles(additionalFiles);
    wxCrafter::WriteGeneratedOutput(
        baseCpp, baseHeader, headers, additionalFiles, autoGenComment, [](const wxFileName& filename) {
#if !STANDALONE_BUILD
            clSourceFormatEvent event(wxEVT_FORMAT_FILE);
            event.SetFileName(filename.GetFullPath());
            EventNotifier::Get()->ProcessEvent(event);
            NotifyFileSaved(filename);
#endif
        });

    // Export the XRC output if required
    if (wxcProjectMetadata::Get().GetGenerateXRC()) {
        wxString xrcFilePath = wxcProjectMetadata::Get().GetXrcFileName();
        if (!xrcFilePath.empty()) {
            wxString XrcOutput;
            GenerateXrcOutput(XrcOutput);
            if (!XrcOutput.empty()) {
                TopLevelWinWrapper::WrapXRC(XrcOutput);
                // Format the output and make it human readable
                wxStringInputStream str(XrcOutput);
                wxStringOutputStream out;
                wxXmlDocument doc(str);
                if (!doc.Save(out)) {
                    wxMessageBox(XrcOutput);
                }

                wxFileName fnXrcFilePath(xrcFilePath);
                wxCrafter::MakeAbsToProject(fnXrcFilePath);
                wxCrafter::WriteFile(fnXrcFilePath.GetFullPath(), out.GetString(), true);
#if !STANDALONE_BUILD
                NotifyFileSaved(fnXrcFilePath);
#endif
            }
        }
    }

    // And finally, generate the Bitmap resource file
    auto requestDesignerRefresh = []() {
        wxCommandEvent evt(wxEVT_REFRESH_DESIGNER);
        EventNotifier::Get()->AddPendingEvent(evt);
    };
    auto bitmapGenerationStart = []() {
        wxFrame* topFrame = EventNotifier::Get()->TopFrame();
        if (topFrame) {
            topFrame->SetStatusText("Generating bitmap code...");
        }
        wxBeginBusyCursor();
    };
    auto bitmapGenerationEnd = []() {
        if (wxIsBusy()) {
            wxEndBusyCursor();
        }
        wxFrame* topFrame = EventNotifier::Get()->TopFrame();
        if (topFrame) {
            topFrame->SetStatusText("Ready");
        }
    };

    const auto ret = wxcCodeGeneratorHelper::Get().CreateXRC(requestDesignerRefresh,
                                                             bitmapGenerationStart,
                                                             bitmapGenerationEnd,
#if STANDALONE_BUILD
                                                             nullptr
#else
                                                             &NotifyFileSaved
#endif
    );
    if (!ret.ok()) {
        ::wxMessageBox(ret.message(), "wxCrafter", wxOK | wxCENTER | wxICON_ERROR, wxCrafter::TopFrame());
    }
}

void GUICraftMainPanel::BatchGenerate(const wxArrayString& files)
{
    wxArrayString projectsGenerated;
    wxArrayString wxcpFiles;

#ifdef STANDALONE_BUILD
    wxUnusedVar(files);
    wxFileDialog openFileDialog(this,
                                _("Select wxCrafter files:"),
                                "",
                                "",
                                "wxCrafter Project Files (*.wxcp)|*.wxcp",
                                wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE);
    if (openFileDialog.ShowModal() != wxID_OK) {
        return;
    }
    openFileDialog.GetPaths(wxcpFiles);
#else
    wxcpFiles = files;
#endif

    if (wxcpFiles.IsEmpty()) {
        return;
    }
    if (wxcProjectMetadata::Get().IsLoaded()) {
        ::wxMessageBox(_("Please close the current wxCrafter project before batch generating code"),
                       "wxCrafter",
                       wxOK | wxCENTER | wxICON_ERROR);
        return;
    }

    {
        wxBusyCursor bc;
        wxBusyInfo bi(_("Generating Code..."));

        // Lock the UI
        wxWindowUpdateLocker locker(EventNotifier::Get()->TopFrame());

        for (const auto& filename : wxcpFiles) {
            wxString fileContent;
            if (FileUtils::ReadFileContent(filename, fileContent)) {
                LoadProject(filename, fileContent);
                DoGenerateCode(InteractionMode::HideDialogs, SaveMode::SaveBeforeGeneration);
                projectsGenerated.Add(filename);
            }
        }

        // Close any project
        if (wxcProjectMetadata::Get().IsLoaded()) {
            wxCommandEvent e;
            OnCloseProject(e);
        }
    }
    if (!projectsGenerated.IsEmpty()) {
        wxString message;
        message << _("Generated Code For the following projects:\n") << StringUtils::wxImplode(projectsGenerated, "\n");
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
