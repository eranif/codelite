#include "project.h"
#include "virtualdirectoryselector.h"
#include "imanager.h"
#include "wxfbitemdlg.h"
#include "globals.h"

wxFBItemDlg::wxFBItemDlg( wxWindow* parent, IManager *mgr )
		: wxFBItemBaseDlg( parent )
		, m_mgr(mgr)
{
	TreeItemInfo item = m_mgr->GetSelectedTreeItemInfo( TreeFileView );
	if ( item.m_item.IsOk() && item.m_itemType == ProjectItem::TypeVirtualDirectory ) {
		m_textCtrlVD->SetValue(VirtualDirectorySelector::DoGetPath( m_mgr->GetTree( TreeFileView ), item.m_item, false ));
	}
	m_textCtrlClassName->SetFocus();
	GetSizer()->Fit(this);
}

void wxFBItemDlg::OnGenerate( wxCommandEvent& event )
{
	wxUnusedVar(event);

	if ( !IsValidCppIndetifier(m_textCtrlClassName->GetValue()) ) {
		wxMessageBox(wxT("Invalid C++ class name"), wxT("CodeLite"), wxOK|wxICON_WARNING);
		return;
	}

	EndModal(wxID_OK);
}

void wxFBItemDlg::OnGenerateUI( wxUpdateUIEvent& event )
{
	bool titleCondition;
	if(m_textCtrlTitle->IsEnabled()){
		titleCondition = !m_textCtrlTitle->IsEmpty();
	} else {
		titleCondition = true;
	}
	
	event.Enable(!m_textCtrlClassName->IsEmpty() && titleCondition && !m_textCtrlVD->IsEmpty());
}

void wxFBItemDlg::OnCancel( wxCommandEvent& event )
{
	wxUnusedVar(event);
	EndModal(wxID_CANCEL);
}

void wxFBItemDlg::OnBrowseVD(wxCommandEvent& event)
{
	VirtualDirectorySelector dlg(this, m_mgr->GetWorkspace(), m_textCtrlVD->GetValue());
	if ( dlg.ShowModal() == wxID_OK ) {
		m_textCtrlVD->SetValue( dlg.GetVirtualDirectoryPath() );
	}
}

wxFBItemInfo wxFBItemDlg::GetData()
{
	wxFBItemInfo info;
	info.className = m_textCtrlClassName->GetValue();
	info.kind = wxFBItemKind_Unknown;
	info.title = m_textCtrlTitle->GetValue();
	info.virtualFolder = m_textCtrlVD->GetValue();
	info.file = m_textCtrlClassName->GetValue().Lower();
	
	return info;
}

void wxFBItemDlg::DisableTitleField()
{
	m_textCtrlTitle->Enable(false);
}
