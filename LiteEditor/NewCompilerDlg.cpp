#include "NewCompilerDlg.h"
#include "windowattrmanager.h"
#include "build_settings_config.h"

NewCompilerDlg::NewCompilerDlg(wxWindow* parent)
    : NewCompilerDlgBase(parent)
{
    BuildSettingsConfigCookie cookie;
    m_choiceCompilers->Append("<None>");
    CompilerPtr cmp = BuildSettingsConfigST::Get()->GetFirstCompiler(cookie);
    while ( cmp ) {
        m_choiceCompilers->Append(cmp->GetName());
        cmp = BuildSettingsConfigST::Get()->GetNextCompiler(cookie);
    }

    m_choiceCompilers->SetStringSelection("<None>");
    WindowAttrManager::Load(this, "NewCompilerDlg");
}

NewCompilerDlg::~NewCompilerDlg()
{
    WindowAttrManager::Save(this, "NewCompilerDlg");
}

void NewCompilerDlg::OnOkUI(wxUpdateUIEvent& event)
{
    event.Enable( !m_textCtrlCompilerName->IsEmpty() );
}

wxString NewCompilerDlg::GetMasterCompiler() const
{
    wxString compilerName = m_choiceCompilers->GetStringSelection();
    if ( compilerName == "<None>" ) {
        return wxEmptyString;
    }
    return compilerName;
}
