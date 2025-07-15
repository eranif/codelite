#ifndef ACTIVITYRINDICATORWRAPPER_H
#define ACTIVITYRINDICATORWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class ActivityrIndicatorWrapper : public wxcWidget
{
public:
    ActivityrIndicatorWrapper();
    ~ActivityrIndicatorWrapper() override = default;

public:
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    wxString CppCtorCode() const override;
    void LoadPropertiesFromXRC(const wxXmlNode* node) override;
    void LoadPropertiesFromwxFB(const wxXmlNode* node) override;
    void LoadPropertiesFromwxSmith(const wxXmlNode* node) override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
    wxcWidget* Clone() const override { return new ActivityrIndicatorWrapper(); }
};

#endif // ACTIVITYRINDICATORWRAPPER_H
