#include "ActivityrIndicatorWrapper.h"
#include "allocator_mgr.h"

ActivityrIndicatorWrapper::ActivityrIndicatorWrapper()
    : wxcWidget(ID_WXACTIVITYINDICATOR)
{
    RegisterEvent(wxT("wxEVT_COMMAND_BUTTON_CLICKED"), wxT("wxCommandEvent"),
                  _("Process a wxEVT_COMMAND_BUTTON_CLICKED event, when the button is clicked."),
                  wxT("wxCommandEventHandler"));
    m_namePattern = wxT("m_activityCtrl");
    SetPropertyString(_("Common Settings"), "wxActivityIndicator");
    SetName(GenerateName());
}

ActivityrIndicatorWrapper::~ActivityrIndicatorWrapper() {}

void ActivityrIndicatorWrapper::GetIncludeFile(wxArrayString& headers) const
{
    headers.Add("#include <wx/activityindicator.h>");
}

wxString ActivityrIndicatorWrapper::GetWxClassName() const { return wxT("wxActivityIndicator"); }

wxString ActivityrIndicatorWrapper::CppCtorCode() const
{
    wxString code;
    code << CPPStandardWxCtor("0");
    return code;
}

void ActivityrIndicatorWrapper::LoadPropertiesFromXRC(const wxXmlNode* node) { wxUnusedVar(node); }

void ActivityrIndicatorWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node)
{
    wxcWidget::LoadPropertiesFromXRC(node);
}

void ActivityrIndicatorWrapper::LoadPropertiesFromwxSmith(const wxXmlNode* node) { wxUnusedVar(node); }

void ActivityrIndicatorWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << XRCPrefix() << XRCStyle() << XRCSize() << XRCCommonAttributes() << XRCSuffix();
}
