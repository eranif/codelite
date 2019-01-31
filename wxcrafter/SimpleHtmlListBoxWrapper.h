#ifndef SIMPLEHTMLLISTBOXWRAPPER_H
#define SIMPLEHTMLLISTBOXWRAPPER_H

#include "wxc_widget.h"

class SimpleHtmlListBoxWrapper : public wxcWidget
{
public:
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual wxString CppCtorCode() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;

    virtual wxcWidget* Clone() const { return new SimpleHtmlListBoxWrapper(); }

    SimpleHtmlListBoxWrapper();
    virtual ~SimpleHtmlListBoxWrapper();
};

#endif // SIMPLEHTMLLISTBOXWRAPPER_H
