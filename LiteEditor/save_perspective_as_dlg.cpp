#include "save_perspective_as_dlg.h"
#include "perspectivemanager.h"
#include "manager.h"

SavePerspectiveDlg::SavePerspectiveDlg( wxWindow* parent)
	: SavePerspectiveDlgBase( parent )
{
	m_comboBox->SetFocus();
	
	m_comboBox->Append(ManagerST::Get()->GetPerspectiveManager().GetAllPerspectives());
	m_comboBox->SetValue(ManagerST::Get()->GetPerspectiveManager().GetActive());
}

wxString SavePerspectiveDlg::GetPerspectiveName() const
{
	return m_comboBox->GetValue();
}

