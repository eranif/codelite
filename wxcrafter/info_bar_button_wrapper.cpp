#include "info_bar_button_wrapper.h"
#include "allocator_mgr.h"
#include "winid_property.h"
#include "wxgui_defs.h"

InfoBarButtonWrapper::InfoBarButtonWrapper()
    : wxcWidget(ID_WXINFOBARBUTTON)
{
    m_properties.DeleteValues();
    m_styles.Clear();
    m_sizerFlags.Clear();

    SetPropertyString(_("Common Settings"), "wxInfoBarButton");
    AddProperty(new CategoryProperty(_("Common Settings")));
    AddProperty(new WinIdProperty());
    AddProperty(new StringProperty(PROP_NAME, "", _("Name")));
    AddProperty(
        new StringProperty(PROP_LABEL, _("My Label"),
                           _("The label of the button. It may only be empty if the button ID is one of the stock ids "
                             "in which case the corresponding stock label (see wxGetStockLabel()) will be used")));

    RegisterEvent(wxT("wxEVT_COMMAND_BUTTON_CLICKED"), wxT("wxCommandEvent"),
                  _("Process a wxEVT_COMMAND_BUTTON_CLICKED event, when the button is clicked."),
                  wxT("wxCommandEventHandler"));

    m_namePattern = "m_infoBarButton";
    SetName(GenerateName());
}

InfoBarButtonWrapper::~InfoBarButtonWrapper() {}

wxcWidget* InfoBarButtonWrapper::Clone() const { return new InfoBarButtonWrapper(); }

wxString InfoBarButtonWrapper::CppCtorCode() const
{
    wxString cppCode;
    cppCode << GetParent()->GetName() << "->AddButton(" << GetId() << ", " << Label() << ");\n";
    return cppCode;
}

void InfoBarButtonWrapper::GetIncludeFile(wxArrayString& headers) const { wxUnusedVar(headers); }

wxString InfoBarButtonWrapper::GetWxClassName() const { return ""; }

void InfoBarButtonWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    wxUnusedVar(type);
    text << "<object class=\"wxInfoBarButton\" name=\"" << GetId() << "\">" << XRCLabel() << XRCSuffix();
}
