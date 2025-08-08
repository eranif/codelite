#ifndef SCROLLEDWINDOWWRAPPER_H
#define SCROLLEDWINDOWWRAPPER_H

#include "wxc_widget.h"

class ScrolledWindowWrapper : public wxcWidget
{

public:
    ScrolledWindowWrapper();
    ~ScrolledWindowWrapper() override = default;

    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void LoadPropertiesFromXRC(const wxXmlNode* node) override;
    void LoadPropertiesFromwxFB(const wxXmlNode* node) override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
    bool IsContainer() const override { return true; }
};

#endif // SCROLLEDWINDOWWRAPPER_H
