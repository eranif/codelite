#ifndef RIBBONGALLERYWRAPPER_H
#define RIBBONGALLERYWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class RibbonGalleryWrapper : public wxcWidget
{
public:
    RibbonGalleryWrapper();
    virtual ~RibbonGalleryWrapper();

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
    virtual bool IsLicensed() const { return wxcSettings::Get().IsLicensed(); }
};

#endif // RIBBONGALLERYWRAPPER_H
