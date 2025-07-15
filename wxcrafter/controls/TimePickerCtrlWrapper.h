#ifndef TIMERCTRLWRAPPER_H
#define TIMERCTRLWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class TimePickerCtrlWrapper : public wxcWidget
{
public:
    TimePickerCtrlWrapper();
    ~TimePickerCtrlWrapper() override = default;

public:
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    wxString CppCtorCode() const override;
    void LoadPropertiesFromXRC(const wxXmlNode* node) override;
    void LoadPropertiesFromwxFB(const wxXmlNode* node) override;
    void LoadPropertiesFromwxSmith(const wxXmlNode* node) override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
    wxcWidget* Clone() const override { return new TimePickerCtrlWrapper(); }
};

#endif // TIMERCTRLWRAPPER_H
