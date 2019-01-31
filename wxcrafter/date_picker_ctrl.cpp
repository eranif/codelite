#include "date_picker_ctrl.h"
#include "allocator_mgr.h"
#include "wxgui_defs.h"
#include <wx/datectrl.h>

DatePickerCtrl::DatePickerCtrl()
    : wxcWidget(ID_WXDATEPICKER)
{
    PREPEND_STYLE_FALSE(wxDP_SPIN);
    PREPEND_STYLE_FALSE(wxDP_DROPDOWN);
    PREPEND_STYLE_TRUE(wxDP_DEFAULT);
    PREPEND_STYLE_FALSE(wxDP_ALLOWNONE);
    PREPEND_STYLE_FALSE(wxDP_SHOWCENTURY);

    RegisterEvent(wxT("wxEVT_DATE_CHANGED"), wxT("wxDateEvent"),
                  _("This event fires when the user changes the current selection in the control."));
    m_namePattern = wxT("m_datePicker");
    SetName(GenerateName());
}

DatePickerCtrl::~DatePickerCtrl() {}

wxcWidget* DatePickerCtrl::Clone() const { return new DatePickerCtrl(); }

wxString DatePickerCtrl::CppCtorCode() const
{
    // wxDatePickerCtrl(wxWindow *parent, wxWindowID id, const wxDateTime& dt = wxDefaultDateTime, const wxPoint& pos =
    // wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDP_DEFAULT | wxDP_SHOWCENTURY, const
    // wxValidator& validator = wxDefaultValidator, const wxString& name = "datectrl")
    wxString cpp;
    cpp << GetName() << wxT(" = new ") << GetRealClassName() << wxT("(") << GetWindowParent() << wxT(", ") << WindowID()
        << wxT(", ") << wxT("wxDefaultDateTime, ") << wxT("wxDefaultPosition, ") << SizeAsString() << wxT(", ")
        << StyleFlags(wxT("wxDP_DEFAULT | wxDP_SHOWCENTURY")) << wxT(");\n");

    cpp << CPPCommonAttributes();
    return cpp;
}

void DatePickerCtrl::GetIncludeFile(wxArrayString& headers) const { headers.Add(wxT("#include <wx/datectrl.h>")); }

wxString DatePickerCtrl::GetWxClassName() const { return wxT("wxDatePickerCtrl"); }

void DatePickerCtrl::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << XRCPrefix() << XRCStyle() << XRCSize() << XRCCommonAttributes() << XRCSuffix();
}
