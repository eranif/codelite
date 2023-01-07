#include "grid_sizer_wrapper.h"

#include "allocator_mgr.h"
#include "wxgui_helpers.h"
#include "xmlutils.h"

GridSizerWrapper::GridSizerWrapper()
{
    m_type = ID_WXGRIDSIZER;
    m_styles.Clear(); // Sizer has no styles

    SetPropertyString(_("Common Settings"), "wxGridSizer");
    AddProperty(new StringProperty(PROP_COLS, "2", _("Number of columns in the grid")));
    AddProperty(new StringProperty(PROP_ROWS, "0", _("Number of rows in the grid")));
    AddProperty(new StringProperty(PROP_HGAP, "0", _("The horizontal gap between grid cells")));
    AddProperty(new StringProperty(PROP_VGAP, "0", _("The vertical gap between grid cells")));

    EnableSizerFlag("wxEXPAND", true);
    m_sizerItem.SetProportion(1);

    m_namePattern = "gridSizer";
    SetName(GenerateName());
}

GridSizerWrapper::~GridSizerWrapper() {}

wxString GridSizerWrapper::CppCtorCode() const
{
    wxString code;

    if(!KeepAsClassMember()) {
        code << "wxGridSizer* ";
    }

    code << GetName() << " = new wxGridSizer(" << PropertyString(PROP_ROWS) << ", " << PropertyString(PROP_COLS) << ", "
         << PropertyString(PROP_VGAP) << ", " << PropertyString(PROP_HGAP) << ");\n";

    code << GenerateMinSizeCode();

    if(IsMainSizer()) {
        if(GetParent()->IsTopWindow()) {
            code << "this->SetSizer(" << GetName() << ");\n";
        } else {
            code << GetParent()->GetName() << "->SetSizer(" << GetName() << ");\n";
        }
    }
    return code;
}

void GridSizerWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << wxT("<object class=\"wxGridSizer\">") << GenerateMinSizeXRC() << "<cols>" << PropertyString(PROP_COLS)
         << "</cols>"
         << "<rows>" << PropertyString(PROP_ROWS) << "</rows>"
         << "<vgap>" << PropertyString(PROP_VGAP) << "</vgap>"
         << "<hgap>" << PropertyString(PROP_HGAP) << "</hgap>";
    ChildrenXRC(text, type);
    text << "</object>";
}

wxString GridSizerWrapper::GetWxClassName() const { return "wxGridSizer"; }

void GridSizerWrapper::LoadPropertiesFromXRC(const wxXmlNode* node)
{
    wxcWidget::LoadPropertiesFromXRC(node); // Baseclass stuff first

    DoLoadXRCProperties(node);
}

void GridSizerWrapper::LoadPropertiesFromwxSmith(const wxXmlNode* node)
{
    wxcWidget::LoadPropertiesFromXRC(node); // Baseclass stuff first

    DoLoadXRCProperties(node);
}

void GridSizerWrapper::DoLoadXRCProperties(const wxXmlNode* node)
{
    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, "cols");
    if(propertynode) {
        SetPropertyString(PROP_COLS, propertynode->GetNodeContent());
    }

    propertynode = XmlUtils::FindFirstByTagName(node, "rows");
    if(propertynode) {
        SetPropertyString(PROP_ROWS, propertynode->GetNodeContent());
    }

    propertynode = XmlUtils::FindFirstByTagName(node, "vgap");
    if(propertynode) {
        SetPropertyString(PROP_VGAP, propertynode->GetNodeContent());
    }

    propertynode = XmlUtils::FindFirstByTagName(node, "hgap");
    if(propertynode) {
        SetPropertyString(PROP_HGAP, propertynode->GetNodeContent());
    }
}

void GridSizerWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxFB(node);

    wxXmlNode* propertynode = XmlUtils::FindNodeByName(node, "property", "cols");
    if(propertynode) {
        SetPropertyString(PROP_COLS, propertynode->GetNodeContent());
    }

    propertynode = XmlUtils::FindNodeByName(node, "property", "rows");
    if(propertynode) {
        SetPropertyString(PROP_ROWS, propertynode->GetNodeContent());
    }

    propertynode = XmlUtils::FindNodeByName(node, "property", "vgap");
    if(propertynode) {
        SetPropertyString(PROP_VGAP, propertynode->GetNodeContent());
    }

    propertynode = XmlUtils::FindNodeByName(node, "property", "hgap");
    if(propertynode) {
        SetPropertyString(PROP_HGAP, propertynode->GetNodeContent());
    }
}
