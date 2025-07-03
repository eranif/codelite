#ifndef LISTCTRLWRAPPER_H
#define LISTCTRLWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase

class ListCtrlWrapper : public wxcWidget
{

public:
    ListCtrlWrapper();
    ~ListCtrlWrapper() override = default;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
};

#endif // LISTCTRLWRAPPER_H
