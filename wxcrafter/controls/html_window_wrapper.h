#ifndef HTMLWINDOWWRAPPER_H
#define HTMLWINDOWWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase

class HtmlWindowWrapper : public wxcWidget
{

public:
    HtmlWindowWrapper();
    virtual ~HtmlWindowWrapper();

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    void GetIncludeFile(wxArrayString& headers) const;
    wxString GetWxClassName() const;
    virtual void LoadPropertiesFromXRC(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxSmith(const wxXmlNode* node);
    void ToXRC(wxString& text, XRC_TYPE type) const;
};

#endif // HTMLWINDOWWRAPPER_H
