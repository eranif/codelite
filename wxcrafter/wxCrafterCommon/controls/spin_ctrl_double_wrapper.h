#pragma once

#include "wxc_widget.h"

class SpinCtrlDoubleWrapper : public wxcWidget
{
public:
    SpinCtrlDoubleWrapper();
    ~SpinCtrlDoubleWrapper() override = default;

    wxcWidget* Clone() const override { return new SpinCtrlDoubleWrapper(); }
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
    void LoadPropertiesFromXRC(const wxXmlNode* node) override;
    void LoadPropertiesFromwxFB(const wxXmlNode* node) override;

private:
    /// The spin control numbers, sanitised: `max` >= `min` and `value` within the range.
    /// The strings are suitable for use in C++ and XRC.
    struct Numbers {
        wxString min, max, value, inc;
        int digits = 0;
    };
    Numbers GetNumbers() const;
    void DoLoadNumbers(const wxXmlNode* node, bool fromXRC);
};
