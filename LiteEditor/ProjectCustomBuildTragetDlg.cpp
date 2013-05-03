#include "ProjectCustomBuildTragetDlg.h"
#include "windowattrmanager.h"

const wxString ProjectCustomBuildTragetDlg::CUSTOM_TARGET_BUILD               = wxT("Build");
const wxString ProjectCustomBuildTragetDlg::CUSTOM_TARGET_CLEAN               = wxT("Clean");
const wxString ProjectCustomBuildTragetDlg::CUSTOM_TARGET_REBUILD             = wxT("Rebuild");
const wxString ProjectCustomBuildTragetDlg::CUSTOM_TARGET_COMPILE_SINGLE_FILE = wxT("Compile Single File");
const wxString ProjectCustomBuildTragetDlg::CUSTOM_TARGET_PREPROCESS_FILE     = wxT("Preprocess File");

ProjectCustomBuildTragetDlg::ProjectCustomBuildTragetDlg(wxWindow* parent, const wxString &targetName, const wxString& targetCommand)
    : ProjectCustomBuildTragetDlgBase(parent)
{
    m_textCtrlCommand->ChangeValue( targetCommand );
    m_textCtrlTargetName->ChangeValue( targetName );
    WindowAttrManager::Load(this, "ProjectCustomBuildTragetDlg", NULL);
}

ProjectCustomBuildTragetDlg::~ProjectCustomBuildTragetDlg()
{
    WindowAttrManager::Save(this, "ProjectCustomBuildTragetDlg", NULL);
}
void ProjectCustomBuildTragetDlg::OnEditTargetNameUI(wxUpdateUIEvent& event)
{
    event.Enable( !IsPredefinedTarget(GetTargetName()) );
}

bool ProjectCustomBuildTragetDlg::IsPredefinedTarget(const wxString& name)
{
    return name == CUSTOM_TARGET_BUILD || 
           name == CUSTOM_TARGET_CLEAN || 
           name == CUSTOM_TARGET_REBUILD || 
           name == CUSTOM_TARGET_COMPILE_SINGLE_FILE ||
           name == CUSTOM_TARGET_PREPROCESS_FILE;
}
