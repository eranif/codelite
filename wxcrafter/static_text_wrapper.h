#ifndef STATICTEXTWRAPPER_H
#define STATICTEXTWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase

class StaticTextWrapper : public wxcWidget
{
protected:
    virtual void GetIncludeFile(wxArrayString& headers) const;

public:
    StaticTextWrapper();
    virtual ~StaticTextWrapper();

public:
    virtual wxString GetWxClassName() const;
    virtual wxString CppCtorCode() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
    virtual void LoadPropertiesFromXRC(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxFB(const wxXmlNode* node);
    virtual wxcWidget* Clone() const;
};

#endif // STATICTEXTWRAPPER_H
