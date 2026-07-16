#ifndef STATICTEXTWRAPPER_H
#define STATICTEXTWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase

class StaticTextWrapper : public wxcWidget
{
protected:
    void GetIncludeFile(wxArrayString& headers) const override;

public:
    StaticTextWrapper();
    ~StaticTextWrapper() override = default;

public:
    wxString GetWxClassName() const override;
    wxString CppCtorCode() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
    void LoadPropertiesFromXRC(const wxXmlNode* node) override;
    void LoadPropertiesFromwxFB(const wxXmlNode* node) override;
    wxcWidget* Clone() const override;
};

#endif // STATICTEXTWRAPPER_H
