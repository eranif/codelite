#ifndef SPLITTERWINDOWWRAPPER_H
#define SPLITTERWINDOWWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase

class SplitterWindowWrapper : public wxcWidget
{
public:
    SplitterWindowWrapper();
    ~SplitterWindowWrapper() override = default;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void LoadPropertiesFromXRC(const wxXmlNode* node) override;
    void LoadPropertiesFromwxFB(const wxXmlNode* node) override;
    void LoadPropertiesFromwxSmith(const wxXmlNode* node) override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
    bool IsContainer() const override { return true; }

    bool IsSplitVertically() const;
    int GetSashPos() const;
};

#endif // SPLITTERWINDOWWRAPPER_H
