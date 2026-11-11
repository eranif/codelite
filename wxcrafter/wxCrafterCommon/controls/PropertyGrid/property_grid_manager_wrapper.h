#pragma once

#include "wxc_widget.h" // Base class: wxcWidget

class PropertyGridManagerWrapper : public wxcWidget
{
public:
    PropertyGridManagerWrapper();
    ~PropertyGridManagerWrapper() override = default;

    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    wxString DoGenerateCppCtorCode_End() const override;
};
