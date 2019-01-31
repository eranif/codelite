#ifndef GRIDSIZERWRAPPER_H
#define GRIDSIZERWRAPPER_H

#include "sizer_wrapper_base.h"
#include "string_property.h"
#include "wxc_widget.h"

class GridSizerWrapper : public SizerWrapperBase
{
protected:
    wxString m_name;

public:
    GridSizerWrapper();
    virtual ~GridSizerWrapper();
    virtual void DoLoadXRCProperties(const wxXmlNode* node); // Shared by LoadPropertiesFromXRC and FromwxSmith

public:
    virtual wxString CppCtorCode() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
    virtual wxString GetWxClassName() const;
    virtual void LoadPropertiesFromXRC(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxFB(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxSmith(const wxXmlNode* node);
    virtual wxcWidget* Clone() const { return new GridSizerWrapper(); }
};

#endif // SIZERWRAPPER_H
