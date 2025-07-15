#ifndef MENUWRAPPER_H
#define MENUWRAPPER_H

#include "allocator_mgr.h"
#include "wxc_widget.h"

class MenuWrapper : public wxcWidget
{
public:
    MenuWrapper();
    ~MenuWrapper() override = default;

    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
};

class SubMenuWrapper : public MenuWrapper
{
public:
    SubMenuWrapper()
        : MenuWrapper()
    {
        SetType(ID_WXSUBMENU);
    }

    ~SubMenuWrapper() override = default;
    wxcWidget* Clone() const override { return new SubMenuWrapper(); }
};

#endif // MENUWRAPPER_H
