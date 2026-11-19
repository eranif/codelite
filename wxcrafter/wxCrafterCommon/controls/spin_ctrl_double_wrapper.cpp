#include "spin_ctrl_double_wrapper.h"

#include "Properties/string_property.h"
#include "wxgui_defs.h"
#include "wxgui_helpers.h"
#include "xml/xmlutils.h"

#include <wx/spinctrl.h>

namespace
{
/// Parse `str` as a number using the "C" locale. Return `defval` if `str` is empty or not a number.
double ToDouble(const wxString& str, double defval)
{
    double d = 0.0;
    wxString s = str;
    s.Trim().Trim(false);
    if (s.empty() || !s.ToCDouble(&d)) {
        return defval;
    }
    return d;
}

/// Format `d` for use in C++ / XRC code. Always uses '.' as the decimal separator
wxString FormatDouble(double d) { return wxString::FromCDouble(d); }
} // namespace

SpinCtrlDoubleWrapper::SpinCtrlDoubleWrapper()
    : wxcWidget(ID_WXSPINCTRLDOUBLE)
{
    PREPEND_STYLE_TRUE(wxSP_ARROW_KEYS);
    PREPEND_STYLE_FALSE(wxSP_WRAP);

    RegisterEvent(wxT("wxEVT_SPINCTRLDOUBLE"),
                  wxT("wxSpinDoubleEvent"),
                  _("Generated whenever the numeric value of the spinctrl is updated"));
    RegisterEventCommand(wxT("wxEVT_COMMAND_TEXT_UPDATED"),
                         _("Process a wxEVT_COMMAND_TEXT_UPDATED event, when the spinCtrl text changes."));

    SetPropertyString(_("Common Settings"), "wxSpinCtrlDouble");
    Add<StringProperty>(PROP_VALUE, wxT("0"), _("The initial value"));
    Add<StringProperty>(PROP_MINVALUE, wxT("0"), _("Minimal value"));
    Add<StringProperty>(PROP_MAXVALUE, wxT("100"), _("Maximal value"));
    Add<StringProperty>(PROP_INCREMENT, wxT("1"), _("The amount by which the value changes with each step"));
    Add<StringProperty>(PROP_DIGITS, wxT("0"), _("The number of digits shown after the decimal point"));

    m_namePattern = wxT("m_spinCtrlDouble");
    SetName(GenerateName());
}

SpinCtrlDoubleWrapper::Numbers SpinCtrlDoubleWrapper::GetNumbers() const
{
    double min = ToDouble(PropertyString(PROP_MINVALUE), 0.0);
    double max = ToDouble(PropertyString(PROP_MAXVALUE), 100.0);
    if (max < min) {
        max = min + 100.0; // Protect against max < min, which upsets some toolkits
    }

    double value = ToDouble(PropertyString(PROP_VALUE), 0.0);
    value = wxMax(min, wxMin(max, value));

    double inc = ToDouble(PropertyString(PROP_INCREMENT), 1.0);
    if (inc <= 0.0) {
        inc = 1.0;
    }

    Numbers n;
    n.min = FormatDouble(min);
    n.max = FormatDouble(max);
    n.value = FormatDouble(value);
    n.inc = FormatDouble(inc);
    n.digits = wxMax(0, wxCrafter::ToNumber(PropertyString(PROP_DIGITS), 0));
    return n;
}

wxString SpinCtrlDoubleWrapper::CppCtorCode() const
{
    const Numbers n = GetNumbers();

    // wxSpinCtrlDouble(wxWindow* parent, wxWindowID id, const wxString& value, const wxPoint& pos, const wxSize& size,
    //                  long style, double min, double max, double initial, double inc, const wxString& name)
    wxString code;
    code << GetName() << " = new " << GetRealClassName() << "(" << GetWindowParent() << ", " << WindowID()
         << ", wxEmptyString, wxDefaultPosition, " << SizeAsString() << ", " << StyleFlags("wxSP_ARROW_KEYS") << ", "
         << n.min << ", " << n.max << ", " << n.value << ", " << n.inc << ");\n";
    code << CPPCommonAttributes();
    if (n.digits > 0) {
        code << GetName() << "->SetDigits(" << n.digits << ");\n";
    }
    return code;
}

void SpinCtrlDoubleWrapper::GetIncludeFile(wxArrayString& headers) const
{
    headers.Add(wxT("#include <wx/spinctrl.h>"));
}

wxString SpinCtrlDoubleWrapper::GetWxClassName() const { return wxT("wxSpinCtrlDouble"); }

void SpinCtrlDoubleWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    const Numbers n = GetNumbers();

    // Same as wxSpinCtrl: don't use the "subclass" property in the XRC,
    // see: https://github.com/eranif/codelite/issues/3054
    text << "<object class=\"" << GetWxClassName() << "\" name=\"" << wxCrafter::XMLEncode(GetName()) << "\">"
         << XRCStyle() << XRCSize() << XRCCommonAttributes() << "<value>" << n.value << "</value>"
         << "<min>" << n.min << "</min>"
         << "<max>" << n.max << "</max>"
         << "<inc>" << n.inc << "</inc>"
         << "<digits>" << n.digits << "</digits>" << XRCSuffix();
}

void SpinCtrlDoubleWrapper::DoLoadNumbers(const wxXmlNode* node, bool fromXRC)
{
    // wxFB writes the properties as <property name="..."> and XRC as <...>
    auto read = [&](const char* name, const wxString& propName) {
        wxXmlNode* propertynode =
            fromXRC ? XmlUtils::FindFirstByTagName(node, name) : XmlUtils::FindNodeByName(node, "property", name);
        if (propertynode) {
            SetPropertyString(propName, propertynode->GetNodeContent());
        }
    };
    read("min", PROP_MINVALUE);
    read("max", PROP_MAXVALUE);
    read("value", PROP_VALUE);
    read("inc", PROP_INCREMENT);
    read("digits", PROP_DIGITS);
}

void SpinCtrlDoubleWrapper::LoadPropertiesFromXRC(const wxXmlNode* node)
{
    wxcWidget::LoadPropertiesFromXRC(node);
    DoLoadNumbers(node, true);
}

void SpinCtrlDoubleWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node)
{
    wxcWidget::LoadPropertiesFromwxFB(node);
    DoLoadNumbers(node, false);
}
