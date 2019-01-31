#include "toolbar_base_wrapper.h"
#include "allocator_mgr.h"
#include "tool_bar_item_wrapper.h"
#include "top_level_win_wrapper.h"
#include "winid_property.h"
#include "wx/pen.h"
#include "wxgui_defs.h"
#include "wxgui_helpers.h"
#include "xmlutils.h"
#include "xy_pair.h"
#include <wx/aui/auibar.h>
#include <wx/toolbar.h>

ToolbarBaseWrapper::ToolbarBaseWrapper(int type)
    : wxcWidget(type)
{
    m_styles.Clear();

    PREPEND_STYLE_TRUE(wxTB_FLAT);
    PREPEND_STYLE_FALSE(wxTB_DOCKABLE);
    PREPEND_STYLE_FALSE(wxTB_HORIZONTAL);
    PREPEND_STYLE_FALSE(wxTB_VERTICAL);
    PREPEND_STYLE_FALSE(wxTB_TEXT);
    PREPEND_STYLE_FALSE(wxTB_NOICONS);
    PREPEND_STYLE_FALSE(wxTB_NODIVIDER);
    PREPEND_STYLE_FALSE(wxTB_NOALIGN);
    PREPEND_STYLE_FALSE(wxTB_HORZ_LAYOUT);
    PREPEND_STYLE_FALSE(wxTB_HORZ_TEXT);

    m_properties.DeleteValues();
    AddProperty(new CategoryProperty(_("Common Settings"), "wxToolBar"));
    AddProperty(new WinIdProperty());
    AddProperty(new StringProperty(PROP_SIZE, wxT("-1,-1"),
                                   _("The control size. It is recommended to leave it as -1,-1 and "
                                     "let\nthe sizers calculate the best size for the window")));
    AddProperty(new StringProperty(PROP_NAME, wxT(""), _("C++ member name")));
    AddProperty(new StringProperty(PROP_TOOLTIP, wxT(""), _("Tooltip")));

    AddProperty(new CategoryProperty(_("ToolBar")));
    AddProperty(new StringProperty(PROP_BITMAP_SIZE, wxT("16,16"), _("Sets the default size of each tool bitmap")));
    AddProperty(
        new StringProperty(PROP_MARGINS, wxT("-1,-1"), _("Sets the values to be used as margins for the toolbar.")));
    AddProperty(new StringProperty(PROP_PADDING, wxT("1"), _("Sets the space between tools.")));
    AddProperty(new StringProperty(PROP_SEPARATOR_SIZE, wxT("5"), _("Sets the width of separators.")));
    
    AddProperty(new CategoryProperty(_("Subclass")));
    AddProperty(new StringProperty(PROP_SUBCLASS_NAME, wxT(""),
                                   _("The name of the derived class. Used both for C++ and XRC generated code.")));
    AddProperty(new StringProperty(PROP_SUBCLASS_INCLUDE, wxT(""),
                                   _("(C++ only) The name of any extra header file to be #included e.g. mydialog.h")));
    AddText(PROP_SUBCLASS_STYLE,
            _("Override the default class style with the content of this field.\nThe style should be | separated"));
    m_namePattern = wxT("m_toolbar");
    SetName(GenerateName());
}

ToolbarBaseWrapper::~ToolbarBaseWrapper() {}

wxString ToolbarBaseWrapper::CppCtorCode() const
{
    wxString code;
    bool isMainToolbar = (GetParent() && GetParent()->GetType() == ID_WXFRAME);
    bool isSubclassed = (GetRealClassName() != GetWxClassName());
    if(isMainToolbar && !isSubclassed) {
        code << GetName() << wxT(" = this->CreateToolBar(") << StyleFlags(wxT("wxTB_FLAT|wxTB_HORIZONTAL")) << wxT(", ")
             << WindowID() << wxT(");\n");
    } else {
        code << CPPStandardWxCtor(wxT("wxTB_HORIZONTAL|wxNO_BORDER"));
    }

    XYPair pr(PropertyString(PROP_BITMAP_SIZE), 16, 16);
    code << GetName() << wxT("->SetToolBitmapSize(wxSize") << pr.ToString(true) << wxT(");\n");

    XYPair margins(PropertyString(PROP_MARGINS), -1, -1);
    if(margins != XYPair(-1, -1)) { code << GetName() << wxT("->SetMargins(") << margins.ToString() << wxT(");\n"); }
    wxString padding = PropertyString(PROP_PADDING);
    if(padding != "1") { code << GetName() << wxT("->SetToolPacking(") << padding << wxT(");\n"); }
    wxString sepWidth = PropertyString(PROP_SEPARATOR_SIZE);
    if(sepWidth != "5") { code << GetName() << wxT("->SetToolSeparation(") << sepWidth << wxT(");\n"); }
    return code;
}

void ToolbarBaseWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add(wxT("#include <wx/toolbar.h>")); }

wxString ToolbarBaseWrapper::GetWxClassName() const { return wxT("wxToolBar"); }

void ToolbarBaseWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    if(type == XRC_DESIGNER && GetParent()->GetType() == ID_WXFRAME) {
        // This ToolBar is a direct child of the main frame
        // however, since our designer preview is wxPanel based
        // we cant really place it here, so return empty string
        // we hack it later
        return;
    }

    XYPair prSize(PropertyString(PROP_BITMAP_SIZE), 16, 16);
    XYPair prMargins(PropertyString(PROP_MARGINS), -1, -1);

    text << XRCPrefix() << XRCStyle() << wxT("<bitmapsize>") << prSize.ToString() << wxT("</bitmapsize>");

    if(prMargins != XYPair(-1, -1)) { text << wxT("<margins>") << prMargins.ToString() << wxT("</margins>"); }

    wxString padding = PropertyString(PROP_PADDING);
    if(padding != "1") { text << wxT("<packing>") << padding << wxT("</packing>"); }
    wxString sepWidth = PropertyString(PROP_SEPARATOR_SIZE);
    if(sepWidth != "5") { text << wxT("<separation>") << sepWidth << wxT("</separation>"); }

    ChildrenXRC(text, type);
    text << XRCSuffix();
}

wxString ToolbarBaseWrapper::DesignerXRC() const
{
    wxString toolbarXRC;
    toolbarXRC << wxT("<object class=\"") << GetWxClassName() << wxT("\" name=\"TOOL_BAR_ID\">");
    XYPair prSize(PropertyString(PROP_BITMAP_SIZE), 16, 16);
    XYPair prMargins(PropertyString(PROP_MARGINS), -1, -1);

    toolbarXRC << XRCStyle() << wxT("<bitmapsize>") << prSize.ToString() << wxT("</bitmapsize>");

    if(prMargins != XYPair(-1, -1)) { toolbarXRC << wxT("<margins>") << prMargins.ToString() << wxT("</margins>"); }

    ChildrenXRC(toolbarXRC, XRC_DESIGNER);
    toolbarXRC << XRCSuffix();
    return toolbarXRC;
}

void ToolbarBaseWrapper::LoadPropertiesFromXRC(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromXRC(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("bitmapsize"));
    if(propertynode) { SetPropertyString(PROP_BITMAP_SIZE, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("margins"));
    if(propertynode) { SetPropertyString(PROP_MARGINS, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("packing"));
    if(propertynode) { SetPropertyString(PROP_PADDING, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("separation"));
    if(propertynode) { SetPropertyString(PROP_SEPARATOR_SIZE, propertynode->GetNodeContent()); }
}

void ToolbarBaseWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxFB(node);

    // wxC doesn't (yet) do 'separation' or 'packing'
    wxXmlNode* propertynode = XmlUtils::FindNodeByName(node, "property", "bitmapsize");
    if(propertynode) { SetPropertyString(PROP_BITMAP_SIZE, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindNodeByName(node, "property", "margins");
    if(propertynode) { SetPropertyString(PROP_MARGINS, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindNodeByName(node, "property", "packing");
    if(propertynode) { SetPropertyString(PROP_PADDING, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindNodeByName(node, "property", "separation");
    if(propertynode) { SetPropertyString(PROP_SEPARATOR_SIZE, propertynode->GetNodeContent()); }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// AUI ToolBar
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

AuiToolbarWrapper::AuiToolbarWrapper()
    : wxcWidget(ID_WXAUITOOLBAR)
{
    m_styles.Clear();
    m_properties.DeleteValues();
    EnableSizerFlag(wxT("wxEXPAND"), true);

    AddProperty(new CategoryProperty(_("Common Settings")));
    AddProperty(new WinIdProperty());
    AddProperty(new StringProperty(PROP_SIZE, wxT("-1,-1"),
                                   _("The control size. It is recommended to leave it as -1,-1 and "
                                     "let\nthe sizers calculate the best size for the window")));
    AddProperty(new StringProperty(PROP_NAME, wxT(""), _("C++ member name")));
    AddProperty(new StringProperty(PROP_TOOLTIP, wxT(""), _("Tooltip")));

    AddProperty(new CategoryProperty(_("wxAuiToolBar")));
    AddProperty(new StringProperty(PROP_BITMAP_SIZE, wxT("16,16"), _("Sets the default size of each tool bitmap")));
    AddProperty(
        new StringProperty(PROP_MARGINS, wxT("-1,-1"), _("Set the values to be used as margins for the toolbar.")));

    PREPEND_STYLE_FALSE(wxAUI_TB_TEXT);
    PREPEND_STYLE_FALSE(wxAUI_TB_NO_TOOLTIPS);
    PREPEND_STYLE_FALSE(wxAUI_TB_NO_AUTORESIZE);
    PREPEND_STYLE_FALSE(wxAUI_TB_GRIPPER);
    PREPEND_STYLE_FALSE(wxAUI_TB_OVERFLOW);
    PREPEND_STYLE_FALSE(wxAUI_TB_VERTICAL);
    PREPEND_STYLE_FALSE(wxAUI_TB_HORZ_LAYOUT);
    PREPEND_STYLE_FALSE(wxAUI_TB_HORZ_TEXT);
    PREPEND_STYLE_TRUE(wxAUI_TB_DEFAULT_STYLE);
    PREPEND_STYLE_TRUE(wxAUI_TB_PLAIN_BACKGROUND);

    m_namePattern = wxT("m_auibar");
    SetName(GenerateName());
}

wxString AuiToolbarWrapper::CppCtorCode() const
{
    wxString code;
    code << CPPStandardWxCtor(wxT("wxAUI_TB_DEFAULT_STYLE"));
    XYPair pr(PropertyString(PROP_BITMAP_SIZE), 16, 16);
    code << GetName() << wxT("->SetToolBitmapSize(wxSize") << pr.ToString(true) << wxT(");\n");

    XYPair margins(PropertyString(PROP_MARGINS), -1, -1);
    if(margins != XYPair(-1, -1)) { code << GetName() << wxT("->SetMargins(") << margins.ToString() << wxT(");\n"); }
    return code;
}

wxString AuiToolbarWrapper::DoGenerateClassMember() const { return GenerateClassMembers(this); }

void AuiToolbarWrapper::DoGenerateExtraFunctions(wxString& decl, wxString& impl) const
{
    GenerateExtraFunctions(this, decl, impl);
}

void AuiToolbarWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    XYPair prSize(PropertyString(PROP_BITMAP_SIZE), 16, 16);
    XYPair prMargins(PropertyString(PROP_MARGINS), -1, -1);

    text << XRCPrefix() << XRCStyle() << XRCCommonAttributes() << wxT("<bitmapsize>") << prSize.ToString()
         << wxT("</bitmapsize>");

    if(prMargins != XYPair(-1, -1)) { text << wxT("<margins>") << prMargins.ToString() << wxT("</margins>"); }

    ChildrenXRC(text, type);
    text << XRCSuffix();
}

void AuiToolbarWrapper::LoadPropertiesFromXRC(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromXRC(node);

    // wxC doesn't (yet) do 'separation' or 'packing'
    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("bitmapsize"));
    if(propertynode) { SetPropertyString(PROP_BITMAP_SIZE, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("margins"));
    if(propertynode) { SetPropertyString(PROP_MARGINS, propertynode->GetNodeContent()); }
}

void AuiToolbarWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxFB(node);

    // wxC doesn't (yet) do 'separation' or 'packing'
    wxXmlNode* propertynode = XmlUtils::FindNodeByName(node, "property", "bitmapsize");
    if(propertynode) { SetPropertyString(PROP_BITMAP_SIZE, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindNodeByName(node, "property", "margins");
    if(propertynode) { SetPropertyString(PROP_MARGINS, propertynode->GetNodeContent()); }
}

wxString AuiToolbarWrapper::CppDtorCode() const { return AuiToolbarWrapperBase::CppDtorCode(this); }

void AuiToolbarWrapper::GetIncludeFile(wxArrayString& headers) const { BaseGetIncludeFile(headers); }
