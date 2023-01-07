#ifndef HYPERLINKCTRLWRAPPER_H
#define HYPERLINKCTRLWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase

class HyperLinkCtrlWrapper : public wxcWidget
{

public:
    HyperLinkCtrlWrapper();
    virtual ~HyperLinkCtrlWrapper();

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

#endif // HYPERLINKCTRLWRAPPER_H
