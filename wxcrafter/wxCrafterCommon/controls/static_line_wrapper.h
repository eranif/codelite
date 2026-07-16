#ifndef STATICLINEWRAPPER_H
#define STATICLINEWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase

class StaticLineWrapper : public wxcWidget
{

public:
    StaticLineWrapper();
    ~StaticLineWrapper() override = default;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
};

#endif // STATICLINEWRAPPER_H
