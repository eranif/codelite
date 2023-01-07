#ifndef RIBBONTOOLSEPARATOR_H
#define RIBBONTOOLSEPARATOR_H

#include "wxc_widget.h" // Base class: wxcWidget

class RibbonToolSeparator : public wxcWidget
{
public:
    RibbonToolSeparator();
    virtual ~RibbonToolSeparator();

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
};

#endif // RIBBONTOOLSEPARATOR_H
