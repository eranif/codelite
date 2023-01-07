#ifndef TEXTCTRLWRAPPER_H
#define TEXTCTRLWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase

class TextCtrlWrapper : public wxcWidget
{

public:
    TextCtrlWrapper();
    virtual ~TextCtrlWrapper();

public:
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual wxString CppCtorCode() const;
    virtual void LoadPropertiesFromXRC(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxFB(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxSmith(const wxXmlNode* node);
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
    virtual wxcWidget* Clone() const;
};

#endif // TEXTCTRLWRAPPER_H
