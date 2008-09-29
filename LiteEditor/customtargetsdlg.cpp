#include "envvar_dlg.h"
#include "globals.h"
#include "customtargetsdlg.h"

CustomTargetsDlg::CustomTargetsDlg( wxWindow* parent )
		: CustomTargetsBase( parent )
		, m_selectedItem(wxNOT_FOUND)
{
	m_listCtrlTargets->InsertColumn(0, wxT("Target"));
	m_listCtrlTargets->InsertColumn(1, wxT("Command"));
	m_listCtrlTargets->SetFocus();
}

void CustomTargetsDlg::OnItemActivated( wxListEvent& event )
{
	m_selectedItem = event.m_itemIndex;
	DoEditItem(m_selectedItem);
}

void CustomTargetsDlg::OnItemSelected( wxListEvent& event )
{
	m_selectedItem = event.m_itemIndex;
}

void CustomTargetsDlg::OnNewTarget( wxCommandEvent& event )
{
	wxUnusedVar(event);

	EnvVarDlg dlg(this);
	dlg.SetTitle(wxT("New target"));
	dlg.SetStaticText1(wxT("Target Name:"));
	dlg.SetStaticText2(wxT("Command:"));
	if (dlg.ShowModal() == wxID_OK) {
		long item = AppendListCtrlRow(m_listCtrlTargets);
		DoUpdateItem(item, dlg.GetName(), dlg.GetValue());
	}
}

void CustomTargetsDlg::OnEditTarget( wxCommandEvent& event )
{
	wxUnusedVar(event);
	if (m_selectedItem != wxNOT_FOUND) {
		DoEditItem(m_selectedItem);
	}
}

void CustomTargetsDlg::OnEditUI( wxUpdateUIEvent& event )
{
	event.Enable(m_selectedItem != wxNOT_FOUND);
}

void CustomTargetsDlg::OnDeleteTarget( wxCommandEvent& event )
{
	wxUnusedVar(event);
	if (m_selectedItem != wxNOT_FOUND) {
		m_listCtrlTargets->DeleteItem(m_selectedItem);
		m_selectedItem = wxNOT_FOUND;
	}
}

void CustomTargetsDlg::OnDeleteUI( wxUpdateUIEvent& event )
{
	event.Enable(m_selectedItem != wxNOT_FOUND);
}

void CustomTargetsDlg::DoEditItem(long item)
{
	if (item != wxNOT_FOUND) {
		wxString target = GetColumnText(m_listCtrlTargets, item, 0);
		wxString cmd    = GetColumnText(m_listCtrlTargets, item, 1);
		EnvVarDlg dlg(this);
		dlg.SetTitle(wxT("Edit target"));
		dlg.SetStaticText1(wxT("Target Name:"));
		dlg.SetStaticText2(wxT("Command:"));
		dlg.SetName(target);
		dlg.SetValue(cmd);

		if (dlg.ShowModal() == wxID_OK) {
			DoUpdateItem(item, dlg.GetName(), dlg.GetValue());
		}
	}
}

void CustomTargetsDlg::DoUpdateItem(long item, const wxString& target, const wxString& cmd)
{
	SetColumnText(m_listCtrlTargets, item, 0, target);
	SetColumnText(m_listCtrlTargets, item, 1, cmd);
	
	if(m_listCtrlTargets->GetItemCount()){
		m_listCtrlTargets->SetColumnWidth(0, wxLIST_AUTOSIZE);
		m_listCtrlTargets->SetColumnWidth(1, wxLIST_AUTOSIZE);
	}
}

void CustomTargetsDlg::OnButtonOK(wxCommandEvent& event)
{
	wxUnusedVar(event);
	m_targets.clear();
	// update the targets and close the dialog
	for (long item = 0; item < m_listCtrlTargets->GetItemCount(); item++) {
		m_targets[GetColumnText(m_listCtrlTargets, item, 0)] = GetColumnText(m_listCtrlTargets, item, 1);
	}
	EndModal(wxID_OK);
}

void CustomTargetsDlg::SetTargets(const std::map<wxString, wxString>& targets)
{
	this->m_targets = targets;
	m_listCtrlTargets->DeleteAllItems();
	std::map<wxString, wxString>::iterator iter = m_targets.begin();
	for (; iter != m_targets.end(); iter++) {
		long item = AppendListCtrlRow(m_listCtrlTargets);
		SetColumnText(m_listCtrlTargets, item, 0, iter->first);
		SetColumnText(m_listCtrlTargets, item, 1, iter->second);
	}
	
	if(m_listCtrlTargets->GetItemCount()){
		m_listCtrlTargets->SetColumnWidth(0, wxLIST_AUTOSIZE);
		m_listCtrlTargets->SetColumnWidth(1, wxLIST_AUTOSIZE);
	}
}

const std::map<wxString,wxString>& CustomTargetsDlg::GetTargets() const
{
	return m_targets;
}
