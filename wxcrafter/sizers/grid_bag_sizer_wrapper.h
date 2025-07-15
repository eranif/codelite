#ifndef GRIDBAGSIZERWRAPPER_H
#define GRIDBAGSIZERWRAPPER_H

#include "sizer_wrapper_base.h"
#include "wxc_widget.h" // Base class: WrapperBase

class GridBagSizerWrapper : public SizerWrapperBase
{
protected:
    wxString m_name;

public:
    GridBagSizerWrapper();
    ~GridBagSizerWrapper() override = default;
    void DoLoadXRCProperties(const wxXmlNode* node); // Shared by LoadPropertiesFromXRC and FromwxSmith

protected:
    wxString DoGenerateCppCtorCode_End() const override;

public:
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString CppCtorCode() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
    wxString GetWxClassName() const override;
    void LoadPropertiesFromXRC(const wxXmlNode* node) override;
    void LoadPropertiesFromwxFB(const wxXmlNode* node) override;
    void LoadPropertiesFromwxSmith(const wxXmlNode* node) override;
    wxcWidget* Clone() const override { return new GridBagSizerWrapper(); }
};

#endif // SIZERWRAPPER_H
