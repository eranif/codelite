#include "WebToolsSettings.h"
#include "windowattrmanager.h"
#include "WebToolsConfig.h"
#include "globals.h"
#include "NodeJSLocator.h"
#include "clNodeJS.h"

WebToolsSettings::WebToolsSettings(wxWindow* parent)
    : WebToolsSettingsBase(parent)
    , m_modified(false)
{
    ::wxPGPropertyBooleanUseCheckbox(m_pgMgr->GetGrid());
    {
        WebToolsConfig& config = WebToolsConfig::Get();

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
        m_pgPropNodeExpress->SetValue(config.HasJavaScriptFlag(WebToolsConfig::kJSNodeExpress));
        m_pgPropWebPack->SetValue(config.HasJavaScriptFlag(WebToolsConfig::kJSWebPack));
        m_pgPropPortNumber->SetValue(config.GetPortNumber());
        
        // XML
        m_checkBoxEnableXmlCC->SetValue(config.HasXmlFlag(WebToolsConfig::kXmlEnableCC));
        // HTML
        m_checkBoxEnableHtmlCC->SetValue(config.HasHtmlFlag(WebToolsConfig::kHtmlEnableCC));
        // NodeJS
        m_filePickerNodeJS->SetPath(config.GetNodejs());
        m_filePickerNpm->SetPath(config.GetNpm());
        m_checkBoxJSLint->SetValue(config.IsLintOnSave());
    }
    ::clSetDialogBestSizeAndPosition(this);
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
    DoSave();
    event.Skip();
}

void WebToolsSettings::OnModified(wxCommandEvent& event) { m_modified = true; }
void WebToolsSettings::OnSuggestNodeJSPaths(wxCommandEvent& event)
{
    NodeJSLocator locator;
    locator.Locate();

    m_filePickerNodeJS->SetPath(locator.GetNodejs());
    m_filePickerNpm->SetPath(locator.GetNpm());
    m_modified = true;
}

void WebToolsSettings::OnNodejsPath(wxFileDirPickerEvent& event)
{
    m_modified = true;
    event.Skip();
}
void WebToolsSettings::OnNpmPath(wxFileDirPickerEvent& event)
{
    m_modified = true;
    event.Skip();
}

void WebToolsSettings::OnApply(wxCommandEvent& event) { DoSave(); }

void WebToolsSettings::DoSave()
{
    WebToolsConfig& config = WebToolsConfig::Get();

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
    config.EnableJavaScriptFlag(WebToolsConfig::kJSNodeExpress, m_pgPropNodeExpress->GetValue().GetBool());
    config.EnableJavaScriptFlag(WebToolsConfig::kJSWebPack, m_pgPropWebPack->GetValue().GetBool());
    config.SetPortNumber(m_pgPropPortNumber->GetValue().GetInteger());

    // XML
    config.EnableXmlFlag(WebToolsConfig::kXmlEnableCC, m_checkBoxEnableXmlCC->IsChecked());

    // HTML
    config.EnableHtmlFlag(WebToolsConfig::kHtmlEnableCC, m_checkBoxEnableHtmlCC->IsChecked());

    // NodeJS
    config.SetNodejs(m_filePickerNodeJS->GetPath());
    config.SetNpm(m_filePickerNpm->GetPath());
    config.SetLintOnSave(m_checkBoxJSLint->IsChecked());

    wxFileName fnNodeJS(config.GetNodejs());
    wxArrayString hints;
    if(fnNodeJS.FileExists()) { hints.Add(fnNodeJS.GetPath()); }
    clNodeJS::Get().Initialise(hints);

    m_modified = false;
}
void WebToolsSettings::OnLintOnSave(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_modified = true;
}
