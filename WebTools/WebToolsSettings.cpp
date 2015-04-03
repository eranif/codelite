#include "WebToolsSettings.h"
#include "windowattrmanager.h"
#include "WebToolsConfig.h"

WebToolsSettings::WebToolsSettings(wxWindow* parent)
    : WebToolsSettingsBase(parent)
    , m_modified(false)
{
    {
        WebToolsConfig config;
        config.Load();
        m_pgPropEnableJSCC->SetValue(config.HasJavaScriptFlag(WebToolsConfig::kJSEnableCC));
        m_pgPropLogging->SetValue(config.HasJavaScriptFlag(WebToolsConfig::kJSEnableVerboseLogging));
        m_pgPropBrowser->SetValue(config.HasJavaScriptFlag(WebToolsConfig::kJSLibraryBrowser));
        m_pgPropChai->SetValue(config.HasJavaScriptFlag(WebToolsConfig::kJSLibraryChai));
        m_pgPropEcma5->SetValue(config.HasJavaScriptFlag(WebToolsConfig::kJSLibraryEcma5));
        m_pgPropEcma5->SetValue(config.HasJavaScriptFlag(WebToolsConfig::kJSLibraryEcma6));
        m_pgPropJQuery->SetValue(config.HasJavaScriptFlag(WebToolsConfig::kJSLibraryJQuery));
        m_pgPropUnderscore->SetValue(config.HasJavaScriptFlag(WebToolsConfig::kJSLibraryUnderscore));
    }
    
    CenterOnParent();
    WindowAttrManager::Load(this, "WebToolsSettings");
}

WebToolsSettings::~WebToolsSettings() { WindowAttrManager::Save(this, "WebToolsSettings"); }

void WebToolsSettings::OnJSValueChanged(wxPropertyGridEvent& event)
{
    event.Skip();
    m_modified = true;
}

void WebToolsSettings::OnOKUI(wxUpdateUIEvent& event) { event.Enable(m_modified); }

void WebToolsSettings::OnOK(wxCommandEvent& event)
{
    event.Skip();

    // Save the settings
    {
        WebToolsConfig config;
        config.Load();
        config.EnableJavaScriptFlag(WebToolsConfig::kJSEnableCC, m_pgPropEnableJSCC->GetValue().GetBool());
        config.EnableJavaScriptFlag(WebToolsConfig::kJSEnableVerboseLogging, m_pgPropLogging->GetValue().GetBool());
        config.EnableJavaScriptFlag(WebToolsConfig::kJSLibraryBrowser, m_pgPropBrowser->GetValue().GetBool());
        config.EnableJavaScriptFlag(WebToolsConfig::kJSLibraryChai, m_pgPropChai->GetValue().GetBool());
        config.EnableJavaScriptFlag(WebToolsConfig::kJSLibraryEcma5, m_pgPropEcma5->GetValue().GetBool());
        config.EnableJavaScriptFlag(WebToolsConfig::kJSLibraryEcma6, m_pgPropEcma6->GetValue().GetBool());
        config.EnableJavaScriptFlag(WebToolsConfig::kJSLibraryJQuery, m_pgPropJQuery->GetValue().GetBool());
        config.EnableJavaScriptFlag(WebToolsConfig::kJSLibraryUnderscore, m_pgPropUnderscore->GetValue().GetBool());
        config.Save();
    }
}
