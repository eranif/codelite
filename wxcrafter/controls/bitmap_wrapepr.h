#ifndef BITMAPWRAPEPR_H
#define BITMAPWRAPEPR_H

#include "wxc_widget.h" // Base class: wxcWidget

class BitmapWrapepr : public wxcWidget
{
public:
    BitmapWrapepr();
    virtual ~BitmapWrapepr();

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
};

#endif // BITMAPWRAPEPR_H
