#include "grid_bag_sizer_wrapper.h"
#include "allocator_mgr.h"
#include "wxgui_helpers.h"
#include "xmlutils.h"

GridBagSizerWrapper::GridBagSizerWrapper()
{
    m_type = ID_WXGRIDBAGSIZER;
    m_styles.Clear(); // Sizer has no styles
    SetPropertyString(_("Common Settings"), "wxGridBagSizer");
    AddProperty(
        new StringProperty(PROP_GROW_COLS, wxT(""), wxT("Which columns are allowed to grow. Comma separated list")));
    AddProperty(
        new StringProperty(PROP_GROW_ROWS, wxT(""), wxT("Which rows are allowed to grow. Comma separated list")));
    AddProperty(new StringProperty(PROP_HGAP, wxT("0"), wxT("The horizontal gap between grid cells")));
    AddProperty(new StringProperty(PROP_VGAP, wxT("0"), wxT("The vertical gap between grid cells")));

    EnableSizerFlag("wxEXPAND", true);
    m_sizerItem.SetProportion(1);

    m_namePattern = wxT("gridBagSizer");
    SetName(GenerateName());
}

GridBagSizerWrapper::~GridBagSizerWrapper() {}

wxString GridBagSizerWrapper::CppCtorCode() const
{
    wxString code;

    if(!wxcSettings::Get().HasFlag(wxcSettings::SIZERS_AS_MEMBERS)) code << "wxGridBagSizer* ";

    code << GetName() << " = new wxGridBagSizer(" << PropertyString(PROP_VGAP) << wxT(", ") << PropertyString(PROP_HGAP)
         << wxT(");\n");
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

void GridBagSizerWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << wxT("<object class=\"wxGridBagSizer\">") << GenerateMinSizeXRC() << wxT("<vgap>")
         << PropertyString(PROP_VGAP) << wxT("</vgap>") << wxT("<hgap>") << PropertyString(PROP_HGAP) << wxT("</hgap>")
         << wxT("<growablecols>") << PropertyString(PROP_GROW_COLS) << wxT("</growablecols>") << wxT("<growablerows>")
         << PropertyString(PROP_GROW_ROWS) << wxT("</growablerows>");
    ChildrenXRC(text, type);
    text << wxT("</object>");
}

wxString GridBagSizerWrapper::GetWxClassName() const { return wxT("wxGridBagSizer"); }

void GridBagSizerWrapper::GetIncludeFile(wxArrayString& headers) const
{
    SizerWrapperBase::GetIncludeFile(headers);
    headers.Add(wxT("#include <wx/gbsizer.h>"));
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
    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("vgap"));
    if(propertynode) { SetPropertyString(PROP_VGAP, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("hgap"));
    if(propertynode) { SetPropertyString(PROP_HGAP, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("growablecols"));
    if(propertynode) { SetPropertyString(PROP_GROW_COLS, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("growablerows"));
    if(propertynode) { SetPropertyString(PROP_GROW_ROWS, propertynode->GetNodeContent()); }
}

void GridBagSizerWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxFB(node);

    wxXmlNode* propertynode = XmlUtils::FindNodeByName(node, "property", "vgap");
    if(propertynode) { SetPropertyString(PROP_VGAP, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindNodeByName(node, "property", "hgap");
    if(propertynode) { SetPropertyString(PROP_HGAP, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindNodeByName(node, "property", "growablecols");
    if(propertynode) { SetPropertyString(PROP_GROW_COLS, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindNodeByName(node, "property", "growablerows");
    if(propertynode) { SetPropertyString(PROP_GROW_ROWS, propertynode->GetNodeContent()); }
}

wxString GridBagSizerWrapper::DoGenerateCppCtorCode_End() const
{
    wxString code;
    wxArrayString growCols = wxCrafter::Split(PropertyString(PROP_GROW_COLS), wxT(","));
    wxArrayString growRows = wxCrafter::Split(PropertyString(PROP_GROW_ROWS), wxT(","));
    for(size_t i = 0; i < growCols.GetCount(); i++) {
        code << GetName() << wxT("->AddGrowableCol(") << growCols.Item(i) << wxT(");\n");
    }

    for(size_t i = 0; i < growRows.GetCount(); i++) {
        code << GetName() << wxT("->AddGrowableRow(") << growRows.Item(i) << wxT(");\n");
    }
    return code;
}
