#include "static_line_wrapper.h"
#include "allocator_mgr.h"
#include <wx/statline.h>

StaticLineWrapper::StaticLineWrapper()
    : wxcWidget(ID_WXSTATICLINE)
{
    SetPropertyString(_("Common Settings"), "wxStaticLine");
    PREPEND_STYLE(wxLI_HORIZONTAL, true);
    PREPEND_STYLE(wxLI_VERTICAL, false);

    EnableSizerFlag(wxT("wxEXPAND"), true);

    m_namePattern = wxT("m_staticLine");
    SetName(GenerateName());
}

StaticLineWrapper::~StaticLineWrapper() {}

wxcWidget* StaticLineWrapper::Clone() const { return new StaticLineWrapper(); }

wxString StaticLineWrapper::CppCtorCode() const
{
    // m_staticline1 = new wxStaticLine( m_panel11, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    wxString code;
    code << GetName() << wxT(" = new wxStaticLine(") << GetWindowParent() << wxT(", ") << WindowID() << wxT(", ")
         << wxT("wxDefaultPosition, ") << SizeAsString() << wxT(", ") << StyleFlags(wxT("0")) << wxT(");\n");
    code << CPPCommonAttributes();
    return code;
}

void StaticLineWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add(wxT("#include <wx/statline.h>")); }

wxString StaticLineWrapper::GetWxClassName() const { return wxT("wxStaticLine"); }

void StaticLineWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << XRCPrefix() << XRCStyle() << XRCCommonAttributes() << XRCSuffix();
}
