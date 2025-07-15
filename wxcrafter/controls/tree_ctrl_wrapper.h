#ifndef TREECTRLWRAPPER_H
#define TREECTRLWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase

class TreeCtrlWrapper : public wxcWidget
{

public:
    TreeCtrlWrapper();
    ~TreeCtrlWrapper() override = default;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
};

#endif // TREECTRLWRAPPER_H
