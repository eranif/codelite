#include "wrap_sizer_wrapper.h"

#include "Properties/bool_property.h"
#include "Properties/choice_property.h"
#include "wxgui_defs.h"
#include "xml/xmlutils.h"

WrapSizerWrapper::WrapSizerWrapper()
{
    m_type = ID_WXWRAPSIZER;
    m_styles.Clear(); // Sizer has no styles

    const wxArrayString arr = {"wxVERTICAL", "wxHORIZONTAL"};

    SetPropertyString(_("Common Settings"), "wxWrapSizer");
    Add<ChoiceProperty>(PROP_ORIENTATION, arr, 1, _("Sizer orientation"));
    Add<BoolProperty>(PROP_WRAP_EXTEND_LAST,
                      true,
                      _("Extend the last item on each line to fill the remaining space (wxEXTEND_LAST_ON_EACH_LINE)"));
    Add<BoolProperty>(PROP_WRAP_REMOVE_LEADING,
                      true,
                      _("Remove the space items at the start of each line (wxREMOVE_LEADING_SPACES)"));

    EnableSizerFlag("wxEXPAND", true);
    m_sizerItem.SetProportion(1);

    m_namePattern = "wrapSizer";
    SetName(GenerateName());
}

wxString WrapSizerWrapper::FlagsAsString() const
{
    wxString flags;
    if (IsPropertyChecked(PROP_WRAP_EXTEND_LAST)) {
        flags << "wxEXTEND_LAST_ON_EACH_LINE";
    }
    if (IsPropertyChecked(PROP_WRAP_REMOVE_LEADING)) {
        if (!flags.empty()) {
            flags << "|";
        }
        flags << "wxREMOVE_LEADING_SPACES";
    }
    return flags.empty() ? wxString("0") : flags;
}

void WrapSizerWrapper::DoLoadFlags(const wxString& flags)
{
    SetPropertyString(PROP_WRAP_EXTEND_LAST, flags.Contains("wxEXTEND_LAST_ON_EACH_LINE") ? "1" : "0");
    SetPropertyString(PROP_WRAP_REMOVE_LEADING, flags.Contains("wxREMOVE_LEADING_SPACES") ? "1" : "0");
}

void WrapSizerWrapper::GetIncludeFile(wxArrayString& headers) const
{
    SizerWrapperBase::GetIncludeFile(headers);
    headers.Add("#include <wx/wrapsizer.h>");
}

wxString WrapSizerWrapper::CppCtorCode() const
{
    wxString code;
    if (!KeepAsClassMember()) {
        code << "wxWrapSizer* ";
    }

    code << GetName() << " = new wxWrapSizer(" << PropertyString(PROP_ORIENTATION) << ", " << FlagsAsString() << ");\n";

    code << GenerateMinSizeCode();

    if (IsMainSizer()) {
        if (GetParent()->IsTopWindow()) {
            code << "this->SetSizer(" << GetName() << ");\n";
        } else {
            code << GetParent()->GetName() << "->SetSizer(" << GetName() << ");\n";
        }
    }
    return code;
}

void WrapSizerWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << "<object class=\"wxWrapSizer\">" << GenerateMinSizeXRC() << "<orient>" << PropertyString(PROP_ORIENTATION)
         << "</orient>"
         << "<flag>" << FlagsAsString() << "</flag>";
    ChildrenXRC(text, type);
    text << "</object>";
}

wxString WrapSizerWrapper::GetWxClassName() const { return "wxWrapSizer"; }

void WrapSizerWrapper::LoadPropertiesFromXRC(const wxXmlNode* node)
{
    wxcWidget::LoadPropertiesFromXRC(node); // Baseclass stuff first

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, "orient");
    if (propertynode) {
        SetPropertyString(PROP_ORIENTATION, propertynode->GetNodeContent());
    }

    propertynode = XmlUtils::FindFirstByTagName(node, "flag");
    if (propertynode) {
        DoLoadFlags(propertynode->GetNodeContent());
    }
}

void WrapSizerWrapper::LoadPropertiesFromwxSmith(const wxXmlNode* node)
{
    // wxSmith has no wxWrapSizer, but use the same layout as XRC in case it is ever imported
    LoadPropertiesFromXRC(node);
}

void WrapSizerWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node)
{
    wxcWidget::LoadPropertiesFromwxFB(node);

    wxXmlNode* propertynode = XmlUtils::FindNodeByName(node, "property", "orient");
    if (propertynode) {
        SetPropertyString(PROP_ORIENTATION, propertynode->GetNodeContent());
    }

    propertynode = XmlUtils::FindNodeByName(node, "property", "flags");
    if (propertynode) {
        DoLoadFlags(propertynode->GetNodeContent());
    }
}
