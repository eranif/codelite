#include "ribbon_bar_wrapper.h"

#include "allocator_mgr.h"
#include "choice_property.h"
#include "ribbon_page_wrapper.h"

#include <wx/ribbon/bar.h>

RibbonBarWrapper::RibbonBarWrapper()
    : wxcWidget(ID_WXRIBBONBAR)
{
    RegisterEvent("wxEVT_COMMAND_RIBBONBAR_PAGE_CHANGED", "wxRibbonBarEvent",
                  _("Triggered after the transition from one page being active to a different page being active."));
    RegisterEvent("wxEVT_COMMAND_RIBBONBAR_PAGE_CHANGING", "wxRibbonBarEvent",
                  _("Triggered prior to the transition from one page being active to a different page being active, "
                    "and can veto the change."));
    RegisterEvent("wxEVT_COMMAND_RIBBONBAR_TAB_MIDDLE_DOWN", "wxRibbonBarEvent",
                  _("Triggered when the middle mouse button is pressed on a tab."));
    RegisterEvent("wxEVT_COMMAND_RIBBONBAR_TAB_MIDDLE_UP", "wxRibbonBarEvent",
                  _("Triggered when the middle mouse button is released on a tab."));
    RegisterEvent("wxEVT_COMMAND_RIBBONBAR_TAB_RIGHT_DOWN", "wxRibbonBarEvent",
                  _("Triggered when the right mouse button is pressed on a tab."));
    RegisterEvent("wxEVT_COMMAND_RIBBONBAR_TAB_RIGHT_UP", "wxRibbonBarEvent",
                  _("Triggered when the right mouse button is released on a tab."));
    RegisterEvent("wxEVT_COMMAND_RIBBONBAR_TAB_LEFT_DCLICK", "wxRibbonBarEvent",
                  _("Triggered when the left mouse button is double clicked on a tab."));

    PREPEND_STYLE_TRUE(wxRIBBON_BAR_DEFAULT_STYLE);
    PREPEND_STYLE_FALSE(wxRIBBON_BAR_FOLDBAR_STYLE);
    PREPEND_STYLE_FALSE(wxRIBBON_BAR_SHOW_PAGE_LABELS);
    PREPEND_STYLE_FALSE(wxRIBBON_BAR_SHOW_PAGE_ICONS);
    PREPEND_STYLE_FALSE(wxRIBBON_BAR_FLOW_HORIZONTAL);
    PREPEND_STYLE_FALSE(wxRIBBON_BAR_FLOW_VERTICAL);
    PREPEND_STYLE_FALSE(wxRIBBON_BAR_SHOW_PANEL_EXT_BUTTONS);
    PREPEND_STYLE_FALSE(wxRIBBON_BAR_SHOW_PANEL_MINIMISE_BUTTONS);

    SetPropertyString(_("Common Settings"), "wxRibbonBar");
    DelProperty(PROP_CONTROL_SPECIFIC_SETTINGS);
    AddProperty(new CategoryProperty("wxRibbonBar"));

    wxArrayString themes;
    themes.Add("Default");
    themes.Add("Generic");
    themes.Add("MSW");

    AddProperty(new ChoiceProperty(PROP_RIBBON_THEME, themes, 0, _("Select the ribbon bar theme")));
    m_namePattern = "m_ribbonBar";
    SetName(GenerateName());
}

RibbonBarWrapper::~RibbonBarWrapper() {}

wxcWidget* RibbonBarWrapper::Clone() const { return new RibbonBarWrapper(); }

wxString RibbonBarWrapper::CppCtorCode() const
{
    wxString cppCode;
    cppCode << CPPStandardWxCtor("wxRIBBON_BAR_DEFAULT_STYLE");
    wxString theme = PropertyString(PROP_RIBBON_THEME);
    if(theme == "MSW") {
        cppCode << GetName() << "->SetArtProvider(new wxRibbonMSWArtProvider);\n";
    } else if(theme == "Generic") {
        cppCode << GetName() << "->SetArtProvider(new wxRibbonAUIArtProvider);\n";
    } else {
        cppCode << GetName() << "->SetArtProvider(new wxRibbonDefaultArtProvider);\n";
    }
    return cppCode;
}

void RibbonBarWrapper::GetIncludeFile(wxArrayString& headers) const
{
    headers.Add("#include <wx/ribbon/bar.h>");
    headers.Add("#include <wx/ribbon/art.h>");
}

wxString RibbonBarWrapper::GetWxClassName() const { return "wxRibbonBar"; }

void RibbonBarWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << XRCPrefix() << XRCCommonAttributes() << XRCStyle() << XRCSize();
    // set the theme
    wxString theme = PropertyString(PROP_RIBBON_THEME);
    if(theme == "Generic") {
        theme = "aui";
    } else if(theme == "MSW") {
        theme = "msw";
    } else {
        theme = "default";
    }
    text << "<art-provider>" << theme << "</art-provider>";
    ChildrenXRC(text, type);
    text << XRCSuffix();
}

wxString RibbonBarWrapper::DoGenerateCppCtorCode_End() const { return wxString() << GetName() << "->Realize();\n"; }

void RibbonBarWrapper::SetSelection(RibbonPageWrapper* page)
{
    wxcWidget::List_t& children = GetChildren();
    wxcWidget::List_t::iterator iter = children.begin();
    for(; iter != children.end(); ++iter) {
        RibbonPageWrapper* p = dynamic_cast<RibbonPageWrapper*>(*iter);
        if(p) {
            p->Select(p == page);
        }
    }
}

RibbonPageWrapper* RibbonBarWrapper::GetSelection() const
{
    const wxcWidget::List_t& children = GetChildren();
    wxcWidget::List_t::const_iterator iter = children.begin();
    for(; iter != children.end(); ++iter) {
        RibbonPageWrapper* p = dynamic_cast<RibbonPageWrapper*>(*iter);
        if(p && p->IsSelected()) {
            return p;
        }
    }
    return NULL;
}
