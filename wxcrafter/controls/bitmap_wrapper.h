#ifndef BITMAPWRAPPER_H
#define BITMAPWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class BitmapWrapper : public wxcWidget
{
public:
    BitmapWrapper();
    ~BitmapWrapper() override = default;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
};

#endif // BITMAPWRAPPER_H
