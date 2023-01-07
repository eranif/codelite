#include "data_view_list_ctrl_wrapper.h"

#include "allocator_mgr.h"

#include <wx/dataview.h>

DataViewListCtrlWrapper::DataViewListCtrlWrapper()
    : wxcWidget(ID_WXDATAVIEWLISTCTRL)
{
    PREPEND_STYLE(wxDV_SINGLE, true);
    PREPEND_STYLE(wxDV_MULTIPLE, false);
    PREPEND_STYLE(wxDV_ROW_LINES, true);
    PREPEND_STYLE(wxDV_HORIZ_RULES, false);
    PREPEND_STYLE(wxDV_VERT_RULES, false);
    PREPEND_STYLE(wxDV_VARIABLE_LINE_HEIGHT, false);
    PREPEND_STYLE(wxDV_NO_HEADER, false);

    RegisterEvent("wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED", "wxDataViewEvent",
                  _("Process a wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED event"));
    RegisterEvent("wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED", "wxDataViewEvent",
                  _("Process a wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED event.\nThis event is triggered by double "
                    "clicking an item or pressing some special key (usually \"Enter\") when it is focused"));
    RegisterEvent("wxEVT_COMMAND_DATAVIEW_ITEM_START_EDITING", "wxDataViewEvent",
                  _("Process a wxEVT_COMMAND_DATAVIEW_ITEM_START_EDITING event.\nThis event can be vetoed in order to "
                    "prevent editing on an item by item basis"));
    RegisterEvent("wxEVT_COMMAND_DATAVIEW_ITEM_EDITING_STARTED", "wxDataViewEvent",
                  _("Process a wxEVT_COMMAND_DATAVIEW_ITEM_EDITING_STARTED event"));
    RegisterEvent("wxEVT_COMMAND_DATAVIEW_ITEM_EDITING_DONE", "wxDataViewEvent",
                  _("Process a wxEVT_COMMAND_DATAVIEW_ITEM_EDITING_DONE event"));
    RegisterEvent("wxEVT_COMMAND_DATAVIEW_ITEM_COLLAPSING", "wxDataViewEvent",
                  _("Process a wxEVT_COMMAND_DATAVIEW_ITEM_COLLAPSING event"));
    RegisterEvent("wxEVT_COMMAND_DATAVIEW_ITEM_COLLAPSED", "wxDataViewEvent",
                  _("Process a wxEVT_COMMAND_DATAVIEW_ITEM_COLLAPSED event"));
    RegisterEvent("wxEVT_COMMAND_DATAVIEW_ITEM_EXPANDING", "wxDataViewEvent",
                  _("Process a wxEVT_COMMAND_DATAVIEW_ITEM_EXPANDING event"));
    RegisterEvent("wxEVT_COMMAND_DATAVIEW_ITEM_EXPANDED", "wxDataViewEvent",
                  _("Process a wxEVT_COMMAND_DATAVIEW_ITEM_EXPANDED event"));
    RegisterEvent("wxEVT_COMMAND_DATAVIEW_ITEM_VALUE_CHANGED", "wxDataViewEvent",
                  _("Process a wxEVT_COMMAND_DATAVIEW_ITEM_VALUE_CHANGED event"));
    RegisterEvent("wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU", "wxDataViewEvent",
                  _("Process a wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU event generated when the user right clicks "
                    "inside the control.\nNotice that this menu is generated even if the click didn't occur on any "
                    "valid item, in this case wxDataViewEvent::GetItem() simply returns an invalid item."));
    RegisterEvent("wxEVT_COMMAND_DATAVIEW_COLUMN_HEADER_CLICK", "wxDataViewEvent",
                  _("Process a wxEVT_COMMAND_DATAVIEW_COLUMN_HEADER_CLICK event"));
    RegisterEvent("wxEVT_COMMAND_DATAVIEW_COLUMN_HEADER_RIGHT_CLICK", "wxDataViewEvent",
                  _("Process a wxEVT_COMMAND_DATAVIEW_COLUMN_HEADER_RIGHT_CLICK event"));
    RegisterEvent("wxEVT_COMMAND_DATAVIEW_COLUMN_SORTED", "wxDataViewEvent",
                  _("Process a wxEVT_COMMAND_DATAVIEW_COLUMN_SORTED event"));
    RegisterEvent("wxEVT_COMMAND_DATAVIEW_COLUMN_REORDERED", "wxDataViewEvent",
                  _("Process a wxEVT_COMMAND_DATAVIEW_COLUMN_REORDERED event"));
    RegisterEvent("wxEVT_COMMAND_DATAVIEW_ITEM_BEGIN_DRAG", "wxDataViewEvent",
                  _("Process a wxEVT_COMMAND_DATAVIEW_ITEM_BEGIN_DRAG event"));
    RegisterEvent("wxEVT_COMMAND_DATAVIEW_ITEM_DROP_POSSIBLE", "wxDataViewEvent",
                  _("Process a wxEVT_COMMAND_DATAVIEW_ITEM_DROP_POSSIBLE event"));
    RegisterEvent("wxEVT_COMMAND_DATAVIEW_ITEM_DROP", "wxDataViewEvent",
                  _("Process a wxEVT_COMMAND_DATAVIEW_ITEM_DROP event"));

    SetPropertyString(_("Common Settings"), "wxDataViewListCtrl");
    m_namePattern = "m_dvListCtrl";
    DelProperty(_("Control Specific Settings"));
    SetName(GenerateName());
}

DataViewListCtrlWrapper::~DataViewListCtrlWrapper() {}

wxcWidget* DataViewListCtrlWrapper::Clone() const { return new DataViewListCtrlWrapper(); }

wxString DataViewListCtrlWrapper::CppCtorCode() const
{
    wxString code = CPPStandardWxCtor("wxDV_SINGLE|wxDV_ROW_LINES");
    return code;
}

void DataViewListCtrlWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add("#include <wx/dataview.h>"); }

wxString DataViewListCtrlWrapper::GetWxClassName() const { return "wxDataViewListCtrl"; }

void DataViewListCtrlWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    if(type == XRC_LIVE) {
        text << XRCUnknown();

    } else {
        text << XRCPrefix() << XRCSize() << XRCStyle() << XRCCommonAttributes();

        // Columns...
        ChildrenXRC(text, type);

        text << XRCSuffix();
    }
}
