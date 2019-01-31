#include "ribbon_panel_wrapper.h"
#include "allocator_mgr.h"
#include "file_ficker_property.h"
#include "string_property.h"
#include "wxc_bitmap_code_generator.h"
#include "wxgui_defs.h"
#include <wx/ribbon/panel.h>

RibbonPanelWrapper::RibbonPanelWrapper()
    : wxcWidget(ID_WXRIBBONPANEL)
{
    PREPEND_STYLE_TRUE(wxRIBBON_PANEL_DEFAULT_STYLE);
    PREPEND_STYLE_FALSE(wxRIBBON_PANEL_NO_AUTO_MINIMISE);
    PREPEND_STYLE_FALSE(wxRIBBON_PANEL_EXT_BUTTON);
    PREPEND_STYLE_FALSE(wxRIBBON_PANEL_MINIMISE_BUTTON);
    PREPEND_STYLE_FALSE(wxRIBBON_PANEL_STRETCH);
    PREPEND_STYLE_FALSE(wxRIBBON_PANEL_FLEXIBLE);

    SetPropertyString(_("Common Settings"), "wxRibbonPanel");
    AddProperty(new StringProperty(PROP_LABEL, _("My Label"), _("The Label")));
    AddProperty(new BitmapPickerProperty(
        PROP_BITMAP_PATH, "", _("Icon to be used in place of the panel's children when the panel is minimised")));

    RegisterEvent("wxEVT_COMMAND_RIBBONPANEL_EXTBUTTON_ACTIVATED", "wxRibbonPanelEvent",
                  _("Triggered when the user activate the panel extension button"));
    m_namePattern = "m_ribbonPanel";

    SetName(GenerateName());
}

RibbonPanelWrapper::~RibbonPanelWrapper() {}

wxcWidget* RibbonPanelWrapper::Clone() const { return new RibbonPanelWrapper(); }

wxString RibbonPanelWrapper::CppCtorCode() const
{
    wxString cppCode;
    wxcCodeGeneratorHelper::Get().AddBitmap(PropertyFile(PROP_BITMAP_PATH));

    cppCode << GetName() << " = new " << GetRealClassName() << "(" << GetWindowParent() << ", " << GetId() << ", "
            << Label() << ", " << wxcCodeGeneratorHelper::Get().BitmapCode(PropertyFile(PROP_BITMAP_PATH)) << ", "
            << "wxDefaultPosition, " << SizeAsString() << ", " << StyleFlags("wxRIBBON_PANEL_DEFAULT_STYLE") << ");\n";
    cppCode << CPPCommonAttributes();
    return cppCode;
}

void RibbonPanelWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add("#include <wx/ribbon/panel.h>"); }

wxString RibbonPanelWrapper::GetWxClassName() const { return "wxRibbonPanel"; }

void RibbonPanelWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << XRCPrefix() << XRCCommonAttributes() << XRCSize() << XRCBitmap("icon") << XRCStyle() << XRCLabel();
    ChildrenXRC(text, type);
    text << XRCSuffix();
}
