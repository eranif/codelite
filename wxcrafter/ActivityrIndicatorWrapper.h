#ifndef ACTIVITYRINDICATORWRAPPER_H
#define ACTIVITYRINDICATORWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class ActivityrIndicatorWrapper : public wxcWidget
{
public:
    ActivityrIndicatorWrapper();
    virtual ~ActivityrIndicatorWrapper();

public:
    void GetIncludeFile(wxArrayString& headers) const;
    wxString GetWxClassName() const;
    wxString CppCtorCode() const;
    void LoadPropertiesFromXRC(const wxXmlNode* node);
    void LoadPropertiesFromwxFB(const wxXmlNode* node);
    void LoadPropertiesFromwxSmith(const wxXmlNode* node);
    void ToXRC(wxString& text, XRC_TYPE type) const;
    wxcWidget* Clone() const { return new ActivityrIndicatorWrapper(); }
};

#endif // ACTIVITYRINDICATORWRAPPER_H
