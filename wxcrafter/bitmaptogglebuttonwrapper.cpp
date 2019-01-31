#include "bitmaptogglebuttonwrapper.h"
#include "allocator_mgr.h"
#include "wxc_bitmap_code_generator.h"
#include "wxgui_defs.h"

BitmapToggleButtonWrapper::BitmapToggleButtonWrapper()
    : wxcWidget(ID_WXBITMAPTOGGLEBUTTON)
{
    SetPropertyString(_("Common Settings"), "wxBitmapToggleButton");
    AddProperty(new BitmapPickerProperty(PROP_BITMAP_PATH, "", wxT("The bitmap")));
    AddProperty(new BoolProperty(PROP_CHECKED, wxT("Initial state"), wxT("The button initial state")));

    PREPEND_STYLE(wxBU_BOTTOM, false);
    PREPEND_STYLE(wxBU_EXACTFIT, false);
    PREPEND_STYLE(wxBU_LEFT, false);
    PREPEND_STYLE(wxBU_RIGHT, false);
    PREPEND_STYLE(wxBU_TOP, false);

    RegisterEventCommand(wxT("wxEVT_COMMAND_TOGGLEBUTTON_CLICKED"), wxT("Handles a toggle button click event."));

    m_namePattern = "m_bmpToggleBtn";
    SetName(GenerateName());
}

BitmapToggleButtonWrapper::~BitmapToggleButtonWrapper() {}

wxcWidget* BitmapToggleButtonWrapper::Clone() const { return new BitmapToggleButtonWrapper(); }

wxString BitmapToggleButtonWrapper::CppCtorCode() const
{
    wxcCodeGeneratorHelper::Get().AddBitmap(PropertyFile(PROP_BITMAP_PATH));
    wxString cpp;
    cpp << GetName() << wxT(" = new ") << GetRealClassName() << wxT("(") << GetWindowParent() << wxT(", ") << WindowID()
        << wxT(", ") << wxcCodeGeneratorHelper::Get().BitmapCode(PropertyFile(PROP_BITMAP_PATH)) << wxT(", ")
        << wxT("wxDefaultPosition, ") << SizeAsString() << wxT(", ") << StyleFlags("0") << wxT(");\n");
    cpp << CPPCommonAttributes();
    cpp << GetName() << wxT("->SetValue(") << PropertyBool(PROP_CHECKED) << wxT(");\n");
    return cpp;
}

void BitmapToggleButtonWrapper::GetIncludeFile(wxArrayString& headers) const
{
    headers.Add(wxT("#include <wx/tglbtn.h>"));
    headers.Add(wxT("#include <wx/button.h>"));
}

wxString BitmapToggleButtonWrapper::GetWxClassName() const { return "wxBitmapToggleButton"; }

void BitmapToggleButtonWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << XRCPrefix() << XRCStyle() << XRCSize() << XRCBitmap() << XRCCommonAttributes() << wxT("<checked>")
         << PropertyString(PROP_CHECKED) << wxT("</checked>") << XRCSuffix();
}
