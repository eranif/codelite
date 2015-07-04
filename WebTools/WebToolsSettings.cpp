#include "WebToolsSettings.h"
#include "windowattrmanager.h"
#include "WebToolsConfig.h"
#include "globals.h"

WebToolsSettings::WebToolsSettings(wxWindow* parent)
    : WebToolsSettingsBase(parent)
    , m_modified(false)
{
    ::wxPGPropertyBooleanUseCheckbox(m_pgMgr->GetGrid());
    {
        WebToolsConfig config;
        config.Load();
        // JS
        m_checkBoxEnableJsCC->SetValue(config.HasJavaScriptFlag(WebToolsConfig::kJSEnableCC));
        m_pgPropLogging->SetValue(config.HasJavaScriptFlag(WebToolsConfig::kJSEnableVerboseLogging));
        m_pgPropBrowser->SetValue(config.HasJavaScriptFlag(WebToolsConfig::kJSLibraryBrowser));
        m_pgPropChai->SetValue(config.HasJavaScriptFlag(WebToolsConfig::kJSLibraryChai));
        m_pgPropEcma5->SetValue(config.HasJavaScriptFlag(WebToolsConfig::kJSLibraryEcma5));
        m_pgPropEcma6->SetValue(config.HasJavaScriptFlag(WebToolsConfig::kJSLibraryEcma6));
        m_pgPropJQuery->SetValue(config.HasJavaScriptFlag(WebToolsConfig::kJSLibraryJQuery));
        m_pgPropUnderscore->SetValue(config.HasJavaScriptFlag(WebToolsConfig::kJSLibraryUnderscore));
        m_pgPropAngular->SetValue(config.HasJavaScriptFlag(WebToolsConfig::kJSPluginAngular));
        m_pgPropNode->SetValue(config.HasJavaScriptFlag(WebToolsConfig::kJSPluginNode));
        m_pgPropRequireJS->SetValue(config.HasJavaScriptFlag(WebToolsConfig::kJSPluginRequireJS));
        m_pgPropStrings->SetValue(config.HasJavaScriptFlag(WebToolsConfig::kJSPluginStrings));
        m_pgPropQML->SetValue(config.HasJavaScriptFlag(WebToolsConfig::kJSPluginQML));
        
        // XML
        m_checkBoxEnableXmlCC->SetValue(config.HasXmlFlag(WebToolsConfig::kXmlEnableCC));
        // HTML
        m_checkBoxEnableHtmlCC->SetValue(config.HasHtmlFlag(WebToolsConfig::kHtmlEnableCC));
    }

    CenterOnParent();
    SetName("WebToolsSettings");
    WindowAttrManager::Load(this);
}

WebToolsSettings::~WebToolsSettings() {}

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
        // JS
        config.EnableJavaScriptFlag(WebToolsConfig::kJSEnableCC, m_checkBoxEnableJsCC->IsChecked());
        config.EnableJavaScriptFlag(WebToolsConfig::kJSEnableVerboseLogging, m_pgPropLogging->GetValue().GetBool());
        config.EnableJavaScriptFlag(WebToolsConfig::kJSLibraryBrowser, m_pgPropBrowser->GetValue().GetBool());
        config.EnableJavaScriptFlag(WebToolsConfig::kJSLibraryChai, m_pgPropChai->GetValue().GetBool());
        config.EnableJavaScriptFlag(WebToolsConfig::kJSLibraryEcma5, m_pgPropEcma5->GetValue().GetBool());
        config.EnableJavaScriptFlag(WebToolsConfig::kJSLibraryEcma6, m_pgPropEcma6->GetValue().GetBool());
        config.EnableJavaScriptFlag(WebToolsConfig::kJSLibraryJQuery, m_pgPropJQuery->GetValue().GetBool());
        config.EnableJavaScriptFlag(WebToolsConfig::kJSLibraryUnderscore, m_pgPropUnderscore->GetValue().GetBool());
        config.EnableJavaScriptFlag(WebToolsConfig::kJSPluginAngular, m_pgPropAngular->GetValue().GetBool());
        config.EnableJavaScriptFlag(WebToolsConfig::kJSPluginNode, m_pgPropNode->GetValue().GetBool());
        config.EnableJavaScriptFlag(WebToolsConfig::kJSPluginRequireJS, m_pgPropRequireJS->GetValue().GetBool());
        config.EnableJavaScriptFlag(WebToolsConfig::kJSPluginStrings, m_pgPropStrings->GetValue().GetBool());
        config.EnableJavaScriptFlag(WebToolsConfig::kJSPluginQML, m_pgPropQML->GetValue().GetBool());
        // XML
        config.EnableXmlFlag(WebToolsConfig::kXmlEnableCC, m_checkBoxEnableXmlCC->IsChecked());
        // HTML
        config.EnableHtmlFlag(WebToolsConfig::kHtmlEnableCC, m_checkBoxEnableHtmlCC->IsChecked());
        config.Save();
    }
}
void WebToolsSettings::OnModified(wxCommandEvent& event)
{
    m_modified = true;
}
