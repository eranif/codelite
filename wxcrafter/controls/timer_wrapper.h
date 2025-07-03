#ifndef TIMERWRAPPER_H
#define TIMERWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class TimerWrapper : public wxcWidget
{
public:
    TimerWrapper();
    ~TimerWrapper() override = default;

protected:
    wxString CppDtorCode() const override;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;

    bool IsValidParent() const override { return false; }
    bool IsWxWindow() const override { return false; }
};

#endif // TIMERWRAPPER_H
