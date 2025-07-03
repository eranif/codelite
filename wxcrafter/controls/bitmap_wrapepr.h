#ifndef BITMAPWRAPEPR_H
#define BITMAPWRAPEPR_H

#include "wxc_widget.h" // Base class: wxcWidget

class BitmapWrapepr : public wxcWidget
{
public:
    BitmapWrapepr();
    ~BitmapWrapepr() override = default;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
};

#endif // BITMAPWRAPEPR_H
