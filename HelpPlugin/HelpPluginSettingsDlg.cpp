#include "HelpPluginSettingsDlg.h"
#include "windowattrmanager.h"
#include "HelpPluginSettings.h"

HelpPluginSettingsDlg::HelpPluginSettingsDlg(wxWindow* parent)
    : HelpPluginSettingsDlgBase(parent)
    , m_modified(false)
{
    HelpPluginSettings settings;
    settings.Load();

    m_pgPropCMake->SetValue(settings.GetCmakeDocset());
    m_pgPropCSS->SetValue(settings.GetCssDocset());
    m_pgPropCxx->SetValue(settings.GetCxxDocset());
    m_pgPropHtml->SetValue(settings.GetHtmlDocset());
    m_pgPropJS->SetValue(settings.GetJsDocset());
    m_pgPropJava->SetValue(settings.GetJavaDocset());
    m_pgPropPHP->SetValue(settings.GetPhpDocset());
    WindowAttrManager::Load(this);
}

HelpPluginSettingsDlg::~HelpPluginSettingsDlg() {}

void HelpPluginSettingsDlg::OnOKUI(wxUpdateUIEvent& event) { event.Enable(m_modified); }

void HelpPluginSettingsDlg::OnDocsetChanged(wxPropertyGridEvent& event) { m_modified = true; }
void HelpPluginSettingsDlg::OnOK(wxCommandEvent& event)
{
    event.Skip();
    HelpPluginSettings settings;
    settings.Load();
    settings.SetCmakeDocset(m_pgPropCMake->GetValue().GetString());
    settings.SetCssDocset(m_pgPropCSS->GetValue().GetString());
    settings.SetCxxDocset(m_pgPropCxx->GetValue().GetString());
    settings.SetHtmlDocset(m_pgPropHtml->GetValue().GetString());
    settings.SetJsDocset(m_pgPropJS->GetValue().GetString());
    settings.SetJavaDocset(m_pgPropJava->GetValue().GetString());
    settings.SetPhpDocset(m_pgPropPHP->GetValue().GetString());
    settings.Save();
    EndModal(wxID_OK);
}
