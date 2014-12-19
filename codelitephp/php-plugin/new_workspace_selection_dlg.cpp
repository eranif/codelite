#include "new_workspace_selection_dlg.h"
#include "windowattrmanager.h"
#include "php_configuration_data.h"

NewWorkspaceSelectionDlg::NewWorkspaceSelectionDlg(wxWindow* parent)
    : NewWorkspaceSelectionDlgBase(parent)
{
    GetSizer()->Fit(this);
    PHPConfigurationData conf;
    m_radioBox->SetSelection(conf.Load().GetWorkspaceType());
    WindowAttrManager::Load(this, "NewWorkspaceSelectionDlg");
}

NewWorkspaceSelectionDlg::~NewWorkspaceSelectionDlg()
{
    PHPConfigurationData conf;
    conf.Load().SetWorkspaceType(m_radioBox->GetSelection()).Save();
    WindowAttrManager::Save(this, "NewWorkspaceSelectionDlg");
}
