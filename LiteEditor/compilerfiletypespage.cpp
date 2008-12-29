#include "compilerfiletypespage.h"
#include "editcmpfileinfodlg.h"
#include "compiler.h"
#include <wx/msgdlg.h>
#include "globals.h"
#include "build_settings_config.h"

CompilerFileTypePage::CompilerFileTypePage( wxWindow* parent, const wxString &cmpname )
		: CompilerFileTypesBase( parent )
		, m_cmpname(cmpname)
		, m_selectedFileType(wxNOT_FOUND)
{
	InitFileTypes();
}

void CompilerFileTypePage::OnFileTypeActivated(wxListEvent& event)
{
	if (m_selectedFileType != wxNOT_FOUND) {
		EditCmpFileInfo dlg(this);
		dlg.SetCompilationLine(GetColumnText(m_listCtrlFileTypes, m_selectedFileType, 2));
		dlg.SetExtension(GetColumnText(m_listCtrlFileTypes, m_selectedFileType, 0));
		dlg.SetKind(GetColumnText(m_listCtrlFileTypes, m_selectedFileType, 1));

		if (dlg.ShowModal() == wxID_OK) {
			SetColumnText(m_listCtrlFileTypes, m_selectedFileType, 2, dlg.GetCompilationLine());
			SetColumnText(m_listCtrlFileTypes, m_selectedFileType, 0, dlg.GetExtension().Lower());
			SetColumnText(m_listCtrlFileTypes, m_selectedFileType, 1, dlg.GetKind());
		}
	}
	event.Skip();
}

void CompilerFileTypePage::OnFileTypeDeSelected(wxListEvent& event)
{
#ifndef __WXMAC__
	m_selectedFileType = wxNOT_FOUND;
#endif
	event.Skip();
}

void CompilerFileTypePage::OnFileTypeSelected(wxListEvent& event)
{
	m_selectedFileType = event.m_itemIndex;
	event.Skip();
}

void CompilerFileTypePage::OnDeleteFileType(wxCommandEvent& event)
{
	if (m_selectedFileType != wxNOT_FOUND) {
		if (wxMessageBox(_("Are you sure you want to delete this file type?"), wxT("CodeLite"), wxYES_NO|wxCANCEL) == wxYES) {
			m_listCtrlFileTypes->DeleteItem(m_selectedFileType);
			m_selectedFileType = wxNOT_FOUND;
		}
	}
}

void CompilerFileTypePage::OnNewFileType(wxCommandEvent& event)
{
	EditCmpFileInfo dlg(this);
	if (dlg.ShowModal() == wxID_OK) {
		long newItem = AppendListCtrlRow(m_listCtrlFileTypes);
		SetColumnText(m_listCtrlFileTypes, newItem, 2, dlg.GetCompilationLine());
		SetColumnText(m_listCtrlFileTypes, newItem, 0, dlg.GetExtension().Lower());
		SetColumnText(m_listCtrlFileTypes, newItem, 1, dlg.GetKind());
	}
}

void CompilerFileTypePage::InitFileTypes()
{
	m_listCtrlFileTypes->Freeze();
	m_listCtrlFileTypes->ClearAll();
	m_listCtrlFileTypes->InsertColumn(0, wxT("Extension"));
	m_listCtrlFileTypes->InsertColumn(1, wxT("Kind"));
	m_listCtrlFileTypes->InsertColumn(2, wxT("Compilation Line"));

	//populate the list control
	CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(m_cmpname);
	if (cmp) {
		std::map<wxString, Compiler::CmpFileTypeInfo> fileTypes = cmp->GetFileTypes();
		std::map<wxString, Compiler::CmpFileTypeInfo>::iterator iter = fileTypes.begin();
		for ( ; iter != fileTypes.end(); iter++ ) {
			Compiler::CmpFileTypeInfo ft = iter->second;

			long item = AppendListCtrlRow(m_listCtrlFileTypes);
			SetColumnText(m_listCtrlFileTypes, item, 0, ft.extension);
			SetColumnText(m_listCtrlFileTypes, item, 1, ft.kind == Compiler::CmpFileKindSource ? wxT("Source") : wxT("Resource"));
			SetColumnText(m_listCtrlFileTypes, item, 2, ft.compilation_line);
		}
	}

	m_listCtrlFileTypes->SetColumnWidth(0, 70);
	m_listCtrlFileTypes->SetColumnWidth(1, 70);
	m_listCtrlFileTypes->SetColumnWidth(2, wxLIST_AUTOSIZE);

	m_listCtrlFileTypes->Thaw();
}

void CompilerFileTypePage::Save(CompilerPtr cmp)
{
	std::map<wxString, Compiler::CmpFileTypeInfo> fileTypes;
	int count = m_listCtrlFileTypes->GetItemCount();
	for (int i=0; i<count; i++) {
		Compiler::CmpFileTypeInfo ft;
		ft.extension = GetColumnText(m_listCtrlFileTypes, i, 0);
		ft.kind = GetColumnText(m_listCtrlFileTypes, i, 1) == wxT("Resource") ? Compiler::CmpFileKindResource : Compiler::CmpFileKindSource;
		ft.compilation_line = GetColumnText(m_listCtrlFileTypes, i, 2);

		fileTypes[ft.extension] = ft;
	}

	cmp->SetFileTypes(fileTypes);
}
