#include "ToolBoxPanel.h"

#include "UI/wxguicraft_main_view.h"
#include "allocator_mgr.h"
#include "aui/clAuiBook.hpp"
#include "cl_command_event.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "wxgui_bitmaploader.h"

#include <vector>
#include <wx/bmpbuttn.h>
#include <wx/intl.h>

namespace
{
/// A button of the palette: the control that it adds
struct ToolBoxTool {
    int id;              // The ID of the control (it is also the ID of the button)
    const char* icon;    // The name of the bitmap
    const char* tooltip; // Not translated yet
};

/// A group of buttons, laid in a grid. Groups are placed side by side.
struct ToolBoxGroup {
    int columns;
    bool separatorBefore; // A vertical line between this group and the previous one
    std::vector<ToolBoxTool> tools;
};

/// A page of the palette
struct ToolBoxPage {
    const char* title; // Not translated yet
    const char* icon;  // The name of the bitmap of the tab, or nullptr
    bool selected;
    std::vector<ToolBoxGroup> groups;
};

// clang-format off
const std::vector<ToolBoxPage>& GetToolBoxPages()
{
    static const std::vector<ToolBoxPage> pages = {
        {wxTRANSLATE("Forms"), "wxdialog", true, {
            {4, false, {
                {ID_WXDIALOG, "wxdialog", wxTRANSLATE("wxDialog")},
                {ID_WXFRAME, "wxframe", wxTRANSLATE("wxFrame")},
                {ID_WXPANEL_TOPLEVEL, "wxpanel", wxTRANSLATE("wxPanel")},
                {ID_WXWIZARD, "wxwizard", wxTRANSLATE("wxWizard")},
            }},
            {2, true, {
                {ID_WXIMAGELIST, "wximglist", wxTRANSLATE("wxImagList")},
                {ID_WXPOPUPWINDOW, "wxpopupwindow", wxTRANSLATE("wxPopupWindow")},
                {ID_WXPOPUPTRANSIENTWINDOW, "wxpopuptransientwindow", wxTRANSLATE("wxPopupTransientWindow")},
                {ID_WXBITMAP, "wxbitmap", wxTRANSLATE("wxBitmap")},
            }},
        }},
        {wxTRANSLATE("Sizers"), "wxflexgridsizer", false, {
            {5, false, {
                {ID_WXBOXSIZER, "wxboxsizer_v", wxTRANSLATE("wxBoxSizer")},
                {ID_WXFLEXGRIDSIZER, "wxflexgridsizer", wxTRANSLATE("wxFlexGridSizer")},
                {ID_WXSTATICBOXSIZER, "wxstaticboxsizer", wxTRANSLATE("wxStaticBoxSizer")},
                {ID_WXGRIDSIZER, "wxgridsizer", wxTRANSLATE("wxGridSizer")},
                {ID_WXGRIDBAGSIZER, "wxgridbagsizer", wxTRANSLATE("wxGridBagSizer")},
                {ID_WXSTDDLGBUTTONSIZER, "stddlgbuttonsizer", wxTRANSLATE("wxStdDialogButtonSizer")},
                {ID_WXSTDBUTTON, "wxbutton", wxTRANSLATE("A standard wxButton that can be placed in side a wxStdDialogButtonSizer class")},
            }},
            {1, true, {
                {ID_WXSPACER, "spacer", wxTRANSLATE("Stretchable spacer")},
            }},
            {1, false, {
                {ID_WXAUIMANAGER, "wxauimgr", wxTRANSLATE("wxAuiManager")},
            }},
        }},
        {wxTRANSLATE("Containers"), "wxnotebook", false, {
            {2, false, {
                {ID_WXPANEL, "wxpanel", wxTRANSLATE("wxPanel")},
                {ID_WXSCROLLEDWIN, "wxscrolledwindow", wxTRANSLATE("wxScrolledWindow")},
                {ID_WXSPLITTERWINDOW, "wxsplitterwindow", wxTRANSLATE("wxSplitterWindow")},
                {ID_WXSPLITTERWINDOW_PAGE, "wxpanel", wxTRANSLATE("Splitter Window Page")},
                {ID_WXPANEL_NOTEBOOK_PAGE, "notebook-page", wxTRANSLATE("Notebook Page")},
                {ID_WXWIZARDPAGE, "wxwizardpage", wxTRANSLATE("wxWizardPage")},
            }},
            {3, true, {
                {ID_WXNOTEBOOK, "wxnotebook", wxTRANSLATE("wxNotebook")},
                {ID_WXLISTBOOK, "wxlistbook", wxTRANSLATE("wxListbook")},
                {ID_WXCHOICEBOOK, "wxchoicebook", wxTRANSLATE("wxChoicebook")},
                {ID_WXTREEBOOK, "wxtreebook", wxTRANSLATE("wxTreebook")},
                {ID_WXTOOLBOOK, "wxtoolbook", wxTRANSLATE("wxToolbook")},
                {ID_WXAUINOTEBOOK, "wxauinotebook", wxTRANSLATE("wxAuiNotebook")},
                {ID_WXSIMPLEBOOK, "wxsimplebook", wxTRANSLATE("wxSimplebook")},
            }},
            {1, true, {
                {ID_WXCOLLAPSIBLEPANE, "wxcollapsiblepane", wxTRANSLATE("wxCollapsiblePane")},
                {ID_WXCOLLAPSIBLEPANE_PANE, "wxpanel", wxTRANSLATE("wxCollapsiblePane Pane")},
            }},
            {2, true, {
                {ID_WXGLCANVAS, "wxglcanvas", wxTRANSLATE("wxGLCanvas")},
            }},
        }},
        {wxTRANSLATE("Controls"), "wxbutton", false, {
            {3, false, {
                {ID_WXBUTTON, "wxbutton", wxTRANSLATE("wxButton")},
                {ID_WXBITMAPBUTTON, "wxbitmapbutton", wxTRANSLATE("wxBitmapButton")},
                {ID_WXHYPERLINK, "wxhyperlink", wxTRANSLATE("wxHyperlinkCtrl")},
                {ID_WXTOGGLEBUTTON, "wxtogglebutton", wxTRANSLATE("wxToggleButton")},
                {ID_WXSPINBUTTON, "wxspinbutton", wxTRANSLATE("wxSpinButton")},
                {ID_WXSPINCTRL, "wxspinctrl", wxTRANSLATE("wxSpinCtrl")},
                {ID_WXBITMAPTOGGLEBUTTON, "wxbitmaptogglebutton", wxTRANSLATE("wxBitmapToggleButton")},
            }},
            {2, true, {
                {ID_WXCHECKBOX, "wxcheckbox", wxTRANSLATE("wxCheckBox")},
                {ID_WXRADIOBOX, "wxradiobox", wxTRANSLATE("wxRadioBox")},
                {ID_WXRADIOBUTTON, "wxradiobutton", wxTRANSLATE("wxRadioButton")},
            }},
            {3, true, {
                {ID_WXSTATICTEXT, "wxstatictext", wxTRANSLATE("wxStaticText")},
                {ID_WXSTATICLINE, "wxstaticline", wxTRANSLATE("wxStaticLine")},
                {ID_WXTEXTCTRL, "wxtextctrl", wxTRANSLATE("wxTextCtrl")},
                {ID_WXRICHTEXT, "wxrichtextctrl", wxTRANSLATE("wxRichTextCtrl")},
                {ID_WXSTATICBITMAP, "wxbitmap", wxTRANSLATE("wxStaticBitmap")},
                {ID_WXHTMLWIN, "wxhtmlwindow", wxTRANSLATE("wxHtmlWindow")},
                {ID_WXSTC, "stc", wxTRANSLATE("wxStyledTextCtrl")},
            }},
            {2, true, {
                {ID_WXCHOICE, "wxchoice", wxTRANSLATE("wxChoice")},
                {ID_WXCOMBOBOX, "wxcombobox", wxTRANSLATE("wxComboBox")},
                {ID_WXBITMAPCOMBOBOX, "wxbitmapcombobox", wxTRANSLATE("wxBitmapComboBox")},
            }},
            {3, true, {
                {ID_WXLISTBOX, "wxlistbox", wxTRANSLATE("wxListBox")},
                {ID_WXSIMPLEHTMLLISTBOX, "wxhtmllistbox", wxTRANSLATE("wxSimpleHtmlListBox")},
                {ID_WXCHECKLISTBOX, "wxchecklistbox", wxTRANSLATE("wxCheckListBox")},
                {ID_WXREARRANGELIST, "wxrearrangelist", wxTRANSLATE("wxRearrangeList")},
                {ID_WXLISTCTRL, "wxlistctrl", wxTRANSLATE("wxListCtrl")},
                {ID_WXGRID, "wxgrid", wxTRANSLATE("wxGrid")},
                {ID_WXTREECTRL, "wxtreectrl", wxTRANSLATE("wxTreeCtrl")},
            }},
            {2, true, {
                {ID_WXACTIVITYINDICATOR, "wxactivityindicator", wxTRANSLATE("wxActivityIndicator")},
                {ID_WXSLIDER, "wxslider", wxTRANSLATE("wxSlider")},
                {ID_WXGAUGE, "wxgauge", wxTRANSLATE("wxGauge")},
                {ID_WXSCROLLBAR, "wxscrollbar", wxTRANSLATE("wxScrollBar")},
            }},
            {4, true, {
                {ID_WXSEARCHCTRL, "wxsearchctrl", wxTRANSLATE("wxSearchCtrl")},
                {ID_WXCOLORPICKER, "wxcolourpicker", wxTRANSLATE("wxColourPickerCtrl")},
                {ID_WXFONTPICKER, "wxfontpickerctrl", wxTRANSLATE("wxFontPickerCtrl")},
                {ID_WXFILEPICKER, "wxfilepickerctrl", wxTRANSLATE("wxFilePickerCtrl")},
                {ID_WXDIRPICKER, "wxdirpickerctrl", wxTRANSLATE("wxDirPickerCtrl")},
                {ID_WXDATEPICKER, "wxdatepickerctrl", wxTRANSLATE("wxDatePickerCtrl")},
                {ID_WXTIMEPICKERCTRL, "wxtimepickerctrl", wxTRANSLATE("wxTimePickerCtrl")},
                {ID_WXCALEDARCTRL, "wxcalendarctrl", wxTRANSLATE("wxCalendarCtrl")},
                {ID_WXGENERICDIRCTRL, "wxgenericdirctrl", wxTRANSLATE("wxGenericDirCtrl")},
                {ID_WXCUSTOMCONTROL, "custom", wxTRANSLATE("Custom Control")},
            }},
        }},
        {wxTRANSLATE("Menu / ToolBar"), "wxmenu", false, {
            {4, false, {
                {ID_WXMENUBAR, "wxmenubar", wxTRANSLATE("wxMenuBar")},
                {ID_WXMENU, "wxmenu", wxTRANSLATE("wxMenu")},
                {ID_WXSUBMENU, "wxsubmenu", wxTRANSLATE("Sub Menu")},
                {ID_WXMENUITEM, "wxmenuitem", wxTRANSLATE("wxMenuItem")},
            }},
            {2, true, {
                {ID_WXTOOLBAR, "wxtoolbar", wxTRANSLATE("wxToolBar")},
                {ID_WXAUITOOLBAR, "wxauitoolbar", wxTRANSLATE("wxAuiToolBar")},
                {ID_WXTOOLBARITEM, "wxmenuitem", wxTRANSLATE("Toolbar Item")},
                {ID_WXTOOLBARITEM_SEPARATOR, "wxstaticline_v", wxTRANSLATE("Toolbar Separator")},
            }},
            {2, true, {
                {ID_WXSTATUSBAR, "wxstatusbar", wxTRANSLATE("wxStatusBar")},
            }},
        }},
        {wxTRANSLATE("wxRibbonBar"), "wxribbonbar", false, {
            {2, false, {
                {ID_WXRIBBONBAR, "wxribbonbar", wxTRANSLATE("wxRibbonBar")},
                {ID_WXRIBBONPAGE, "wxribbonpage", wxTRANSLATE("wxRibbonPage")},
                {ID_WXRIBBONPANEL, "wxribbonpanel", wxTRANSLATE("wxRibbonPanel")},
            }},
            {2, true, {
                {ID_WXRIBBONBUTTONBAR, "wxribbonbuttonbar", wxTRANSLATE("wxRibbonButtonBar")},
                {ID_WXRIBBONBUTTON, "wxribbonbutton", wxTRANSLATE("wxRibbonButton (Normal)")},
                {ID_WXRIBBONDROPDOWNBUTTON, "wxribbonbutton_dropdown", wxTRANSLATE("wxRibbonButton (Dropdown)")},
                {ID_WXRIBBONHYBRIDBUTTON, "wxribbonbutton_hybrid", wxTRANSLATE("wxRibbonButton (Hybrid)")},
                {ID_WXRIBBONTOGGLEBUTTON, "wxribbonbutton_toggle", wxTRANSLATE("wxRibbonButton (Toggle)")},
            }},
            {2, true, {
                {ID_WXRIBBONTOOLBAR, "wxtoolbar", wxTRANSLATE("wxRibbonToolBar")},
                {ID_WXRIBBONTOOL, "wxribbontool", wxTRANSLATE("wxRibbonTool (Normal)")},
                {ID_WXRIBBONDROPDOWNTOOL, "wxribbontool_dropdown", wxTRANSLATE("wxRibbonTool (Dropdown)")},
                {ID_WXRIBBONHYBRIDTOOL, "wxribbontool_hybrid", wxTRANSLATE("wxRibbonTool (Hybrid)")},
                {ID_WXRIBBONTOGGLETOOL, "wxribbontool_toggle", wxTRANSLATE("wxRibbonTool (Toggle)")},
                {ID_WXRIBBONTOOLSEPARATOR, "wxstaticline", wxTRANSLATE("wxRibbonTool (Separator)")},
            }},
            {1, true, {
                {ID_WXRIBBONGALLERY, "wxribbongallery", wxTRANSLATE("wxRibbonGallery")},
                {ID_WXRIBBONGALLERYITME, "wxribbongalleryitem", wxTRANSLATE("wxRibbonGalleryItem")},
            }},
        }},
        {wxTRANSLATE("Advanced"), nullptr, false, {
            {3, false, {
                {ID_WXDATAVIEWLISTCTRL, "wxdataviewlistctrl", wxTRANSLATE("wxDataViewListCtrl")},
                {ID_WXDATAVIEWTREECTRL, "wxdataviewtreectrl", wxTRANSLATE("wxDataViewTreeCtrl")},
                {ID_WXDATAVIEWTREELISTCTRL, "wxdataviewctrl", wxTRANSLATE("wxDataViewCtrl (with tree-list custom model)")},
                {ID_WXDATAVIEWCOL, "wxcolumn", wxTRANSLATE("Add a column...")},
            }},
            {2, true, {
                {ID_WXPROPERTYGRIDMANAGER, "wxpropertygridmanager", wxTRANSLATE("wxPropertyGridManager")},
                {ID_WXTREELISTCTRL, "wxtreelistctrl", wxTRANSLATE("wxTreeListCtrl")},
                {ID_WXPGPROPERTY, "wxpgproperty", wxTRANSLATE("wxPGProperty")},
            }},
            {2, true, {
                {ID_WXBANNERWINDOW, "wxbannerwindow", wxTRANSLATE("wxBannerWindow")},
                {ID_WXCOMMANDLINKBUTTON, "wxcommandlinkbutton", wxTRANSLATE("wxCommandLinkButton")},
                {ID_WXTIMER, "wxtimer", wxTRANSLATE("wxTimer")},
                {ID_WXTASKBARICON, "wxtaskbaricon", wxTRANSLATE("wxTaskBarIcon")},
            }},
            {1, false, {
                {ID_WXINFOBAR, "wxinfobar", wxTRANSLATE("wxInfoBar")},
                {ID_WXINFOBARBUTTON, "wxbutton", wxTRANSLATE("wxInfoBar Button")},
            }},
            {2, true, {
                {ID_WXWEBVIEW, "wxwebview", wxTRANSLATE("wxWebView")},
                {ID_WXANIMATIONCTRL, "wxanimationctrl", wxTRANSLATE("wxAnimationCtrl")},
                {ID_WXMEDIACTRL, "wxmediactrl-16", wxTRANSLATE("wxMediaCtrl")},
            }},
        }},
    };
    return pages;
}
// clang-format on
} // namespace

ToolBoxPanel::ToolBoxPanel(wxWindow* parent, GUICraftMainPanel* mainView)
    : wxPanel(parent, wxID_ANY)
    , m_mainView(mainView)
{
    SetSizer(new wxBoxSizer(wxVERTICAL));

    // The tabs are not closable or movable
    m_notebook =
        new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TOP | wxAUI_NB_SCROLL_BUTTONS);
    clAuiBook::ApplyTabArt(m_notebook);
    GetSizer()->Add(m_notebook, 1, wxALL | wxEXPAND, FromDIP(3));

    const wxCrafter::ResourceLoader bitmaps;
    for (const ToolBoxPage& pageInfo : GetToolBoxPages()) {
        wxPanel* page = new wxPanel(m_notebook);
        wxBoxSizer* pageSizer = new wxBoxSizer(wxHORIZONTAL);
        page->SetSizer(pageSizer);

        for (const ToolBoxGroup& group : pageInfo.groups) {
            if (group.separatorBefore) {
                pageSizer->Add(new wxStaticLine(page, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL),
                               0,
                               wxALL | wxEXPAND,
                               FromDIP(3));
            }

            wxFlexGridSizer* grid = new wxFlexGridSizer(0, group.columns, 0, 0);
            pageSizer->Add(grid, 0, wxALIGN_LEFT, FromDIP(3));
            for (const ToolBoxTool& tool : group.tools) {
                wxBitmapButton* button = new wxBitmapButton(
                    page, tool.id, bitmaps.Bundle(tool.icon), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
                button->SetToolTip(wxGetTranslation(tool.tooltip));
                button->Bind(wxEVT_UPDATE_UI, &ToolBoxPanel::OnControlUI, this);
                grid->Add(button, 0, wxALL, FromDIP(3));
            }
        }

        const wxBitmapBundle tabIcon = pageInfo.icon != nullptr ? bitmaps.Bundle(pageInfo.icon) : wxBitmapBundle();
#if wxCHECK_VERSION(3, 3, 0)
        m_notebook->AddPage(page, wxGetTranslation(pageInfo.title), pageInfo.selected, tabIcon);
#else
        m_notebook->AddPage(page,
                            wxGetTranslation(pageInfo.title),
                            pageInfo.selected,
                            tabIcon.IsOk() ? tabIcon.GetBitmap(wxSize(16, 16)) : wxNullBitmap);
#endif
    }

    GetSizer()->Fit(this);
    EventNotifier::Get()->Bind(wxEVT_CMD_COLOURS_FONTS_UPDATED, &ToolBoxPanel::OnEditorColoursUpdated, this);
}

ToolBoxPanel::~ToolBoxPanel()
{
    EventNotifier::Get()->Unbind(wxEVT_CMD_COLOURS_FONTS_UPDATED, &ToolBoxPanel::OnEditorColoursUpdated, this);
}

void ToolBoxPanel::OnEditorColoursUpdated(clCommandEvent& event)
{
    event.Skip();
    clAuiBook::ApplyTabArt(m_notebook);
    m_notebook->Refresh();
}

void ToolBoxPanel::OnControlUI(wxUpdateUIEvent& event)
{

    GUICraftItemData* data = m_mainView->GetSelItemData();

    if (data && data->m_wxcWidget) {
        wxcWidget* widget = data->m_wxcWidget;

        if (widget->GetType() == ID_WXSCROLLEDWIN && event.GetId() == ID_WXAUIMANAGER &&
            Allocator::Instance()->GetInsertionType(event.GetId(), data->m_wxcWidget->GetType(), false) ==
                Allocator::INSERT_CHILD) {
            event.Enable(false);

        } else if (widget->IsTopWindow() && widget->HasMenuBar() && event.GetId() == ID_WXMENUBAR) {
            event.Enable(false);

        } else if (widget->IsTopWindow() && widget->HasToolBar() && event.GetId() == ID_WXTOOLBAR) {
            event.Enable(false);

        } else if (widget->IsTopWindow() && widget->HasStatusBar() && event.GetId() == ID_WXSTATUSBAR) {
            event.Enable(false);

        } else if (Allocator::Instance()->GetInsertionType(
                       event.GetId(), data->m_wxcWidget->GetType(), false, widget) != Allocator::INSERT_NONE) {
            event.Enable(true);

        } else {
            event.Enable(false);
        }

    } else if (!data) {
        // Root item is selected
        int type = wxcWidget::GetWidgetType(event.GetId());
        event.Enable(type == TYPE_FORM || type == TYPE_WIZARD || type == TYPE_FORM_FRAME || type == TYPE_IMGLIST);

    } else {
        event.Enable(false);
    }
}
