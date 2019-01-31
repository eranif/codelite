#include "list_ctrl_wrapper.h"
#include "allocator_mgr.h"
#include "wxgui_defs.h"
#include <wx/listctrl.h>

ListCtrlWrapper::ListCtrlWrapper()
    : wxcWidget(ID_WXLISTCTRL)
{
    SetPropertyString(_("Common Settings"), "wxListCtrl");

    PREPEND_STYLE(wxLC_LIST, false);
    PREPEND_STYLE(wxLC_REPORT, true);
    PREPEND_STYLE(wxLC_VIRTUAL, false);
    PREPEND_STYLE(wxLC_ICON, false);
    PREPEND_STYLE(wxLC_SMALL_ICON, false);
    PREPEND_STYLE(wxLC_ALIGN_TOP, false);
    PREPEND_STYLE(wxLC_ALIGN_LEFT, false);
    PREPEND_STYLE(wxLC_EDIT_LABELS, false);
    PREPEND_STYLE(wxLC_NO_HEADER, false);
    PREPEND_STYLE(wxLC_SINGLE_SEL, false);
    PREPEND_STYLE(wxLC_SORT_ASCENDING, false);
    PREPEND_STYLE(wxLC_SORT_DESCENDING, false);
    PREPEND_STYLE(wxLC_HRULES, false);
    PREPEND_STYLE(wxLC_VRULES, false);

    RegisterEvent(wxT("wxEVT_COMMAND_LIST_ITEM_SELECTED"), wxT("wxListEvent"), wxT("The item has been selected"));
    RegisterEvent(wxT("wxEVT_COMMAND_LIST_ITEM_DESELECTED"), wxT("wxListEvent"), wxT("The item has been deselected"));
    RegisterEvent(wxT("wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK"), wxT("wxListEvent"),
                  wxT("The right mouse button has been clicked on an item"));
    RegisterEvent(wxT("wxEVT_COMMAND_LIST_ITEM_MIDDLE_CLICK"), wxT("wxListEvent"),
                  wxT("The middle mouse button has been clicked on an item"));
    RegisterEvent(wxT("wxEVT_COMMAND_LIST_ITEM_ACTIVATED"), wxT("wxListEvent"),
                  wxT("The item has been activated (ENTER or double click)"));
    RegisterEvent(wxT("wxEVT_COMMAND_LIST_ITEM_FOCUSED"), wxT("wxListEvent"),
                  wxT("The currently focused item has changed"));
    RegisterEvent(wxT("wxEVT_COMMAND_LIST_KEY_DOWN"), wxT("wxListEvent"), wxT("A key has been pressed"));

    RegisterEvent(wxT("wxEVT_COMMAND_LIST_COL_CLICK"), wxT("wxListEvent"),
                  wxT("A column (m_col) has been left-clicked"));
    RegisterEvent(wxT("wxEVT_COMMAND_LIST_COL_RIGHT_CLICK"), wxT("wxListEvent"),
                  wxT("A column (m_col) has been right-clicked"));
    RegisterEvent(wxT("wxEVT_COMMAND_LIST_COL_BEGIN_DRAG"), wxT("wxListEvent"),
                  wxT("The user started resizing a column - can be vetoed"));
    RegisterEvent(wxT("wxEVT_COMMAND_LIST_COL_DRAGGING"), wxT("wxListEvent"),
                  wxT("The divider between columns is being dragged"));
    RegisterEvent(wxT("wxEVT_COMMAND_LIST_COL_END_DRAG"), wxT("wxListEvent"),
                  wxT("A column has been resized by the user"));

    RegisterEvent(wxT("wxEVT_COMMAND_LIST_BEGIN_DRAG"), wxT("wxListEvent"),
                  wxT("Begin dragging with the left mouse button"));
    RegisterEvent(wxT("wxEVT_COMMAND_LIST_BEGIN_RDRAG"), wxT("wxListEvent"),
                  wxT("Begin dragging with the right mouse button."));
    RegisterEvent(wxT("wxEVT_COMMAND_LIST_BEGIN_LABEL_EDIT"), wxT("wxListEvent"),
                  wxT("Begin editing a label. This can be prevented by calling Veto()"));
    RegisterEvent(wxT("wxEVT_COMMAND_LIST_END_LABEL_EDIT"), wxT("wxListEvent"),
                  wxT("Finish editing a label. This can be prevented by calling Veto()"));
    RegisterEvent(wxT("wxEVT_COMMAND_LIST_DELETE_ITEM"), wxT("wxListEvent"), wxT("Delete an item"));
    RegisterEvent(wxT("wxEVT_COMMAND_LIST_DELETE_ALL_ITEMS"), wxT("wxListEvent"), wxT("Delete all items"));
    RegisterEvent(wxT("wxEVT_COMMAND_LIST_INSERT_ITEM"), wxT("wxListEvent"), wxT("An item has been inserted"));

    m_namePattern = wxT("m_listCtrl");
    SetName(GenerateName());
}

ListCtrlWrapper::~ListCtrlWrapper() {}

wxcWidget* ListCtrlWrapper::Clone() const { return new ListCtrlWrapper(); }

wxString ListCtrlWrapper::CppCtorCode() const
{
    // wxListCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size =
    // wxDefaultSize,
    //            long style = wxLC_ICON, const wxValidator& validator = wxDefaultValidator, const wxString& name =
    //            wxListCtrlNameStr)
    wxString code;
    code << GetName() << wxT(" = new ") << GetRealClassName() << "(" << GetWindowParent() << wxT(", ") << WindowID()
         << wxT(", ") << wxT("wxDefaultPosition, ") << SizeAsString() << wxT(", ") << StyleFlags(wxT("0"))
         << wxT(");\n");
    code << CPPCommonAttributes();
    return code;
}

void ListCtrlWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add(wxT("#include <wx/listctrl.h>")); }

wxString ListCtrlWrapper::GetWxClassName() const { return wxT("wxListCtrl"); }

void ListCtrlWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << XRCPrefix() << XRCStyle() << XRCSize() << XRCCommonAttributes();

    // add columns
    ChildrenXRC(text, type);

    text << XRCSuffix();
}
