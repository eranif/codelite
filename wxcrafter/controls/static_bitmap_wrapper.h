#ifndef STATICBITMAPWRAPPER_H
#define STATICBITMAPWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase

class StaticBitmapWrapper : public wxcWidget
{

public:
    StaticBitmapWrapper();
    virtual ~StaticBitmapWrapper();

public:
    virtual wxcWidget* Clone() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual wxString CppCtorCode() const;
    virtual void LoadPropertiesFromXRC(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxFB(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxSmith(const wxXmlNode* node);
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
};

#endif // STATICBITMAPWRAPPER_H
