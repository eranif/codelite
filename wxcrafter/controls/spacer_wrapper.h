#ifndef SPACERWRAPPER_H
#define SPACERWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase

class SpacerWrapper : public wxcWidget
{
public:
    SpacerWrapper();
    virtual ~SpacerWrapper();

    virtual bool IsEventHandler() const { return false; }
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    void GetIncludeFile(wxArrayString& headers) const;
    wxString GetWxClassName() const;
    void LoadPropertiesFromwxFB(const wxXmlNode* node);
    void ToXRC(wxString& text, XRC_TYPE type) const;

    bool IsSizer() const { return true; }
};

#endif // SPACERWRAPPER_H
