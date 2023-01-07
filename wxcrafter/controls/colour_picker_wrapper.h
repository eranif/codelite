#ifndef COLOURPICKERWRAPPER_H
#define COLOURPICKERWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase

class ColourPickerWrapper : public wxcWidget
{
public:
    ColourPickerWrapper();
    virtual ~ColourPickerWrapper();

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

#endif // COLOURPICKERWRAPPER_H
