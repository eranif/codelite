#ifndef SIZERWRAPPER_H
#define SIZERWRAPPER_H

#include "sizer_wrapper_base.h"
#include "wxc_widget.h" // Base class: WrapperBase

class FlexGridSizerWrapper : public SizerWrapperBase
{
protected:
    wxString m_name;

public:
    FlexGridSizerWrapper();
    ~FlexGridSizerWrapper() override = default;
    void DoLoadXRCProperties(const wxXmlNode* node); // Shared by LoadPropertiesFromXRC and FromwxSmith

public:
    wxString CppCtorCode() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
    wxString GetWxClassName() const override;
    void LoadPropertiesFromXRC(const wxXmlNode* node) override;
    void LoadPropertiesFromwxFB(const wxXmlNode* node) override;
    void LoadPropertiesFromwxSmith(const wxXmlNode* node) override;
    wxcWidget* Clone() const override { return new FlexGridSizerWrapper(); }
};

#endif // SIZERWRAPPER_H
