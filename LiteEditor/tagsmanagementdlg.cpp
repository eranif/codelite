#include "tagsmanagementdlg.h"
#include "keyvaluetabledlg.h"
#include "variable_entry.h"
#include "ctags_manager.h"
#include "editor_config.h"
#include "tagsmanagementconf.h"
#include <wx/xrc/xmlres.h>
#include <wx/imaglist.h>
#include "windowattrmanager.h"
#include "globals.h"
#include "manager.h"

TagsManagementDlg::TagsManagementDlg( wxWindow* parent )
		: TagsManagementBaseDlg( parent )
		, m_itemDb(wxNOT_FOUND)
{
	m_listCtrlDatabases->InsertColumn(0, _("Database"));
	m_listCtrlDatabases->SetColumnWidth(0, 300);
	m_listCtrlDatabases->SetFocus();

	wxImageList *il = new wxImageList(16, 16, true);
	il->Add(wxXmlResource::Get()->LoadBitmap(wxT("svn_repo")));
	il->Add(wxXmlResource::Get()->LoadBitmap(wxT("tags_selected")));
	m_listCtrlDatabases->AssignImageList(il, wxIMAGE_LIST_SMALL);

	m_listCtrlPaths->InsertColumn(0, _("Original Path"));
	m_listCtrlPaths->InsertColumn(1, _("Actual Path"));
	m_listCtrlPaths->SetColumnWidth(0, 300);
	m_listCtrlPaths->SetColumnWidth(1, 300);

	// populate the control
	TagsManagementConf data;
	EditorConfigST::Get()->ReadObject(wxT("TagsManagementConf"), &data);

	for (size_t i=0; i<data.GetFiles().GetCount(); i++) {
		wxFileName fn(data.GetFiles().Item(i));
		if (fn.FileExists()) {
			DoAddItem(fn, false);
		}
	}

	TagsDatabase *activeDb = TagsManagerST::Get()->GetExtDatabase();
	if (activeDb && activeDb->IsOpen()) {
		DoAddItem(activeDb->GetDatabaseFileName(), true);
	}

	WindowAttrManager::Load(this, wxT("TagsManagementDlgAttr"), NULL);
}

TagsManagementDlg::~TagsManagementDlg()
{
	for (long i=0; i<m_listCtrlDatabases->GetItemCount(); i++) {
		wxFileName* fn = (wxFileName*)  m_listCtrlDatabases->GetItemData(i);
		if (fn) {
			delete fn;
		}
	}
	m_listCtrlDatabases->ClearAll();
	WindowAttrManager::Save(this, wxT("TagsManagementDlgAttr"), NULL);
}

void TagsManagementDlg::OnTagsDbDeSelected( wxListEvent& event )
{
	m_itemDb = wxNOT_FOUND;
	m_staticTextFullPath->SetLabel(wxT(""));
	m_listCtrlPaths->DeleteAllItems();
}

void TagsManagementDlg::OnTagsDbSelected( wxListEvent& event )
{
	m_itemDb = event.m_itemIndex;

	// update the full path
	if (m_itemDb != wxNOT_FOUND) {
		wxFileName *fn = (wxFileName *)m_listCtrlDatabases->GetItemData(m_itemDb);
		if (fn) {
			m_staticTextFullPath->SetLabel(fn->GetFullPath());
			DoLoadPathsTable(*fn);
		}
	}
}

void TagsManagementDlg::OnAddDatabase( wxCommandEvent& event )
{
	// open file selector and select database
	wxString path = ManagerST::Get()->GetStarupDirectory();
	wxString new_db = wxFileSelector(_("Select tags database:"), path.c_str(), wxT(""), wxT(""), wxT("*.tags"), 0, this);
	if (new_db.IsEmpty() == false) {
		wxLogMessage(new_db);
		wxFileName fn(new_db);
		DoAddItem(fn, false);
	}
}

void TagsManagementDlg::OnRemoveDatabase( wxCommandEvent& event )
{
	if (m_itemDb != wxNOT_FOUND) {
		DoDeleteItem(m_itemDb);
		m_itemDb = wxNOT_FOUND;
	}
}

void TagsManagementDlg::OnRemoveDatabaseUI( wxUpdateUIEvent& event )
{
	event.Enable(m_itemDb != wxNOT_FOUND);
}

long TagsManagementDlg::IsFileExist(const wxFileName& filename)
{
	for (long i=0; i<m_listCtrlDatabases->GetItemCount(); i++) {
		if (GetColumnText(m_listCtrlDatabases, i, 0) == filename.GetFullName()) {
			return i;
		}
	}
	return wxNOT_FOUND;
}

void TagsManagementDlg::DoDeleteItem(long item)
{
	wxFileName* fn = (wxFileName*)  m_listCtrlDatabases->GetItemData(item);
	if (fn) {
		delete fn;
	}
	m_listCtrlDatabases->DeleteItem(item);
}

long TagsManagementDlg::DoAddItem(const wxFileName& fn, bool isActive)
{
	long item = IsFileExist(fn);
	if (item == wxNOT_FOUND) {
		item = AppendListCtrlRow(m_listCtrlDatabases);
		SetColumnText(m_listCtrlDatabases, item, 0, fn.GetFullName(), isActive ? 1 : 0);
		m_listCtrlDatabases->SetItemPtrData(item, (wxUIntPtr)new wxFileName(fn));
	} else {
		SetColumnText(m_listCtrlDatabases, item, 0, fn.GetFullName(), isActive ? 1 : 0);
	}
	return item;
}

void TagsManagementDlg::OnOk(wxCommandEvent& e)
{
	TagsManagementConf data;
	wxArrayString files;

	for (long i=0; i<m_listCtrlDatabases->GetItemCount(); i++) {
		wxFileName* fn = (wxFileName*)  m_listCtrlDatabases->GetItemData(i);
		files.Add(fn->GetFullPath());
	}
	data.SetFiles(files);
	EditorConfigST::Get()->WriteObject(wxT("TagsManagementConf"), &data);

	EndModal(wxID_OK);
}

void TagsManagementDlg::OnSetActive(wxCommandEvent& e)
{
	if (m_itemDb != wxNOT_FOUND) {
		wxFileName *fn = (wxFileName *)m_listCtrlDatabases->GetItemData(m_itemDb);
		if (fn) {
			for (long i=0; i<m_listCtrlDatabases->GetItemCount(); i++) {
				SetColumnText(m_listCtrlDatabases, i, 0, GetColumnText(m_listCtrlDatabases, i, 0), i == m_itemDb ? 1 : 0);
			}
			ManagerST::Get()->SetExternalDatabase(*fn);
		}
	}
}

void TagsManagementDlg::OnFixPaths(wxCommandEvent& e)
{
	// get the path
	if (m_itemDb != wxNOT_FOUND) {
		wxFileName *fn = (wxFileName *)m_listCtrlDatabases->GetItemData(m_itemDb);
		if (fn) {
			TagsDatabase db;
			db.OpenDatabase(*fn);

			KeyValueTableDlg dlg(this, &db);
			dlg.ShowModal();

			// Notify tags manager to reload the file paths from the database
			TagsManagerST::Get()->ReloadExtDbPaths();

			// Load the paths table
			DoLoadPathsTable(*fn);
		}
	}
}

void TagsManagementDlg::DoLoadPathsTable(const wxFileName& fn)
{
	// set the tags paths
	m_listCtrlPaths->DeleteAllItems();
	TagsDatabase db;
	db.OpenDatabase(fn);
	if (db.IsOpen()) {
		std::vector<VariableEntryPtr> vars;
		db.GetVariables(vars);

		for (size_t i=0; i<vars.size(); i++) {
			long item = AppendListCtrlRow(m_listCtrlPaths);
			SetColumnText(m_listCtrlPaths, item, 0, vars.at(i)->GetName());
			SetColumnText(m_listCtrlPaths, item, 1, vars.at(i)->GetValue());
		}
	}
}
