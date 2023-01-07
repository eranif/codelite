#ifndef INFOBARBUTTONWRAPPER_H
#define INFOBARBUTTONWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class InfoBarButtonWrapper : public wxcWidget
{
public:
    InfoBarButtonWrapper();
    virtual ~InfoBarButtonWrapper();

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
};

#endif // INFOBARBUTTONWRAPPER_H
