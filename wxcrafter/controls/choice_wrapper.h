#ifndef CHOICEWRAPPER_H
#define CHOICEWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase

class ChoiceWrapper : public wxcWidget
{

public:
    ChoiceWrapper();
    virtual ~ChoiceWrapper();

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

#endif // CHOICEWRAPPER_H
