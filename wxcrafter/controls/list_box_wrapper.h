#ifndef LISTBOXWRAPPER_H
#define LISTBOXWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase

class ListBoxWrapper : public wxcWidget
{

public:
    ListBoxWrapper();
    virtual ~ListBoxWrapper();

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

#endif // LISTBOXWRAPPER_H
