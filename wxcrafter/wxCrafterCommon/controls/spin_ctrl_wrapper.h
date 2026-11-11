#pragma once

#include "spin_wrapper_base.h" // Base class: SpinWrapperBase

class SpinCtrlWrapper : public SpinWrapperBase
{

public:
    SpinCtrlWrapper();
    ~SpinCtrlWrapper() override = default;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
};
