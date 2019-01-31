#include "ribbon_tool_separator.h"
#include "allocator_mgr.h"
#include "category_property.h"
#include "string_property.h"
#include "wxgui_defs.h"
#include "wxgui_helpers.h"

RibbonToolSeparator::RibbonToolSeparator()
    : wxcWidget(ID_WXRIBBONTOOLSEPARATOR)
{
    m_properties.DeleteValues();
    m_sizerFlags.Clear();

    AddProperty(new CategoryProperty(_("wxRibbonToolBar Separator")));
    AddProperty(new StringProperty(PROP_NAME, wxT(""), _("Name")));

    m_namePattern = "m_separator";
    SetName(GenerateName());
}

RibbonToolSeparator::~RibbonToolSeparator() {}

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
