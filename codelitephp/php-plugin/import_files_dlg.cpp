#include "import_files_dlg.h"
#include <windowattrmanager.h>
#include "php_workspace.h"

ImportFilesDlg::ImportFilesDlg( wxWindow* parent )
    : ImportFilesBaseDlg( parent )
{
    m_textCtrlFolderPath->ChangeValue(PHPWorkspace::Get()->GetFilename().GetPath());
    WindowAttrManager::Load(this, wxT("ImportFilesDlg"), NULL);
}

ImportFilesDlg::~ImportFilesDlg()
{
    WindowAttrManager::Save(this, wxT("ImportFilesDlg"), NULL);
}

void ImportFilesDlg::OnBrowse(wxCommandEvent& event)
{
    wxString path = ::wxDirSelector(_("Select a folder"));
    if ( !path.IsEmpty() ) {
        m_textCtrlFolderPath->SetValue( path );
    }
}
void ImportFilesDlg::OnOkUI(wxUpdateUIEvent& event)
{
    event.Enable( !m_textCtrlFolderPath->IsEmpty() );
}
