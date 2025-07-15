#ifndef LISTBOXWRAPPER_H
#define LISTBOXWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase

class ListBoxWrapper : public wxcWidget
{

public:
    ListBoxWrapper();
    ~ListBoxWrapper() override = default;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void LoadPropertiesFromXRC(const wxXmlNode* node) override;
    void LoadPropertiesFromwxFB(const wxXmlNode* node) override;
    void LoadPropertiesFromwxSmith(const wxXmlNode* node) override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
};

#endif // LISTBOXWRAPPER_H
