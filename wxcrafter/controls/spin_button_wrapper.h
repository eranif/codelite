#ifndef SPINBUTTONWRAPPER_H
#define SPINBUTTONWRAPPER_H

#include "spin_wrapper_base.h" // Base class: SpinWrapperBase

class SpinButtonWrapper : public SpinWrapperBase
{

public:
    SpinButtonWrapper();
    ~SpinButtonWrapper() override = default;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
};

#endif // SPINBUTTONWRAPPER_H
