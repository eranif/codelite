#ifndef WEBTOOLSSETTINGS_H
#define WEBTOOLSSETTINGS_H
#include "WebToolsBase.h"

class WebToolsSettings : public WebToolsSettingsBase
{
    bool m_modified;
public:
    WebToolsSettings(wxWindow* parent);
    virtual ~WebToolsSettings();
protected:
    virtual void OnModified(wxCommandEvent& event);
    virtual void OnOK(wxCommandEvent& event);
    virtual void OnOKUI(wxUpdateUIEvent& event);
    virtual void OnJSValueChanged(wxPropertyGridEvent& event);
};
#endif // WEBTOOLSSETTINGS_H
