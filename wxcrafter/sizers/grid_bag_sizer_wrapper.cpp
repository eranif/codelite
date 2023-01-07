#include "grid_bag_sizer_wrapper.h"

#include "allocator_mgr.h"
#include "wxgui_helpers.h"
#include "xmlutils.h"

GridBagSizerWrapper::GridBagSizerWrapper()
{
    m_type = ID_WXGRIDBAGSIZER;
    m_styles.Clear(); // Sizer has no styles
    SetPropertyString(_("Common Settings"), "wxGridBagSizer");
    AddProperty(new StringProperty(PROP_GROW_COLS, "", _("Which columns are allowed to grow. Comma separated list")));
    AddProperty(new StringProperty(PROP_GROW_ROWS, "", _("Which rows are allowed to grow. Comma separated list")));
    AddProperty(new StringProperty(PROP_HGAP, "0", _("The horizontal gap between grid cells")));
    AddProperty(new StringProperty(PROP_VGAP, "0", _("The vertical gap between grid cells")));

    EnableSizerFlag("wxEXPAND", true);
    m_sizerItem.SetProportion(1);

    m_namePattern = "gridBagSizer";
    SetName(GenerateName());
}

GridBagSizerWrapper::~GridBagSizerWrapper() {}

wxString GridBagSizerWrapper::CppCtorCode() const
{
    wxString code;

    if(!KeepAsClassMember()) {
        code << "wxGridBagSizer* ";
    }

    code << GetName() << " = new wxGridBagSizer(" << PropertyString(PROP_VGAP) << ", " << PropertyString(PROP_HGAP)
         << ");\n";
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

void GridBagSizerWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << wxT("<object class=\"wxGridBagSizer\">") << GenerateMinSizeXRC() << "<vgap>" << PropertyString(PROP_VGAP)
         << "</vgap>"
         << "<hgap>" << PropertyString(PROP_HGAP) << "</hgap>"
         << "<growablecols>" << PropertyString(PROP_GROW_COLS) << "</growablecols>"
         << "<growablerows>" << PropertyString(PROP_GROW_ROWS) << "</growablerows>";
    ChildrenXRC(text, type);
    text << "</object>";
}

wxString GridBagSizerWrapper::GetWxClassName() const { return "wxGridBagSizer"; }

void GridBagSizerWrapper::GetIncludeFile(wxArrayString& headers) const
{
    SizerWrapperBase::GetIncludeFile(headers);
    headers.Add("#include <wx/gbsizer.h>");
}

void GridBagSizerWrapper::LoadPropertiesFromXRC(const wxXmlNode* node)
{
    wxcWidget::LoadPropertiesFromXRC(node); // Baseclass stuff first

    DoLoadXRCProperties(node);
}

void GridBagSizerWrapper::LoadPropertiesFromwxSmith(const wxXmlNode* node)
{
    wxcWidget::LoadPropertiesFromwxSmith(node); // Baseclass stuff first

    DoLoadXRCProperties(node);
}

void GridBagSizerWrapper::DoLoadXRCProperties(const wxXmlNode* node)
{
    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, "vgap");
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

void GridBagSizerWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxFB(node);

    wxXmlNode* propertynode = XmlUtils::FindNodeByName(node, "property", "vgap");
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

wxString GridBagSizerWrapper::DoGenerateCppCtorCode_End() const
{
    wxString code;
    wxArrayString growCols = wxCrafter::Split(PropertyString(PROP_GROW_COLS), ",");
    wxArrayString growRows = wxCrafter::Split(PropertyString(PROP_GROW_ROWS), ",");
    for(size_t i = 0; i < growCols.GetCount(); i++) {
        code << GetName() << "->AddGrowableCol(" << growCols.Item(i) << ");\n";
    }

    for(size_t i = 0; i < growRows.GetCount(); i++) {
        code << GetName() << "->AddGrowableRow(" << growRows.Item(i) << ");\n";
    }
    return code;
}
