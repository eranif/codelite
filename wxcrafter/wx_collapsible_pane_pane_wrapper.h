#ifndef WXCOLLAPSIBLEPANEPANEWRAPPER_H
#define WXCOLLAPSIBLEPANEPANEWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class wxCollapsiblePanePaneWrapper : public wxcWidget
{
public:
    wxCollapsiblePanePaneWrapper();
    virtual ~wxCollapsiblePanePaneWrapper();

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
};

#endif // WXCOLLAPSIBLEPANEPANEWRAPPER_H
