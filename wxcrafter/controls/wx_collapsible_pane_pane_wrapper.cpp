#include "wx_collapsible_pane_pane_wrapper.h"
#include "allocator_mgr.h"
#include "wxgui_helpers.h"

wxCollapsiblePanePaneWrapper::wxCollapsiblePanePaneWrapper()
    : wxcWidget(ID_WXCOLLAPSIBLEPANE_PANE)
{
    m_properties.DeleteValues();
    m_styles.Clear();
    m_sizerFlags.Clear();

    AddProperty(new StringProperty(PROP_NAME, _("Name"), _("Name")));
    m_namePattern = "m_collpaneWin";
    SetName(GenerateName());
}

wxCollapsiblePanePaneWrapper::~wxCollapsiblePanePaneWrapper() {}

wxcWidget* wxCollapsiblePanePaneWrapper::Clone() const { return new wxCollapsiblePanePaneWrapper(); }

wxString wxCollapsiblePanePaneWrapper::CppCtorCode() const { return ""; }

void wxCollapsiblePanePaneWrapper::GetIncludeFile(wxArrayString& headers) const { wxUnusedVar(headers); }

wxString wxCollapsiblePanePaneWrapper::GetWxClassName() const { return ""; }

void wxCollapsiblePanePaneWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    wxString xrc;
    xrc << "<object class=\"panewindow\" name=\"" << wxCrafter::XMLEncode(GetName()) << "\"";
    if(!PropertyString(PROP_SUBCLASS_NAME).empty()) {
        xrc << " subclass=\"" << wxCrafter::XMLEncode(PropertyString(PROP_SUBCLASS_NAME)) << "\"";
    }
    xrc << ">";
    xrc << XRCSize() << XRCCommonAttributes() << XRCStyle();
    ChildrenXRC(xrc, type);
    xrc << XRCSuffix();

    text << xrc;
}
