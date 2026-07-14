#include "allocator_mgr.h"

#include "ActivityIndicatorWrapper.h"
#include "AnimationCtrlWrapper.h"
#include "AuiToolBarTopLevel.h"
#include "BitmapComboxWrapper.h"
#include "RearrangeListWrapper.h"
#include "SimpleHtmlListBoxWrapper.h"
#include "TimePickerCtrlWrapper.h"
#include "UI/EventsEditorDlg.h"
#include "aui_manager_wrapper.h"
#include "aui_notebook_wrapper.h"
#include "banner_window_wrapper.h"
#include "bitmap_button_wrapper.h"
#include "bitmap_wrapper.h"
#include "bitmaptogglebuttonwrapper.h"
#include "box_sizer_wrapper.h"
#include "button_wrapper.h"
#include "calendar_ctrl_wrapper.h"
#include "check_box_wrapper.h"
#include "check_list_box_wrapper.h"
#include "choice_book_wrapper.h"
#include "choice_wrapper.h"
#include "colour_picker_wrapper.h"
#include "combobox_wrapper.h"
#include "command_link_button_wrapper.h"
#include "controls/Containers/collapsible_pane_wrapper.h"
#include "controls/Containers/notebook_page_wrapper.h"
#include "controls/Containers/notebook_wrapper.h"
#include "controls/Containers/panel_wrapper.h"
#include "controls/Containers/scrolled_window_wrapper.h"
#include "controls/Containers/tool_book_wrapper.h"
#include "controls/Containers/tree_book_wrapper.h"
#include "controls/Containers/wizard_page_wrapper.h"
#include "controls/Containers/wx_collapsible_pane_pane_wrapper.h"
#include "controls/Grid/grid_column_wrapper.h"
#include "controls/Grid/grid_row_wrapper.h"
#include "controls/Grid/grid_wrapper.h"
#include "controls/PropertyGrid/property_grid_manager_wrapper.h"
#include "controls/PropertyGrid/property_grid_wrapper.h"
#include "controls/RibbonBar/ribbon_bar_wrapper.h"
#include "controls/RibbonBar/ribbon_button.h"
#include "controls/RibbonBar/ribbon_button_bar_wrapper.h"
#include "controls/RibbonBar/ribbon_gallery_item_wrapper.h"
#include "controls/RibbonBar/ribbon_gallery_wrapper.h"
#include "controls/RibbonBar/ribbon_page_wrapper.h"
#include "controls/RibbonBar/ribbon_panel_wrapper.h"
#include "controls/RibbonBar/ribbon_tool_bar_wrapper.h"
#include "controls/RibbonBar/ribbon_tool_separator.h"
#include "controls/menu_toolbar/menu_bar_wrapper.h"
#include "controls/menu_toolbar/menu_item_wrapper.h"
#include "controls/menu_toolbar/menu_wrapper.h"
#include "controls/menu_toolbar/status_bar_wrapper.h"
#include "controls/menu_toolbar/tool_bar_item_wrapper.h"
#include "controls/menu_toolbar/toolbar_base_wrapper.h"
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
#include "globals.h"
#include "grid_bag_sizer_wrapper.h"
#include "grid_sizer_wrapper.h"
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
#include "panel_wrapper_top_level.h"
#include "popup_window_wrapper.h"
#include "radio_box_wrapper.h"
#include "radio_button_wrapper.h"
#include "rich_text_ctrl_wrapper.h"
#include "scroll_bar_wrapper.h"
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
#include "std_button_wrapper.h"
#include "std_dialog_button_sizer_wrapper.h"
#include "styled_text_ctrl_wrapper.h"
#include "task_bar_icon_wrapper.h"
#include "text_ctrl_wrapper.h"
#include "timer_wrapper.h"
#include "toggle_button_wrapper.h"
#include "tree_ctrl_wrapper.h"
#include "tree_list_ctrl_column_wrapper.h"
#include "tree_list_ctrl_wrapper.h"
#include "web_view_wrapper.h"
#include "wizard_wrapper.h"
#include "wxc_widget.h"
#include "wxgui_bitmaploader.h"

Allocator* Allocator::ms_instance = 0;

// --------------------------------------------------------

namespace
{
wxImageList* CreateAllocatorImageList() { return new wxImageList(16, 16, true); }

int AddImageToAllocator(wxImageList* list, const wxCrafter::ResourceLoader& loader, const wxString& bmpName)
{
    if (!list) {
        return -1;
    }
    return list->Add(loader.Bitmap(bmpName));
}

} // namespace

Allocator::Allocator()
    : m_imageList(CreateAllocatorImageList())
{
    // Add the tree control images
    AddImageToAllocator(m_imageList, m_bmpLoader, wxT("wxgui"));

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
    Register(new ComboBoxWrapper(), "wxcombobox");
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
    Register(new NotebookPageWrapper(),
             "notebook-page",
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
    Register(new BitmapWrapper(), "wxbitmap");
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
    Register(new ActivityIndicatorWrapper(), "wxactivityindicator");
    Register(new TimePickerCtrlWrapper(), "wxtimepickerctrl");

    // ADD_NEW_CONTROL
    DoLinkAll();
}

Allocator* Allocator::Instance()
{
    if (ms_instance == 0) {
        ms_instance = new Allocator();
    }
    return ms_instance;
}

void Allocator::Release()
{
    delete ms_instance;
    ms_instance = nullptr;
}

void Allocator::Register(wxcWidget* obj, const wxString& bmpname, int id)
{
    int objId = id;
    if (objId == -1) {
        objId = obj->GetType();
    }
    m_imageIds[objId] = AddImageToAllocator(m_imageList, m_bmpLoader, bmpname);
}

int Allocator::GetImageId(int controlId) const
{
    ImageMap_t::const_iterator iter = m_imageIds.find(controlId);
    if (iter == m_imageIds.end())
        return -1;
    return iter->second;
}

bool Allocator::CanPaste(wxcWidget* source, wxcWidget* target) const
{
    CHECK_POINTER_RET_FALSE(source);
    CHECK_POINTER_RET_FALSE(target);
    int insertType = Allocator::Instance()->GetInsertionType(source->GetType(), target->GetType(), false);

    // we don't allow moving around top level items
    if (source->IsTopWindow() && target->IsTopWindow())
        // allow copying top level windows
        return true;

    if (insertType == Allocator::INSERT_CHILD) {
        return true;

    } else if (insertType == Allocator::INSERT_MAIN_SIZER) {
        if (target->HasMainSizer())
            return false;
        else
            return true;
    } else if (insertType == Allocator::INSERT_SIBLING) {
        // check that the parent can accept it as a child
        CHECK_POINTER_RET_FALSE(target->GetParent());
        int parentInsertType =
            Allocator::Instance()->GetInsertionType(source->GetType(), target->GetParent()->GetType(), false);
        return parentInsertType == Allocator::INSERT_CHILD;

    } else {
        return false;
    }
}

void Allocator::DoLink(int selected, int aboutToBeInsert, int relation)
{
    InsertMap_t::iterator iter = m_relations.find(selected);
    if (iter == m_relations.end()) {
        m_relations.insert(std::make_pair(selected, std::map<int, int>()));
    }
    std::map<int, int>& m = m_relations[selected];
    m.insert(std::make_pair(aboutToBeInsert, relation));
}

int Allocator::GetInsertionType(int controlId, int targetControlId, bool allowPrompt, wxcWidget* selectedWidget) const
{
    int aboutToBeInsert = wxcWidget::GetWidgetType(controlId);
    int selected = wxcWidget::GetWidgetType(targetControlId);

    bool selectionIsDirectChildOfAUI = false;
    if (selectedWidget && selectedWidget->GetParent() && selectedWidget->GetParent()->GetType() == ID_WXAUIMANAGER) {
        // the selected widget is a direct child of wxAuiManager
        selectionIsDirectChildOfAUI = true;
    }

    InsertMap_t::const_iterator iter = m_relations.find(selected);
    if (iter == m_relations.end())
        return INSERT_NONE;

    // Get the relations allowed with 'selected'
    const std::map<int, int>& m = m_relations.find(selected)->second;
    std::map<int, int>::const_iterator itRelation = m.find(aboutToBeInsert);
    if (itRelation == m.end())
        return INSERT_NONE;

    if (selectionIsDirectChildOfAUI && itRelation->second == INSERT_SIBLING) {
        // allow sibling only if wxAuiManager allows to accept it as child
        const std::map<int, int>& m = m_relations.find(TYPE_AUI_MGR)->second;
        std::map<int, int>::const_iterator it = m.find(aboutToBeInsert);
        if (it == m.end()) {
            // not allowed
            return INSERT_NONE;
        }
    }
    int type = itRelation->second;
    if (type == INSERT_PROMPT_CHILD_OR_SIBLING && allowPrompt) {
        // there is an ambiguity to resolve here - prompt the user
        int res =
            ::PromptForYesNoDialogWithCheckbox(_("This item can be placed either as a sibling or as a child of the "
                                                 "target widget\nWhere should wxCrafter position this widget?"),
                                               "wxCrafterInsertionType",
                                               _("As a Sibling"),
                                               _("As a Child"));
        if (res == wxID_YES) {
            return INSERT_SIBLING;

        } else {
            return INSERT_CHILD;
        }

    } else if (type == INSERT_PROMPT_CHILD_OR_SIBLING) {
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
