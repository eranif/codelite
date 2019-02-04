#include "collapsible_pane_wrapper.h"
#include "allocator_mgr.h"
#include "bool_property.h"
#include "wxc_settings.h"
#include "wxgui_defs.h"
#include "wxgui_helpers.h"
#include <wx/collpane.h>

CollapsiblePaneWrapper::CollapsiblePaneWrapper()
    : wxcWidget(ID_WXCOLLAPSIBLEPANE)
{
    RegisterEvent("wxEVT_COMMAND_COLLPANE_CHANGED", "wxCollapsiblePaneEvent",
                  _("The user expanded or collapsed the collapsible pane"));
    RegisterEvent("wxEVT_NAVIGATION_KEY", "wxNavigationKeyEvent", _("Process a navigation key event"));

    PREPEND_STYLE(wxCP_DEFAULT_STYLE, true);
    PREPEND_STYLE(wxCP_NO_TLW_RESIZE, false);

    SetPropertyString(_("Common Settings"), "wxCollapsiblePane");
    AddProperty(new StringProperty(PROP_LABEL, _("Label"), _("The label")));
    AddProperty(new BoolProperty(PROP_COLLAPSED, true, _("Set the state of the collapsible pane")));
    m_namePattern = "m_collPane";
    SetName(GenerateName());
}

CollapsiblePaneWrapper::~CollapsiblePaneWrapper() {}

wxcWidget* CollapsiblePaneWrapper::Clone() const { return new CollapsiblePaneWrapper(); }

wxString CollapsiblePaneWrapper::CppCtorCode() const
{
    wxString cppCode = CPPStandardWxCtorWithLabel("wxCP_DEFAULT_STYLE");
    return cppCode;
}

void CollapsiblePaneWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add("#include <wx/collpane.h>"); }

wxString CollapsiblePaneWrapper::GetWxClassName() const { return "wxCollapsiblePane"; }

void CollapsiblePaneWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{

    text << XRCPrefix() << XRCLabel() << XRCSize() << XRCCommonAttributes();

    if(type == XRC_DESIGNER) {
        text << "<style>wxCP_NO_TLW_RESIZE</style>";

    } else {
        text << XRCStyle()  << "<collapsed>" << PropertyString(PROP_COLLAPSED) << "</collapsed>";
    }

    ChildrenXRC(text, type);

    text << XRCSuffix();
}

bool CollapsiblePaneWrapper::IsLicensed() const { return wxcSettings::Get().IsLicensed(); }
