#ifndef DATEPICKERCTRL_H
#define DATEPICKERCTRL_H

#include "wxc_widget.h" // Base class: WrapperBase

class DatePickerCtrl : public wxcWidget
{

public:
    DatePickerCtrl();
    ~DatePickerCtrl() override = default;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
};

#endif // DATEPICKERCTRL_H
