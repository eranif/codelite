#ifndef CALENDARCTRLWRAPPER_H
#define CALENDARCTRLWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase

class CalendarCtrlWrapper : public wxcWidget
{

public:
    CalendarCtrlWrapper();
    ~CalendarCtrlWrapper() override = default;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
};

#endif // CALENDARCTRLWRAPPER_H
