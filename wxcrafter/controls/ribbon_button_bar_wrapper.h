#ifndef RIBBONBUTTONBARWRAPPER_H
#define RIBBONBUTTONBARWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class RibbonButtonBarWrapper : public wxcWidget
{
public:
    RibbonButtonBarWrapper();
    virtual ~RibbonButtonBarWrapper();

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
    virtual wxString DoGenerateCppCtorCode_End() const;
};

#endif // RIBBONBUTTONBARWRAPPER_H
