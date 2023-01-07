#include "tree_ctrl_wrapper.h"
#include "allocator_mgr.h"
#include "wxgui_defs.h"
#include <wx/treectrl.h>

TreeCtrlWrapper::TreeCtrlWrapper()
    : wxcWidget(ID_WXTREECTRL)
{
    SetPropertyString(_("Common Settings"), "wxTreeCtrl");
    PREPEND_STYLE(wxTR_EDIT_LABELS, false);
    PREPEND_STYLE(wxTR_NO_BUTTONS, false);
    PREPEND_STYLE(wxTR_HAS_BUTTONS, false);
    PREPEND_STYLE(wxTR_TWIST_BUTTONS, false);
    PREPEND_STYLE(wxTR_NO_LINES, false);
    PREPEND_STYLE(wxTR_FULL_ROW_HIGHLIGHT, false);
    PREPEND_STYLE(wxTR_LINES_AT_ROOT, false);
    PREPEND_STYLE(wxTR_HIDE_ROOT, false);
    PREPEND_STYLE(wxTR_ROW_LINES, false);
    PREPEND_STYLE(wxTR_HAS_VARIABLE_ROW_HEIGHT, false);
    PREPEND_STYLE(wxTR_SINGLE, false);
    PREPEND_STYLE(wxTR_MULTIPLE, false);
    PREPEND_STYLE(wxTR_DEFAULT_STYLE, true);

    RegisterEvent(wxT("wxEVT_COMMAND_TREE_BEGIN_DRAG"), wxT("wxTreeEvent"),
                  _("The user has started dragging an item with the left mouse button.\nThe event handler must call "
                    "wxTreeEvent::Allow() for the drag operation to continue."));
    RegisterEvent(wxT("wxEVT_COMMAND_TREE_BEGIN_RDRAG"), wxT("wxTreeEvent"),
                  _("The user has started dragging an item with the right mouse button.\nThe event handler must call "
                    "wxTreeEvent::Allow() for the drag operation to continue."));
    RegisterEvent(wxT("wxEVT_COMMAND_TREE_END_DRAG"), wxT("wxTreeEvent"),
                  _("The user has released the mouse after dragging an item."));
    RegisterEvent(wxT("wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT"), wxT("wxTreeEvent"),
                  _("Begin editing a label. This can be prevented by calling Veto()."));
    RegisterEvent(wxT("wxEVT_COMMAND_TREE_END_LABEL_EDIT"), wxT("wxTreeEvent"),
                  _("The user has finished editing a label. This can be prevented by calling Veto()."));
    RegisterEvent(wxT("wxEVT_COMMAND_TREE_DELETE_ITEM"), wxT("wxTreeEvent"), _("A tree item has been deleted."));
    RegisterEvent(wxT("wxEVT_COMMAND_TREE_ITEM_EXPANDED"), wxT("wxTreeEvent"), _("The item has been expanded."));
    RegisterEvent(wxT("wxEVT_COMMAND_TREE_ITEM_EXPANDING"), wxT("wxTreeEvent"),
                  _("The item is being expanded. This can be prevented by calling Veto()."));
    RegisterEvent(wxT("wxEVT_COMMAND_TREE_ITEM_COLLAPSED"), wxT("wxTreeEvent"), _("The item has been collapsed."));
    RegisterEvent(wxT("wxEVT_COMMAND_TREE_ITEM_COLLAPSING"), wxT("wxTreeEvent"),
                  _("The item is being collapsed. This can be prevented by calling Veto()."));
    RegisterEvent(wxT("wxEVT_COMMAND_TREE_SEL_CHANGED"), wxT("wxTreeEvent"), _("Selection has changed."));
    RegisterEvent(wxT("wxEVT_COMMAND_TREE_SEL_CHANGING"), wxT("wxTreeEvent"),
                  _("Selection is changing. This can be prevented by calling Veto()."));
    RegisterEvent(wxT("wxEVT_COMMAND_TREE_KEY_DOWN"), wxT("wxTreeEvent"), _("A key has been pressed."));
    RegisterEvent(wxT("wxEVT_COMMAND_TREE_ITEM_ACTIVATED"), wxT("wxTreeEvent"),
                  _("An item has been activated (e.g. double clicked)."));
    RegisterEvent(wxT("wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK"), wxT("wxTreeEvent"),
                  _("The user has clicked the item with the right mouse button."));
    RegisterEvent(wxT("wxEVT_COMMAND_TREE_ITEM_MIDDLE_CLICK"), wxT("wxTreeEvent"),
                  _("The user has clicked the item with the middle mouse button."));
    RegisterEvent(wxT("wxEVT_COMMAND_TREE_STATE_IMAGE_CLICK"), wxT("wxTreeEvent"),
                  _("The state image has been clicked. Windows only."));
    RegisterEvent(wxT("wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP"), wxT("wxTreeEvent"),
                  _("The opportunity to set the item tooltip is being given to the application\n (call "
                    "wxTreeEvent::SetToolTip). Windows only."));
    RegisterEvent(wxT("wxEVT_COMMAND_TREE_ITEM_MENU"), wxT("wxTreeEvent"),
                  _("The context menu for the selected item has been requested,\neither by a right click or by using "
                    "the menu key."));

    m_namePattern = wxT("m_treeCtrl");
    SetName(GenerateName());
}

TreeCtrlWrapper::~TreeCtrlWrapper() {}

wxcWidget* TreeCtrlWrapper::Clone() const { return new TreeCtrlWrapper(); }

wxString TreeCtrlWrapper::CppCtorCode() const
{
    wxString code;
    code << GetName() << wxT(" = new ") << GetRealClassName() << "(" << GetWindowParent() << wxT(", ") << WindowID()
         << wxT(", ") << wxT("wxDefaultPosition, ") << SizeAsString() << wxT(", ")
         << StyleFlags(wxT("wxTR_DEFAULT_STYLE")) << wxT(");\n");
    code << CPPCommonAttributes();
    return code;
}

void TreeCtrlWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add(wxT("#include <wx/treectrl.h>")); }

wxString TreeCtrlWrapper::GetWxClassName() const { return wxT("wxTreeCtrl"); }

void TreeCtrlWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << XRCPrefix() << XRCStyle() << XRCSize() << XRCCommonAttributes() << XRCSuffix();
}
