#include "ribbon_tool_separator.h"

#include "Properties/category_property.h"
#include "Properties/string_property.h"
#include "allocator_mgr.h"
#include "wxgui_defs.h"
#include "wxgui_helpers.h"

RibbonToolSeparator::RibbonToolSeparator()
    : wxcWidget(ID_WXRIBBONTOOLSEPARATOR)
{
    m_properties.Clear();
    m_sizerFlags.Clear();

    Add<CategoryProperty>(_("wxRibbonToolBar Separator"));
    Add<StringProperty>(PROP_NAME, wxT(""), _("Name"));

    m_namePattern = "m_separator";
    SetName(GenerateName());
}

wxcWidget* RibbonToolSeparator::Clone() const { return new RibbonToolSeparator(); }

wxString RibbonToolSeparator::CppCtorCode() const
{
    wxString code;
    code << GetParent()->GetName() << "->AddSeparator();\n";
    return code;
}

void RibbonToolSeparator::GetIncludeFile(wxArrayString& headers) const { wxUnusedVar(headers); }

wxString RibbonToolSeparator::GetWxClassName() const { return ""; }

void RibbonToolSeparator::ToXRC(wxString& text, XRC_TYPE type) const
{
    wxUnusedVar(type);
    text << "<object class=\"tool\" name=\"" << wxCrafter::XMLEncode(GetName()) << "\">"
         << "<separator>1</separator>" << XRCSuffix();
}
