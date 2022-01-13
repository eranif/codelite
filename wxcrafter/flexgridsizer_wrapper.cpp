#include "flexgridsizer_wrapper.h"

#include "allocator_mgr.h"
#include "wxgui_helpers.h"
#include "xmlutils.h"

FlexGridSizerWrapper::FlexGridSizerWrapper()
{
    m_type = ID_WXFLEXGRIDSIZER;
    m_styles.Clear(); // Sizer has no styles

    SetPropertyString(_("Common Settings"), "wxFlexGridSizer");
    AddProperty(new StringProperty(PROP_COLS, "2", _("Number of columns in the grid")));
    AddProperty(new StringProperty(PROP_ROWS, "0", _("Number of rows in the grid")));
    AddProperty(new StringProperty(PROP_GROW_COLS, "", _("Which columns are allowed to grow. Comma separated list")));
    AddProperty(new StringProperty(PROP_GROW_ROWS, "", _("Which rows are allowed to grow. Comma separated list")));
    AddProperty(new StringProperty(PROP_HGAP, "0", _("The horizontal gap between grid cells")));
    AddProperty(new StringProperty(PROP_VGAP, "0", _("The vertical gap between grid cells")));

    EnableSizerFlag("wxEXPAND", true);
    m_sizerItem.SetProportion(1);

    m_namePattern = "flexGridSizer";
    SetName(GenerateName());
}

FlexGridSizerWrapper::~FlexGridSizerWrapper() {}

wxString FlexGridSizerWrapper::CppCtorCode() const
{
    wxString code;

    wxArrayString growCols = wxCrafter::Split(PropertyString(PROP_GROW_COLS), ",");
    wxArrayString growRows = wxCrafter::Split(PropertyString(PROP_GROW_ROWS), ",");

    if(!KeepAsClassMember()) {
        code << "wxFlexGridSizer* ";
    }

    code << GetName() << " = new wxFlexGridSizer(" << PropertyString(PROP_ROWS) << ", " << PropertyString(PROP_COLS)
         << ", " << PropertyString(PROP_VGAP) << ", " << PropertyString(PROP_HGAP) << ");\n"
         << GetName() << "->SetFlexibleDirection( wxBOTH );\n"
         << GetName() << "->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );\n";

    for(size_t i = 0; i < growCols.GetCount(); i++) {
        code << GetName() << "->AddGrowableCol(" << growCols.Item(i) << ");\n";
    }

    for(size_t i = 0; i < growRows.GetCount(); i++) {
        code << GetName() << "->AddGrowableRow(" << growRows.Item(i) << ");\n";
    }
    code << GenerateMinSizeCode();
    if(IsMainSizer()) {
        if(GetParent()->IsTopWindow()) {
            code << "this->SetSizer(" << GetName() << ");\n";
        } else {
            code << GetWindowParent() << "->SetSizer(" << GetName() << ");\n";
        }
    }
    return code;
}

void FlexGridSizerWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << wxT("<object class=\"wxFlexGridSizer\">") << GenerateMinSizeXRC() << "<cols>" << PropertyString(PROP_COLS)
         << "</cols>"
         << "<rows>" << PropertyString(PROP_ROWS) << "</rows>"
         << "<vgap>" << PropertyString(PROP_VGAP) << "</vgap>"
         << "<hgap>" << PropertyString(PROP_HGAP) << "</hgap>"
         << "<growablecols>" << PropertyString(PROP_GROW_COLS) << "</growablecols>"
         << "<growablerows>" << PropertyString(PROP_GROW_ROWS) << "</growablerows>";
    ChildrenXRC(text, type);
    text << "</object>";
}

wxString FlexGridSizerWrapper::GetWxClassName() const { return "wxFlexGridSizer"; }

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

    propertynode = XmlUtils::FindFirstByTagName(node, "growablecols");
    if(propertynode) {
        SetPropertyString(PROP_GROW_COLS, propertynode->GetNodeContent());
    }

    propertynode = XmlUtils::FindFirstByTagName(node, "growablerows");
    if(propertynode) {
        SetPropertyString(PROP_GROW_ROWS, propertynode->GetNodeContent());
    }
}

void FlexGridSizerWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node)
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

    propertynode = XmlUtils::FindNodeByName(node, "property", "growablecols");
    if(propertynode) {
        SetPropertyString(PROP_GROW_COLS, propertynode->GetNodeContent());
    }

    propertynode = XmlUtils::FindNodeByName(node, "property", "growablerows");
    if(propertynode) {
        SetPropertyString(PROP_GROW_ROWS, propertynode->GetNodeContent());
    }
}
