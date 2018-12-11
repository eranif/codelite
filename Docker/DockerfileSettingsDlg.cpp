#include "ColoursAndFontsManager.h"
#include "DockerfileSettingsDlg.h"
#include "lexer_configuration.h"

DockerfileSettingsDlg::DockerfileSettingsDlg(wxWindow* parent, clDockerBuildableFile::Ptr_t info)
    : DockerfileSettingsDlgBase(parent)
    , m_info(info)
{
    m_stcBuild->SetText(m_info->GetBuildOptions());
    m_stcRun->SetText(m_info->GetRunOptions());
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("text");
    lexer->Apply(m_stcBuild);
    lexer->Apply(m_stcRun);
}

DockerfileSettingsDlg::~DockerfileSettingsDlg() {}

void DockerfileSettingsDlg::OnOK(wxCommandEvent& event)
{
    event.Skip();
    wxString buildOptions = m_stcBuild->GetText();
    buildOptions.Trim().Trim(false);
    m_info->SetBuildOptions(buildOptions);

    wxString runOptions = m_stcRun->GetText();
    runOptions.Trim().Trim(false);
    m_info->SetRunOptions(runOptions);
}
