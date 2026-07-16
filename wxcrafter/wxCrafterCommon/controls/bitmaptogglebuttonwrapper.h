#ifndef BITMAPTOGGLEBUTTONWRAPPER_H
#define BITMAPTOGGLEBUTTONWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class BitmapToggleButtonWrapper : public wxcWidget
{
public:
    BitmapToggleButtonWrapper();
    ~BitmapToggleButtonWrapper() override = default;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
};

#endif // BITMAPTOGGLEBUTTONWRAPPER_H
