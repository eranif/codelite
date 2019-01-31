#include "spin_ctrl_wrapper.h"
#include "allocator_mgr.h"
#include "string_property.h"
#include "wxgui_defs.h"
#include "wxgui_helpers.h"
#include "xmlutils.h"
#include <wx/spinctrl.h>

SpinCtrlWrapper::SpinCtrlWrapper()
    : SpinWrapperBase(ID_WXSPINCTRL)
{
    PREPEND_STYLE_TRUE(wxSP_ARROW_KEYS);
    PREPEND_STYLE_FALSE(wxSP_WRAP);

    RegisterEvent(wxT("wxEVT_SPINCTRL"), wxT("wxSpinEvent"),
                  _("Generated whenever the numeric value of the spinctrl is updated"));
    RegisterEventCommand(wxT("wxEVT_COMMAND_TEXT_UPDATED"),
                         wxT("Process a wxEVT_COMMAND_TEXT_UPDATED event, when the combobox text changes."));

    SetPropertyString(_("Common Settings"), "wxSpinCtrl");
    AddProperty(new StringProperty(PROP_VALUE, wxT("0"), _("The initial value")));
    AddProperty(new StringProperty(PROP_MINVALUE, wxT("0"), _("Minimal value")));
    AddProperty(new StringProperty(PROP_MAXVALUE, wxT("100"), _("Maximal value")));

    m_namePattern = wxT("m_spinCtrl");
    SetName(GenerateName());
}

SpinCtrlWrapper::~SpinCtrlWrapper() {}

wxcWidget* SpinCtrlWrapper::Clone() const { return new SpinCtrlWrapper(); }

wxString SpinCtrlWrapper::CppCtorCode() const
{
    wxString code;
    wxString min, max;
    int imin, imax, val;
    EnsureSaneValues(min, max, imin, imax, val);

    code << CPPStandardWxCtorWithValue(wxT("wxSP_ARROW_KEYS"));
    code << GetName() << wxT("->SetRange(") << min << wxT(", ") << max << wxT(");\n");
    // Set the value again now the range has been set: a value >100 (the default max) would previously have been
    // rejected
    code << GetName() << wxT("->SetValue(") << val << wxT(");\n");
    return code;
}

void SpinCtrlWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add(wxT("#include <wx/spinctrl.h>")); }

wxString SpinCtrlWrapper::GetWxClassName() const { return wxT("wxSpinCtrl"); }

void SpinCtrlWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    wxString min, max;
    int imin, imax, val;
    EnsureSaneValues(min, max, imin, imax, val);

    text << XRCPrefix() << XRCStyle() << XRCSize() << XRCCommonAttributes() << wxT("<value>")
         << wxCrafter::XMLEncode(wxCrafter::ToString(val)) << wxT("</value>") << wxT("<min>")
         << wxCrafter::XMLEncode(min) << wxT("</min>") << wxT("<max>") << wxCrafter::XMLEncode(max) << wxT("</max>")
         << XRCSuffix();
}
