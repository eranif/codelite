#ifndef BANNERWINDOWWRAPPER_H
#define BANNERWINDOWWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class BannerWindowWrapper : public wxcWidget
{
public:
    BannerWindowWrapper();
    virtual ~BannerWindowWrapper();

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
    virtual bool IsLicensed() const;
};

#endif // BANNERWINDOWWRAPPER_H
