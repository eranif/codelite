#include "grid_sizer_wrapper.h"
#include "allocator_mgr.h"
#include "wxgui_helpers.h"
#include "xmlutils.h"

GridSizerWrapper::GridSizerWrapper()
{
    m_type = ID_WXGRIDSIZER;
    m_styles.Clear(); // Sizer has no styles

    SetPropertyString(_("Common Settings"), "wxGridSizer");
    AddProperty(new StringProperty(PROP_COLS, wxT("2"), wxT("Number of columns in the grid")));
    AddProperty(new StringProperty(PROP_ROWS, wxT("0"), wxT("Number of rows in the grid")));
    AddProperty(new StringProperty(PROP_HGAP, wxT("0"), wxT("The horizontal gap between grid cells")));
    AddProperty(new StringProperty(PROP_VGAP, wxT("0"), wxT("The vertical gap between grid cells")));

    EnableSizerFlag("wxEXPAND", true);
    m_sizerItem.SetProportion(1);

    m_namePattern = wxT("gridSizer");
    SetName(GenerateName());
}

GridSizerWrapper::~GridSizerWrapper() {}

wxString GridSizerWrapper::CppCtorCode() const
{
    wxString code;

    if(!wxcSettings::Get().HasFlag(wxcSettings::SIZERS_AS_MEMBERS)) code << "wxGridSizer* ";

    code << GetName() << " = new wxGridSizer(" << PropertyString(PROP_ROWS) << wxT(", ") << PropertyString(PROP_COLS)
         << wxT(", ") << PropertyString(PROP_VGAP) << wxT(", ") << PropertyString(PROP_HGAP) << wxT(");\n");

    code << GenerateMinSizeCode();

    if(IsMainSizer()) {
        if(GetParent()->IsTopWindow()) {
            code << wxT("this->SetSizer(") << GetName() << wxT(");\n");
        } else {
            code << GetParent()->GetName() << wxT("->SetSizer(") << GetName() << wxT(");\n");
        }
    }
    return code;
}

void GridSizerWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << wxT("<object class=\"wxGridSizer\">") << GenerateMinSizeXRC() << wxT("<cols>") << PropertyString(PROP_COLS)
         << wxT("</cols>") << wxT("<rows>") << PropertyString(PROP_ROWS) << wxT("</rows>") << wxT("<vgap>")
         << PropertyString(PROP_VGAP) << wxT("</vgap>") << wxT("<hgap>") << PropertyString(PROP_HGAP) << wxT("</hgap>");
    ChildrenXRC(text, type);
    text << wxT("</object>");
}

wxString GridSizerWrapper::GetWxClassName() const { return wxT("wxGridSizer"); }

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
    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("cols"));
    if(propertynode) { SetPropertyString(PROP_COLS, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("rows"));
    if(propertynode) { SetPropertyString(PROP_ROWS, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("vgap"));
    if(propertynode) { SetPropertyString(PROP_VGAP, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("hgap"));
    if(propertynode) { SetPropertyString(PROP_HGAP, propertynode->GetNodeContent()); }
}

void GridSizerWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxFB(node);

    wxXmlNode* propertynode = XmlUtils::FindNodeByName(node, "property", "cols");
    if(propertynode) { SetPropertyString(PROP_COLS, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindNodeByName(node, "property", "rows");
    if(propertynode) { SetPropertyString(PROP_ROWS, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindNodeByName(node, "property", "vgap");
    if(propertynode) { SetPropertyString(PROP_VGAP, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindNodeByName(node, "property", "hgap");
    if(propertynode) { SetPropertyString(PROP_HGAP, propertynode->GetNodeContent()); }
}
