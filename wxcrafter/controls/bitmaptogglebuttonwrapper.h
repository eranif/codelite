#ifndef BITMAPTOGGLEBUTTONWRAPPER_H
#define BITMAPTOGGLEBUTTONWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class BitmapToggleButtonWrapper : public wxcWidget
{
public:
    BitmapToggleButtonWrapper();
    virtual ~BitmapToggleButtonWrapper();

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
};

#endif // BITMAPTOGGLEBUTTONWRAPPER_H
