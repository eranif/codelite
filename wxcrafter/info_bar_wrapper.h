#ifndef INFOBARWRAPPER_H
#define INFOBARWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class InfoBarWrapper : public wxcWidget
{
public:
    InfoBarWrapper();
    virtual ~InfoBarWrapper();

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
    virtual bool IsLicensed() const;
};

#endif // INFOBARWRAPPER_H
