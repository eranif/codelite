#ifndef STATICLINEWRAPPER_H
#define STATICLINEWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase

class StaticLineWrapper : public wxcWidget
{

public:
    StaticLineWrapper();
    virtual ~StaticLineWrapper();

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
};

#endif // STATICLINEWRAPPER_H
