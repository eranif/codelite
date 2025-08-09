#include "ActivityIndicatorWrapper.h"

#include "allocator_mgr.h"

ActivityIndicatorWrapper::ActivityIndicatorWrapper()
    : wxcWidget(ID_WXACTIVITYINDICATOR)
{
    RegisterEvent(wxT("wxEVT_COMMAND_BUTTON_CLICKED"),
                  wxT("wxCommandEvent"),
                  _("Process a wxEVT_COMMAND_BUTTON_CLICKED event, when the button is clicked."),
                  wxT("wxCommandEventHandler"));
    m_namePattern = wxT("m_activityCtrl");
    SetPropertyString(_("Common Settings"), "wxActivityIndicator");
    SetName(GenerateName());
}

void ActivityIndicatorWrapper::GetIncludeFile(wxArrayString& headers) const
{
    headers.Add("#include <wx/activityindicator.h>");
}

wxString ActivityIndicatorWrapper::GetWxClassName() const { return wxT("wxActivityIndicator"); }

wxString ActivityIndicatorWrapper::CppCtorCode() const
{
    wxString code;
    code << CPPStandardWxCtor("0");
    return code;
}

void ActivityIndicatorWrapper::LoadPropertiesFromXRC(const wxXmlNode* node) { wxUnusedVar(node); }

void ActivityIndicatorWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node) { wxcWidget::LoadPropertiesFromXRC(node); }

void ActivityIndicatorWrapper::LoadPropertiesFromwxSmith(const wxXmlNode* node) { wxUnusedVar(node); }

void ActivityIndicatorWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << XRCPrefix() << XRCStyle() << XRCSize() << XRCCommonAttributes() << XRCSuffix();
}
