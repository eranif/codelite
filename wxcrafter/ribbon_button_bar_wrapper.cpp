#include "ribbon_button_bar_wrapper.h"
#include "allocator_mgr.h"
#include <wx/ribbon/buttonbar.h>

RibbonButtonBarWrapper::RibbonButtonBarWrapper()
    : wxcWidget(ID_WXRIBBONBUTTONBAR)
{
    SetPropertyString(_("Common Settings"), "wxRibbonButtonBar");
    m_namePattern = "m_ribbonButtonBar";
    SetName(GenerateName());
}

RibbonButtonBarWrapper::~RibbonButtonBarWrapper() {}

wxcWidget* RibbonButtonBarWrapper::Clone() const { return new RibbonButtonBarWrapper(); }

wxString RibbonButtonBarWrapper::CppCtorCode() const
{
    wxString cppcode;
    cppcode << CPPStandardWxCtor("0");
    return cppcode;
}

void RibbonButtonBarWrapper::GetIncludeFile(wxArrayString& headers) const
{
    headers.Add("#include <wx/ribbon/buttonbar.h>");
}

wxString RibbonButtonBarWrapper::GetWxClassName() const { return "wxRibbonButtonBar"; }

void RibbonButtonBarWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << XRCPrefix() << XRCSize() << XRCStyle();
    ChildrenXRC(text, type);
    text << XRCSuffix();
}

wxString RibbonButtonBarWrapper::DoGenerateCppCtorCode_End() const
{
    return wxString() << GetName() << "->Realize();\n\n";
}
