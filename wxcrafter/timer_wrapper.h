#ifndef TIMERWRAPPER_H
#define TIMERWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class TimerWrapper : public wxcWidget
{
public:
    TimerWrapper();
    virtual ~TimerWrapper();

protected:
    wxString CppDtorCode() const;

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;

    virtual bool IsValidParent() const { return false; }
    virtual bool IsWxWindow() const { return false; }
    virtual bool IsLicensed() const;
};

#endif // TIMERWRAPPER_H
