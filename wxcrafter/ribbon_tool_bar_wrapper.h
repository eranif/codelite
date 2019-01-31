#ifndef RIBBONTOOLBARWRAPPER_H
#define RIBBONTOOLBARWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class RibbonToolBarWrapper : public wxcWidget
{
public:
    RibbonToolBarWrapper();
    virtual ~RibbonToolBarWrapper();

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
    virtual wxString DoGenerateCppCtorCode_End() const;
    virtual bool IsLicensed() const { return wxcSettings::Get().IsLicensed(); }
};

#endif // RIBBONTOOLBARWRAPPER_H
