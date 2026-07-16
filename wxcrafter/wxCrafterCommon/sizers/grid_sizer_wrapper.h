#ifndef GRIDSIZERWRAPPER_H
#define GRIDSIZERWRAPPER_H

#include "sizer_wrapper_base.h"
#include "wxc_widget.h"

class GridSizerWrapper : public SizerWrapperBase
{
protected:
    wxString m_name;

public:
    GridSizerWrapper();
    ~GridSizerWrapper() override = default;
    void DoLoadXRCProperties(const wxXmlNode* node); // Shared by LoadPropertiesFromXRC and FromwxSmith

public:
    wxString CppCtorCode() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
    wxString GetWxClassName() const override;
    void LoadPropertiesFromXRC(const wxXmlNode* node) override;
    void LoadPropertiesFromwxFB(const wxXmlNode* node) override;
    void LoadPropertiesFromwxSmith(const wxXmlNode* node) override;
    wxcWidget* Clone() const override { return new GridSizerWrapper(); }
};

#endif // SIZERWRAPPER_H
