#ifndef MENUWRAPPER_H
#define MENUWRAPPER_H

#include "allocator_mgr.h"
#include "wxc_widget.h"

class MenuWrapper : public wxcWidget
{
public:
    MenuWrapper();
    virtual ~MenuWrapper();

    wxcWidget* Clone() const;
    wxString CppCtorCode() const;
    void GetIncludeFile(wxArrayString& headers) const;
    wxString GetWxClassName() const;
    void ToXRC(wxString& text, XRC_TYPE type) const;
};

class SubMenuWrapper : public MenuWrapper
{
public:
    SubMenuWrapper()
        : MenuWrapper()
    {
        SetType(ID_WXSUBMENU);
    }

    virtual ~SubMenuWrapper() {}
    wxcWidget* Clone() const { return new SubMenuWrapper(); }
};

#endif // MENUWRAPPER_H
