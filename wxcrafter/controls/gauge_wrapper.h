#ifndef GAUGEWRAPPER_H
#define GAUGEWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase

class GaugeWrapper : public wxcWidget
{

public:
    GaugeWrapper();
    virtual ~GaugeWrapper();

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void LoadPropertiesFromXRC(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxFB(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxSmith(const wxXmlNode* node);
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
};

#endif // GAUGEWRAPPER_H
