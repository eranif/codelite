#include "codedesigneritemdlg.h"
#include "project.h"
#include "virtualdirectoryselector.h"
#include "imanager.h"
#include "globals.h"

CodeDesignerItemDlg::CodeDesignerItemDlg(wxWindow *parent, IManager *mgr, bool allowmain)
	: CodeDesignerItemBaseDlg(parent)
{
	m_AllowMain = allowmain;
	m_mgr = mgr;
}

CodeDesignerItemDlg::~CodeDesignerItemDlg()
{
}

void CodeDesignerItemDlg::OnCancel(wxCommandEvent& event)
{
	EndModal( wxID_CANCEL );
}

void CodeDesignerItemDlg::OnInit(wxInitDialogEvent& event)
{
	TreeItemInfo item = m_mgr->GetSelectedTreeItemInfo( TreeFileView );
	if ( item.m_item.IsOk() && item.m_itemType == ProjectItem::TypeVirtualDirectory )
	{
		m_textTreeFolder->SetValue(VirtualDirectorySelector::DoGetPath( m_mgr->GetTree( TreeFileView ), item.m_item, false ));
	}

	m_textPackageName->SetValue( m_CDInfo.m_PackageName );
	m_checkGenerate->SetValue( true );

	event.Skip();
}

void CodeDesignerItemDlg::OnOk(wxCommandEvent& event)
{
	if( m_textDiagramName->GetValue().IsEmpty() )
	{
		wxMessageBox( _("Diagram name cannot be empty"), _("CodeLite"), wxICON_WARNING | wxOK );
		m_textDiagramName->SetFocus();
		return;
	}

	if( m_textPackageName->GetValue().IsEmpty() )
	{
		wxMessageBox( _("Package name cannot be empty"), _("CodeLite"), wxICON_WARNING | wxOK );
		m_textPackageName->SetFocus();
		return;
	}

	if( m_textTreeFolder->GetValue().IsEmpty() )
	{
		wxMessageBox( _("Folder name cannot be empty"), _("CodeLite"), wxICON_WARNING | wxOK );
		m_textTreeFolder->SetFocus();
		return;
	}

	m_CDInfo.m_DiagramName = m_textDiagramName->GetValue();
	m_CDInfo.m_PackageName = m_textPackageName->GetValue();
	m_CDInfo.m_TreeFolderName = m_textTreeFolder->GetValue();
	m_CDInfo.m_UseMain = m_checkBoxMain->GetValue();
	m_CDInfo.m_GenerateCode = m_checkGenerate->GetValue();
	
	if( m_choiceLanguage->GetStringSelection() == wxT("C") )
	{
		m_CDInfo.m_Language = wxT("udCLanguage");
	}
	else if( m_choiceLanguage->GetStringSelection() == wxT("C++") )
	{
		m_CDInfo.m_Language = wxT("udCPPLanguage");
	}
	else if( m_choiceLanguage->GetStringSelection() == wxT("Python") )
	{
		m_CDInfo.m_Language = wxT("udPythonLanguage");
	}

	EndModal( wxID_OK );
}

void CodeDesignerItemDlg::OnUpdateMain(wxUpdateUIEvent& event)
{
	event.Enable( m_AllowMain );
}

void CodeDesignerItemDlg::OnBrowse(wxCommandEvent& event)
{
	VirtualDirectorySelector dlg(this, m_mgr->GetWorkspace(), m_textTreeFolder->GetValue());
	if ( dlg.ShowModal() == wxID_OK )
	{
		m_textTreeFolder->SetValue( dlg.GetVirtualDirectoryPath() );
	}
}
