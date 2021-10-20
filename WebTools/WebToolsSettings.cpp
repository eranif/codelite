#include "NodeJSLocator.h"
#include "WebToolsConfig.h"
#include "WebToolsSettings.h"
#include "clNodeJS.h"
#include "globals.h"
#include "windowattrmanager.h"

WebToolsSettings::WebToolsSettings(wxWindow* parent)
    : WebToolsSettingsBase(parent)
    , m_modified(false)
{
    {
        WebToolsConfig& config = WebToolsConfig::Get();
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
    if(fnNodeJS.FileExists()) {
        hints.Add(fnNodeJS.GetPath());
    }
    clNodeJS::Get().Initialise(hints);

    m_modified = false;
}
void WebToolsSettings::OnLintOnSave(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_modified = true;
}
