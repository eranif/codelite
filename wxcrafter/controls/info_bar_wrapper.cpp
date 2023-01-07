#include "info_bar_wrapper.h"

#include "allocator_mgr.h"
#include "choice_property.h"
#include "wxgui_helpers.h"

InfoBarWrapper::InfoBarWrapper()
    : wxcWidget(ID_WXINFOBAR)
{
    SetPropertyString(_("Common Settings"), "wxInfoBar");
    m_styles.Clear();
    m_namePattern = "m_infobar";
    SetName(GenerateName());
}

InfoBarWrapper::~InfoBarWrapper() {}

wxcWidget* InfoBarWrapper::Clone() const { return new InfoBarWrapper(); }

wxString InfoBarWrapper::CppCtorCode() const
{
    wxString cppCode;
    cppCode << GetName() << " = new " << GetRealClassName() << "(" << GetWindowParent() << ", " << GetId() << ");\n";
    cppCode << GetName() << "->SetSize(" << SizeAsString() << ");\n";
    cppCode << CPPCommonAttributes();
    return cppCode;
}

void InfoBarWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add("#include <wx/infobar.h>"); }

wxString InfoBarWrapper::GetWxClassName() const { return "wxInfoBar"; }

void InfoBarWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    if(type == XRC_LIVE) {
        text << XRCUnknown();
    } else {

        text << XRCPrefix() << XRCSize() << XRCStyle() << XRCCommonAttributes()
             << "<message> Something happened...</message>"
             << "<icon-id>wxICON_INFORMATION</icon-id>";

        ChildrenXRC(text, type);

        text << XRCSuffix();
    }
}
