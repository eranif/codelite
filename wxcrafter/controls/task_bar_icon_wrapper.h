#ifndef TASKBARICONWRAPPER_H
#define TASKBARICONWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class TaskBarIconWrapper : public wxcWidget
{
public:
    TaskBarIconWrapper();
    ~TaskBarIconWrapper() override = default;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    wxString CppDtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
};

#endif // TASKBARICONWRAPPER_H
