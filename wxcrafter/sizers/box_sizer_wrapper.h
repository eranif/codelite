#ifndef BOXSIZERWRAPPER_H
#define BOXSIZERWRAPPER_H

#include "sizer_wrapper_base.h"
#include "wxc_widget.h" // Base class: WrapperBase

class BoxSizerWrapper : public SizerWrapperBase
{

public:
    BoxSizerWrapper();
    virtual ~BoxSizerWrapper();

public:
    virtual wxString GetWxClassName() const;
    virtual wxString CppCtorCode() const;
    virtual void LoadPropertiesFromXRC(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxFB(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxSmith(const wxXmlNode* node);
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
    virtual wxcWidget* Clone() const;
    void SetOrientation(const wxString& orient);
};

#endif // BOXSIZERWRAPPER_H
