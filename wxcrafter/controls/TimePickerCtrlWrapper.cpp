#include "TimePickerCtrlWrapper.h"
#include "allocator_mgr.h"

TimePickerCtrlWrapper::TimePickerCtrlWrapper()
    : wxcWidget(ID_WXTIMEPICKERCTRL)
{
    RegisterEvent(wxT("wxEVT_TIME_CHANGED"), wxT("wxDateEvent"),
                  _("This event fires when the user changes the current selection in the control"));
    m_namePattern = wxT("m_timePickerCtrl");
    SetPropertyString(_("Common Settings"), "wxTimePickerCtrl");
    SetName(GenerateName());
}

TimePickerCtrlWrapper::~TimePickerCtrlWrapper() {}

void TimePickerCtrlWrapper::GetIncludeFile(wxArrayString& headers) const
{
    headers.Add("#include <wx/timectrl.h>");
    headers.Add("#include <wx/dateevt.h>");
}

wxString TimePickerCtrlWrapper::GetWxClassName() const { return wxT("wxTimePickerCtrl"); }

wxString TimePickerCtrlWrapper::CppCtorCode() const
{
    wxString cpp;
    cpp << GetName() << wxT(" = new ") << GetRealClassName() << wxT("(") << GetWindowParent() << wxT(", ") << WindowID()
        << wxT(", wxDefaultDateTime, wxDefaultPosition, ") << SizeAsString() << wxT(", wxTP_DEFAULT);\n");
    cpp << CPPCommonAttributes();
    return cpp;
}

void TimePickerCtrlWrapper::LoadPropertiesFromXRC(const wxXmlNode* node) { wxUnusedVar(node); }

void TimePickerCtrlWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node) { wxUnusedVar(node); }

void TimePickerCtrlWrapper::LoadPropertiesFromwxSmith(const wxXmlNode* node) { wxUnusedVar(node); }

void TimePickerCtrlWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << XRCPrefix() << XRCStyle() << XRCSize() << XRCCommonAttributes() << XRCSuffix();
}
