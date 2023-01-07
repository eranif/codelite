#ifndef BITMAPCOMBOXWRAPPER_H
#define BITMAPCOMBOXWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class BitmapComboxWrapper : public wxcWidget
{
public:
    BitmapComboxWrapper();
    virtual ~BitmapComboxWrapper();

public:
    virtual wxcWidget* Clone() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual wxString CppCtorCode() const;
    virtual void LoadPropertiesFromXRC(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxFB(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxSmith(const wxXmlNode* node);
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
};

#endif // BITMAPCOMBOXWRAPPER_H
