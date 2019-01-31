#include "spin_button_wrapper.h"
#include "allocator_mgr.h"
#include "wxgui_defs.h"
#include "wxgui_helpers.h"
#include "xmlutils.h"
#include <wx/spinbutt.h>

SpinButtonWrapper::SpinButtonWrapper()
    : SpinWrapperBase(ID_WXSPINBUTTON)
{
    PREPEND_STYLE_FALSE(wxSP_HORIZONTAL);
    PREPEND_STYLE_TRUE(wxSP_VERTICAL);
    PREPEND_STYLE_FALSE(wxSP_ARROW_KEYS);
    PREPEND_STYLE_FALSE(wxSP_WRAP);

    RegisterEvent(wxT("wxEVT_SPIN_UP"), wxT("wxSpinEvent"), _("Generated when left/up arrow is pressed."));
    RegisterEvent(wxT("wxEVT_SPIN_DOWN"), wxT("wxSpinEvent"), _("Generated when right/down arrow is pressed."));
    RegisterEvent(wxT("wxEVT_SPIN"), wxT("wxSpinEvent"), _("Generated whenever an arrow is pressed."));

    SetPropertyString(_("Common Settings"), "wxSpinButton");
    AddProperty(new StringProperty(PROP_VALUE, wxT("0"), _("The initial value")));
    AddProperty(new StringProperty(PROP_MINVALUE, wxT("0"), _("Minimal value")));
    AddProperty(new StringProperty(PROP_MAXVALUE, wxT("100"), _("Maximal value")));

    m_namePattern = wxT("m_spinButton");
    SetName(GenerateName());
}

SpinButtonWrapper::~SpinButtonWrapper() {}

wxcWidget* SpinButtonWrapper::Clone() const { return new SpinButtonWrapper(); }

wxString SpinButtonWrapper::CppCtorCode() const
{
    wxString code;
    wxString min, max;
    int imin, imax, val;
    EnsureSaneValues(min, max, imin, imax, val);

    code << CPPStandardWxCtor(wxT("wxSP_VERTICAL"));
    code << GetName() << wxT("->SetRange(") << min << wxT(", ") << max << wxT(");\n");
    code << GetName() << wxT("->SetValue(") << val << wxT(");\n");

    return code;
}
void SpinButtonWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add(wxT("#include <wx/spinbutt.h>")); }

wxString SpinButtonWrapper::GetWxClassName() const { return wxT("wxSpinButton"); }

void SpinButtonWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    wxString min, max;
    int imin, imax, val;
    EnsureSaneValues(min, max, imin, imax, val);

    text << XRCPrefix() << XRCStyle() << XRCSize() << XRCCommonAttributes() << wxT("<value>")
         << wxCrafter::XMLEncode(wxCrafter::ToString(val)) << wxT("</value>") << wxT("<min>")
         << wxCrafter::XMLEncode(min) << wxT("</min>") << wxT("<max>") << wxCrafter::XMLEncode(max) << wxT("</max>")
         << XRCSuffix();
}
