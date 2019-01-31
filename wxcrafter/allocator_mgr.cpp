#include "allocator_mgr.h"
#include "ActivityrIndicatorWrapper.h"
#include "AnimationCtrlWrapper.h"
#include "AuiToolBarTopLevel.h"
#include "BitmapComboxWrapper.h"
#include "EventsEditorDlg.h"
#include "RearrangeListWrapper.h"
#include "SimpleHtmlListBoxWrapper.h"
#include "TimePickerCtrlWrapper.h"
#include "aui_manager_wrapper.h"
#include "aui_notebook_wrapper.h"
#include "banner_window_wrapper.h"
#include "bitmap_button_wrapper.h"
#include "bitmap_wrapepr.h"
#include "bitmaptogglebuttonwrapper.h"
#include "bool_property.h"
#include "box_sizer_wrapper.h"
#include "button_wrapper.h"
#include "calendar_ctrl_wrapper.h"
#include "check_box_wrapper.h"
#include "check_list_box_wrapper.h"
#include "choice_book_wrapper.h"
#include "choice_property.h"
#include "choice_wrapper.h"
#include "collapsible_pane_wrapper.h"
#include "colour_picker_wrapper.h"
#include "combox_wrapper.h"
#include "command_link_button_wrapper.h"
#include "custom_control_wrapper.h"
#include "data_view_list_ctrl_column.h"
#include "data_view_list_ctrl_wrapper.h"
#include "data_view_tree_ctrl_wrapper.h"
#include "data_view_tree_list_ctrl_wrapper.h"
#include "date_picker_ctrl.h"
#include "dialog_wrapper.h"
#include "dir_picker_ctrl_wrapper.h"
#include "file_picker_ctrl_wrapper.h"
#include "flexgridsizer_wrapper.h"
#include "font_picker_ctrl_wrapper.h"
#include "frame_wrapper.h"
#include "gauge_wrapper.h"
#include "generic_dir_ctrl_wrapper.h"
#include "gl_canvas_wrapper.h"
#include "grid_bag_sizer_wrapper.h"
#include "grid_column_wrapper.h"
#include "grid_row_wrapper.h"
#include "grid_sizer_wrapper.h"
#include "grid_wrapper.h"
#include "html_window_wrapper.h"
#include "hyper_link_ctrl_wrapper.h"
#include "image_list_wrapper.h"
#include "info_bar_button_wrapper.h"
#include "info_bar_wrapper.h"
#include "list_book_wrapper.h"
#include "list_box_wrapper.h"
#include "list_ctrl_column_wrapper.h"
#include "list_ctrl_wrapper.h"
#include "media_ctrl_wrapper.h"
#include "menu_bar_wrapper.h"
#include "menu_item_wrapper.h"
#include "menu_wrapper.h"
#include "notebook_page_wrapper.h"
#include "notebook_wrapper.h"
#include "panel_wrapper.h"
#include "panel_wrapper_top_level.h"
#include "popup_window_wrapper.h"
#include "property_base.h"
#include "property_grid_manager_wrapper.h"
#include "property_grid_wrapper.h"
#include "radio_box_wrapper.h"
#include "radio_button_wrapper.h"
#include "ribbon_bar_wrapper.h"
#include "ribbon_button.h"
#include "ribbon_button_bar_wrapper.h"
#include "ribbon_gallery_item_wrapper.h"
#include "ribbon_gallery_wrapper.h"
#include "ribbon_page_wrapper.h"
#include "ribbon_panel_wrapper.h"
#include "ribbon_tool_bar_wrapper.h"
#include "ribbon_tool_separator.h"
#include "rich_text_ctrl_wrapper.h"
#include "scroll_bar_wrapper.h"
#include "scrolled_window_wrapper.h"
#include "search_ctrl_wrapper.h"
#include "simple_book_wrapper.h"
#include "slider_wrapper.h"
#include "spacer_wrapper.h"
#include "spin_button_wrapper.h"
#include "spin_ctrl_wrapper.h"
#include "splitter_window_page.h"
#include "splitter_window_wrapper.h"
#include "static_bitmap_wrapper.h"
#include "static_box_sizer_wrapper.h"
#include "static_line_wrapper.h"
#include "static_text_wrapper.h"
#include "status_bar_wrapper.h"
#include "std_button_wrapper.h"
#include "std_dialog_button_sizer_wrapper.h"
#include "string_property.h"
#include "styled_text_ctrl_wrapper.h"
#include "task_bar_icon_wrapper.h"
#include "text_ctrl_wrapper.h"
#include "timer_wrapper.h"
#include "toggle_button_wrapper.h"
#include "tool_bar_item_wrapper.h"
#include "tool_book_wrapper.h"
#include "toolbar_base_wrapper.h"
#include "tree_book_wrapper.h"
#include "tree_ctrl_wrapper.h"
#include "tree_list_ctrl_column_wrapper.h"
#include "tree_list_ctrl_wrapper.h"
#include "web_view_wrapper.h"
#include "wizard_page_wrapper.h"
#include "wizard_wrapper.h"
#include "wx_collapsible_pane_pane_wrapper.h"
#include "wxc_widget.h"
#include "wxcrafter_plugin.h"
#include "wxgui_bitmaploader.h"
#include <globals.h>
#include <wx/menu.h>

Allocator* Allocator::ms_instance = 0;
EventsDatabase Allocator::m_commonEvents;

//-------------------------------------------------------------
const FLAGS_t __ONE__ = 1;

FLAGS_t MT_TOP_LEVEL = __ONE__ << 1;               // Submenu
FLAGS_t MT_SIZERS = __ONE__ << 2;                  // Submenu
FLAGS_t MT_CONTROLS = __ONE__ << 3;                // Submenu
FLAGS_t MT_CONTAINERS = __ONE__ << 4;              // Submenu
FLAGS_t MT_NOTEBOOK_PAGES = __ONE__ << 5;          // Top
FLAGS_t MT_SPLITTERWIN_PAGES = __ONE__ << 6;       // Top
FLAGS_t MT_LIST_CTRL_COLUMNS = __ONE__ << 7;       // Top
FLAGS_t MT_PROJECT = __ONE__ << 8;                 // Top
FLAGS_t MT_COMMON_MENU = __ONE__ << 9;             // Top
FLAGS_t MT_EVENTS = __ONE__ << 10;                 // Top
FLAGS_t MT_PREVIEW_CODE = __ONE__ << 11;           // Top
FLAGS_t MT_EDIT = __ONE__ << 12;                   // Top
FLAGS_t MT_WIZARDPAGE = __ONE__ << 13;             // Top
FLAGS_t MT_MENUBAR = __ONE__ << 14;                // Top
FLAGS_t MT_MENU = __ONE__ << 15;                   // Top
FLAGS_t MT_CONTROL_EVENTS = __ONE__ << 16;         // Top
FLAGS_t MT_TOOLBAR = __ONE__ << 17;                // Top
FLAGS_t MT_DV_LIST_CTRL_COL = __ONE__ << 18;       // Top
FLAGS_t MT_COLLPANE = __ONE__ << 19;               // Top
FLAGS_t MT_INFOBAR = __ONE__ << 20;                // Top
FLAGS_t MT_IMGLIST = __ONE__ << 21;                // Top
FLAGS_t MT_TIMER = __ONE__ << 22;                  // Top
FLAGS_t MT_AUIMGR = __ONE__ << 23;                 // Top
FLAGS_t MT_PASTE = __ONE__ << 24;                  // Top
FLAGS_t MT_PG_PROPERTY = __ONE__ << 25;            // Top
FLAGS_t MT_PG_MANAGER = __ONE__ << 26;             // Top
FLAGS_t MT_RIBBON = __ONE__ << 27;                 // Top
FLAGS_t MT_RIBBON_PAGE = __ONE__ << 28;            // Top
FLAGS_t MT_RIBBON_BUTTON_BAR = __ONE__ << 29;      // Top
FLAGS_t MT_RIBBON_PANEL = __ONE__ << 30;           // Top
FLAGS_t MT_RIBBON_TOOL_BAR = __ONE__ << 31;        // Top
FLAGS_t MT_RIBBON_GALLERY = __ONE__ << 32;         // Top
FLAGS_t MT_SIZERS_TYPE = __ONE__ << 33;            // Top
FLAGS_t MT_GRID = __ONE__ << 34;                   // Top
FLAGS_t MT_TREE_LIST_CTRL_COLUMNS = __ONE__ << 35; // Top
FLAGS_t MT_TASKBARICON = __ONE__ << 36;            // Top
FLAGS_t MT_INSERT_INTO_SIZER = __ONE__ << 37;      // Top

// --------------------------------------------------------

#define MENU_ENTRY(id, label, bmpname)              \
    menuItem = new wxMenuItem(menu, id, label);     \
    menuItem->SetBitmap(bmpLoader.Bitmap(bmpname)); \
    menu->Append(menuItem);

#define MENU_SEPARATOR() menu->AppendSeparator();

Allocator::Allocator()
    : m_imageList(new wxImageList(16, 16, true))
{
    // Add the tree control images
    m_imageList->Add(m_bmpLoader.Bitmap(wxT("wxgui"))); // 0

    Register(new BoxSizerWrapper(), "wxboxsizer_v");
    Register(new FlexGridSizerWrapper(), "wxflexgridsizer");
    Register(new FrameWrapper(), "wxframe");
    Register(new ButtonWrapper(), "wxbutton");
    Register(new BitmapButtonWrapper(), "wxbitmapbutton");
    Register(new StaticTextWrapper(), "wxstatictext");
    Register(new TextCtrlWrapper(), "wxtextctrl");
    Register(new PanelWrapperTopLevel(), "wxpanel");
    Register(new PanelWrapper(), "wxpanel");
    Register(new StaticBitmapWrapper(), "wxbitmap");
    Register(new ComboxWrapper(), "wxcombobox");
    Register(new ChoiceWrapper(), "wxchoice");
    Register(new ListBoxWrapper(), "wxlistbox");
    Register(new ListCtrlWrapper(), "wxlistctrl");
    Register(new ListCtrlColumnWrapper(), "wxcolumn");
    Register(new CheckBoxWrapper(), "wxcheckbox");
    Register(new RadioBoxWrapper(), "wxradiobox");
    Register(new RadioButtonWrapper(), "wxradiobutton");
    Register(new StaticLineWrapper(), "wxstaticline");
    Register(new SliderWrapper(), "wxslider");
    Register(new GaugeWrapper(), "wxgauge");
    Register(new DialogWrapper(), "wxdialog");
    Register(new TreeCtrlWrapper(), "wxtreectrl");
    Register(new HtmlWindowWrapper(), "wxhtmlwindow");
    Register(new RichTextCtrlWrapper(), "wxrichtextctrl");
    Register(new CheckListBoxWrapper(), "wxchecklistbox");
    Register(new GridWrapper(), "wxgrid");
    Register(new ToggleButtonWrapper(), "wxtogglebutton");
    Register(new SearchCtrlWrapper(), "wxsearchctrl");
    Register(new ColourPickerWrapper(), "wxcolourpicker");
    Register(new FontPickerCtrlWrapper(), "wxfontpickerctrl");
    Register(new FilePickerCtrlWrapper(), "wxfilepickerctrl");
    Register(new DirPickerCtrlWrapper(), "wxdirpickerctrl");
    Register(new DatePickerCtrl(), "wxdatepickerctrl");
    Register(new CalendarCtrlWrapper(), "wxcalendarctrl");
    Register(new ScrollBarWrapper(), "wxscrollbar");
    Register(new SpinCtrlWrapper(), "wxspinctrl");
    Register(new SpinButtonWrapper(), "wxspinbutton");
    Register(new HyperLinkCtrlWrapper(), "wxhyperlink");
    Register(new GenericDirCtrlWrapper(), "wxgenericdirctrl");
    Register(new ScrolledWindowWrapper(), "wxscrolledwindow");
    Register(new NotebookWrapper(), "wxnotebook");
    Register(new NotebookPageWrapper(), "notebook-page");
    Register(new NotebookPageWrapper(), "notebook-page",
             ID_WXTREEBOOK_SUB_PAGE); // re-register the same object but with different ID
    Register(new ToolBookWrapper(), "wxtoolbook");
    Register(new ListBookWrapper(), "wxlistbook");
    Register(new ChoiceBookWrapper(), "wxchoicebook");
    Register(new TreeBookWrapper(), "wxtreebook");
    Register(new SplitterWindowWrapper(), "wxsplitterwindow");
    Register(new SplitterWindowPage(), "wxpanel");
    Register(new StaticBoxSizerWrapper(), "wxstaticboxsizer");
    Register(new WizardWrapper(), "wxwizard");
    Register(new WizardPageWrapper(), "wxwizardpage");
    Register(new GridSizerWrapper(), "wxgridsizer");
    Register(new GridBagSizerWrapper(), "wxgridbagsizer");
    Register(new SpacerWrapper(), "spacer");
    Register(new AuiNotebookWrapper(), "wxauinotebook");
    Register(new MenuBarWrapper(), "wxmenubar");
    Register(new MenuWrapper(), "wxmenu");
    Register(new SubMenuWrapper(), "wxsubmenu");
    Register(new MenuItemWrapper(), "wxmenuitem");
    Register(new ToolbarWrapper(), "wxtoolbar");
    Register(new ToolBarItemWrapper(), "wxmenuitem");
    Register(new ToolBarItemSpaceWrapper(), "spacer");
    Register(new AuiToolBarItemNonStretchSpaceWrapper(), "spacer");
    Register(new AuiToolBarItemSpaceWrapper(), "spacer");
    Register(new AuiToolBarLabelWrapper(), "wxstatictext");
    Register(new StatusBarWrapper(), "wxstatusbar");
    Register(new AuiToolbarWrapper(), "wxauitoolbar");
    Register(new CustomControlWrapper(), "custom");
    Register(new StyledTextCtrlWrapper(), "stc");
    Register(new StdDialogButtonSizerWrapper(), "stddlgbuttonsizer");
    Register(new StdButtonWrapper(), "wxbutton");
    Register(new DataViewListCtrlWrapper(), "wxdataviewlistctrl");
    Register(new DataViewListCtrlColumn(), "wxcolumn");
    Register(new DataViewTreeCtrlWrapper(), "wxdataviewtreectrl");
    Register(new DataViewTreeListCtrlWrapper(), "wxdataviewctrl");
    Register(new BannerWindowWrapper(), "wxbannerwindow");
    Register(new CommandLinkButtonWrapper(), "wxcommandlinkbutton");
    Register(new CollapsiblePaneWrapper(), "wxcollapsiblepane");
    Register(new wxCollapsiblePanePaneWrapper(), "wxpanel");
    Register(new InfoBarWrapper(), "wxinfobar");
    Register(new InfoBarButtonWrapper(), "wxbutton");
    Register(new WebViewWrapper(), "wxwebview");
    Register(new AuiManagerWrapper(), "wxauimgr");
    Register(new BitmapWrapepr(), "wxbitmap");
    Register(new ImageListWrapper(), "wximglist");
    Register(new TimerWrapper(), "wxtimer");
    Register(new PopupWindowWrapper(), "wxpopupwindow");
    Register(new PropertyGridManagerWrapper(), "wxpropertygridmanager");
    Register(new PropertyGridWrapper(), "wxpgproperty");
    Register(new PropertyGridWrapper(), "wxpgproperty", ID_WXPGPROPERTY_SUB);
    Register(new RibbonBarWrapper(), "wxribbonbar");
    Register(new RibbonPageWrapper(), "wxribbonpage");
    Register(new RibbonPanelWrapper(), "wxribbonpanel");
    Register(new RibbonButtonBarWrapper(), "wxribbonbuttonbar");
    Register(new RibbonButton(), "wxribbonbutton");
    Register(new RibbonButtonHybrid(), "wxribbonbutton_hybrid");
    Register(new RibbonButtonDropdown(), "wxribbonbutton_dropdown");
    Register(new RibbonButtonToggle(), "wxribbonbutton_toggle");
    Register(new RibbonToolBarWrapper(), "wxtoolbar");
    Register(new RibbonTool(), "wxribbontool");
    Register(new RibbonToolHybrid(), "wxribbontool_hybrid");
    Register(new RibbonToolDropdown(), "wxribbontool_dropdown");
    Register(new RibbonToolToggle(), "wxribbontool_toggle");
    Register(new RibbonGalleryWrapper(), "wxribbongallery");
    Register(new RibbonGalleryItemWrapper(), "wxribbongalleryitem");
    Register(new RibbonToolSeparator(), "wxstaticline");
    Register(new ToolBarItemSeparatorWrapper(), "wxstaticline_v");
    Register(new GLCanvasWrapper(), "wxglcanvas");
    Register(new GridColumnWrapper(), "grid-column");
    Register(new GridRowWrapper(), "grid-row");
    Register(new MediaCtrlWrapper(), "wxmediactrl-16");
    Register(new TreeListCtrlWrapper(), "wxtreelistctrl");
    Register(new TreeListCtrlColumnWrapper(), "grid-column");
    Register(new SimpleBookWrapper(), "wxsimplebook");
    Register(new TaskBarIconWrapper(), "wxtaskbaricon");
    Register(new BitmapToggleButtonWrapper(), "wxbitmaptogglebutton");
    Register(new AuiToolBarTopLevelWrapper(), "wxauitoolbar");
    Register(new AnimationCtrlWrapper(), "wxanimationctrl");
    Register(new BitmapComboxWrapper(), "wxbitmapcombobox");
    Register(new RearrangeListWrapper(), "wxrearrangelist");
    Register(new SimpleHtmlListBoxWrapper(), "wxhtmllistbox");
    Register(new ActivityrIndicatorWrapper(), "wxactivityindicator");
    Register(new TimePickerCtrlWrapper(), "wxtimepickerctrl");

    // ADD_NEW_CONTROL
    DoLinkAll();
}

Allocator::~Allocator() {}

wxMenu* Allocator::CreateControlsMenu() const
{
    wxCrafter::ResourceLoader bmpLoader;

    wxMenu* menu = new wxMenu;
    wxMenuItem* menuItem = NULL;

    MENU_ENTRY(ID_WXBUTTON, wxT("wxButton"), wxT("wxbutton"));
    MENU_ENTRY(ID_WXBITMAPBUTTON, wxT("wxBitmapButton"), wxT("wxbitmapbutton"));
    MENU_ENTRY(ID_WXTOGGLEBUTTON, wxT("wxToggleButton"), wxT("wxtogglebutton"));
    MENU_ENTRY(ID_WXBITMAPTOGGLEBUTTON, wxT("wxBitmapToggleButton"), wxT("wxbitmaptogglebutton"));
    MENU_ENTRY(ID_WXSPINCTRL, wxT("wxSpinCtrl"), wxT("wxspinctrl"));
    MENU_ENTRY(ID_WXSPINBUTTON, wxT("wxSpinButton"), wxT("wxspinbutton"));
    MENU_ENTRY(ID_WXHYPERLINK, wxT("wxHyperlinkCtrl"), wxT("wxhyperlink"));
    MENU_ENTRY(ID_WXCOMMANDLINKBUTTON, wxT("wxCommandLinkButton"), wxT("wxcommandlinkbutton"));

    MENU_SEPARATOR();

    MENU_ENTRY(ID_WXCHECKBOX, wxT("wxCheckBox"), wxT("wxcheckbox"));
    MENU_ENTRY(ID_WXRADIOBOX, wxT("wxRadioBox"), wxT("wxradiobox"));
    MENU_ENTRY(ID_WXRADIOBUTTON, wxT("wxRadioButton"), wxT("wxradiobutton"));
    MENU_SEPARATOR();

    MENU_ENTRY(ID_WXSTATICTEXT, wxT("wxStaticText"), wxT("wxstatictext"));
    MENU_ENTRY(ID_WXSTATICLINE, wxT("wxStaticLine"), wxT("wxstaticline"));
    MENU_ENTRY(ID_WXTEXTCTRL, wxT("wxTextCtrl"), wxT("wxtextctrl"));
    MENU_ENTRY(ID_WXRICHTEXT, wxT("wxRichTextCtrl"), wxT("wxrichtextctrl"));
    MENU_ENTRY(ID_WXSTC, wxT("wxStyledTextCtrl"), wxT("stc"));
    MENU_SEPARATOR();

    MENU_ENTRY(ID_WXSTATICBITMAP, wxT("wxStaticBitmap"), wxT("wxbitmap"));
    MENU_SEPARATOR();

    MENU_ENTRY(ID_WXCOMBOBOX, wxT("wxComboBox"), wxT("wxcombobox"));
    MENU_ENTRY(ID_WXCHOICE, wxT("wxChoice"), wxT("wxchoice"));
    MENU_ENTRY(ID_WXBITMAPCOMBOBOX, wxT("wxBitmapComboBox"), wxT("wxbitmapcombobox"));
    MENU_SEPARATOR();

    MENU_ENTRY(ID_WXLISTBOX, wxT("wxListBox"), wxT("wxlistbox"));
    MENU_ENTRY(ID_WXSIMPLEHTMLLISTBOX, wxT("wxSimpleHtmlListBox"), wxT("wxhtmllistbox"));
    MENU_ENTRY(ID_WXCHECKLISTBOX, wxT("wxCheckListBox"), wxT("wxchecklistbox"));
    MENU_ENTRY(ID_WXREARRANGELIST, wxT("wxRearrangeList"), wxT("wxrearrangelist"));
    MENU_ENTRY(ID_WXLISTCTRL, wxT("wxListCtrl"), wxT("wxlistctrl"));
    MENU_ENTRY(ID_WXTREELISTCTRL, wxT("wxTreeListCtrl"), wxT("wxtreelistctrl"));
    MENU_ENTRY(ID_WXTREECTRL, wxT("wxTreeCtrl"), wxT("wxtreectrl"));
    MENU_ENTRY(ID_WXGRID, wxT("wxGrid"), wxT("wxgrid"));
    MENU_ENTRY(ID_WXDATAVIEWLISTCTRL, wxT("wxDataViewListCtrl"), "wxdataviewlistctrl");
    MENU_ENTRY(ID_WXDATAVIEWTREECTRL, wxT("wxDataViewTreeCtrl"), "wxdataviewtreectrl");
    MENU_ENTRY(ID_WXDATAVIEWTREELISTCTRL, wxT("wxDataViewCtrl (with tree-list custom model)"), "wxdataviewctrl");
    MENU_SEPARATOR();

    MENU_ENTRY(ID_WXSLIDER, wxT("wxSlider"), wxT("wxslider"));
    MENU_ENTRY(ID_WXSCROLLBAR, wxT("wxScrollBar"), wxT("wxscrollbar"));
    MENU_ENTRY(ID_WXGAUGE, wxT("wxGauge"), wxT("wxgauge"));
    MENU_SEPARATOR();

    MENU_ENTRY(ID_WXHTMLWIN, wxT("wxHtmlWindow"), wxT("wxhtmlwindow"));
    MENU_ENTRY(ID_WXWEBVIEW, "wxWebView", "wxwebview");
    MENU_ENTRY(ID_WXBANNERWINDOW, wxT("wxBannerWindow"), wxT("wxbannerwindow"));
    MENU_ENTRY(ID_WXINFOBAR, "wxInfoBar", "wxinfobar");
    MENU_ENTRY(ID_WXMEDIACTRL, "wxMediaCtrl", "wxmediactrl-16");
    MENU_ENTRY(ID_WXANIMATIONCTRL, "wxAnimationCtrl", "wxanimationctrl");
    MENU_ENTRY(ID_WXACTIVITYINDICATOR, "wxActivityIndicator", "wxactivityindicator");
    MENU_ENTRY(ID_WXTIMEPICKERCTRL, "wxTimePickerCtrl", "wxtimepickerctrl");
    MENU_SEPARATOR();

    MENU_ENTRY(ID_WXSEARCHCTRL, wxT("wxSearchCtrl"), wxT("wxsearchctrl"));
    MENU_ENTRY(ID_WXCOLORPICKER, wxT("wxColourPickerCtrl"), wxT("wxcolourpicker"));
    MENU_ENTRY(ID_WXFONTPICKER, wxT("wxFontPickerCtrl"), wxT("wxfontpickerctrl"));
    MENU_ENTRY(ID_WXFILEPICKER, wxT("wxFilePickerCtrl"), wxT("wxfilepickerctrl"));
    MENU_ENTRY(ID_WXDIRPICKER, wxT("wxDirPickerCtrl"), wxT("wxdirpickerctrl"));
    MENU_ENTRY(ID_WXDATEPICKER, wxT("wxDatePickerCtrl"), wxT("wxdatepickerctrl"));
    MENU_ENTRY(ID_WXCALEDARCTRL, wxT("wxCalendarCtrl"), wxT("wxcalendarctrl"));
    MENU_ENTRY(ID_WXGENERICDIRCTRL, wxT("wxGenericDirCtrl"), wxT("wxgenericdirctrl"));
    // ADD_NEW_CONTROL
    return menu;
}

Allocator* Allocator::Instance()
{
    if(ms_instance == 0) { ms_instance = new Allocator(); }
    return ms_instance;
}

void Allocator::Release()
{
    if(ms_instance) { delete ms_instance; }
    ms_instance = 0;
}

wxcWidget* Allocator::Create(int id)
{
    Map_t::iterator iter = m_objs.find(id);
    if(iter == m_objs.end()) return NULL;

    return iter->second->Clone();
}

void Allocator::Register(wxcWidget* obj, const wxString& bmpname, int id)
{
    int objId = id;
    if(objId == -1) { objId = obj->GetType(); }
    m_objs[objId] = obj;
    m_imageIds[objId] = m_imageList->Add(m_bmpLoader.Bitmap(bmpname));
}

int Allocator::GetImageId(int controlId) const
{
    ImageMap_t::const_iterator iter = m_imageIds.find(controlId);
    if(iter == m_imageIds.end()) return -1;
    return iter->second;
}

wxcWidget* Allocator::CreateWrapperFromJSON(const JSONElement& json)
{
    wxcWidget* wrapper = NULL;

    int type = json.namedObject(wxT("m_type")).toInt();
    wrapper = Create(type);
    if(!wrapper) return NULL;

    wrapper->UnSerialize(json);
    return wrapper;
}

wxMenu* Allocator::CreateSizerTypeMenu() const
{
    wxMenu* menu = new wxMenu;
    wxMenuItem* menuItem = NULL;
    wxCrafter::ResourceLoader bmpLoader;
    MENU_ENTRY(ID_CHANGE_SIZER_BOXSIZER, "wxBoxSizer", "wxboxsizer_v");
    MENU_ENTRY(ID_CHANGE_SIZER_STATICBOXSIZER, "wxStaticBoxSizer", "wxstaticboxsizer");
    MENU_ENTRY(ID_CHANGE_SIZER_FLEXGRIDSIZER, "wxFlexGridSizer", "wxflexgridsizer");
    MENU_ENTRY(ID_CHANGE_SIZER_GRIDSIZER, "wxGridSizer", "wxgridsizer");
    MENU_ENTRY(ID_CHANGE_SIZER_GRIDBAGSIZER, "wxGridBagSizer", "wxgridbagsizer");
    return menu;
}

wxMenu* Allocator::CreateInsertIntoSizerMenu() const
{
    wxMenu* menu = new wxMenu;
    wxMenuItem* menuItem = NULL;
    wxCrafter::ResourceLoader bmpLoader;
    MENU_ENTRY(ID_INSERT_INTO_SIZER_BOXSIZER, "wxBoxSizer", "wxboxsizer_v");
    MENU_ENTRY(ID_INSERT_INTO_SIZER_STATICBOXSIZER, "wxStaticBoxSizer", "wxstaticboxsizer");
    MENU_ENTRY(ID_INSERT_INTO_SIZER_FLEXGRIDSIZER, "wxFlexGridSizer", "wxflexgridsizer");
    MENU_ENTRY(ID_INSERT_INTO_SIZER_GRIDSIZER, "wxGridSizer", "wxgridsizer");
    MENU_ENTRY(ID_INSERT_INTO_SIZER_GRIDBAGSIZER, "wxGridBagSizer", "wxgridbagsizer");
    return menu;
}

wxMenu* Allocator::CreateSizersMenu() const
{
    wxMenu* menu = new wxMenu;
    wxMenuItem* menuItem = NULL;
    wxCrafter::ResourceLoader bmpLoader;
    MENU_ENTRY(ID_WXBOXSIZER, wxT("wxBoxSizer"), wxT("wxboxsizer_v"));
    MENU_ENTRY(ID_WXSTATICBOXSIZER, wxT("wxStaticBoxSizer"), wxT("wxstaticboxsizer"));
    MENU_ENTRY(ID_WXFLEXGRIDSIZER, wxT("wxFlexGridSizer"), wxT("wxflexgridsizer"));
    MENU_ENTRY(ID_WXGRIDSIZER, wxT("wxGridSizer"), wxT("wxgridsizer"));
    MENU_ENTRY(ID_WXGRIDBAGSIZER, wxT("wxGridBagSizer"), wxT("wxgridbagsizer"));
    MENU_SEPARATOR();
    MENU_ENTRY(ID_WXSTDDLGBUTTONSIZER, wxT("wxStdDialogButtonSizer"), wxT("stddlgbuttonsizer"));
    MENU_ENTRY(ID_WXSTDBUTTON, wxT("Standard wxButton"), wxT("wxbutton"));
    MENU_SEPARATOR();
    MENU_ENTRY(ID_WXSPACER, wxT("Spacer"), wxT("spacer"));
    // ADD_NEW_CONTROL
    return menu;
}

wxMenu* Allocator::CreateTopLevelMenu() const
{
    wxMenu* menu = new wxMenu;
    wxMenuItem* menuItem = NULL;
    wxCrafter::ResourceLoader bmpLoader;

    MENU_ENTRY(ID_WXFRAME, wxT("New wxFrame"), wxT("wxframe"));
    MENU_ENTRY(ID_WXDIALOG, wxT("New wxDialog"), wxT("wxdialog"));
    MENU_ENTRY(ID_WXWIZARD, wxT("New wxWizard"), wxT("wxwizard"));
    MENU_ENTRY(ID_WXPANEL_TOPLEVEL, wxT("New wxPanel"), wxT("wxpanel"));
    MENU_ENTRY(ID_WXAUITOOLBARTOPLEVEL, wxT("wxAuiToolBar"), wxT("wxauitoolbar"));
    MENU_SEPARATOR();
    MENU_ENTRY(ID_WXIMAGELIST, wxT("New wxImageList"), wxT("wximglist"));
    MENU_ENTRY(ID_WXPOPUPWINDOW, wxT("New wxPopupWindow"), wxT("wxpopupwindow"));

    // ADD_NEW_CONTROL
    return menu;
}

wxMenu* Allocator::CreateContainersMenu() const
{
    wxMenu* menu = new wxMenu;
    wxCrafter::ResourceLoader bmpLoader;
    wxMenuItem* menuItem = NULL;

    MENU_ENTRY(ID_WXPANEL, wxT("wxPanel"), wxT("wxpanel"));
    MENU_ENTRY(ID_WXSCROLLEDWIN, wxT("wxScrolledWindow"), wxT("wxscrolledwindow"));
    MENU_ENTRY(ID_WXSPLITTERWINDOW, wxT("wxSplitterWindow"), wxT("wxsplitterwindow"));
    MENU_ENTRY(ID_WXWIZARDPAGE, wxT("wxWizardSimplePage"), wxT("wxwizardpage"));
    MENU_SEPARATOR();
    MENU_ENTRY(ID_WXGLCANVAS, wxT("wxGLCanvas"), wxT("wxglcanvas"));
    MENU_SEPARATOR();
    MENU_ENTRY(ID_WXCOLLAPSIBLEPANE, "wxCollapsiblePane", "wxcollapsiblepane");
    MENU_ENTRY(ID_WXCOLLAPSIBLEPANE_PANE, "wxCollapsiblePane Window", "wxpanel");

    MENU_SEPARATOR();
    MENU_ENTRY(ID_WXNOTEBOOK, wxT("wxNotebook"), wxT("wxnotebook"));
    MENU_ENTRY(ID_WXTOOLBOOK, wxT("wxToolbook"), wxT("wxtoolbook"));
    MENU_ENTRY(ID_WXLISTBOOK, wxT("wxListbook"), wxT("wxlistbook"));
    MENU_ENTRY(ID_WXCHOICEBOOK, wxT("wxChoicebook"), wxT("wxchoicebook"));
    MENU_ENTRY(ID_WXTREEBOOK, wxT("wxTreebook"), wxT("wxtreebook"));
    MENU_ENTRY(ID_WXAUINOTEBOOK, wxT("wxAuiNotebook"), wxT("wxauinotebook"));
    MENU_ENTRY(ID_WXSIMPLEBOOK, wxT("wxSimplebook"), wxT("wxsimplebook"));
    // ADD_NEW_CONTROL

    return menu;
}

void Allocator::Initialize() { m_commonEvents.FillCommonEvents(); }

void Allocator::PrepareMenu(wxMenu& menu, wxcWidget* item)
{
    // ADD_NEW_CONTROL
    bool isChildOfTreeBook = false;
    if(item) {
        wxcWidget* parent = item->GetParent();
        while(parent) {
            if(parent->GetType() == ID_WXTREEBOOK) {
                isChildOfTreeBook = true;
                break;
            }
            parent = parent->GetParent();
        }
    }

    FLAGS_t flags = DoGetValidMenus(item);
    if(flags & MT_PG_MANAGER) { menu.Append(ID_WXPGPROPERTY, _("Add wxPGProperty")); }
    if(flags & MT_PG_PROPERTY) { menu.Append(ID_WXPGPROPERTY_SUB, _("Add wxPGProperty Child")); }
    if(flags & MT_INFOBAR) { menu.Append(ID_WXINFOBARBUTTON, _("Add Button")); }

    if(flags & MT_IMGLIST) { menu.Append(ID_WXBITMAP, _("Add Bitmap")); }

    if(flags & MT_GRID) {
        menu.Append(ID_WXGRIDCOL, _("Add Column"));
        menu.Append(ID_WXGRIDROW, _("Add Row"));
    }

    if(flags & MT_COLLPANE) { menu.Append(ID_WXCOLLAPSIBLEPANE_PANE, _("Add Pane")); }

    if(flags & MT_TOOLBAR) {
        menu.Append(ID_WXTOOLBARITEM, _("Add Tool"));
        menu.Append(wxID_ANY, _("Add Control"), CreateControlsMenu());
        if(item->GetType() == ID_WXAUITOOLBAR || item->GetType() == ID_WXAUITOOLBARTOPLEVEL) {
            menu.Append(ID_WXAUITOOLBARLABEL, _("Add Label"));
            menu.Append(ID_WXAUITOOLBARITEM_SPACE, _("Add Spacer"));
            menu.Append(ID_WXAUITOOLBARITEM_STRETCHSPACE, _("Add Stretch Spacer"));
        } else {
            menu.Append(ID_WXTOOLBARITEM_STRETCHSPACE, _("Add Stretch Spacer"));
        }

        menu.Append(ID_WXTOOLBARITEM_SEPARATOR, _("Add Separator"));
    }

    if(flags & MT_MENUBAR) { menu.Append(ID_WXMENU, _("Add wxMenu")); }

    if(flags & MT_MENU) {
        menu.Append(ID_WXMENUITEM, _("Add Menu Item"));
        menu.Append(ID_WXSUBMENU, _("Add Sub Menu"));
    }

    if(flags & MT_TOP_LEVEL) {
#if STANDALONE_BUILD
        menu.Append(wxID_NEW, _("New Project..."));
#endif
        menu.Append(ID_FORM_TYPE, _("Add Form"), CreateTopLevelMenu());
    }

    if(flags & MT_WIZARDPAGE) { menu.Append(ID_WXWIZARDPAGE, _("Add Page")); }

    if(flags & MT_SIZERS) { menu.Append(wxID_ANY, _("Add Sizer"), CreateSizersMenu()); }

    if(flags & MT_INSERT_INTO_SIZER) { menu.Append(wxID_ANY, _("Insert into new Sizer"), CreateInsertIntoSizerMenu()); }

    if(flags & MT_SIZERS_TYPE) {
        menu.Append(wxID_ANY, _("Change wxSizer Type"), CreateSizerTypeMenu());
        menu.AppendSeparator();
    }

    if(flags & MT_AUIMGR) { menu.Append(ID_WXAUIMANAGER, _("Add wxAuiManager")); }

    if(flags & MT_CONTAINERS) { menu.Append(wxID_ANY, _("Add Container"), CreateContainersMenu()); }

    if(flags & MT_CONTROLS) { menu.Append(wxID_ANY, _("Add Control"), CreateControlsMenu()); }

    if(flags & MT_RIBBON) { menu.Append(ID_WXRIBBONPAGE, _("Add wxRibbonPage")); }

    if(flags & MT_RIBBON_PAGE) { menu.Append(ID_WXRIBBONPANEL, _("Add wxRibbonPanel")); }

    if(flags & MT_RIBBON_PANEL) {
        menu.Append(ID_WXRIBBONBUTTONBAR, _("Add wxRibbonButtonBar"));
        menu.Append(ID_WXRIBBONTOOLBAR, _("Add wxRibbonToolBar"));
        menu.Append(ID_WXRIBBONGALLERY, _("Add wxRibbonGallery"));
    }

    if(flags & MT_RIBBON_BUTTON_BAR) {
        menu.Append(ID_WXRIBBONBUTTON, _("Add Button"));
        menu.Append(ID_WXRIBBONDROPDOWNBUTTON, _("Add Dropdown Button"));
        menu.Append(ID_WXRIBBONHYBRIDBUTTON, _("Add Hybrid Button"));
        menu.Append(ID_WXRIBBONTOGGLEBUTTON, _("Add Toggle Button"));
    }

    if(flags & MT_RIBBON_TOOL_BAR) {
        menu.Append(ID_WXRIBBONTOOL, _("Add Tool"));
        menu.Append(ID_WXRIBBONDROPDOWNTOOL, _("Add Dropdown Tool"));
        menu.Append(ID_WXRIBBONHYBRIDTOOL, _("Add Hybrid Tool"));
        menu.Append(ID_WXRIBBONTOGGLETOOL, _("Add Toggle Tool"));
    }
    if(flags & MT_RIBBON_GALLERY) { menu.Append(ID_WXRIBBONGALLERYITME, _("Add Gallery Item")); }

    if(flags & MT_NOTEBOOK_PAGES) {
        menu.Append(ID_WXPANEL_NOTEBOOK_PAGE, wxT("Add Notebook Page"));
        if(isChildOfTreeBook) { menu.Append(ID_WXTREEBOOK_SUB_PAGE, wxT("Add Sub Page")); }
    }

    if(flags & MT_SPLITTERWIN_PAGES) { menu.Append(ID_WXSPLITTERWINDOW_PAGE, _("Add Panel"), _("Add Panel")); }

    if(flags & MT_DV_LIST_CTRL_COL) { menu.Append(ID_WXDATAVIEWCOL, wxT("Add Column")); }

    if(flags & MT_LIST_CTRL_COLUMNS) { menu.Append(ID_WXLISTCTRL_COL, wxT("Add List Column")); }

    if(flags & MT_TREE_LIST_CTRL_COLUMNS) { menu.Append(ID_WXTREELISTCTRLCOL, wxT("Add Column")); }

    if(flags & MT_EVENTS) { DoAddEventsMenu(menu); }

    if(flags & MT_CONTROL_EVENTS) { DoAddControlEventsMenu(menu); }

    if(flags & MT_PROJECT) { DoAddProjectMenu(menu); }

    if(flags & MT_PREVIEW_CODE) {
        if(menu.GetMenuItemCount() != 0) menu.AppendSeparator();
        menu.Append(ID_GENERATE_CODE, _("Generate code..."));
    }

    if(flags & MT_TIMER) {
        if(menu.GetMenuItemCount() != 0) menu.AppendSeparator();
        menu.Append(ID_WXTIMER, _("Add wxTimer"));
    }

    if(flags & MT_TASKBARICON) {
        if(menu.GetMenuItemCount() != 0) menu.AppendSeparator();
        menu.Append(ID_WXTASKBARICON, _("Add wxTaskBarIcon"));
    }

    if(flags & MT_EDIT) { DoAddEditMenu(menu); }

    if(flags & MT_PASTE) { menu.Append(ID_PASTE, _("Paste")); }
    // Last, add the common menu
    if(flags & MT_COMMON_MENU) { DoAddCommonMenu(menu); }

    if(item && item->GetType() == ID_WXSTDDLGBUTTONSIZER) {
        menu.PrependSeparator();
        menu.Prepend(ID_WXSTDBUTTON, _("Add Button"));
    }
}

FLAGS_t Allocator::DoGetValidMenus(wxcWidget* item) const
{
    // ADD_NEW_CONTROL
    FLAGS_t menuflags = 0;
    if(!item) {
        menuflags |= MT_PROJECT;
        menuflags |= MT_TOP_LEVEL;
        menuflags |= MT_PASTE;

    } else {

        switch(item->GetType()) {
        case ID_WXRIBBONGALLERYITME:
            menuflags |= MT_COMMON_MENU;
            menuflags |= MT_EDIT;
            break;

        case ID_WXRIBBONGALLERY:
            menuflags |= MT_RIBBON_GALLERY;
            menuflags |= MT_EVENTS;
            menuflags |= MT_COMMON_MENU;
            menuflags |= MT_EDIT;
            break;

        case ID_WXRIBBONBUTTONBAR:
            menuflags |= MT_RIBBON_BUTTON_BAR;
            menuflags |= MT_EVENTS;
            menuflags |= MT_COMMON_MENU;
            menuflags |= MT_EDIT;
            break;

        case ID_WXRIBBONTOOLBAR:
            menuflags |= MT_RIBBON_TOOL_BAR;
            menuflags |= MT_EVENTS;
            menuflags |= MT_COMMON_MENU;
            menuflags |= MT_EDIT;
            break;

        case ID_WXRIBBONPANEL:
            menuflags |= MT_SIZERS;
            menuflags |= MT_RIBBON_PANEL;
            menuflags |= MT_EVENTS;
            menuflags |= MT_COMMON_MENU;
            menuflags |= MT_EDIT;
            break;

        case ID_WXRIBBONPAGE:
            menuflags |= MT_RIBBON_PAGE;
            menuflags |= MT_EVENTS;
            menuflags |= MT_COMMON_MENU;
            menuflags |= MT_EDIT;
            break;

        case ID_WXRIBBONBAR:
            menuflags |= MT_RIBBON;
            menuflags |= MT_EVENTS;
            menuflags |= MT_COMMON_MENU;
            menuflags |= MT_EDIT;
            break;
        case ID_WXINFOBAR:
            menuflags |= MT_INFOBAR;
            menuflags |= MT_EVENTS;
            menuflags |= MT_COMMON_MENU;
            menuflags |= MT_EDIT;
            break;
        case ID_WXIMAGELIST:
            menuflags |= MT_IMGLIST;
            menuflags |= MT_COMMON_MENU;
            menuflags |= MT_EDIT;
            break;
        case ID_WXCOLLAPSIBLEPANE:
            menuflags |= MT_COLLPANE;
            menuflags |= MT_EVENTS;
            menuflags |= MT_COMMON_MENU;
            menuflags |= MT_EDIT;
            menuflags |= MT_INSERT_INTO_SIZER;
            break;
        case ID_WXCOLLAPSIBLEPANE_PANE:
            menuflags |= MT_SIZERS;
            menuflags |= MT_EVENTS;
            menuflags |= MT_COMMON_MENU;
            menuflags |= MT_EDIT;
            break;
        case ID_WXAUITOOLBARTOPLEVEL:
        case ID_WXAUITOOLBAR:
        case ID_WXTOOLBAR:
            menuflags |= MT_EVENTS;
            menuflags |= MT_COMMON_MENU;
            menuflags |= MT_EDIT;
            menuflags |= MT_TOOLBAR;
            break;

        case ID_WXMENUITEM:
        case ID_WXTOOLBARITEM:
            menuflags |= MT_CONTROL_EVENTS;
            menuflags |= MT_COMMON_MENU;
            menuflags |= MT_EDIT;
            break;

        case ID_WXTOOLBARITEM_SEPARATOR:
        case ID_WXTOOLBARITEM_STRETCHSPACE:
        case ID_WXAUITOOLBARLABEL:
        case ID_WXAUITOOLBARITEM_SPACE:
        case ID_WXAUITOOLBARITEM_STRETCHSPACE:
            menuflags |= MT_COMMON_MENU;
            menuflags |= MT_EDIT;
            break;

        case ID_WXMENUBAR:
            menuflags |= MT_COMMON_MENU;
            menuflags |= MT_MENUBAR;
            menuflags |= MT_EDIT;
            break;

        case ID_WXMENU:
        case ID_WXSUBMENU:
            menuflags |= MT_COMMON_MENU;
            menuflags |= MT_MENU;
            menuflags |= MT_EDIT;
            break;

        case ID_WXWIZARD:
            menuflags |= MT_WIZARDPAGE;
            menuflags |= MT_EVENTS;
            menuflags |= MT_COMMON_MENU;
            menuflags |= MT_PREVIEW_CODE;
            menuflags |= MT_EDIT;
            break;

        case ID_WXFRAME:
        case ID_WXDIALOG:
            menuflags |= MT_TASKBARICON;
        // fall through
        case ID_WXPANEL_TOPLEVEL:
        case ID_WXPOPUPWINDOW:
            menuflags |= MT_TIMER;
            if(!item->HasMainSizer() && !item->IsAuiManaged()) {
                menuflags |= MT_SIZERS;
                menuflags |= MT_AUIMGR;
            }
            menuflags |= MT_EVENTS;
            menuflags |= MT_COMMON_MENU;
            menuflags |= MT_EDIT;
            menuflags |= MT_PREVIEW_CODE;
            break;

        case ID_WXSPACER:
            menuflags |= MT_COMMON_MENU;
            menuflags |= MT_EDIT;
            menuflags |= MT_INSERT_INTO_SIZER;
            break;

        case ID_WXBOXSIZER:
        case ID_WXFLEXGRIDSIZER:
        case ID_WXSTATICBOXSIZER:
        case ID_WXGRIDSIZER:
        case ID_WXGRIDBAGSIZER:
            menuflags |= MT_SIZERS_TYPE;
            menuflags |= MT_SIZERS;
            menuflags |= MT_CONTROLS;
            menuflags |= MT_CONTAINERS;
            menuflags |= MT_COMMON_MENU;
            menuflags |= MT_EDIT;
            menuflags |= MT_INSERT_INTO_SIZER;
            break;

        case ID_WXPROPERTYGRIDMANAGER:
            menuflags |= MT_EVENTS;
            menuflags |= MT_PG_MANAGER;
            menuflags |= MT_COMMON_MENU;
            menuflags |= MT_EDIT;
            menuflags |= MT_INSERT_INTO_SIZER;
            break;

        case ID_WXPGPROPERTY:
            menuflags |= MT_PG_PROPERTY;
            menuflags |= MT_COMMON_MENU;
            menuflags |= MT_EDIT;
            break;

        case ID_WXTREELISTCTRL:
            menuflags |= MT_EVENTS;
            menuflags |= MT_TREE_LIST_CTRL_COLUMNS;
            menuflags |= MT_COMMON_MENU;
            menuflags |= MT_EDIT;
            menuflags |= MT_INSERT_INTO_SIZER;
            break;

        case ID_WXLISTCTRL:
            menuflags |= MT_EVENTS;
            menuflags |= MT_LIST_CTRL_COLUMNS;
            menuflags |= MT_COMMON_MENU;
            menuflags |= MT_EDIT;
            menuflags |= MT_INSERT_INTO_SIZER;
            break;

        case ID_WXDATAVIEWLISTCTRL:
        case ID_WXDATAVIEWTREELISTCTRL:
            menuflags |= MT_EVENTS;
            menuflags |= MT_DV_LIST_CTRL_COL;
            menuflags |= MT_COMMON_MENU;
            menuflags |= MT_EDIT;
            menuflags |= MT_INSERT_INTO_SIZER;
            break;

        case ID_WXNOTEBOOK:
        case ID_WXLISTBOOK:
        case ID_WXCHOICEBOOK:
        case ID_WXTOOLBOOK:
        case ID_WXTREEBOOK:
        case ID_WXAUINOTEBOOK:
        case ID_WXSIMPLEBOOK:
            menuflags |= MT_NOTEBOOK_PAGES;
            menuflags |= MT_EVENTS;
            menuflags |= MT_COMMON_MENU;
            menuflags |= MT_EDIT;
            menuflags |= MT_INSERT_INTO_SIZER;
            break;

        case ID_WXSPLITTERWINDOW:
            menuflags |= MT_EVENTS;
            menuflags |= MT_SPLITTERWIN_PAGES;
            menuflags |= MT_COMMON_MENU;
            menuflags |= MT_EDIT;
            menuflags |= MT_INSERT_INTO_SIZER;
            break;

        case ID_WXPANEL:
        case ID_WXSCROLLEDWIN:
            menuflags |= MT_INSERT_INTO_SIZER;
        // fall through
        case ID_WXPANEL_NOTEBOOK_PAGE:
        case ID_WXSPLITTERWINDOW_PAGE:
        case ID_WXWIZARDPAGE:
            menuflags |= MT_SIZERS;
            menuflags |= MT_EVENTS;
            menuflags |= MT_COMMON_MENU;
            menuflags |= MT_EDIT;
            {
                // wxTreebook pages are allowed to have notebook pages as their direct children
                NotebookPageWrapper* page = dynamic_cast<NotebookPageWrapper*>(item);
                if(page && page->IsTreebookPage()) { menuflags |= MT_NOTEBOOK_PAGES; }
            }
            break;
        case ID_WXRIBBONTOOLSEPARATOR:
            menuflags |= MT_COMMON_MENU;
            menuflags |= MT_EDIT;
            break;

        case ID_WXGRIDCOL:
        case ID_WXGRIDROW:
            menuflags |= MT_COMMON_MENU;
            menuflags |= MT_EDIT;
            break;

        case ID_WXGRID:
            menuflags |= MT_GRID;
            menuflags |= MT_EVENTS;
            menuflags |= MT_COMMON_MENU;
            menuflags |= MT_EDIT;
            menuflags |= MT_INSERT_INTO_SIZER;
            break;

        default:
            menuflags |= MT_EVENTS;
            menuflags |= MT_COMMON_MENU;
            menuflags |= MT_EDIT;
            menuflags |= MT_INSERT_INTO_SIZER;
            break;
        }
    }
    return menuflags;
}

void Allocator::DoAddCommonMenu(wxMenu& menu) const
{
    if(menu.GetMenuItemCount() != 0) { menu.AppendSeparator(); }
    menu.Append(ID_MOVE_NODE_UP, wxT("Move Up"));
    menu.Append(ID_MOVE_NODE_DOWN, wxT("Move Down"));
    menu.Append(ID_MOVE_NODE_INTO_SIZER, wxT("Move Left into Higher Sizer"));
    menu.Append(ID_MOVE_NODE_INTO_SIBLING, wxT("Move Right into Sibling Sizer"));
    menu.AppendSeparator();
    menu.Append(ID_DELETE_NODE, wxT("Delete"));
}

void Allocator::DoAddProjectMenu(wxMenu& menu) const
{
    if(menu.GetMenuItemCount() != 0) { menu.AppendSeparator(); }
    menu.Append(ID_SAVE_WXGUI_PROJECT, wxT("Save"));
}

void Allocator::DoAddControlEventsMenu(wxMenu& menu) const
{
    // the events pane is now always visible
    wxUnusedVar(menu);
}

void Allocator::DoAddEventsMenu(wxMenu& menu) const { DoAddControlEventsMenu(menu); }

bool Allocator::CanPaste(wxcWidget* source, wxcWidget* target) const
{
    CHECK_POINTER_RET_FALSE(source);
    CHECK_POINTER_RET_FALSE(target);
    int insertType = Allocator::Instance()->GetInsertionType(source->GetType(), target->GetType(), false);

    // we dont allow moving around top level items
    if(source->IsTopWindow() && target->IsTopWindow())
        // allow copying top level windows
        return true;

    if(insertType == Allocator::INSERT_CHILD) {
        return true;

    } else if(insertType == Allocator::INSERT_MAIN_SIZER) {
        if(target->HasMainSizer())
            return false;
        else
            return true;
    } else if(insertType == Allocator::INSERT_SIBLING) {
        // check that the parent can accept it as a child
        CHECK_POINTER_RET_FALSE(target->GetParent());
        int parentInsertType =
            Allocator::Instance()->GetInsertionType(source->GetType(), target->GetParent()->GetType(), false);
        return parentInsertType == Allocator::INSERT_CHILD;

    } else {
        return false;
    }
}

void Allocator::DoAddEditMenu(wxMenu& menu) const
{
    if(menu.GetMenuItemCount() != 0) menu.AppendSeparator();
    menu.Append(ID_RENAME, _("Rename..."));
    menu.AppendSeparator();
    menu.Append(ID_COPY, _("Copy"));
    menu.Append(ID_CUT, _("Cut"));
    menu.Append(ID_PASTE, _("Paste"));
    menu.Append(ID_DUPLICATE, _("Duplicate"));
}

void Allocator::DoLink(int selected, int aboutToBeInsert, int relation)
{
    InsertMap_t::iterator iter = m_relations.find(selected);
    if(iter == m_relations.end()) { m_relations.insert(std::make_pair(selected, std::map<int, int>())); }
    std::map<int, int>& m = m_relations[selected];
    m.insert(std::make_pair(aboutToBeInsert, relation));
}

int Allocator::GetInsertionType(int controlId, int targetControlId, bool allowPrompt, wxcWidget* selectedWidget) const
{
    int aboutToBeInsert = wxcWidget::GetWidgetType(controlId);
    int selected = wxcWidget::GetWidgetType(targetControlId);

    bool selectionIsDirectChildOfAUI = false;
    if(selectedWidget && selectedWidget->GetParent() && selectedWidget->GetParent()->GetType() == ID_WXAUIMANAGER) {
        // the selected widget is a direct child of wxAuiManager
        selectionIsDirectChildOfAUI = true;
    }

    InsertMap_t::const_iterator iter = m_relations.find(selected);
    if(iter == m_relations.end()) return INSERT_NONE;

    // Get the relations allowed with 'selected'
    const std::map<int, int>& m = m_relations.find(selected)->second;
    std::map<int, int>::const_iterator itRelation = m.find(aboutToBeInsert);
    if(itRelation == m.end()) return INSERT_NONE;

    if(selectionIsDirectChildOfAUI && itRelation->second == INSERT_SIBLING) {
        // allow sibling only if wxAuiManager allows to accept it as child
        const std::map<int, int>& m = m_relations.find(TYPE_AUI_MGR)->second;
        std::map<int, int>::const_iterator it = m.find(aboutToBeInsert);
        if(it == m.end()) {
            // not allowed
            return INSERT_NONE;
        }
    }
    int type = itRelation->second;
    if(type == INSERT_PROMPT_CHILD_OR_SIBLING && allowPrompt) {
        // there is an ambiguity to resolve here - prompt the user
        int res =
            ::PromptForYesNoDialogWithCheckbox(_("This item can be placed either as a sibling or as a child of the "
                                                 "target widget\nWhere should wxCrafter position this widget?"),
                                               "wxCrafterInsertionType", _("As a Sibling"), _("As a Child"));
        if(res == wxID_YES) {
            return INSERT_SIBLING;

        } else {
            return INSERT_CHILD;
        }

    } else if(type == INSERT_PROMPT_CHILD_OR_SIBLING) {
        // prefer sibling over child
        return INSERT_CHILD;

    } else {
        return type;
    }
}

void Allocator::DoLinkAll()
{
    // ADD_NEW_CONTROL

    // TYPE_GRID_CHILD
    DoLink(TYPE_GRID_CHILD, TYPE_GRID_CHILD, INSERT_SIBLING);

    // TYPE_GRID
    DoLink(TYPE_GRID, TYPE_GRID, INSERT_SIBLING);
    DoLink(TYPE_GRID, TYPE_GRID_CHILD, INSERT_CHILD);
    DoLink(TYPE_GRID, TYPE_CONTROL, INSERT_SIBLING);
    DoLink(TYPE_GRID, TYPE_PG_MGR, INSERT_SIBLING);
    DoLink(TYPE_GRID, TYPE_SIZER, INSERT_SIBLING);
    DoLink(TYPE_GRID, TYPE_LIST_CTRL, INSERT_SIBLING);
    DoLink(TYPE_GRID, TYPE_TREE_LIST_CTRL, INSERT_SIBLING);
    DoLink(TYPE_GRID, TYPE_CONTAINER, INSERT_SIBLING);
    DoLink(TYPE_GRID, TYPE_BOOK, INSERT_SIBLING);
    DoLink(TYPE_GRID, TYPE_SPLITTER_WINDOW, INSERT_SIBLING);
    DoLink(TYPE_GRID, TYPE_SPACER, INSERT_SIBLING);
    DoLink(TYPE_GRID, TYPE_TOOLBAR, INSERT_SIBLING);
    DoLink(TYPE_GRID, TYPE_AUITOOLBAR, INSERT_SIBLING);
    DoLink(TYPE_GRID, TYPE_STD_BUTTON_SIZER, INSERT_SIBLING);
    DoLink(TYPE_GRID, TYPE_DV_WITH_COLUMNS, INSERT_SIBLING);
    DoLink(TYPE_GRID, TYPE_COLOLAPSIBLEPANE, INSERT_SIBLING);
    DoLink(TYPE_GRID, TYPE_INFO_BAR, INSERT_SIBLING);
    DoLink(TYPE_GRID, TYPE_RIBBON_BAR, INSERT_SIBLING);

    // TYPE_RIBBON_GALLERY_ITEM
    DoLink(TYPE_RIBBON_GALLERY_ITEM, TYPE_RIBBON_GALLERY_ITEM, INSERT_SIBLING);

    // TYPE_RIBBON_GALLERY
    DoLink(TYPE_RIBBON_GALLERY, TYPE_RIBBON_GALLERY, INSERT_SIBLING);
    DoLink(TYPE_RIBBON_GALLERY, TYPE_RIBBON_GALLERY_ITEM, INSERT_CHILD);

    // TYPE_RIBBON_BUTTON
    DoLink(TYPE_RIBBON_BUTTON, TYPE_RIBBON_BUTTON, INSERT_SIBLING);

    // TYPE_RIBBON_TOOL
    DoLink(TYPE_RIBBON_TOOL, TYPE_RIBBON_TOOL, INSERT_SIBLING);

    // TYPE_RIBBON_BUTTONBAR
    DoLink(TYPE_RIBBON_BUTTONBAR, TYPE_RIBBON_BUTTONBAR, INSERT_SIBLING);
    DoLink(TYPE_RIBBON_BUTTONBAR, TYPE_RIBBON_BUTTON, INSERT_CHILD);

    // TYPE_RIBBON_TOOLBAR
    DoLink(TYPE_RIBBON_TOOLBAR, TYPE_RIBBON_TOOLBAR, INSERT_SIBLING);
    DoLink(TYPE_RIBBON_TOOLBAR, TYPE_RIBBON_TOOL, INSERT_CHILD);

    // TYPE_RIBBON_PAGE
    DoLink(TYPE_RIBBON_PAGE, TYPE_RIBBON_PAGE, INSERT_SIBLING);
    DoLink(TYPE_RIBBON_PAGE, TYPE_RIBBON_PANEL, INSERT_CHILD);

    // TYPE_RIBBON_PANEL
    DoLink(TYPE_RIBBON_PANEL, TYPE_RIBBON_PANEL, INSERT_SIBLING);
    DoLink(TYPE_RIBBON_PANEL, TYPE_RIBBON_BUTTONBAR, INSERT_CHILD);
    DoLink(TYPE_RIBBON_PANEL, TYPE_RIBBON_TOOLBAR, INSERT_CHILD);
    DoLink(TYPE_RIBBON_PANEL, TYPE_RIBBON_GALLERY, INSERT_CHILD);
    DoLink(TYPE_RIBBON_PANEL, TYPE_SIZER, INSERT_MAIN_SIZER);

    // TYPE_RIBBON_BAR
    DoLink(TYPE_RIBBON_BAR, TYPE_RIBBON_BAR, INSERT_SIBLING);
    DoLink(TYPE_RIBBON_BAR, TYPE_RIBBON_PAGE, INSERT_CHILD);
    DoLink(TYPE_RIBBON_BAR, TYPE_CONTROL, INSERT_SIBLING);
    DoLink(TYPE_RIBBON_BAR, TYPE_GRID, INSERT_SIBLING);
    DoLink(TYPE_RIBBON_BAR, TYPE_PG_MGR, INSERT_SIBLING);
    DoLink(TYPE_RIBBON_BAR, TYPE_SIZER, INSERT_SIBLING);
    DoLink(TYPE_RIBBON_BAR, TYPE_LIST_CTRL, INSERT_SIBLING);
    DoLink(TYPE_RIBBON_BAR, TYPE_TREE_LIST_CTRL, INSERT_SIBLING);
    DoLink(TYPE_RIBBON_BAR, TYPE_CONTAINER, INSERT_SIBLING);
    DoLink(TYPE_RIBBON_BAR, TYPE_BOOK, INSERT_SIBLING);
    DoLink(TYPE_RIBBON_BAR, TYPE_SPLITTER_WINDOW, INSERT_SIBLING);
    DoLink(TYPE_RIBBON_BAR, TYPE_SPACER, INSERT_SIBLING);
    DoLink(TYPE_RIBBON_BAR, TYPE_TOOLBAR, INSERT_SIBLING);
    DoLink(TYPE_RIBBON_BAR, TYPE_AUITOOLBAR, INSERT_SIBLING);
    DoLink(TYPE_RIBBON_BAR, TYPE_STD_BUTTON_SIZER, INSERT_SIBLING);
    DoLink(TYPE_RIBBON_BAR, TYPE_DV_WITH_COLUMNS, INSERT_SIBLING);
    DoLink(TYPE_RIBBON_BAR, TYPE_COLOLAPSIBLEPANE, INSERT_SIBLING);
    DoLink(TYPE_RIBBON_BAR, TYPE_INFO_BAR, INSERT_SIBLING);
    DoLink(TYPE_RIBBON_BAR, TYPE_RIBBON_PAGE, INSERT_CHILD);

    // TYPE_TIMER
    DoLink(TYPE_TIMER, TYPE_TIMER, INSERT_SIBLING);

    // TYPE_TASKBARICON
    DoLink(TYPE_TASKBARICON, TYPE_TASKBARICON, INSERT_SIBLING);

    // TYPE_BITMAP
    DoLink(TYPE_BITMAP, TYPE_BITMAP, INSERT_SIBLING);

    DoLink(TYPE_IMGLIST, TYPE_BITMAP, INSERT_CHILD);
    DoLink(TYPE_IMGLIST, TYPE_IMGLIST, INSERT_SIBLING);
    DoLink(TYPE_IMGLIST, TYPE_FORM, INSERT_SIBLING);
    DoLink(TYPE_IMGLIST, TYPE_FORM_FRAME, INSERT_SIBLING);
    DoLink(TYPE_IMGLIST, TYPE_WIZARD, INSERT_SIBLING);

    // TYPE_AUI_MGR
    DoLink(TYPE_AUI_MGR, TYPE_CONTAINER, INSERT_CHILD);
    DoLink(TYPE_AUI_MGR, TYPE_BOOK, INSERT_CHILD);
    DoLink(TYPE_AUI_MGR, TYPE_SPLITTER_WINDOW, INSERT_CHILD);
    DoLink(TYPE_AUI_MGR, TYPE_AUITOOLBAR, INSERT_CHILD);

    DoLink(TYPE_STD_BUTTON, TYPE_STD_BUTTON, INSERT_SIBLING);

    // TYPE_INFO_BAR_BUTTON
    DoLink(TYPE_INFO_BAR_BUTTON, TYPE_INFO_BAR_BUTTON, INSERT_SIBLING);

    DoLink(TYPE_INFO_BAR, TYPE_INFO_BAR_BUTTON, INSERT_CHILD);
    DoLink(TYPE_INFO_BAR, TYPE_INFO_BAR, INSERT_SIBLING);
    DoLink(TYPE_INFO_BAR, TYPE_CONTROL, INSERT_SIBLING);
    DoLink(TYPE_INFO_BAR, TYPE_GRID, INSERT_SIBLING);
    DoLink(TYPE_INFO_BAR, TYPE_SIZER, INSERT_SIBLING);
    DoLink(TYPE_INFO_BAR, TYPE_LIST_CTRL, INSERT_SIBLING);
    DoLink(TYPE_INFO_BAR, TYPE_TREE_LIST_CTRL, INSERT_SIBLING);
    DoLink(TYPE_INFO_BAR, TYPE_CONTAINER, INSERT_SIBLING);
    DoLink(TYPE_INFO_BAR, TYPE_BOOK, INSERT_SIBLING);
    DoLink(TYPE_INFO_BAR, TYPE_SPLITTER_WINDOW, INSERT_SIBLING);
    DoLink(TYPE_INFO_BAR, TYPE_SPACER, INSERT_SIBLING);
    DoLink(TYPE_INFO_BAR, TYPE_TOOLBAR, INSERT_SIBLING);
    DoLink(TYPE_INFO_BAR, TYPE_AUITOOLBAR, INSERT_SIBLING);
    DoLink(TYPE_INFO_BAR, TYPE_STD_BUTTON_SIZER, INSERT_SIBLING);
    DoLink(TYPE_INFO_BAR, TYPE_DV_WITH_COLUMNS, INSERT_SIBLING);
    DoLink(TYPE_INFO_BAR, TYPE_COLOLAPSIBLEPANE, INSERT_SIBLING);
    DoLink(TYPE_INFO_BAR, TYPE_PG_MGR, INSERT_SIBLING);
    DoLink(TYPE_INFO_BAR, TYPE_RIBBON_BAR, INSERT_SIBLING);

    // TYPE_PG_PROPERTY
    DoLink(TYPE_PG_PROPERTY, TYPE_PG_PROPERTY, INSERT_PROMPT_CHILD_OR_SIBLING);

    // TYPE_PG_MGR
    DoLink(TYPE_PG_MGR, TYPE_PG_PROPERTY, INSERT_CHILD);
    DoLink(TYPE_PG_MGR, TYPE_PG_MGR, INSERT_SIBLING);
    DoLink(TYPE_PG_MGR, TYPE_INFO_BAR, INSERT_SIBLING);
    DoLink(TYPE_PG_MGR, TYPE_CONTROL, INSERT_SIBLING);
    DoLink(TYPE_PG_MGR, TYPE_GRID, INSERT_SIBLING);
    DoLink(TYPE_PG_MGR, TYPE_SIZER, INSERT_SIBLING);
    DoLink(TYPE_PG_MGR, TYPE_LIST_CTRL, INSERT_SIBLING);
    DoLink(TYPE_PG_MGR, TYPE_TREE_LIST_CTRL, INSERT_SIBLING);
    DoLink(TYPE_PG_MGR, TYPE_CONTAINER, INSERT_SIBLING);
    DoLink(TYPE_PG_MGR, TYPE_BOOK, INSERT_SIBLING);
    DoLink(TYPE_PG_MGR, TYPE_SPLITTER_WINDOW, INSERT_SIBLING);
    DoLink(TYPE_PG_MGR, TYPE_SPACER, INSERT_SIBLING);
    DoLink(TYPE_PG_MGR, TYPE_TOOLBAR, INSERT_SIBLING);
    DoLink(TYPE_PG_MGR, TYPE_AUITOOLBAR, INSERT_SIBLING);
    DoLink(TYPE_PG_MGR, TYPE_STD_BUTTON_SIZER, INSERT_SIBLING);
    DoLink(TYPE_PG_MGR, TYPE_DV_WITH_COLUMNS, INSERT_SIBLING);
    DoLink(TYPE_PG_MGR, TYPE_COLOLAPSIBLEPANE, INSERT_SIBLING);
    DoLink(TYPE_PG_MGR, TYPE_RIBBON_BAR, INSERT_SIBLING);

    // TYPE_STD_BUTTON_SIZER
    DoLink(TYPE_STD_BUTTON_SIZER, TYPE_STD_BUTTON, INSERT_CHILD);
    DoLink(TYPE_STD_BUTTON_SIZER, TYPE_CONTROL, INSERT_SIBLING);
    DoLink(TYPE_STD_BUTTON_SIZER, TYPE_GRID, INSERT_SIBLING);
    DoLink(TYPE_STD_BUTTON_SIZER, TYPE_PG_MGR, INSERT_SIBLING);
    DoLink(TYPE_STD_BUTTON_SIZER, TYPE_SIZER, INSERT_SIBLING);
    DoLink(TYPE_STD_BUTTON_SIZER, TYPE_LIST_CTRL, INSERT_SIBLING);
    DoLink(TYPE_STD_BUTTON_SIZER, TYPE_TREE_LIST_CTRL, INSERT_SIBLING);
    DoLink(TYPE_STD_BUTTON_SIZER, TYPE_CONTAINER, INSERT_SIBLING);
    DoLink(TYPE_STD_BUTTON_SIZER, TYPE_BOOK, INSERT_SIBLING);
    DoLink(TYPE_STD_BUTTON_SIZER, TYPE_SPLITTER_WINDOW, INSERT_SIBLING);
    DoLink(TYPE_STD_BUTTON_SIZER, TYPE_SPACER, INSERT_SIBLING);
    DoLink(TYPE_STD_BUTTON_SIZER, TYPE_TOOLBAR, INSERT_SIBLING);
    DoLink(TYPE_STD_BUTTON_SIZER, TYPE_AUITOOLBAR, INSERT_SIBLING);
    DoLink(TYPE_STD_BUTTON_SIZER, TYPE_DV_WITH_COLUMNS, INSERT_SIBLING);
    DoLink(TYPE_STD_BUTTON_SIZER, TYPE_COLOLAPSIBLEPANE, INSERT_SIBLING);
    DoLink(TYPE_STD_BUTTON_SIZER, TYPE_INFO_BAR, INSERT_SIBLING);
    DoLink(TYPE_STD_BUTTON_SIZER, TYPE_RIBBON_BAR, INSERT_SIBLING);

    // TYPE_SIZER
    DoLink(TYPE_SIZER, TYPE_SIZER, INSERT_CHILD);
    DoLink(TYPE_SIZER, TYPE_CONTROL, INSERT_CHILD);
    DoLink(TYPE_SIZER, TYPE_GRID, INSERT_CHILD);
    DoLink(TYPE_SIZER, TYPE_PG_MGR, INSERT_CHILD);
    DoLink(TYPE_SIZER, TYPE_LIST_CTRL, INSERT_CHILD);
    DoLink(TYPE_SIZER, TYPE_TREE_LIST_CTRL, INSERT_CHILD);
    DoLink(TYPE_SIZER, TYPE_CONTAINER, INSERT_CHILD);
    DoLink(TYPE_SIZER, TYPE_BOOK, INSERT_CHILD);
    DoLink(TYPE_SIZER, TYPE_SPLITTER_WINDOW, INSERT_CHILD);
    DoLink(TYPE_SIZER, TYPE_SPACER, INSERT_CHILD);
    DoLink(TYPE_SIZER, TYPE_TOOLBAR, INSERT_CHILD);
    DoLink(TYPE_SIZER, TYPE_AUITOOLBAR, INSERT_CHILD);
    DoLink(TYPE_SIZER, TYPE_STD_BUTTON_SIZER, INSERT_CHILD);
    DoLink(TYPE_SIZER, TYPE_DV_WITH_COLUMNS, INSERT_CHILD);
    DoLink(TYPE_SIZER, TYPE_COLOLAPSIBLEPANE, INSERT_CHILD);
    DoLink(TYPE_SIZER, TYPE_INFO_BAR, INSERT_CHILD);
    DoLink(TYPE_SIZER, TYPE_RIBBON_BAR, INSERT_CHILD);

    // TYPE_CONTROL
    DoLink(TYPE_CONTROL, TYPE_CONTROL, INSERT_SIBLING);
    DoLink(TYPE_CONTROL, TYPE_GRID, INSERT_SIBLING);
    DoLink(TYPE_CONTROL, TYPE_PG_MGR, INSERT_SIBLING);
    DoLink(TYPE_CONTROL, TYPE_SIZER, INSERT_SIBLING);
    DoLink(TYPE_CONTROL, TYPE_LIST_CTRL, INSERT_SIBLING);
    DoLink(TYPE_CONTROL, TYPE_TREE_LIST_CTRL, INSERT_SIBLING);
    DoLink(TYPE_CONTROL, TYPE_CONTAINER, INSERT_SIBLING);
    DoLink(TYPE_CONTROL, TYPE_BOOK, INSERT_SIBLING);
    DoLink(TYPE_CONTROL, TYPE_SPLITTER_WINDOW, INSERT_SIBLING);
    DoLink(TYPE_CONTROL, TYPE_SPACER, INSERT_SIBLING);
    DoLink(TYPE_CONTROL, TYPE_TOOLBAR, INSERT_SIBLING);
    DoLink(TYPE_CONTROL, TYPE_AUITOOLBAR, INSERT_SIBLING);
    DoLink(TYPE_CONTROL, TYPE_STD_BUTTON_SIZER, INSERT_SIBLING);
    DoLink(TYPE_CONTROL, TYPE_DV_WITH_COLUMNS, INSERT_SIBLING);
    DoLink(TYPE_CONTROL, TYPE_COLOLAPSIBLEPANE, INSERT_SIBLING);
    DoLink(TYPE_CONTROL, TYPE_INFO_BAR, INSERT_SIBLING);
    DoLink(TYPE_CONTROL, TYPE_RIBBON_BAR, INSERT_SIBLING);

    // TYPE_COLOLAPSIBLEPANE
    DoLink(TYPE_COLOLAPSIBLEPANE, TYPE_COLOLAPSIBLEPANE, INSERT_SIBLING);
    DoLink(TYPE_COLOLAPSIBLEPANE, TYPE_PG_MGR, INSERT_SIBLING);
    DoLink(TYPE_COLOLAPSIBLEPANE, TYPE_CONTROL, INSERT_SIBLING);
    DoLink(TYPE_COLOLAPSIBLEPANE, TYPE_GRID, INSERT_SIBLING);
    DoLink(TYPE_COLOLAPSIBLEPANE, TYPE_SIZER, INSERT_SIBLING);
    DoLink(TYPE_COLOLAPSIBLEPANE, TYPE_LIST_CTRL, INSERT_SIBLING);
    DoLink(TYPE_COLOLAPSIBLEPANE, TYPE_TREE_LIST_CTRL, INSERT_SIBLING);
    DoLink(TYPE_COLOLAPSIBLEPANE, TYPE_CONTAINER, INSERT_SIBLING);
    DoLink(TYPE_COLOLAPSIBLEPANE, TYPE_BOOK, INSERT_SIBLING);
    DoLink(TYPE_COLOLAPSIBLEPANE, TYPE_SPLITTER_WINDOW, INSERT_SIBLING);
    DoLink(TYPE_COLOLAPSIBLEPANE, TYPE_SPACER, INSERT_SIBLING);
    DoLink(TYPE_COLOLAPSIBLEPANE, TYPE_TOOLBAR, INSERT_SIBLING);
    DoLink(TYPE_COLOLAPSIBLEPANE, TYPE_AUITOOLBAR, INSERT_SIBLING);
    DoLink(TYPE_COLOLAPSIBLEPANE, TYPE_STD_BUTTON_SIZER, INSERT_SIBLING);
    DoLink(TYPE_COLOLAPSIBLEPANE, TYPE_DV_WITH_COLUMNS, INSERT_SIBLING);
    DoLink(TYPE_COLOLAPSIBLEPANE, TYPE_COLOLAPSIBLEPANE_PANE, INSERT_CHILD);
    DoLink(TYPE_COLOLAPSIBLEPANE, TYPE_INFO_BAR, INSERT_SIBLING);
    DoLink(TYPE_COLOLAPSIBLEPANE, TYPE_RIBBON_BAR, INSERT_SIBLING);

    // TYPE_LIST_CTRL
    DoLink(TYPE_LIST_CTRL, TYPE_CONTROL, INSERT_SIBLING);
    DoLink(TYPE_LIST_CTRL, TYPE_GRID, INSERT_SIBLING);
    DoLink(TYPE_LIST_CTRL, TYPE_PG_MGR, INSERT_SIBLING);
    DoLink(TYPE_LIST_CTRL, TYPE_LIST_CTRL, INSERT_SIBLING);
    DoLink(TYPE_LIST_CTRL, TYPE_LIST_CTRL_COL, INSERT_CHILD);
    DoLink(TYPE_LIST_CTRL, TYPE_CONTAINER, INSERT_SIBLING);
    DoLink(TYPE_LIST_CTRL, TYPE_BOOK, INSERT_SIBLING);
    DoLink(TYPE_LIST_CTRL, TYPE_SPLITTER_WINDOW, INSERT_SIBLING);
    DoLink(TYPE_LIST_CTRL, TYPE_SPACER, INSERT_SIBLING);
    DoLink(TYPE_LIST_CTRL, TYPE_TOOLBAR, INSERT_SIBLING);
    DoLink(TYPE_LIST_CTRL, TYPE_AUITOOLBAR, INSERT_SIBLING);
    DoLink(TYPE_LIST_CTRL, TYPE_STD_BUTTON_SIZER, INSERT_SIBLING);
    DoLink(TYPE_LIST_CTRL, TYPE_DV_WITH_COLUMNS, INSERT_SIBLING);
    DoLink(TYPE_LIST_CTRL, TYPE_COLOLAPSIBLEPANE, INSERT_SIBLING);
    DoLink(TYPE_LIST_CTRL, TYPE_INFO_BAR, INSERT_SIBLING);
    DoLink(TYPE_LIST_CTRL, TYPE_RIBBON_BAR, INSERT_SIBLING);

    // TYPE_TREE_LIST_CTRL
    DoLink(TYPE_TREE_LIST_CTRL, TYPE_CONTROL, INSERT_SIBLING);
    DoLink(TYPE_TREE_LIST_CTRL, TYPE_GRID, INSERT_SIBLING);
    DoLink(TYPE_TREE_LIST_CTRL, TYPE_PG_MGR, INSERT_SIBLING);
    DoLink(TYPE_TREE_LIST_CTRL, TYPE_LIST_CTRL, INSERT_SIBLING);
    DoLink(TYPE_TREE_LIST_CTRL, TYPE_TREE_LIST_CTRL, INSERT_SIBLING);
    DoLink(TYPE_TREE_LIST_CTRL, TYPE_TREE_LIST_CTRL_COL, INSERT_CHILD);
    DoLink(TYPE_TREE_LIST_CTRL, TYPE_CONTAINER, INSERT_SIBLING);
    DoLink(TYPE_TREE_LIST_CTRL, TYPE_BOOK, INSERT_SIBLING);
    DoLink(TYPE_TREE_LIST_CTRL, TYPE_SPLITTER_WINDOW, INSERT_SIBLING);
    DoLink(TYPE_TREE_LIST_CTRL, TYPE_SPACER, INSERT_SIBLING);
    DoLink(TYPE_TREE_LIST_CTRL, TYPE_TOOLBAR, INSERT_SIBLING);
    DoLink(TYPE_TREE_LIST_CTRL, TYPE_AUITOOLBAR, INSERT_SIBLING);
    DoLink(TYPE_TREE_LIST_CTRL, TYPE_STD_BUTTON_SIZER, INSERT_SIBLING);
    DoLink(TYPE_TREE_LIST_CTRL, TYPE_DV_WITH_COLUMNS, INSERT_SIBLING);
    DoLink(TYPE_TREE_LIST_CTRL, TYPE_COLOLAPSIBLEPANE, INSERT_SIBLING);
    DoLink(TYPE_TREE_LIST_CTRL, TYPE_INFO_BAR, INSERT_SIBLING);
    DoLink(TYPE_TREE_LIST_CTRL, TYPE_RIBBON_BAR, INSERT_SIBLING);

    // TYPE_DV_WITH_COLUMNS
    DoLink(TYPE_DV_WITH_COLUMNS, TYPE_CONTROL, INSERT_SIBLING);
    DoLink(TYPE_DV_WITH_COLUMNS, TYPE_GRID, INSERT_SIBLING);
    DoLink(TYPE_DV_WITH_COLUMNS, TYPE_PG_MGR, INSERT_SIBLING);
    DoLink(TYPE_DV_WITH_COLUMNS, TYPE_DV_WITH_COLUMNS, INSERT_SIBLING);
    DoLink(TYPE_DV_WITH_COLUMNS, TYPE_CONTAINER, INSERT_SIBLING);
    DoLink(TYPE_DV_WITH_COLUMNS, TYPE_BOOK, INSERT_SIBLING);
    DoLink(TYPE_DV_WITH_COLUMNS, TYPE_SPLITTER_WINDOW, INSERT_SIBLING);
    DoLink(TYPE_DV_WITH_COLUMNS, TYPE_SPACER, INSERT_SIBLING);
    DoLink(TYPE_DV_WITH_COLUMNS, TYPE_TOOLBAR, INSERT_SIBLING);
    DoLink(TYPE_DV_WITH_COLUMNS, TYPE_AUITOOLBAR, INSERT_SIBLING);
    DoLink(TYPE_DV_WITH_COLUMNS, TYPE_STD_BUTTON_SIZER, INSERT_SIBLING);
    DoLink(TYPE_DV_WITH_COLUMNS, TYPE_DV_LISTCTRLCOL, INSERT_CHILD);
    DoLink(TYPE_DV_WITH_COLUMNS, TYPE_COLOLAPSIBLEPANE, INSERT_SIBLING);
    DoLink(TYPE_DV_WITH_COLUMNS, TYPE_INFO_BAR, INSERT_SIBLING);
    DoLink(TYPE_DV_WITH_COLUMNS, TYPE_RIBBON_BAR, INSERT_SIBLING);

    // TYPE_LIST_CTRL_COL
    DoLink(TYPE_LIST_CTRL_COL, TYPE_LIST_CTRL_COL, INSERT_SIBLING);

    // TYPE_LIST_CTRL_COL
    DoLink(TYPE_TREE_LIST_CTRL_COL, TYPE_TREE_LIST_CTRL_COL, INSERT_SIBLING);

    // TYPE_DV_LISTCTRLCOL
    DoLink(TYPE_DV_LISTCTRLCOL, TYPE_DV_LISTCTRLCOL, INSERT_SIBLING);

    // TYPE_CONTAINER
    DoLink(TYPE_CONTAINER, TYPE_AUI_MGR, INSERT_CHILD);
    DoLink(TYPE_CONTAINER, TYPE_SIZER, INSERT_MAIN_SIZER);
    DoLink(TYPE_CONTAINER, TYPE_CONTROL, INSERT_SIBLING);
    DoLink(TYPE_CONTAINER, TYPE_GRID, INSERT_SIBLING);
    DoLink(TYPE_CONTAINER, TYPE_PG_MGR, INSERT_SIBLING);
    DoLink(TYPE_CONTAINER, TYPE_LIST_CTRL, INSERT_SIBLING);
    DoLink(TYPE_CONTAINER, TYPE_TREE_LIST_CTRL, INSERT_SIBLING);
    DoLink(TYPE_CONTAINER, TYPE_CONTAINER, INSERT_SIBLING);
    DoLink(TYPE_CONTAINER, TYPE_BOOK, INSERT_SIBLING);
    DoLink(TYPE_CONTAINER, TYPE_SPLITTER_WINDOW, INSERT_SIBLING);
    DoLink(TYPE_CONTAINER, TYPE_SPACER, INSERT_SIBLING);
    DoLink(TYPE_CONTAINER, TYPE_TOOLBAR, INSERT_SIBLING);
    DoLink(TYPE_CONTAINER, TYPE_AUITOOLBAR, INSERT_SIBLING);
    DoLink(TYPE_CONTAINER, TYPE_STD_BUTTON_SIZER, INSERT_SIBLING);
    DoLink(TYPE_CONTAINER, TYPE_DV_WITH_COLUMNS, INSERT_SIBLING);
    DoLink(TYPE_CONTAINER, TYPE_COLOLAPSIBLEPANE, INSERT_SIBLING);
    DoLink(TYPE_CONTAINER, TYPE_INFO_BAR, INSERT_SIBLING);
    DoLink(TYPE_CONTAINER, TYPE_RIBBON_BAR, INSERT_SIBLING);

    // TYPE_COLOLAPSIBLEPANE_PANE
    DoLink(TYPE_COLOLAPSIBLEPANE_PANE, TYPE_SIZER, INSERT_MAIN_SIZER);

    // TYPE_BOOK
    DoLink(TYPE_BOOK, TYPE_SIZER, INSERT_SIBLING);
    DoLink(TYPE_BOOK, TYPE_CONTROL, INSERT_SIBLING);
    DoLink(TYPE_BOOK, TYPE_GRID, INSERT_SIBLING);
    DoLink(TYPE_BOOK, TYPE_PG_MGR, INSERT_SIBLING);
    DoLink(TYPE_BOOK, TYPE_LIST_CTRL, INSERT_SIBLING);
    DoLink(TYPE_BOOK, TYPE_TREE_LIST_CTRL, INSERT_SIBLING);
    DoLink(TYPE_BOOK, TYPE_CONTAINER, INSERT_SIBLING);
    DoLink(TYPE_BOOK, TYPE_BOOK, INSERT_SIBLING);
    DoLink(TYPE_BOOK, TYPE_NOTEBOOK_PAGE, INSERT_CHILD);
    DoLink(TYPE_BOOK, TYPE_SPLITTER_WINDOW, INSERT_SIBLING);
    DoLink(TYPE_BOOK, TYPE_SPACER, INSERT_SIBLING);
    DoLink(TYPE_BOOK, TYPE_TOOLBAR, INSERT_SIBLING);
    DoLink(TYPE_BOOK, TYPE_AUITOOLBAR, INSERT_SIBLING);
    DoLink(TYPE_BOOK, TYPE_STD_BUTTON_SIZER, INSERT_SIBLING);
    DoLink(TYPE_BOOK, TYPE_DV_WITH_COLUMNS, INSERT_SIBLING);
    DoLink(TYPE_BOOK, TYPE_COLOLAPSIBLEPANE, INSERT_SIBLING);
    DoLink(TYPE_BOOK, TYPE_INFO_BAR, INSERT_SIBLING);
    DoLink(TYPE_BOOK, TYPE_RIBBON_BAR, INSERT_SIBLING);

    // TYPE_FORM
    DoLink(TYPE_FORM, TYPE_SIZER, INSERT_MAIN_SIZER);
    DoLink(TYPE_FORM, TYPE_FORM, INSERT_SIBLING);
    DoLink(TYPE_FORM, TYPE_FORM_FRAME, INSERT_SIBLING);
    DoLink(TYPE_FORM, TYPE_WIZARD, INSERT_SIBLING);
    DoLink(TYPE_FORM, TYPE_AUI_MGR, INSERT_CHILD);
    DoLink(TYPE_FORM, TYPE_IMGLIST, INSERT_SIBLING);
    DoLink(TYPE_FORM, TYPE_TIMER, INSERT_CHILD);
    DoLink(TYPE_FORM, TYPE_TASKBARICON, INSERT_CHILD);

    // TYPE_FORM_FRAME
    DoLink(TYPE_FORM_FRAME, TYPE_SIZER, INSERT_MAIN_SIZER);
    DoLink(TYPE_FORM_FRAME, TYPE_FORM, INSERT_SIBLING);
    DoLink(TYPE_FORM_FRAME, TYPE_FORM_FRAME, INSERT_SIBLING);
    DoLink(TYPE_FORM_FRAME, TYPE_WIZARD, INSERT_SIBLING);
    DoLink(TYPE_FORM_FRAME, TYPE_MENUBAR, INSERT_CHILD);
    DoLink(TYPE_FORM_FRAME, TYPE_TOOLBAR, INSERT_CHILD);
    DoLink(TYPE_FORM_FRAME, TYPE_STATUSABR, INSERT_CHILD);
    DoLink(TYPE_FORM_FRAME, TYPE_AUI_MGR, INSERT_CHILD);
    DoLink(TYPE_FORM_FRAME, TYPE_IMGLIST, INSERT_SIBLING);
    DoLink(TYPE_FORM_FRAME, TYPE_TIMER, INSERT_CHILD);
    DoLink(TYPE_FORM_FRAME, TYPE_TASKBARICON, INSERT_CHILD);

    // TYPE_WIZARD
    DoLink(TYPE_WIZARD, TYPE_FORM, INSERT_SIBLING);
    DoLink(TYPE_WIZARD, TYPE_FORM_FRAME, INSERT_SIBLING);
    DoLink(TYPE_WIZARD, TYPE_WIZARD, INSERT_SIBLING);
    DoLink(TYPE_WIZARD, TYPE_WIZARD_PAGE, INSERT_CHILD);
    DoLink(TYPE_WIZARD, TYPE_IMGLIST, INSERT_SIBLING);

    // TYPE_WIZARD_PAGE
    DoLink(TYPE_WIZARD_PAGE, TYPE_SIZER, INSERT_MAIN_SIZER);
    DoLink(TYPE_WIZARD_PAGE, TYPE_WIZARD_PAGE, INSERT_SIBLING);

    // TYPE_NOTEBOOK_PAGE
    DoLink(TYPE_NOTEBOOK_PAGE, TYPE_SIZER, INSERT_MAIN_SIZER);
    DoLink(TYPE_NOTEBOOK_PAGE, TYPE_NOTEBOOK_PAGE, INSERT_SIBLING);

    // TYPE_SPLITTERWIN_PAGE
    DoLink(TYPE_SPLITTERWIN_PAGE, TYPE_SIZER, INSERT_MAIN_SIZER);
    DoLink(TYPE_SPLITTERWIN_PAGE, TYPE_SPLITTERWIN_PAGE, INSERT_SIBLING);

    // TYPE_SPLITTER_WINDOW
    DoLink(TYPE_SPLITTER_WINDOW, TYPE_SIZER, INSERT_SIBLING);
    DoLink(TYPE_SPLITTER_WINDOW, TYPE_CONTROL, INSERT_SIBLING);
    DoLink(TYPE_SPLITTER_WINDOW, TYPE_GRID, INSERT_SIBLING);
    DoLink(TYPE_SPLITTER_WINDOW, TYPE_PG_MGR, INSERT_SIBLING);
    DoLink(TYPE_SPLITTER_WINDOW, TYPE_LIST_CTRL, INSERT_SIBLING);
    DoLink(TYPE_SPLITTER_WINDOW, TYPE_TREE_LIST_CTRL, INSERT_SIBLING);
    DoLink(TYPE_SPLITTER_WINDOW, TYPE_CONTAINER, INSERT_SIBLING);
    DoLink(TYPE_SPLITTER_WINDOW, TYPE_BOOK, INSERT_SIBLING);
    DoLink(TYPE_SPLITTER_WINDOW, TYPE_SPLITTERWIN_PAGE, INSERT_CHILD);
    DoLink(TYPE_SPLITTER_WINDOW, TYPE_SPLITTER_WINDOW, INSERT_SIBLING);
    DoLink(TYPE_SPLITTER_WINDOW, TYPE_SPACER, INSERT_SIBLING);
    DoLink(TYPE_SPLITTER_WINDOW, TYPE_TOOLBAR, INSERT_SIBLING);
    DoLink(TYPE_SPLITTER_WINDOW, TYPE_AUITOOLBAR, INSERT_SIBLING);
    DoLink(TYPE_SPLITTER_WINDOW, TYPE_STD_BUTTON_SIZER, INSERT_SIBLING);
    DoLink(TYPE_SPLITTER_WINDOW, TYPE_DV_WITH_COLUMNS, INSERT_SIBLING);
    DoLink(TYPE_SPLITTER_WINDOW, TYPE_COLOLAPSIBLEPANE, INSERT_SIBLING);
    DoLink(TYPE_SPLITTER_WINDOW, TYPE_INFO_BAR, INSERT_SIBLING);
    DoLink(TYPE_SPLITTER_WINDOW, TYPE_RIBBON_BAR, INSERT_SIBLING);

    // TYPE_SPACER
    DoLink(TYPE_SPACER, TYPE_SIZER, INSERT_SIBLING);
    DoLink(TYPE_SPACER, TYPE_CONTROL, INSERT_SIBLING);
    DoLink(TYPE_SPACER, TYPE_GRID, INSERT_SIBLING);
    DoLink(TYPE_SPACER, TYPE_PG_MGR, INSERT_SIBLING);
    DoLink(TYPE_SPACER, TYPE_LIST_CTRL, INSERT_SIBLING);
    DoLink(TYPE_SPACER, TYPE_TREE_LIST_CTRL, INSERT_SIBLING);
    DoLink(TYPE_SPACER, TYPE_CONTAINER, INSERT_SIBLING);
    DoLink(TYPE_SPACER, TYPE_BOOK, INSERT_SIBLING);
    DoLink(TYPE_SPACER, TYPE_SPLITTER_WINDOW, INSERT_SIBLING);
    DoLink(TYPE_SPACER, TYPE_SPACER, INSERT_SIBLING);
    DoLink(TYPE_SPACER, TYPE_TOOLBAR, INSERT_SIBLING);
    DoLink(TYPE_SPACER, TYPE_AUITOOLBAR, INSERT_SIBLING);
    DoLink(TYPE_SPACER, TYPE_STD_BUTTON_SIZER, INSERT_SIBLING);
    DoLink(TYPE_SPACER, TYPE_DV_WITH_COLUMNS, INSERT_SIBLING);
    DoLink(TYPE_SPACER, TYPE_COLOLAPSIBLEPANE, INSERT_SIBLING);
    DoLink(TYPE_SPACER, TYPE_INFO_BAR, INSERT_SIBLING);
    DoLink(TYPE_SPACER, TYPE_RIBBON_BAR, INSERT_SIBLING);

    // TYPE_MENUBAR
    DoLink(TYPE_MENUBAR, TYPE_MENU, INSERT_CHILD);
    DoLink(TYPE_MENUBAR, TYPE_TOOLBAR, INSERT_SIBLING);
    DoLink(TYPE_MENUBAR, TYPE_SIZER, INSERT_MAIN_SIZER);

    // TYPE_MENU
    DoLink(TYPE_MENU, TYPE_MENU, INSERT_SIBLING);
    DoLink(TYPE_MENU, TYPE_MENUITEM, INSERT_CHILD);
    DoLink(TYPE_MENU, TYPE_SUBMENU, INSERT_CHILD);

    // TYPE_SUBMENU
    DoLink(TYPE_SUBMENU, TYPE_MENUITEM, INSERT_CHILD);
    DoLink(TYPE_SUBMENU, TYPE_SUBMENU, INSERT_CHILD);

    // TYPE_MENUITEM
    DoLink(TYPE_MENUITEM, TYPE_MENUITEM, INSERT_SIBLING);
    DoLink(TYPE_MENUITEM, TYPE_SUBMENU, INSERT_SIBLING);

    // TYPE_TOOLBAR
    DoLink(TYPE_TOOLBAR, TYPE_CONTROL, INSERT_CHILD);
    DoLink(TYPE_TOOLBAR, TYPE_TOOLITEM, INSERT_CHILD);

    // TYPE_AUITOOLBAR
    DoLink(TYPE_AUITOOLBAR, TYPE_CONTROL, INSERT_CHILD);
    DoLink(TYPE_AUITOOLBAR, TYPE_PG_MGR, INSERT_SIBLING);
    DoLink(TYPE_AUITOOLBAR, TYPE_SIZER, INSERT_SIBLING);
    DoLink(TYPE_AUITOOLBAR, TYPE_LIST_CTRL, INSERT_SIBLING);
    DoLink(TYPE_AUITOOLBAR, TYPE_TREE_LIST_CTRL, INSERT_SIBLING);
    DoLink(TYPE_AUITOOLBAR, TYPE_CONTAINER, INSERT_SIBLING);
    DoLink(TYPE_AUITOOLBAR, TYPE_BOOK, INSERT_SIBLING);
    DoLink(TYPE_AUITOOLBAR, TYPE_SPLITTER_WINDOW, INSERT_SIBLING);
    DoLink(TYPE_AUITOOLBAR, TYPE_SPACER, INSERT_SIBLING);
    DoLink(TYPE_AUITOOLBAR, TYPE_TOOLBAR, INSERT_SIBLING);
    DoLink(TYPE_AUITOOLBAR, TYPE_AUITOOLBAR, INSERT_SIBLING);
    DoLink(TYPE_AUITOOLBAR, TYPE_TOOLITEM, INSERT_CHILD);
    DoLink(TYPE_AUITOOLBAR, TYPE_STD_BUTTON_SIZER, INSERT_SIBLING);
    DoLink(TYPE_AUITOOLBAR, TYPE_DV_WITH_COLUMNS, INSERT_SIBLING);
    DoLink(TYPE_AUITOOLBAR, TYPE_COLOLAPSIBLEPANE, INSERT_SIBLING);
    DoLink(TYPE_AUITOOLBAR, TYPE_INFO_BAR, INSERT_SIBLING);
    DoLink(TYPE_AUITOOLBAR, TYPE_RIBBON_BAR, INSERT_SIBLING);

    // TYPE_TOOLITEM
    DoLink(TYPE_TOOLITEM, TYPE_TOOLITEM, INSERT_SIBLING);
    DoLink(TYPE_TOOLITEM, TYPE_CONTROL, INSERT_SIBLING);

    // TYPE_STATUSBAR
    DoLink(TYPE_STATUSABR, TYPE_SIZER, INSERT_MAIN_SIZER);
}

// static
int Allocator::StringToId(const wxString& classname)
{
    // ADD_NEW_CONTROL

    // First the truncated wxFB top-level names :/
    if(classname == wxT("Frame")) return ID_WXFRAME;
    if(classname == wxT("Dialog")) return ID_WXDIALOG;
    if(classname == wxT("Panel")) return ID_WXPANEL_TOPLEVEL;
    if(classname == wxT("Wizard")) return ID_WXWIZARD;
    if(classname == wxT("WizardPageSimple")) return ID_WXWIZARDPAGE;

    if(classname == wxT("wxButton")) return ID_WXBUTTON;
    if(classname == wxT("wxBoxSizer")) return ID_WXBOXSIZER;
    if(classname == wxT("wxFrame")) return ID_WXFRAME;
    if(classname == wxT("wxFlexGridSizer")) return ID_WXFLEXGRIDSIZER;
    if(classname == wxT("wxBitmapButton")) return ID_WXBITMAPBUTTON;
    if(classname == wxT("wxStaticText")) return ID_WXSTATICTEXT;
    if(classname == wxT("wxTextCtrl")) return ID_WXTEXTCTRL;
    if(classname == wxT("wxPanel")) return ID_WXPANEL;
    if(classname == wxT("wxStaticBitmap")) return ID_WXSTATICBITMAP;
    if(classname == wxT("wxComboBox")) return ID_WXCOMBOBOX;
    if(classname == wxT("wxChoice")) return ID_WXCHOICE;
    if(classname == wxT("wxListBox")) return ID_WXLISTBOX;
    if(classname == wxT("wxListCtrl")) return ID_WXLISTCTRL;
    if(classname == wxT("listcol")) return ID_WXLISTCTRL_COL;
    if(classname == wxT("wxCheckBox")) return ID_WXCHECKBOX;
    if(classname == wxT("wxRadioBox")) return ID_WXRADIOBOX;
    if(classname == wxT("wxRadioButton")) return ID_WXRADIOBUTTON;
    if(classname == wxT("wxStaticLine")) return ID_WXSTATICLINE;
    if(classname == wxT("wxSlider")) return ID_WXSLIDER;
    if(classname == wxT("wxGauge")) return ID_WXGAUGE;
    if(classname == wxT("wxDialog")) return ID_WXDIALOG;
    if(classname == wxT("wxTreeCtrl")) return ID_WXTREECTRL;
    if(classname == wxT("wxHtmlWindow")) return ID_WXHTMLWIN;
    if(classname == wxT("wxRichTextCtrl")) return ID_WXRICHTEXT;
    if(classname == wxT("wxCheckListBox")) return ID_WXCHECKLISTBOX;
    if(classname == wxT("wxGrid")) return ID_WXGRID;
    if(classname == wxT("wxToggleButton")) return ID_WXTOGGLEBUTTON;
    if(classname == wxT("wxBitmapToggleButton")) return ID_WXBITMAPTOGGLEBUTTON;
    if(classname == wxT("wxSearchCtrl")) return ID_WXSEARCHCTRL;
    if(classname == wxT("wxColourPickerCtrl")) return ID_WXCOLORPICKER;
    if(classname == wxT("wxFontPickerCtrl")) return ID_WXFONTPICKER;
    if(classname == wxT("wxFilePickerCtrl")) return ID_WXFILEPICKER;
    if(classname == wxT("wxDirPickerCtrl")) return ID_WXDIRPICKER;
    if(classname == wxT("wxDatePickerCtrl")) return ID_WXDATEPICKER;
    if(classname == wxT("wxCalendarCtrl")) return ID_WXCALEDARCTRL;
    if(classname == wxT("wxScrollBar")) return ID_WXSCROLLBAR;
    if(classname == wxT("wxSpinCtrl")) return ID_WXSPINCTRL;
    if(classname == wxT("wxSpinButton")) return ID_WXSPINBUTTON;
    if(classname == wxT("wxHyperlinkCtrl")) return ID_WXHYPERLINK;
    if(classname == wxT("wxGenericDirCtrl")) return ID_WXGENERICDIRCTRL;
    if(classname == wxT("wxScrolledWindow")) return ID_WXSCROLLEDWIN;
    if(classname == wxT("wxNotebook")) return ID_WXNOTEBOOK;
    if(classname == wxT("wxToolbook")) return ID_WXTOOLBOOK;
    if(classname == wxT("wxListbook")) return ID_WXLISTBOOK;
    if(classname == wxT("wxChoicebook")) return ID_WXCHOICEBOOK;
    if(classname == wxT("wxTreebook")) return ID_WXTREEBOOK;
    if(classname == wxT("notebookpage")) return ID_WXPANEL_NOTEBOOK_PAGE;
    if(classname == wxT("choicebookpage")) return ID_WXPANEL_NOTEBOOK_PAGE;
    if(classname == wxT("listbookpage")) return ID_WXPANEL_NOTEBOOK_PAGE;
    if(classname == wxT("treebookpage"))
        return ID_WXPANEL_NOTEBOOK_PAGE; // Not ID_WXTREEBOOK_SUB_PAGE, which is only used from the AddSubpage menuitem
    if(classname == wxT("wxSplitterWindow")) return ID_WXSPLITTERWINDOW;
    if(classname == wxT("splitteritem"))
        return ID_WXSPLITTERWINDOW_PAGE; // XRCed doesn't use this, but wxFB calls it 'splitteritem'
    if(classname == wxT("wxStaticBoxSizer")) return ID_WXSTATICBOXSIZER;
    if(classname == wxT("wxWizard")) return ID_WXWIZARD;
    if(classname == wxT("wxWizardPage")) return ID_WXWIZARDPAGE;
    if(classname == wxT("wxWizardPageSimple")) return ID_WXWIZARDPAGE;
    if(classname == wxT("wxGridSizer")) return ID_WXGRIDSIZER;
    if(classname == wxT("wxGridBagSizer")) return ID_WXGRIDBAGSIZER;
    if(classname == wxT("spacer")) return ID_WXSPACER;
    if(classname == wxT("wxSpacer"))
        return ID_WXSPACER; // XRC actually uses "spacer", but keep this duplicate for completeness
    if(classname == wxT("wxStdDialogButtonSizer")) return ID_WXSTDDLGBUTTONSIZER;
    if(classname == wxT("stdbutton")) return ID_WXSTDBUTTON;
    if(classname == wxT("wxAuiNotebook")) return ID_WXAUINOTEBOOK;
    if(classname == wxT("wxMenuBar")) return ID_WXMENUBAR;
    if(classname == wxT("wxMenu")) return ID_WXMENU;
    if(classname == wxT("wxMenuItem")) return ID_WXMENUITEM;
    if(classname == wxT("submenu")) return ID_WXSUBMENU;
    if(classname == wxT("wxToolBar")) return ID_WXTOOLBAR;
    if(classname == wxT("tool")) return ID_WXTOOLBARITEM;
    if(classname == wxT("toolSeparator")) return ID_WXTOOLBARITEM;
    if(classname == wxT("space")) return ID_WXTOOLBARITEM;
    if(classname == wxT("separator")) return ID_WXMENUITEM; // but in XRC could actually be a toolbar separator
    if(classname == wxT("wxAuiToolBar")) return ID_WXAUITOOLBAR;
    if(classname == wxT("label")) return ID_WXAUITOOLBARLABEL;
    if(classname == wxT("wxStatusBar")) return ID_WXSTATUSBAR;
    if(classname == wxT("unknown")) return ID_WXCUSTOMCONTROL;       // XRC
    if(classname == wxT("CustomControl")) return ID_WXCUSTOMCONTROL; // wxFB
    if(classname == wxT("Custom")) return ID_WXCUSTOMCONTROL;        // wxSmith
    if(classname == wxT("wxDataViewListCtrl")) return ID_WXDATAVIEWLISTCTRL;
    if(classname == wxT("wxDataViewColumn")) return ID_WXDATAVIEWCOL;
    if(classname == wxT("wxPopupWindow")) return ID_WXPOPUPWINDOW;
    if(classname == wxT("wxSimplebook")) return ID_WXSIMPLEBOOK;
    if(classname == wxT("wxScintilla")) return ID_WXSTC; // wxFB (sometimes?)
    if(classname == wxT("wxTreeListCtrl")) return ID_WXTREELISTCTRL;
    if(classname == wxT("wxAnimationCtrl")) return ID_WXANIMATIONCTRL;
    if(classname == wxT("wxSimpleHtmlListBox")) return ID_WXSIMPLEHTMLLISTBOX;
    if(classname == wxT("wxActivityIndicator")) return ID_WXACTIVITYINDICATOR;
    if(classname == wxT("wxTimePickerCtrl")) return ID_WXTIMEPICKERCTRL;
    return wxNOT_FOUND;
};
