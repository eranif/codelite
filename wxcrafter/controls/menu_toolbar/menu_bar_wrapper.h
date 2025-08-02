#ifndef MENUBARWRAPPER_H
#define MENUBARWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase

class MenuBarWrapper : public wxcWidget
{
public:
    MenuBarWrapper();
    ~MenuBarWrapper() override = default;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;

    wxString DesignerXRC() const;
};

#endif // MENUBARWRAPPER_H
