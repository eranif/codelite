#include "ribbon_tool_bar_wrapper.h"
#include "allocator_mgr.h"
#include "int_property.h"
#include <wx/ribbon/toolbar.h>

RibbonToolBarWrapper::RibbonToolBarWrapper()
    : wxcWidget(ID_WXRIBBONTOOLBAR)
{
    SetPropertyString(_("Common Settings"), "wxRibbonToolBar");
    DelProperty(PROP_CONTROL_SPECIFIC_SETTINGS);
    AddProperty(new CategoryProperty("wxRibbonToolBar"));

    AddProperty(new IntProperty(PROP_RIBBON_TOOLBAR_MIN_ROWS, 1,
                                _("Set the minimum number of rows to distribute tool groups over")));
    AddProperty(
        new IntProperty(PROP_RIBBON_TOOLBAR_MAX_ROWS, -1,
                        _("Set the maximum number of rows to distribute tool groups over. Use -1 as default value")));

    m_namePattern = "m_ribbonToolbar";
    SetName(GenerateName());
}

RibbonToolBarWrapper::~RibbonToolBarWrapper() {}

wxcWidget* RibbonToolBarWrapper::Clone() const { return new RibbonToolBarWrapper(); }

wxString RibbonToolBarWrapper::CppCtorCode() const
{
    wxString cppCode;
    cppCode << CPPStandardWxCtor("0");
    int minRows = PropertyInt(PROP_RIBBON_TOOLBAR_MIN_ROWS, 1);
    int maxRows = PropertyInt(PROP_RIBBON_TOOLBAR_MAX_ROWS, -1);
    if(minRows > 0 && (maxRows >= minRows || maxRows == -1)) {
        cppCode << GetName() << "->SetRows(" << minRows << ", " << maxRows << ");\n";
    }
    return cppCode;
}

void RibbonToolBarWrapper::GetIncludeFile(wxArrayString& headers) const
{
    headers.Add("#include <wx/ribbon/toolbar.h>");
}

wxString RibbonToolBarWrapper::GetWxClassName() const { return "wxRibbonToolBar"; }

void RibbonToolBarWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << XRCPrefix() << XRCCommonAttributes() << XRCSize() << XRCStyle() << "<minrows>"
         << PropertyInt(PROP_RIBBON_TOOLBAR_MIN_ROWS, 1) << "</minrows>"
         << "<maxrows>" << PropertyInt(PROP_RIBBON_TOOLBAR_MAX_ROWS, -1) << "</maxrows>";
    ChildrenXRC(text, type);
    text << XRCSuffix();
}

wxString RibbonToolBarWrapper::DoGenerateCppCtorCode_End() const
{
    return wxString() << GetName() << "->Realize();\n\n";
}
