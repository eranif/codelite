#include "calendar_ctrl_wrapper.h"
#include "allocator_mgr.h"
#include "wxgui_defs.h"
#include <wx/calctrl.h>

CalendarCtrlWrapper::CalendarCtrlWrapper()
    : wxcWidget(ID_WXCALEDARCTRL)
{
    PREPEND_STYLE_FALSE(wxCAL_SUNDAY_FIRST);
    PREPEND_STYLE_FALSE(wxCAL_MONDAY_FIRST);
    PREPEND_STYLE_TRUE(wxCAL_SHOW_HOLIDAYS);
    PREPEND_STYLE_FALSE(wxCAL_NO_YEAR_CHANGE);
    PREPEND_STYLE_FALSE(wxCAL_NO_MONTH_CHANGE);
    PREPEND_STYLE_FALSE(wxCAL_SHOW_SURROUNDING_WEEKS);
    PREPEND_STYLE_FALSE(wxCAL_SEQUENTIAL_MONTH_SELECTION);

    RegisterEvent(wxT("wxEVT_CALENDAR_DOUBLECLICKED"), wxT("wxCalendarEvent"),
                  _("A day was double clicked in the calendar."));
    RegisterEvent(wxT("wxEVT_CALENDAR_DAY_CHANGED"), wxT("wxCalendarEvent"), _("The selected day changed."));
    RegisterEvent(wxT("wxEVT_CALENDAR_MONTH_CHANGED"), wxT("wxCalendarEvent"), _("The selected month changed."));
    RegisterEvent(wxT("wxEVT_CALENDAR_SEL_CHANGED"), wxT("wxCalendarEvent"), _("The selected date changed."));
    RegisterEvent(wxT("wxEVT_CALENDAR_WEEKDAY_CLICKED"), wxT("wxCalendarEvent"),
                  _("User clicked on the week day header"));
    RegisterEvent(wxT("wxEVT_CALENDAR_YEAR_CHANGED"), wxT("wxCalendarEvent"), _("The selected year changed."));

    SetPropertyString(_("Common Settings"), "wxCalendarCtrl");
    m_namePattern = wxT("m_calendar");
    SetName(GenerateName());
}

CalendarCtrlWrapper::~CalendarCtrlWrapper() {}

wxcWidget* CalendarCtrlWrapper::Clone() const { return new CalendarCtrlWrapper(); }

wxString CalendarCtrlWrapper::CppCtorCode() const
{
    // wxCalendarCtrl(wxWindow* parent, wxWindowID id, const wxDateTime& date = wxDefaultDateTime, const wxPoint& pos =
    // wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxCAL_SHOW_HOLIDAYS, const wxString& name =
    // wxCalendarNameStr)
    wxString cpp;
    cpp << GetName() << wxT(" = new ") << GetRealClassName() << wxT("(") << GetWindowParent() << wxT(", ") << WindowID()
        << wxT(", ") << wxT("wxDefaultDateTime, ") << wxT("wxDefaultPosition, ") << SizeAsString() << wxT(", ")
        << StyleFlags(wxT("wxCAL_SHOW_HOLIDAYS")) << wxT(");\n");

    cpp << CPPCommonAttributes();
    return cpp;
}

void CalendarCtrlWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add(wxT("#include <wx/calctrl.h>")); }

wxString CalendarCtrlWrapper::GetWxClassName() const { return wxT("wxCalendarCtrl"); }

void CalendarCtrlWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << XRCPrefix() << XRCStyle() << XRCSize() << XRCCommonAttributes() << XRCSuffix();
}
