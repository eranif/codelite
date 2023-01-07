#ifndef GENERICDIRCTRLWRAPPER_H
#define GENERICDIRCTRLWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase

class GenericDirCtrlWrapper : public wxcWidget
{
public:
    GenericDirCtrlWrapper();
    virtual ~GenericDirCtrlWrapper();

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    void GetIncludeFile(wxArrayString& headers) const;
    wxString GetWxClassName() const;
    virtual void LoadPropertiesFromXRC(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxFB(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxSmith(const wxXmlNode* node);
    void ToXRC(wxString& text, XRC_TYPE type) const;
};

#endif // GENERICDIRCTRLWRAPPER_H
