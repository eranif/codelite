#ifndef CALENDARCTRLWRAPPER_H
#define CALENDARCTRLWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase

class CalendarCtrlWrapper : public wxcWidget
{

public:
    CalendarCtrlWrapper();
    virtual ~CalendarCtrlWrapper();

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
};

#endif // CALENDARCTRLWRAPPER_H
