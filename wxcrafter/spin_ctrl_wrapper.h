#ifndef SPINCTRLWRAPPER_H
#define SPINCTRLWRAPPER_H

#include "spin_wrapper_base.h" // Base class: SpinWrapperBase

class SpinCtrlWrapper : public SpinWrapperBase
{

public:
    SpinCtrlWrapper();
    virtual ~SpinCtrlWrapper();

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
};

#endif // SPINCTRLWRAPPER_H
