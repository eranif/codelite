#include "globals.h"
#include "externaltoolsdata.h"
#include "newtooldlg.h"
#include "externaltooldlg.h"

ExternalToolDlg::ExternalToolDlg( wxWindow* parent )
		: ExternalToolBaseDlg( parent )
		, m_item(wxNOT_FOUND)
{
	Initialize();
}

void ExternalToolDlg::OnItemActivated( wxListEvent& event )
{
	m_item = event.m_itemIndex;
	DoEditEntry(event.m_itemIndex);
}

void ExternalToolDlg::OnItemDeSelected( wxListEvent& event )
{
	wxUnusedVar(event);
	m_item = wxNOT_FOUND;
}

void ExternalToolDlg::OnItemSelected( wxListEvent& event )
{
	m_item = event.m_itemIndex;
}

void ExternalToolDlg::OnButtonNew( wxCommandEvent& event )
{
	NewToolDlg dlg(this, wxEmptyString, wxEmptyString, wxEmptyString, wxEmptyString );
	if (dlg.ShowModal() == wxID_OK) {
		DoUpdateEntry(dlg.GetToolId(), dlg.GetPath(), dlg.GetWorkingDirectory(), dlg.GetArguments());
	}
}

void ExternalToolDlg::OnButtonNewUI( wxUpdateUIEvent& event )
{
	// maximum of 10 items
	event.Enable(m_listCtrlTools->GetItemCount() < 10);
}

void ExternalToolDlg::OnButtonEdit( wxCommandEvent& event )
{
	wxUnusedVar(event);
	DoEditEntry(m_item);
}

void ExternalToolDlg::OnButtonEditUI( wxUpdateUIEvent& event )
{
	event.Enable(m_item != wxNOT_FOUND);
}

void ExternalToolDlg::OnButtonDelete( wxCommandEvent& event )
{
	m_listCtrlTools->DeleteItem(m_item);
}

void ExternalToolDlg::OnButtonDeleteUI( wxUpdateUIEvent& event )
{
	event.Enable(m_item != wxNOT_FOUND);
}

void ExternalToolDlg::Initialize()
{
	m_listCtrlTools->InsertColumn(0, wxT("ID"));
	m_listCtrlTools->InsertColumn(1, wxT("Path"));
	m_listCtrlTools->InsertColumn(2, wxT("Arguments"));
	m_listCtrlTools->InsertColumn(3, wxT("Working directory"));

	// TODO: populate the list from the settings
	m_listCtrlTools->SetColumnWidth(0, 100);
	m_listCtrlTools->SetColumnWidth(1, wxLIST_AUTOSIZE);
	m_listCtrlTools->SetColumnWidth(2, wxLIST_AUTOSIZE);
	m_listCtrlTools->SetColumnWidth(3, wxLIST_AUTOSIZE);
}

void ExternalToolDlg::DoUpdateEntry(const wxString& id, const wxString& path, const wxString& workingDirectory, const wxString& arguments)
{
	// try to see if 'id' already exist in the list control
	long item(wxNOT_FOUND);
	for(size_t i=0; i<(size_t)m_listCtrlTools->GetItemCount(); i++){
		if(GetColumnText(m_listCtrlTools, i, 0) == id){
			item = i;
			break;
		}
	}
	
	// append new row
	if(item == wxNOT_FOUND){
		item = AppendListCtrlRow(m_listCtrlTools);
	}
	
	SetColumnText(m_listCtrlTools, item, 0, id);
	SetColumnText(m_listCtrlTools, item, 1, path);
	SetColumnText(m_listCtrlTools, item, 2, arguments);
	SetColumnText(m_listCtrlTools, item, 3, workingDirectory);

	m_listCtrlTools->SetColumnWidth(0, 150);
	m_listCtrlTools->SetColumnWidth(1, wxLIST_AUTOSIZE);
	m_listCtrlTools->SetColumnWidth(2, wxLIST_AUTOSIZE);
	m_listCtrlTools->SetColumnWidth(3, wxLIST_AUTOSIZE);
}

void ExternalToolDlg::DoEditEntry(long item)
{
	wxString id = GetColumnText(m_listCtrlTools, m_item, 0);
	wxString path = GetColumnText(m_listCtrlTools, m_item, 1);
	wxString args = GetColumnText(m_listCtrlTools, m_item, 2);
	wxString wd = GetColumnText(m_listCtrlTools, m_item, 3);

	NewToolDlg dlg(this, id, path, wd, args);
	if (dlg.ShowModal() == wxID_OK) {
		DoUpdateEntry(dlg.GetToolId(), dlg.GetPath(), dlg.GetWorkingDirectory(), dlg.GetArguments());
	}
}

std::vector<ToolInfo> ExternalToolDlg::GetTools()
{
	std::vector<ToolInfo> tools;
	for(size_t i=0; i<(size_t)m_listCtrlTools->GetItemCount(); i++){
		ToolInfo ti;
		ti.SetId(GetColumnText(m_listCtrlTools, i, 0));
		ti.SetPath(GetColumnText(m_listCtrlTools, i, 1));
		ti.SetArguments(GetColumnText(m_listCtrlTools, i, 2));
		ti.SetWd(GetColumnText(m_listCtrlTools, i, 3));
		tools.push_back(ti);
	}
	return tools;
}

void ExternalToolDlg::SetTools(const std::vector<ToolInfo>& tools)
{
	m_listCtrlTools->Freeze();
	m_listCtrlTools->DeleteAllItems();
	
	for(size_t i=0; i<tools.size(); i++){
		ToolInfo ti = tools.at(i);
		long item = AppendListCtrlRow(m_listCtrlTools);
		SetColumnText(m_listCtrlTools, item, 0, ti.GetId());
		SetColumnText(m_listCtrlTools, item, 1, ti.GetPath());
		SetColumnText(m_listCtrlTools, item, 2, ti.GetArguments());
		SetColumnText(m_listCtrlTools, item, 3, ti.GetWd());
	}
	
	m_listCtrlTools->SetColumnWidth(0, 150);
	m_listCtrlTools->SetColumnWidth(1, wxLIST_AUTOSIZE);
	m_listCtrlTools->SetColumnWidth(2, wxLIST_AUTOSIZE);
	m_listCtrlTools->SetColumnWidth(3, wxLIST_AUTOSIZE);
	
	m_listCtrlTools->Thaw();
}
