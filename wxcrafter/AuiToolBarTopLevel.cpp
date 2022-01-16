#include "AuiToolBarTopLevel.h"

#include "allocator_mgr.h"
#include "virtual_folder_property.h"
#include "winid_property.h"
#include "xy_pair.h"

#include <wx/aui/auibar.h>

AuiToolBarTopLevelWrapper::AuiToolBarTopLevelWrapper()
    : TopLevelWinWrapper(ID_WXAUITOOLBARTOPLEVEL)
{
    m_properties.DeleteValues();
    m_sizerFlags.Clear();

    SetPropertyString(_("Common Settings"), "wxAuiToolBar");

    AddCategory(_("wxAuiToolBar"));
    AddProperty(new WinIdProperty());
    AddProperty(new StringProperty(PROP_NAME, "", _("The generated C++ class name")));
    AddProperty(new StringProperty(PROP_SIZE, "-1,-1",
                                   _("The control size. It is recommended to leave it as -1,-1 and "
                                     "let\nthe sizers calculate the best size for the window")));
    AddProperty(new StringProperty(PROP_TOOLTIP, "", _("Tooltip")));

    AddCategory(_("Inherited C++ Class Properties"));
    AddProperty(new StringProperty(PROP_INHERITED_CLASS, "",
                                   _("Inherited class name\nFill this field to generate a class that inherits from the "
                                     "base class,\nwhere you should place all your application logic.\ne.g. for a "
                                     "generated class 'FooDialogBase', you might enter 'FooDialog' here.")));
    AddProperty(new StringProperty(
        PROP_FILE, "",
        _("The name for the inherited class's files (without any file extension).\nwxCrafter will generate a "
          "$(FILE).cpp and $(FILE).hpp\ne.g. for an inherited class 'FooDialog', you might enter 'foodialog' here.")));
    AddProperty(new StringProperty(PROP_CLASS_DECORATOR, "",
                                   _("MSW Only\nC++ macro decorator - allows exporting this class from a DLL")));

    if(m_properties.Contains(PROP_NAME)) {
        m_properties.Item(PROP_NAME)->SetTooltip(_("The generated C++ class name"));
    }

    AddProperty(
        new VirtualFolderProperty(PROP_VIRTUAL_FOLDER, "", _("codelite's virtual folder for the generated files")));

    AddCategory(_("Control Specific Settings"));
    AddProperty(new StringProperty(PROP_BITMAP_SIZE, "16,16", _("Sets the default size of each tool bitmap")));
    AddProperty(new StringProperty(PROP_MARGINS, "-1,-1", _("Set the values to be used as margins for the toolbar.")));

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

    m_namePattern = "m_auibar";
    SetName(GenerateName());
}

AuiToolBarTopLevelWrapper::~AuiToolBarTopLevelWrapper() {}

wxString AuiToolBarTopLevelWrapper::BaseCtorDecl() const
{
    wxString code;
    code << "    " << CreateBaseclassName() << "(wxWindow *parent, wxWindowID id = " << WindowID()
         << ", const wxPoint &position = wxDefaultPosition"
         << ", const wxSize &size = " << SizeAsString() << ", long style = " << StyleFlags("wxAUI_TB_DEFAULT_STYLE")
         << ");\n";
    return code;
}

wxString AuiToolBarTopLevelWrapper::BaseCtorImplPrefix() const
{
    wxString code;
    code << CreateBaseclassName() << "::" << CreateBaseclassName() << "("
         << "wxWindow *parent, "
         << "wxWindowID id, "
         << "const wxPoint &position, "
         << "const wxSize &size, "
         << "long style"
         << ")\n"
         << "    : " << GetRealClassName() << "(parent, id, position, size, style)\n";
    return code;
}

wxcWidget* AuiToolBarTopLevelWrapper::Clone() const { return new AuiToolBarTopLevelWrapper(); }

wxString AuiToolBarTopLevelWrapper::CppCtorCode() const
{
    wxString code;
    XYPair pr(PropertyString(PROP_BITMAP_SIZE), 16, 16);
    code << "SetToolBitmapSize(wxSize" << pr.ToString(true) << ");\n";

    XYPair margins(PropertyString(PROP_MARGINS), -1, -1);
    if(margins != XYPair(-1, -1)) {
        code << "    SetMargins(" << margins.ToString() << ");\n";
    }
    return code;
}

wxString AuiToolBarTopLevelWrapper::DesignerXRC(bool forPreviewDialog) const
{
    wxString text;
    text << "<object class=\"wxPanel\" name=\"PreviewPanel\">"
         << "<size>-1,-1</size>"
         // Fake a sizer
         << "<object class=\"wxBoxSizer\">"
         << "<orient>wxVERTICAL</orient>"
         << "<object class=\"sizeritem\">"
         << "<flag>wxALL|wxALIGN_CENTER_HORIZONTAL|wxEXPAND</flag>"
         << "<border>0</border>";

    ToXRC(text, XRC_DESIGNER);

    text << "</object>"  // sizeritem
         << "</object>"  // wxBoxSizer
         << "</object>"; // PreviewPanel
    WrapXRC(text);
    return text;
}

wxString AuiToolBarTopLevelWrapper::DoGenerateClassMember() const { return GenerateClassMembers(this); }

void AuiToolBarTopLevelWrapper::GetIncludeFile(wxArrayString& headers) const { BaseGetIncludeFile(headers); }

wxString AuiToolBarTopLevelWrapper::GetWxClassName() const { return "wxAuiToolBar"; }

void AuiToolBarTopLevelWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    if(type == XRC_PREVIEW) {
        text << "<object class=\"wxPanel\" name=\"" << GetName() << "\">"
             << "<size>-1,-1</size>"
             // Fake a sizer
             << "<object class=\"wxBoxSizer\">"
             << "<orient>wxVERTICAL</orient>"
             << "<object class=\"sizeritem\">"
             << "<flag>wxALL|wxALIGN_CENTER_HORIZONTAL|wxEXPAND</flag>"
             << "<border>5</border>";
    }
    XYPair prSize(PropertyString(PROP_BITMAP_SIZE), 16, 16);
    XYPair prMargins(PropertyString(PROP_MARGINS), -1, -1);

    if(type == XRC_PREVIEW) {
        // IN Preview Mode, append to the class name
        // a number to distinguish it from the panel
        // that we added earlier in this function
        text << "<object class=\"" << GetWxClassName() << "\" name=\"" << GetName() << "1"
             << "\">";
    } else {
        text << XRCPrefix();
    }
    text << XRCStyle() << XRCCommonAttributes() << "<bitmapsize>" << prSize.ToString() << "</bitmapsize>";

    if(prMargins != XYPair(-1, -1)) {
        text << "<margins>" << prMargins.ToString() << "</margins>";
    }

    ChildrenXRC(text, type);
    text << XRCSuffix();

    if(type == XRC_PREVIEW) {
        text << "</object>"  // sizeritem
             << "</object>"  // wxBoxSizer
             << "</object>"; // PreviewPanel
        WrapXRC(text);
    }
}

void AuiToolBarTopLevelWrapper::DoGenerateExtraFunctions(wxString& decl, wxString& impl) const
{
    GenerateExtraFunctions(this, decl, impl);
}
