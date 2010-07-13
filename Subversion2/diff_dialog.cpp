#include "diff_dialog.h"
#include "windowattrmanager.h"
#include "imanager.h"

DiffDialog::DiffDialog( wxWindow* parent, IManager *manager )
	: DiffDialogBase( parent )
	, m_manager(manager)
{
	m_textCtrlFromRev->SetFocus();
	WindowAttrManager::Load(this, wxT("DiffDialog"), m_manager->GetConfigTool());
}

DiffDialog::~DiffDialog()
{
	WindowAttrManager::Save(this, wxT("DiffDialog"), m_manager->GetConfigTool());
}

wxString DiffDialog::GetFromRevision() const
{
	wxString from = m_textCtrlFromRev->GetValue();
	from.Trim().Trim(false);

	if(from.IsEmpty()) {
		return wxT("BASE");
	} else {
		return from;
	}
}

wxString DiffDialog::GetToRevision() const
{
	wxString to = m_textCtrlToRev->GetValue();
	to.Trim().Trim(false);
	return to;
}
