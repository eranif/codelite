#ifndef BITMAPBUTTON_H
#define BITMAPBUTTON_H

#include "wxc_widget.h" // Base class: WrapperBase

class BitmapButtonWrapper : public wxcWidget
{
protected:
    virtual void GetIncludeFile(wxArrayString& headers) const;

public:
    BitmapButtonWrapper();
    virtual ~BitmapButtonWrapper();

public:
    virtual wxString GetWxClassName() const;
    virtual wxString CppCtorCode() const;
    virtual void LoadPropertiesFromXRC(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxFB(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxSmith(const wxXmlNode* node);
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;

    virtual wxcWidget* Clone() const { return new BitmapButtonWrapper(); }
};

#endif // BITMAPBUTTON_H
