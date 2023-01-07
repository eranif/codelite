#ifndef SCROLLEDWINDOWWRAPPER_H
#define SCROLLEDWINDOWWRAPPER_H

#include "wxc_widget.h"

class ScrolledWindowWrapper : public wxcWidget
{

public:
    ScrolledWindowWrapper();
    virtual ~ScrolledWindowWrapper();

    wxcWidget* Clone() const;
    wxString CppCtorCode() const;
    void GetIncludeFile(wxArrayString& headers) const;
    wxString GetWxClassName() const;
    virtual void LoadPropertiesFromXRC(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxFB(const wxXmlNode* node);
    void ToXRC(wxString& text, XRC_TYPE type) const;
    virtual bool IsContainer() const { return true; }
};

#endif // SCROLLEDWINDOWWRAPPER_H
