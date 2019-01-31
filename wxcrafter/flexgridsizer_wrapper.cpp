#include "flexgridsizer_wrapper.h"
#include "allocator_mgr.h"
#include "wxgui_helpers.h"
#include "xmlutils.h"

FlexGridSizerWrapper::FlexGridSizerWrapper()
{
    m_type = ID_WXFLEXGRIDSIZER;
    m_styles.Clear(); // Sizer has no styles

    SetPropertyString(_("Common Settings"), "wxFlexGridSizer");
    AddProperty(new StringProperty(PROP_COLS, wxT("2"), wxT("Number of columns in the grid")));
    AddProperty(new StringProperty(PROP_ROWS, wxT("0"), wxT("Number of rows in the grid")));
    AddProperty(
        new StringProperty(PROP_GROW_COLS, wxT(""), wxT("Which columns are allowed to grow. Comma separated list")));
    AddProperty(
        new StringProperty(PROP_GROW_ROWS, wxT(""), wxT("Which rows are allowed to grow. Comma separated list")));
    AddProperty(new StringProperty(PROP_HGAP, wxT("0"), wxT("The horizontal gap between grid cells")));
    AddProperty(new StringProperty(PROP_VGAP, wxT("0"), wxT("The vertical gap between grid cells")));
    m_namePattern = wxT("flexGridSizer");

    EnableSizerFlag("wxEXPAND", true);
    m_sizerItem.SetProportion(1);

    SetName(GenerateName());
}

FlexGridSizerWrapper::~FlexGridSizerWrapper() {}

wxString FlexGridSizerWrapper::CppCtorCode() const
{
    wxString code;

    wxArrayString growCols = wxCrafter::Split(PropertyString(PROP_GROW_COLS), wxT(","));
    wxArrayString growRows = wxCrafter::Split(PropertyString(PROP_GROW_ROWS), wxT(","));

    if(!wxcSettings::Get().HasFlag(wxcSettings::SIZERS_AS_MEMBERS)) code << "wxFlexGridSizer* ";

    code << GetName() << " = new wxFlexGridSizer(" << PropertyString(PROP_ROWS) << wxT(", ")
         << PropertyString(PROP_COLS) << wxT(", ") << PropertyString(PROP_VGAP) << wxT(", ")
         << PropertyString(PROP_HGAP) << wxT(");\n") << GetName() << wxT("->SetFlexibleDirection( wxBOTH );\n")
         << GetName() << wxT("->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );\n");

    for(size_t i = 0; i < growCols.GetCount(); i++) {
        code << GetName() << wxT("->AddGrowableCol(") << growCols.Item(i) << wxT(");\n");
    }

    for(size_t i = 0; i < growRows.GetCount(); i++) {
        code << GetName() << wxT("->AddGrowableRow(") << growRows.Item(i) << wxT(");\n");
    }
    code << GenerateMinSizeCode();
    if(IsMainSizer()) {
        if(GetParent()->IsTopWindow()) {
            code << wxT("this->SetSizer(") << GetName() << wxT(");\n");
        } else {
            code << GetWindowParent() << wxT("->SetSizer(") << GetName() << wxT(");\n");
        }
    }
    return code;
}

void FlexGridSizerWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << wxT("<object class=\"wxFlexGridSizer\">") << GenerateMinSizeXRC() << wxT("<cols>")
         << PropertyString(PROP_COLS) << wxT("</cols>") << wxT("<rows>") << PropertyString(PROP_ROWS) << wxT("</rows>")
         << wxT("<vgap>") << PropertyString(PROP_VGAP) << wxT("</vgap>") << wxT("<hgap>") << PropertyString(PROP_HGAP)
         << wxT("</hgap>") << wxT("<growablecols>") << PropertyString(PROP_GROW_COLS) << wxT("</growablecols>")
         << wxT("<growablerows>") << PropertyString(PROP_GROW_ROWS) << wxT("</growablerows>");
    ChildrenXRC(text, type);
    text << wxT("</object>");
}

wxString FlexGridSizerWrapper::GetWxClassName() const { return wxT("wxFlexGridSizer"); }

void FlexGridSizerWrapper::LoadPropertiesFromXRC(const wxXmlNode* node)
{
    wxcWidget::LoadPropertiesFromXRC(node); // Baseclass stuff first

    DoLoadXRCProperties(node);
}

void FlexGridSizerWrapper::LoadPropertiesFromwxSmith(const wxXmlNode* node)
{
    wxcWidget::LoadPropertiesFromwxSmith(node); // Baseclass stuff first

    DoLoadXRCProperties(node);
}

void FlexGridSizerWrapper::DoLoadXRCProperties(const wxXmlNode* node)
{
    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("cols"));
    if(propertynode) { SetPropertyString(PROP_COLS, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("rows"));
    if(propertynode) { SetPropertyString(PROP_ROWS, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("vgap"));
    if(propertynode) { SetPropertyString(PROP_VGAP, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("hgap"));
    if(propertynode) { SetPropertyString(PROP_HGAP, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("growablecols"));
    if(propertynode) { SetPropertyString(PROP_GROW_COLS, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("growablerows"));
    if(propertynode) { SetPropertyString(PROP_GROW_ROWS, propertynode->GetNodeContent()); }
}

void FlexGridSizerWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node)
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

    propertynode = XmlUtils::FindNodeByName(node, "property", "growablecols");
    if(propertynode) { SetPropertyString(PROP_GROW_COLS, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindNodeByName(node, "property", "growablerows");
    if(propertynode) { SetPropertyString(PROP_GROW_ROWS, propertynode->GetNodeContent()); }
}
