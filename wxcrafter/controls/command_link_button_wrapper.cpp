#include "command_link_button_wrapper.h"

#include "allocator_mgr.h"
#include "bitmap_picker_property.h"
#include "bool_property.h"
#include "file_ficker_property.h"
#include "wxc_bitmap_code_generator.h"
#include "wxgui_defs.h"
#include "wxgui_helpers.h"

#include <wx/button.h>

CommandLinkButtonWrapper::CommandLinkButtonWrapper()
    : wxcWidget(ID_WXCOMMANDLINKBUTTON)
{
    PREPEND_STYLE(wxBU_BOTTOM, false);
    PREPEND_STYLE(wxBU_EXACTFIT, false);
    PREPEND_STYLE(wxBU_LEFT, false);
    PREPEND_STYLE(wxBU_RIGHT, false);
    PREPEND_STYLE(wxBU_TOP, false);

    SetPropertyString(_("Common Settings"), "wxCommandLinkButton");
    DelProperty(_("Control Specific Settings"));
    AddProperty(new CategoryProperty("wxCommandLinkButton"));

    RegisterEvent("wxEVT_COMMAND_BUTTON_CLICKED", "wxCommandEvent",
                  _("Process a wxEVT_COMMAND_BUTTON_CLICKED event, when the button is clicked."),
                  "wxCommandEventHandler");

    AddProperty(new StringProperty(PROP_LABEL, _("Label"),
                                   _("First line of text on the button, typically the label of an action that will be "
                                     "made when the button is pressed")));
    AddProperty(new StringProperty(
        PROP_NOTE, "", _("Second line of text describing the action performed when the button is pressed")));
    AddProperty(new BitmapPickerProperty(PROP_BITMAP_PATH, "", _("Select the bitmap file")));
    AddProperty(new BoolProperty(PROP_DEFAULT_BUTTON, false, _("Make this button the default button")));

    m_namePattern = "m_cmdLnkBtn";
    SetName(GenerateName());
}

CommandLinkButtonWrapper::~CommandLinkButtonWrapper() {}

wxcWidget* CommandLinkButtonWrapper::Clone() const { return new CommandLinkButtonWrapper(); }

wxString CommandLinkButtonWrapper::CppCtorCode() const
{
    wxcCodeGeneratorHelper::Get().AddBitmap(PropertyFile(PROP_BITMAP_PATH));
    wxString cppCode;
    cppCode << GetName() << " = new " << GetRealClassName() << "(" << GetWindowParent() << ", " << GetId() << ", "
            << Label() << ", " << wxCrafter::UNDERSCORE(PropertyString(PROP_NOTE)) << ", "
            << "wxDefaultPosition, " << SizeAsString() << ", " << StyleFlags("0") << ");\n";

    wxString bmpCpp = wxcCodeGeneratorHelper::Get().BitmapCode(PropertyFile(PROP_BITMAP_PATH));
    if(bmpCpp != "wxNullBitmap") {
        cppCode << GetName() << "->SetBitmap(" << bmpCpp << ", wxLEFT );\n";
    }

    if(PropertyBool(PROP_DEFAULT_BUTTON) == "true") {
        cppCode << GetName() << "->SetDefault();\n";
    }

    cppCode << CPPCommonAttributes();
    return cppCode;
}

void CommandLinkButtonWrapper::GetIncludeFile(wxArrayString& headers) const
{
    headers.Add("#include <wx/commandlinkbutton.h>");
}

wxString CommandLinkButtonWrapper::GetWxClassName() const { return "wxCommandLinkButton"; }

void CommandLinkButtonWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    wxUnusedVar(type);
    text << XRCPrefix() << XRCSize() << XRCStyle() << XRCCommonAttributes() << XRCLabel() << XRCBitmap() << "<note>"
         << wxCrafter::CDATA(PropertyString(PROP_NOTE)) << "</note>" << XRCSuffix();
}
