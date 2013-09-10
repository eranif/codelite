#include "PluginWizard.h"
#include "globals.h"
#include <wx/msgdlg.h>
#include <wx/dir.h>

PluginWizard::PluginWizard(wxWindow* parent)
    : PluginWizardBase(parent)
{
}

PluginWizard::~PluginWizard()
{
}

bool PluginWizard::Run(NewPluginData& pd)
{
    bool res = RunWizard( GetFirstPage() );
    if ( res ) {
        pd.SetCodelitePath( m_dirPickerCodeliteDir->GetPath() );
        pd.SetPluginDescription( m_textCtrlDescription->GetValue() );
        pd.SetPluginName( m_textCtrlName->GetValue() );
        pd.SetProjectPath( m_textCtrlPreview->GetValue() );
    }
    return res;
}

void PluginWizard::OnFinish(wxWizardEvent& event)
{
    event.Skip();
}

void PluginWizard::OnPageChanging(wxWizardEvent& event)
{
    if ( event.GetDirection() && event.GetPage() == m_pages.at(0)) {
        wxString pluginName = m_textCtrlName->GetValue();
        pluginName.Trim();
        if ( pluginName.IsEmpty() || !::IsValidCppIndetifier(pluginName) ) {
            ::wxMessageBox(_("Invalid plugin name"), "codelite");
            event.Veto();
            return;
        }
    } else if ( event.GetDirection() && event.GetPage() == m_pages.at(1)) {
        if ( !wxDir::Exists( m_dirPickerCodeliteDir->GetPath() ) ) {
            ::wxMessageBox(_("codelite folder does not exists"), "codelite");
            event.Veto();
            return;
        }

        if ( !wxDir::Exists( m_dirPickerPluginPath->GetPath() ) ) {
            ::wxMessageBox(_("The selected plugin folder does not exist"), "codelite");
            event.Veto();
            return;
        }
    }
}
void PluginWizard::OnProjectPathChanged(wxFileDirPickerEvent& event)
{
    wxFileName project(event.GetPath(), m_textCtrlName->GetValue());
    project.SetExt("project");
    project.AppendDir(m_textCtrlName->GetValue());
    m_textCtrlPreview->ChangeValue( project.GetFullPath() );
}
