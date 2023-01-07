#ifndef SCROLLBARWRAPPER_H
#define SCROLLBARWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase

class ScrollBarWrapper : public wxcWidget
{

public:
    ScrollBarWrapper();
    virtual ~ScrollBarWrapper();

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void LoadPropertiesFromXRC(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxFB(const wxXmlNode* node);
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
};

#endif // SCROLLBARWRAPPER_H
