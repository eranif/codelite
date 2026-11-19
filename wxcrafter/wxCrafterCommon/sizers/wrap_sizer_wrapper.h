#pragma once

#include "sizer_wrapper_base.h"
#include "wxc_widget.h" // Base class: WrapperBase

class WrapSizerWrapper : public SizerWrapperBase
{
public:
    WrapSizerWrapper();
    ~WrapSizerWrapper() override = default;

    void GetIncludeFile(wxArrayString& headers) const override;
    wxString CppCtorCode() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
    wxString GetWxClassName() const override;
    void LoadPropertiesFromXRC(const wxXmlNode* node) override;
    void LoadPropertiesFromwxSmith(const wxXmlNode* node) override;
    void LoadPropertiesFromwxFB(const wxXmlNode* node) override;
    wxcWidget* Clone() const override { return new WrapSizerWrapper(); }

private:
    /// The wxWrapSizer flags as a C++ / XRC expression, e.g. "wxEXTEND_LAST_ON_EACH_LINE|wxREMOVE_LEADING_SPACES"
    wxString FlagsAsString() const;
    void DoLoadFlags(const wxString& flags);
};
