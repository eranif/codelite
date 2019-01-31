#ifndef SPINBUTTONWRAPPER_H
#define SPINBUTTONWRAPPER_H

#include "spin_wrapper_base.h" // Base class: SpinWrapperBase

class SpinButtonWrapper : public SpinWrapperBase
{

public:
    SpinButtonWrapper();
    virtual ~SpinButtonWrapper();

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
};

#endif // SPINBUTTONWRAPPER_H
