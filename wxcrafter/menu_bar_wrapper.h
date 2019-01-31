#ifndef MENUBARWRAPPER_H
#define MENUBARWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase

class MenuBarWrapper : public wxcWidget
{
public:
    MenuBarWrapper();
    virtual ~MenuBarWrapper();

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
    wxString DesignerXRC() const;
};

#endif // MENUBARWRAPPER_H
