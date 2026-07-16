#ifndef SEARCHCTRLWRAPPER_H
#define SEARCHCTRLWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase

class SearchCtrlWrapper : public wxcWidget
{

public:
    SearchCtrlWrapper();
    ~SearchCtrlWrapper() override = default;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
};

#endif // SEARCHCTRLWRAPPER_H
