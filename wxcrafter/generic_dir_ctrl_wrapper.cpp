#include "generic_dir_ctrl_wrapper.h"
#include "allocator_mgr.h"
#include "bool_property.h"
#include "wxgui_defs.h"
#include "wxgui_helpers.h"
#include "xmlutils.h"
#include <wx/dirctrl.h>

GenericDirCtrlWrapper::GenericDirCtrlWrapper()
    : wxcWidget(ID_WXGENERICDIRCTRL)
{
    SetPropertyString(_("Common Settings"), "wxGenericDirCtrl");
    AddProperty(new StringProperty(PROP_DEFAULT_FOLDER, wxT(""), _("Sets the default path")));
    AddProperty(new StringProperty(PROP_FILTER, wxT(""),
                                   _("Sets the filter string. The syntax is similar to the one used in wxFileDialog")));
    AddProperty(new StringProperty(PROP_DEFAULT_FILTER, wxT(""), _("Sets the current filter index (zero-based).")));
    AddProperty(new BoolProperty(PROP_SHOW_HIDDEN, false, _("Show hidden files")));

    PREPEND_STYLE_FALSE(wxDIRCTRL_DIR_ONLY);
    PREPEND_STYLE_TRUE(wxDIRCTRL_3D_INTERNAL);
    PREPEND_STYLE_FALSE(wxDIRCTRL_SELECT_FIRST);
    PREPEND_STYLE_FALSE(wxDIRCTRL_SHOW_FILTERS);
    PREPEND_STYLE_FALSE(wxDIRCTRL_EDIT_LABELS);

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

    m_namePattern = wxT("m_genericDirCtrl");
    SetName(GenerateName());
}

GenericDirCtrlWrapper::~GenericDirCtrlWrapper() {}

wxcWidget* GenericDirCtrlWrapper::Clone() const { return new GenericDirCtrlWrapper(); }

wxString GenericDirCtrlWrapper::CppCtorCode() const
{
    // wxGenericDirCtrl(wxWindow* parent, const wxWindowID id = -1, const wxString& dir = wxDirDialogDefaultFolderStr,
    // const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style =
    // wxDIRCTRL_3D_INTERNAL|wxSUNKEN_BORDER, const wxString& filter = wxEmptyString, int defaultFilter = 0, const
    // wxString& name = wxTreeCtrlNameStr)
    wxString code;
    code << GetName() << wxT(" = new ") << GetRealClassName() << wxT("(") << GetWindowParent() << wxT(", ")
         << WindowID() << wxT(", ") << wxCrafter::WXT(PropertyString(PROP_DEFAULT_FOLDER)) << wxT(", ")
         << wxT("wxDefaultPosition, ") << SizeAsString() << wxT(", ")
         << StyleFlags(wxT("wxDIRCTRL_3D_INTERNAL|wxSUNKEN_BORDER")) << wxT(");\n");

    code << CPPCommonAttributes();

    // Set the default path
    wxString path = PropertyString(PROP_DEFAULT_FOLDER);

    path.Trim().Trim(false);
    if(path.IsEmpty() == false) {
        code << GetName() << wxT("->SetDefaultPath(") << wxCrafter::WXT(path) << wxT(");\n");
    }

    // Set the filter string
    wxString filter = PropertyString(PROP_FILTER);
    filter.Trim().Trim(false);

    if(filter.IsEmpty() == false) { code << GetName() << wxT("->SetFilter(") << wxCrafter::WXT(filter) << wxT(");\n"); }

    // Set the filter index
    wxString filterIndex = PropertyString(PROP_DEFAULT_FILTER);
    filterIndex.Trim().Trim(false);

    int index = wxCrafter::ToNumber(filterIndex, -1);
    if(-1 != index) { code << GetName() << wxT("->SetFilterIndex(") << index << wxT(");\n"); }

    // Show hidden files?
    wxString show = PropertyBool(PROP_SHOW_HIDDEN);
    code << GetName() << wxT("->ShowHidden(") << show << wxT(");\n");

    return code;
}

void GenericDirCtrlWrapper::GetIncludeFile(wxArrayString& headers) const
{
    headers.Add(wxT("#include <wx/dirctrl.h>"));
}

wxString GenericDirCtrlWrapper::GetWxClassName() const { return wxT("wxGenericDirCtrl"); }

void GenericDirCtrlWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << XRCPrefix() << XRCCommonAttributes() << XRCSize() << XRCStyle() << wxT("<defaultfolder>")
         << wxCrafter::CDATA(PropertyString(PROP_DEFAULT_FOLDER)) << wxT("</defaultfolder>") << wxT("<defaultfilter>")
         << PropertyString(PROP_DEFAULT_FILTER) << wxT("</defaultfilter>") << wxT("<filter>")
         << wxCrafter::CDATA(PropertyString(PROP_FILTER)) << wxT("</filter>") << XRCSuffix();
}

void GenericDirCtrlWrapper::LoadPropertiesFromXRC(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromXRC(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("defaultfolder"));
    if(propertynode) { SetPropertyString(PROP_DEFAULT_FOLDER, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("filter"));
    if(propertynode) { SetPropertyString(PROP_FILTER, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("defaultfilter"));
    if(propertynode) { SetPropertyString(PROP_DEFAULT_FILTER, propertynode->GetNodeContent()); }
}

void GenericDirCtrlWrapper::LoadPropertiesFromwxSmith(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromXRC(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("defaultfolder"));
    if(propertynode) { SetPropertyString(PROP_DEFAULT_FOLDER, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("filter"));
    if(propertynode) { SetPropertyString(PROP_FILTER, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("defaultfilter"));
    if(propertynode) { SetPropertyString(PROP_DEFAULT_FILTER, propertynode->GetNodeContent()); }
}

void GenericDirCtrlWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxFB(node);

    wxXmlNode* propertynode = XmlUtils::FindNodeByName(node, "property", "defaultfolder");
    if(propertynode) { SetPropertyString(PROP_DEFAULT_FOLDER, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindNodeByName(node, "property", "filter");
    if(propertynode) { SetPropertyString(PROP_FILTER, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindNodeByName(node, "property", "defaultfilter");
    if(propertynode) { SetPropertyString(PROP_DEFAULT_FILTER, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindNodeByName(node, "property", "show_hidden");
    if(propertynode) {
        PropertyBase* prop = GetProperty(PROP_SHOW_HIDDEN);
        if(prop) { prop->SetValue(propertynode->GetNodeContent()); }
    }
}
