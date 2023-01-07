#include "tree_list_ctrl_wrapper.h"

#include "allocator_mgr.h"

#include <wx/treelist.h>

TreeListCtrlWrapper::TreeListCtrlWrapper()
    : wxcWidget(ID_WXTREELISTCTRL)
{
    SetPropertyString(_("Common Settings"), "wxTreeListCtrl");
    PREPEND_STYLE(wxTL_SINGLE, false);
    PREPEND_STYLE(wxTL_MULTIPLE, false);
    PREPEND_STYLE(wxTL_CHECKBOX, false);
    PREPEND_STYLE(wxTL_3STATE, false);
    PREPEND_STYLE(wxTL_USER_3STATE, false);
    PREPEND_STYLE(wxTL_DEFAULT_STYLE, true);

    RegisterEvent(
        "wxEVT_TREELIST_SELECTION_CHANGED", "wxTreeListEvent",
        _("Process wxEVT_TREELIST_SELECTION_CHANGED event and notifies about the selection change in the "
          "control. In the single selection case the item indicated by the event has been selected and "
          "previously selected item, if any, was deselected. In multiple selection case, the selection of this "
          "item has just changed (it may have been either selected or deselected) but notice that the "
          "selection of other items could have changed as well, use wxTreeListCtrl::GetSelections() to "
          "retrieve the new selection if necessary."));
    RegisterEvent("wxEVT_TREELIST_ITEM_EXPANDING", "wxTreeListEvent",
                  _("Process wxEVT_TREELIST_ITEM_EXPANDING event notifying about the given branch being expanded. This "
                    "event is sent before the expansion occurs and can be vetoed to prevent it from happening."));
    RegisterEvent(
        "wxEVT_TREELIST_ITEM_EXPANDED", "wxTreeListEvent",
        _("Process wxEVT_TREELIST_ITEM_EXPANDED event notifying about the expansion of the given branch. This "
          "event is sent after the expansion occurs and can't be vetoed."));
    RegisterEvent(
        "wxEVT_TREELIST_ITEM_CHECKED", "wxTreeListEvent",
        _("Process wxEVT_TREELIST_ITEM_CHECKED event notifying about the user checking or unchecking the item. "
          "You can use wxTreeListCtrl::GetCheckedState() to retrieve the new item state and "
          "wxTreeListEvent::GetOldCheckedState() to get the previous one."));
    RegisterEvent("wxEVT_TREELIST_ITEM_ACTIVATED", "wxTreeListEvent",
                  _("Process wxEVT_TREELIST_ITEM_ACTIVATED event notifying about the user double clicking the item or "
                    "activating it from keyboard."));
    RegisterEvent("wxEVT_TREELIST_ITEM_CONTEXT_MENU", "wxTreeListEvent",
                  _("Process wxEVT_TREELIST_ITEM_CONTEXT_MENU event indicating that the popup menu for the given item "
                    "should be displayed."));
    RegisterEvent("wxEVT_TREELIST_COLUMN_SORTED", "wxTreeListEvent",
                  _("Process wxEVT_TREELIST_COLUMN_SORTED event indicating that the control contents has just been "
                    "resorted using the specified column. The event doesn't carry the sort direction, use "
                    "GetSortColumn() method if you need to know it."));

    m_namePattern = "m_treeListCtrl";
    SetName(GenerateName());
}

TreeListCtrlWrapper::~TreeListCtrlWrapper() {}

wxcWidget* TreeListCtrlWrapper::Clone() const { return new TreeListCtrlWrapper(); }

wxString TreeListCtrlWrapper::CppCtorCode() const
{
    //  wxTreeListCtrl::wxTreeListCtrl  (   wxWindow * 	parent,
    //    wxWindowID    id,
    //    const wxPoint & 	pos = wxDefaultPosition,
    //    const wxSize & 	size = wxDefaultSize,
    //    long 	style = wxTL_DEFAULT_STYLE,
    //    const wxString & 	name = wxTreeListCtrlNameStr
    //    );

    wxString code;
    code << GetName() << " = new " << GetRealClassName() << "(" << GetWindowParent() << ", " << WindowID() << ", "
         << "wxDefaultPosition, " << SizeAsString() << ", " << StyleFlags("wxTL_DEFAULT_STYLE") << ");\n";
    code << CPPCommonAttributes();
    return code;
}

void TreeListCtrlWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add("#include <wx/treelist.h>"); }

wxString TreeListCtrlWrapper::GetWxClassName() const { return "wxTreeListCtrl"; }

void TreeListCtrlWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << XRCPrefix() << XRCStyle() << XRCSize() << XRCCommonAttributes();

    // add columns
    ChildrenXRC(text, type);

    text << XRCSuffix();
}
