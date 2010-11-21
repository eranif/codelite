#include "svn_patch_dlg.h"
#include "windowattrmanager.h"
#include "editor_config.h"

PatchDlg::PatchDlg( wxWindow* parent )
	: PatchDlgBase( parent )
{
	WindowAttrManager::Load(this, wxT("PatchDlg"), NULL);
	
	long v;
	if(EditorConfigST::Get()->GetLongValue(wxT("m_radioBoxEOLPolicy"), v)) {
		m_radioBoxEOLPolicy->SetSelection(v);
	}
}

PatchDlg::~PatchDlg()
{
	WindowAttrManager::Save(this, wxT("PatchDlg"), NULL);
	EditorConfigST::Get()->SaveLongValue(wxT("m_radioBoxEOLPolicy"), m_radioBoxEOLPolicy->GetSelection());
}
