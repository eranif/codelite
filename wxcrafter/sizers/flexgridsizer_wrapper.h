#ifndef SIZERWRAPPER_H
#define SIZERWRAPPER_H

#include "sizer_wrapper_base.h"
#include "string_property.h"
#include "wxc_widget.h" // Base class: WrapperBase

class FlexGridSizerWrapper : public SizerWrapperBase
{
protected:
    wxString m_name;

public:
    FlexGridSizerWrapper();
    virtual ~FlexGridSizerWrapper();
    virtual void DoLoadXRCProperties(const wxXmlNode* node); // Shared by LoadPropertiesFromXRC and FromwxSmith

public:
    virtual wxString CppCtorCode() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
    virtual wxString GetWxClassName() const;
    virtual void LoadPropertiesFromXRC(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxFB(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxSmith(const wxXmlNode* node);
    virtual wxcWidget* Clone() const { return new FlexGridSizerWrapper(); }
};

#endif // SIZERWRAPPER_H
