#ifndef GRIDBAGSIZERWRAPPER_H
#define GRIDBAGSIZERWRAPPER_H

#include "sizer_wrapper_base.h"
#include "string_property.h"
#include "wxc_widget.h" // Base class: WrapperBase

class GridBagSizerWrapper : public SizerWrapperBase
{
protected:
    wxString m_name;

public:
    GridBagSizerWrapper();
    virtual ~GridBagSizerWrapper();
    virtual void DoLoadXRCProperties(const wxXmlNode* node); // Shared by LoadPropertiesFromXRC and FromwxSmith

protected:
    virtual wxString DoGenerateCppCtorCode_End() const;

public:
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString CppCtorCode() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
    virtual wxString GetWxClassName() const;
    virtual void LoadPropertiesFromXRC(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxFB(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxSmith(const wxXmlNode* node);
    virtual wxcWidget* Clone() const { return new GridBagSizerWrapper(); }
};

#endif // SIZERWRAPPER_H
