#ifndef COLLAPSIBLEPANEWRAPPER_H
#define COLLAPSIBLEPANEWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class CollapsiblePaneWrapper : public wxcWidget
{
public:
    CollapsiblePaneWrapper();
    virtual ~CollapsiblePaneWrapper();

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
};

#endif // COLLAPSIBLEPANEWRAPPER_H
