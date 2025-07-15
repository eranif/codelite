#ifndef BOXSIZERWRAPPER_H
#define BOXSIZERWRAPPER_H

#include "sizer_wrapper_base.h"
#include "wxc_widget.h" // Base class: WrapperBase

class BoxSizerWrapper : public SizerWrapperBase
{

public:
    BoxSizerWrapper();
    ~BoxSizerWrapper() override = default;

public:
    wxString GetWxClassName() const override;
    wxString CppCtorCode() const override;
    void LoadPropertiesFromXRC(const wxXmlNode* node) override;
    void LoadPropertiesFromwxFB(const wxXmlNode* node) override;
    void LoadPropertiesFromwxSmith(const wxXmlNode* node) override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
    wxcWidget* Clone() const override;

    void SetOrientation(const wxString& orient);
};

#endif // BOXSIZERWRAPPER_H
