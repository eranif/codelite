#ifndef PANELWRAPPER_H
#define PANELWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase

class PanelWrapper : public wxcWidget
{
public:
    PanelWrapper();
    virtual ~PanelWrapper();

public:
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual wxString CppCtorCode() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
    virtual bool IsTopWindow() const;
    virtual wxcWidget* Clone() const { return new PanelWrapper(); }
    virtual bool IsContainer() const { return true; }
};

#endif // PANELWRAPPER_H
