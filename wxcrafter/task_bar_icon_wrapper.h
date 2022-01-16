#ifndef TASKBARICONWRAPPER_H
#define TASKBARICONWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class TaskBarIconWrapper : public wxcWidget
{
public:
    TaskBarIconWrapper();
    virtual ~TaskBarIconWrapper();

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual wxString CppDtorCode() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
};

#endif // TASKBARICONWRAPPER_H
