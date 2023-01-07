#ifndef RIBBONGALLERYITEMWRAPPER_H
#define RIBBONGALLERYITEMWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class RibbonGalleryItemWrapper : public wxcWidget
{
public:
    RibbonGalleryItemWrapper();
    virtual ~RibbonGalleryItemWrapper();

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
};

#endif // RIBBONGALLERYITEMWRAPPER_H
