#ifndef RIBBONPANELWRAPPER_H
#define RIBBONPANELWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class RibbonPanelWrapper : public wxcWidget
{
public:
    RibbonPanelWrapper();
    virtual ~RibbonPanelWrapper();

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
};

#endif // RIBBONPANELWRAPPER_H
