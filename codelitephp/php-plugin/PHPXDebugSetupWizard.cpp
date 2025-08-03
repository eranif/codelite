#include "PHPXDebugSetupWizard.h"
#include "php_configuration_data.h"

PHPXDebugSetupWizard::PHPXDebugSetupWizard(wxWindow* parent)
    : PHPXDebugSetupWizardBase(parent)
{
    PHPConfigurationData conf;
    conf.Load();
    
    m_textCtrlIP->ChangeValue(conf.GetXdebugHost());
    m_textCtrlKey->ChangeValue(conf.GetXdebugIdeKey());
    m_textCtrlPort->ChangeValue(wxString() << conf.GetXdebugPort());
}

void PHPXDebugSetupWizard::OnPageChanging(wxWizardEvent& event)
{
    event.Skip();
    if(event.GetDirection() && event.GetPage() == m_wizardPageIDEKey) {
        // build the text to copy
        wxString content;
        content << "xdebug.remote_enable=1\n";
        content << "xdebug.idekey=\"" << m_textCtrlKey->GetValue() << "\"\n";
        content << "xdebug.remote_host=" << m_textCtrlIP->GetValue() << "\n";
        content << "xdebug.remote_port=" << m_textCtrlPort->GetValue() << "\n";

        m_textCtrlPHPIni->ChangeValue(content);
        CallAfter(&PHPXDebugSetupWizard::SelectAllIniText);
    }
}

void PHPXDebugSetupWizard::SelectAllIniText() { m_textCtrlPHPIni->SelectAll(); }
void PHPXDebugSetupWizard::OnFinished(wxWizardEvent& event)
{
    PHPConfigurationData conf;
    conf.Load();

    long portNum(9000);
    m_textCtrlPort->GetValue().ToCLong(&portNum);
    conf.SetXdebugIdeKey(m_textCtrlKey->GetValue()).SetXdebugPort(portNum).SetXdebugHost(m_textCtrlIP->GetValue());
    conf.Save();
}
