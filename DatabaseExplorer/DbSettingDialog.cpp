#include "DbSettingDialog.h"
#include "db_explorer_settings.h"
#include "editor_config.h"
#include "windowattrmanager.h"
#include "globals.h"

#include <wx/dblayer/include/DatabaseLayer.h>

#ifdef DBL_USE_MYSQL
#include <wx/dblayer/include/MysqlDatabaseLayer.h>
#endif

#ifdef DBL_USE_SQLITE
#include <wx/dblayer/include/SqliteDatabaseLayer.h>
#endif

#include <wx/dblayer/include/DatabaseErrorCodes.h>
#include <wx/dblayer/include/DatabaseLayerException.h>

DbSettingDialog::DbSettingDialog(DbViewerPanel *parent, wxWindow* pWindowParent):_DBSettingsDialog( pWindowParent )
{

	m_pParent = parent;
	m_listCtrlRecentFiles->InsertColumn(0, wxT("File name"));
	m_listCtrlRecentFiles->SetColumnWidth(0, 600);
	m_filePickerSqlite->SetFocus();

	LoadHistory();
#ifndef DBL_USE_MYSQL
	m_MySqlPanel->Enable(false);
#endif
#ifndef DBL_USE_POSTGRES
	m_PostgrePanel->Enable(false);
#endif
	WindowAttrManager::Load(this, wxT("DbSettingDialog"), NULL);
}

DbSettingDialog::~DbSettingDialog()
{
	WindowAttrManager::Save(this, wxT("DbSettingDialog"), NULL);
	m_pHistory = new MysqlConnectionHistory();
	if (m_pHistory) {
		for (unsigned int i = 0 ; i < m_listBox2->GetCount(); i++) {
			MysqlHistItem* item = wxDynamicCast(m_listBox2->GetClientData(i), MysqlHistItem);
			if (item) {
				m_pHistory->AddChild((MysqlHistItem* )item->Clone());
			}
		}
		MysqlConnectionHistory::SaveToFile(m_pHistory);
	}
	m_pPgHistory = new PostgresConnectionHistory();
	if (m_pPgHistory) {
		for (unsigned int i = 0 ; i < m_listBoxPg->GetCount(); i++) {
			PostgresHistItem* item = wxDynamicCast(m_listBoxPg->GetClientData(i), PostgresHistItem);
			if (item) {
				m_pPgHistory->AddChild((PostgresHistItem* )item->Clone());
			}
		}
		PostgresConnectionHistory::SaveToFile(m_pPgHistory);
	}
}

void DbSettingDialog::OnCancelClick(wxCommandEvent& event)
{
	Destroy();
}
void DbSettingDialog::OnOkClick(wxCommandEvent& event)
{
#ifdef DBL_USE_MYSQL
	try {
		//MysqlDatabaseLayer *DbLayer = new MysqlDatabaseLayer(m_txServer->GetValue(),wxT(""),m_txUserName->GetValue(),m_txPassword->GetValue());
		IDbAdapter* adapt = new MySqlDbAdapter(m_txServer->GetValue(),m_txUserName->GetValue(),m_txPassword->GetValue());

		wxString serverName = m_txServer->GetValue();
		m_pParent->AddDbConnection(new DbConnection(adapt, serverName));

		m_pParent->SetServer(serverName);
		Destroy();
	} catch (DatabaseLayerException& e) {
		wxString errorMessage = wxString::Format(_("Error (%d): %s"), e.GetErrorCode(), e.GetErrorMessage().c_str());
		wxMessageDialog dlg(this,errorMessage,wxT("DB Error"),wxOK | wxCENTER | wxICON_ERROR);
		dlg.ShowModal();
	} catch( ... ) {
		wxMessageDialog dlg(this,wxT("Unknown error."),wxT("DB Error"),wxOK | wxCENTER | wxICON_ERROR);
		dlg.ShowModal();
	}
#else
	wxMessageBox( wxT("MySQL connection is not supported."), wxT("DB Error"), wxOK | wxICON_WARNING );
#endif
}

void DbSettingDialog::OnSqliteOkClick(wxCommandEvent& event)
{
#ifdef DBL_USE_SQLITE
	try {

		//SqliteDatabaseLayer *DbLayer = new SqliteDatabaseLayer(m_filePickerSqlite->GetPath());
		IDbAdapter* pAdapt = new SQLiteDbAdapter(m_filePickerSqlite->GetPath());


		//m_pParent->SetDbLayer(DbLayer);
		wxString serverName = m_filePickerSqlite->GetPath();
		m_pParent->AddDbConnection(new DbConnection(pAdapt, serverName));

		// Save the recent opened files
		DbExplorerSettings settings;
		EditorConfigST::Get()->ReadObject(wxT("DbExplorerSettings"), &settings);

		wxArrayString files = settings.GetRecentFiles();
		files.Insert(serverName, 0);
		settings.SetRecentFiles(files);
		EditorConfigST::Get()->WriteObject(wxT("DbExplorerSettings"), &settings);

		m_pParent->SetServer(serverName);
		Destroy();

	} catch (DatabaseLayerException& e) {
		wxString errorMessage = wxString::Format(_("Error (%d): %s"), e.GetErrorCode(), e.GetErrorMessage().c_str());
		wxMessageDialog dlg(this,errorMessage,wxT("DB Error"),wxOK | wxCENTER | wxICON_ERROR);
		dlg.ShowModal();
	} catch( ... ) {
		wxMessageDialog dlg(this,wxT("Unknown error."),wxT("DB Error"),wxOK | wxCENTER | wxICON_ERROR);
		dlg.ShowModal();
	}
#else
	wxMessageBox( wxT("SQLite connection is not supported."), wxT("DB Error"), wxOK | wxICON_WARNING );
#endif
}

void DbSettingDialog::OnHistoruUI(wxUpdateUIEvent& event)
{
	//event.Enable( m_listBox2->GetCount() > 0 );
}

void DbSettingDialog::OnHistoryClick(wxCommandEvent& event)
{
	MysqlHistItem* item = wxDynamicCast(event.GetClientData(), MysqlHistItem);
	if (item) {
		m_txName->SetValue(item->GetName());
		m_txServer->SetValue(item->GetServer());
		m_txUserName->SetValue(item->GetUserName());
		m_txPassword->SetValue(item->GetPassword());
	}
}

void DbSettingDialog::OnHistoryDClick(wxCommandEvent& event)
{
}

void DbSettingDialog::OnRemoveClick(wxCommandEvent& event)
{
	int i = m_listBox2->GetSelection();
	if (i > -1) m_listBox2->Delete(i);
}

void DbSettingDialog::OnRmoveUI(wxUpdateUIEvent& event)
{
	event.Enable(m_listBox2->GetSelection() > -1);
}

void DbSettingDialog::OnSaveClick(wxCommandEvent& event)
{
	m_listBox2->Append(m_txName->GetValue(), new MysqlHistItem(m_txName->GetValue(),m_txServer->GetValue(),m_txUserName->GetValue(),m_txPassword->GetValue()));
}

void DbSettingDialog::OnSaveUI(wxUpdateUIEvent& event)
{
	event.Enable(!m_txName->IsEmpty());
}
void DbSettingDialog::OnOKUI(wxUpdateUIEvent& event)
{
	event.Enable(!m_txServer->IsEmpty() && !m_txUserName->IsEmpty());
}
void DbSettingDialog::LoadHistory()
{
	// recent sqlite files
	DbExplorerSettings settings;
	EditorConfigST::Get()->ReadObject(wxT("DbExplorerSettings"), &settings);
	wxArrayString files = settings.GetRecentFiles();

	m_listCtrlRecentFiles->DeleteAllItems();
	for(size_t i=0; i<files.Count(); i++) {
		int idx = AppendListCtrlRow(m_listCtrlRecentFiles);
		SetColumnText(m_listCtrlRecentFiles, idx, 0, files.Item(i));
	}

	m_pHistory = MysqlConnectionHistory::LoadFromFile();
	if (m_pHistory) {
		m_listBox2->Clear();
		SerializableList::compatibility_iterator node = m_pHistory->GetFirstChildNode();
		while( node ) {
			if( node->GetData()->IsKindOf( CLASSINFO(MysqlHistItem)) ) {
				MysqlHistItem *item = (MysqlHistItem*) node->GetData();
				m_listBox2->Append(item->GetName(),item->Clone());
			}
			node = node->GetNext();
		}
	}
	if (m_pHistory) delete m_pHistory;
	m_pHistory = NULL;

	m_pPgHistory = PostgresConnectionHistory::LoadFromFile();
	if (m_pPgHistory) {
		m_listBoxPg->Clear();
		SerializableList::compatibility_iterator node = m_pPgHistory->GetFirstChildNode();
		while( node ) {
			if( node->GetData()->IsKindOf( CLASSINFO(PostgresHistItem)) ) {
				PostgresHistItem *item = (PostgresHistItem*) node->GetData();
				m_listBoxPg->Append(item->GetName(),item->Clone());
			}
			node = node->GetNext();
		}
	}
	if (m_pPgHistory) delete m_pPgHistory;
	m_pPgHistory = NULL;


}

void DbSettingDialog::OnPgCancelClick(wxCommandEvent& event)
{
	Destroy();
}

void DbSettingDialog::OnPgOkClick(wxCommandEvent& event)
{
#ifdef DBL_USE_POSTGRES
	try {
		//MysqlDatabaseLayer *DbLayer = new MysqlDatabaseLayer(m_txServer->GetValue(),wxT(""),m_txUserName->GetValue(),m_txPassword->GetValue());
		IDbAdapter* adapt = new PostgreSqlDbAdapter(m_txPgServer->GetValue(),m_txPgDatabase->GetValue(),m_txPgUserName->GetValue(),m_txPgPassword->GetValue());

		wxString serverName = m_txPgServer->GetValue();
		m_pParent->AddDbConnection(new DbConnection(adapt, serverName));

		m_pParent->SetServer(serverName);
		Destroy();
	} catch (DatabaseLayerException& e) {
		wxString errorMessage = wxString::Format(_("Error (%d): %s"), e.GetErrorCode(), e.GetErrorMessage().c_str());
		wxMessageDialog dlg(this,errorMessage,wxT("DB Error"),wxOK | wxCENTER | wxICON_ERROR);
		dlg.ShowModal();
	} catch( ... ) {
		wxMessageDialog dlg(this,wxT("Unknown error."),wxT("DB Error"),wxOK | wxCENTER | wxICON_ERROR);
		dlg.ShowModal();
	}
#else
	wxMessageBox( wxT("PostgreSQL connection is not supported."), wxT("DB Error"), wxOK | wxICON_WARNING );
#endif
}


void DbSettingDialog::OnPgRemoveClick(wxCommandEvent& event)
{
	int i = m_listBoxPg->GetSelection();
	if (i > -1) m_listBoxPg->Delete(i);
}

void DbSettingDialog::OnPgSaveClick(wxCommandEvent& event)
{
	m_listBoxPg->Append(m_txPgName->GetValue(), new PostgresHistItem(m_txPgName->GetValue(),m_txPgServer->GetValue(),m_txPgUserName->GetValue(),m_txPgPassword->GetValue(),m_txPgDatabase->GetValue()));
}
void DbSettingDialog::OnPgHistoryClick(wxCommandEvent& event)
{
	PostgresHistItem* item = wxDynamicCast(event.GetClientData(), PostgresHistItem);
	if (item) {
		m_txPgName->SetValue(item->GetName());
		m_txPgServer->SetValue(item->GetServer());
		m_txPgUserName->SetValue(item->GetUserName());
		m_txPgPassword->SetValue(item->GetPassword());
		m_txPgDatabase->SetValue(item->GetDatabase());
	}
}

void DbSettingDialog::OnPgHistoryDClick(wxCommandEvent& event)
{
}
void DbSettingDialog::OnPgOKUI(wxUpdateUIEvent& event)
{
	event.Enable(!m_txPgServer->IsEmpty() && !m_txPgUserName->IsEmpty());
}

void DbSettingDialog::OnPgRmoveUI(wxUpdateUIEvent& event)
{
	event.Enable(m_listBoxPg->GetSelection() > -1);
}

void DbSettingDialog::OnPgSaveUI(wxUpdateUIEvent& event)
{
	event.Enable(!m_txPgName->IsEmpty());
}
void DbSettingDialog::OnMySqlPassKeyDown(wxKeyEvent& event)
{
	if (event.GetKeyCode() == WXK_RETURN) {
		wxCommandEvent event2;
		OnOkClick(event2);
	} else event.Skip();

}

void DbSettingDialog::OnPgSqlKeyDown(wxKeyEvent& event)
{
	if (event.GetKeyCode() == WXK_RETURN) {
		wxCommandEvent event2;
		OnPgOkClick(event2);
	} else event.Skip();
}

void DbSettingDialog::OnItemActivated(wxListEvent& event)
{
	wxCommandEvent dummy;
	
	long selecteditem = -1;
	selecteditem = m_listCtrlRecentFiles->GetNextItem(selecteditem, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	
	m_filePickerSqlite->SetPath( GetColumnText(m_listCtrlRecentFiles, (int)selecteditem, 0) );
	OnSqliteOkClick(dummy);
}

void DbSettingDialog::OnItemKeyDown(wxListEvent& event)
{
	if(event.GetKeyCode() == WXK_DELETE || event.GetKeyCode() == WXK_NUMPAD_DELETE) {
		m_listCtrlRecentFiles->DeleteItem(event.GetItem());
	} else {
		event.Skip();
	}
}

void DbSettingDialog::OnItemSelected(wxListEvent& event)
{
	long selecteditem = -1;
	selecteditem = m_listCtrlRecentFiles->GetNextItem(selecteditem, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	
	m_filePickerSqlite->SetPath(GetColumnText(m_listCtrlRecentFiles, (int)selecteditem, 0));
}
