#ifndef SPLITTERWINDOWWRAPPER_H
#define SPLITTERWINDOWWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase

class SplitterWindowWrapper : public wxcWidget
{
public:
    SplitterWindowWrapper();
    virtual ~SplitterWindowWrapper();

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void LoadPropertiesFromXRC(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxFB(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxSmith(const wxXmlNode* node);
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
    virtual bool IsContainer() const { return true; }
    bool IsSplitVertically() const;
    int GetSashPos() const;
};

#endif // SPLITTERWINDOWWRAPPER_H
