#ifndef STATICBOXSIZERWRAPPER_H
#define STATICBOXSIZERWRAPPER_H

#include "sizer_wrapper_base.h" // Base class: SizerWrapperBase

class StaticBoxSizerWrapper : public SizerWrapperBase
{
public:
    StaticBoxSizerWrapper();
    ~StaticBoxSizerWrapper() override = default;

    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    wxString CppCtorCode() const override;
    void LoadPropertiesFromXRC(const wxXmlNode* node) override;
    void LoadPropertiesFromwxFB(const wxXmlNode* node) override;
    void LoadPropertiesFromwxSmith(const wxXmlNode* node) override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
    wxcWidget* Clone() const override;

    void SetOrientation(const wxString& orient);
};

#endif // STATICBOXSIZERWRAPPER_H
