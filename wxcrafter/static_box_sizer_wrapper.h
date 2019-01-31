#ifndef STATICBOXSIZERWRAPPER_H
#define STATICBOXSIZERWRAPPER_H

#include "sizer_wrapper_base.h" // Base class: SizerWrapperBase

class StaticBoxSizerWrapper : public SizerWrapperBase
{
public:
    StaticBoxSizerWrapper();
    virtual ~StaticBoxSizerWrapper();

    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual wxString CppCtorCode() const;
    virtual void LoadPropertiesFromXRC(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxFB(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxSmith(const wxXmlNode* node);
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
    virtual wxcWidget* Clone() const;
    void SetOrientation(const wxString& orient);
};

#endif // STATICBOXSIZERWRAPPER_H
