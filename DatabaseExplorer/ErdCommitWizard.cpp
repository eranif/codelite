//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : ErdCommitWizard.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include "ErdCommitWizard.h"

#include "res/gui/folder.xpm"
#include "res/gui/form_blue.xpm"
#include "res/gui/form_yellow.xpm"
#include "res/gui/wiztest.xpm"

ErdCommitWizard::ErdCommitWizard(wxWindow* parent, xsSerializable* pConnections, const wxString& createScript): wxWizard(parent,wxID_ANY,_T("ERD commit wizard"),
	        wxBitmap(wiztest_xpm)) {

	m_createScript = createScript;
	m_pConnections = pConnections;
	m_pSelectedDatabase = NULL;
	m_pFirstPage = new FirstPage(this);

	DatabasePage* pDbPage = new DatabasePage(this,pConnections);
	m_pFirstPage->SetNext(pDbPage);
	pDbPage->SetPrev(m_pFirstPage);

	BackupPage* pBackPage = new BackupPage(this);
	pDbPage->SetNext(pBackPage);
	pBackPage->SetPrev(pDbPage);

	WriteStructurePage* pWritePage = new WriteStructurePage(this);
	pBackPage->SetNext(pWritePage);
	pWritePage->SetPrev(pBackPage);

	RestorePage* pRestorePage = new RestorePage(this);
	pWritePage->SetNext(pRestorePage);
	pRestorePage->SetPrev(pWritePage);

	LastPage* pLastPage = new LastPage(this);
	pRestorePage->SetNext(pLastPage);
	pLastPage->SetPrev(pRestorePage);

	GetPageAreaSizer()->Add(m_pFirstPage);
}

ErdCommitWizard::~ErdCommitWizard() {
}

wxWizardPageSimple* ErdCommitWizard::GetFirstPage() {
	return m_pFirstPage;
}


FirstPage::FirstPage(wxWizard* parent):wxWizardPageSimple(parent) {
	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
	mainSizer->Add(
	    new wxStaticText(this, wxID_ANY, wxString(_("Wizard for creating db structure\n")) +
	                     wxString(_("on ERD diagram base.\n\n")) +
	                     wxString(_("All tables which really exist in\n")) +
	                     wxString(_("the database, will be deleted during\n")) +
	                     wxString(_("this process, but you can do a backup\n")) +
	                     wxString(_("in the second step."))),
	    0,wxALL,5);
	SetSizer(mainSizer);
	mainSizer->Fit(this);
}

FirstPage::~FirstPage() {
}



DatabasePage::DatabasePage(ErdCommitWizard* parent, xsSerializable* pConnections):wxWizardPageSimple(parent) {
	m_pParentWizard = parent;
	m_pConnections = pConnections;
	m_mainSizer = new wxFlexGridSizer( 2, 1, 0, 0 );
	m_mainSizer->AddGrowableCol( 0 );
	m_mainSizer->AddGrowableRow( 1 );
	m_mainSizer->SetFlexibleDirection( wxBOTH );
	m_mainSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	m_mainSizer->Add(new wxStaticText(this,wxID_ANY,_("Select target database:")));
	m_treeDatabases = new wxTreeCtrl(this,wxID_ANY,wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxTR_HIDE_ROOT);
	m_mainSizer->Add(m_treeDatabases, 0, wxALL|wxEXPAND, 5 );


	SetSizer(m_mainSizer);
	m_mainSizer->Fit(this);
	LoadDatabases();
}

DatabasePage::~DatabasePage() {

}

void DatabasePage::LoadDatabases() {
	// clear items from tree
	m_treeDatabases->DeleteAllItems();
	// create imageList for icons
	wxImageList *pImageList = new wxImageList(16,16,true,3);
	pImageList->Add(wxIcon(folder_xpm));						// folder icon
	pImageList->Add(wxIcon(form_blue_xpm));						// table icon
	pImageList->Add(wxIcon(form_yellow_xpm));						// view icon
	m_treeDatabases->SetImageList(pImageList);

	wxTreeItemId totalRootID = m_treeDatabases->AddRoot(wxString::Format(_("Databases")),-1);

	// ---------------- load connections ----------------------------
	SerializableList::compatibility_iterator connectionNode = m_pConnections->GetFirstChildNode();
	while(connectionNode) {
		DbConnection* pDbCon = (DbConnection*) wxDynamicCast(connectionNode->GetData(),DbConnection);
		if (pDbCon) {
			wxTreeItemId rootID = m_treeDatabases->AppendItem(totalRootID,wxString::Format(_("Databases (%s)"),pDbCon->GetServerName().c_str()),-1,-1, new DbItem(pDbCon));
			m_treeDatabases->Expand(rootID);
			// ----------------------- load databases -------------------------------
			SerializableList::compatibility_iterator dbNode = pDbCon->GetFirstChildNode();
			while(dbNode) {
				Database* pDatabase = wxDynamicCast(dbNode->GetData(), Database);
				if (pDatabase) {
					//wxTreeItemId dbID = m_treeDatabases->AppendItem(rootID,pDatabase->getName(),-1,-1, new DbItem(pDatabase,NULL));//new DbDatabase(db->getName()));
					m_treeDatabases->AppendItem(rootID,pDatabase->GetName(),-1,-1, new DbItem(pDatabase));//new DbDatabase(db->getName()));

				}
				dbNode = dbNode->GetNext();
			}
			// -----------------------------------------------------------------------
		}
		connectionNode = connectionNode->GetNext();
	}

}

bool DatabasePage::TransferDataFromWindow() {
	DbItem* data = (DbItem*) m_treeDatabases->GetItemData(m_treeDatabases->GetSelection());
	if (data) {
		Database* pdb = wxDynamicCast(data->GetData(), Database);
		if (pdb) {
			m_pParentWizard->SetSelectedDatabase(pdb);
		}
	}
	//TODO:LANG:
	if (!m_pParentWizard->GetSelectedDatabase()) {
		wxMessageBox(_("The first time you have to select a target database!"));
		return false;
	}
	return true;
}
BackupPage::BackupPage(ErdCommitWizard* parent):wxWizardPageSimple(parent) {
	m_pParentWizard = parent;
	backuped = false;

	m_mainSizer = new wxFlexGridSizer(8,1,0,0);
	m_mainSizer->AddGrowableCol(0);
	m_mainSizer->SetFlexibleDirection(wxBOTH);
	m_mainSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_mainSizer->Add(new wxStaticText(this,wxID_ANY, _("Backup data file")));
	m_pFileData = new wxFilePickerCtrl(this,wxID_ANY,_("data.sql"),_("Select file"), wxT("SQL file *.sql|*.sql"),wxDefaultPosition, wxDefaultSize, wxFLP_USE_TEXTCTRL|wxFLP_SAVE|wxFLP_OVERWRITE_PROMPT);
	m_mainSizer->Add(m_pFileData, 0, wxALL|wxEXPAND, 5 );
	m_mainSizer->Add(new wxStaticLine(this), 0, wxALL|wxEXPAND, 5 );
	m_pCheckStructure = new wxCheckBox(this, wxID_ANY,_("Backup database structure"));
	m_mainSizer->Add(m_pCheckStructure, 0, wxALL|wxEXPAND, 5 );
	m_mainSizer->Add(new wxStaticText(this,wxID_ANY, _("Backup structure file")));
	m_pFileStructure = new wxFilePickerCtrl(this,wxID_ANY,_("structure.sql"),_("Select file"), wxT("SQL file *.sql|*.sql"),wxDefaultPosition, wxDefaultSize, wxFLP_USE_TEXTCTRL|wxFLP_SAVE|wxFLP_OVERWRITE_PROMPT);
	m_mainSizer->Add(m_pFileStructure, 0, wxALL|wxEXPAND, 5 );
	m_mainSizer->Add(new wxStaticLine(this), 0, wxALL|wxEXPAND, 5 );

	m_btnBackup = new wxButton(this, wxID_ANY, _("Backup!"));
	m_mainSizer->Add(m_btnBackup,0,wxALIGN_CENTER_HORIZONTAL,0);

	SetSizer(m_mainSizer);
	m_mainSizer->Fit(this);

	m_pFileStructure->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(BackupPage::OnFileStructureUI),NULL,this);
	m_btnBackup->Connect(wxEVT_UPDATE_UI,wxUpdateUIEventHandler(BackupPage::OnBtnBackupUI),NULL,this);
	m_btnBackup->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(BackupPage::OnBtnBackupClick),NULL,this);

}

bool BackupPage::TransferDataFromWindow() {
	if (!backuped) {
		wxMessageDialog dlg(this, _("Backing up database data before changing the structure is really good idea. Do you want to continue without doing so?"),_("Backup"),wxYES_NO);
		if (dlg.ShowModal() ==  wxID_YES) {
			return true;
		} else return false;
	}
	return true;
}

BackupPage::~BackupPage() {
}

void BackupPage::OnFileStructureUI(wxUpdateUIEvent& event) {
	if (m_pCheckStructure) {
		event.Enable(m_pCheckStructure->IsChecked());
	} else event.Enable(false);
}

void BackupPage::OnBtnBackupClick(wxCommandEvent& event) {
	m_pParentWizard->GetSelectedDatabase()->RefreshChildrenDetails();
	wxString dumpResult = wxT("");
	DumpClass* pDump = new DumpClass(m_pParentWizard->GetSelectedDatabase()->GetDbAdapter(), m_pParentWizard->GetSelectedDatabase(), m_pFileData->GetPath());
	if (pDump) dumpResult = pDump->DumpData();

	wxMessageBox(_("Data saved! ")+ dumpResult );

	if (m_pCheckStructure->IsChecked()) {
		wxString retStr;


		SerializableList::compatibility_iterator tabNode = m_pParentWizard->GetSelectedDatabase()->GetFirstChildNode();
		while(tabNode) {
			Table* tab = wxDynamicCast(tabNode->GetData(),Table);
			if (tab) {
				retStr.append(m_pParentWizard->GetSelectedDatabase()->GetDbAdapter()->GetCreateTableSql(tab,true));
			}
			tabNode = tabNode->GetNext();
		}

		tabNode = m_pParentWizard->GetSelectedDatabase()->GetFirstChildNode();
		while(tabNode) {
			View* view = wxDynamicCast(tabNode->GetData(),View);
			if (view) {
				retStr.append(m_pParentWizard->GetSelectedDatabase()->GetDbAdapter()->GetCreateViewSql(view,true));
			}
			tabNode = tabNode->GetNext();
		}

		tabNode = m_pParentWizard->GetSelectedDatabase()->GetFirstChildNode();
		while(tabNode) {
			Table* tab = wxDynamicCast(tabNode->GetData(),Table);
			if (tab) {
				retStr.append(m_pParentWizard->GetSelectedDatabase()->GetDbAdapter()->GetAlterTableConstraintSql(tab));
			}
			tabNode = tabNode->GetNext();
		}

		wxTextFile pTextFile(m_pFileStructure->GetPath());

		if (pTextFile.Exists()) {
			pTextFile.Open();
			pTextFile.Clear();
		} else {
			pTextFile.Create();
			pTextFile.Open();
		}
		if (pTextFile.IsOpened()) {
			pTextFile.AddLine(retStr);
			pTextFile.Write(wxTextFileType_None, wxConvUTF8);
			pTextFile.Close();
			wxMessageBox(_("Structure saved!"));
		}

	}
	backuped = true;
}

void BackupPage::OnBtnBackupUI(wxUpdateUIEvent& event) {
	if (m_pCheckStructure->IsChecked()) {
		event.Enable((!m_pFileData->GetPath().IsEmpty())&&(!m_pFileStructure->GetPath().IsEmpty()));
	} else event.Enable(!m_pFileData->GetPath().IsEmpty());
}

bool BackupPage::TransferDataToWindow() {
	backuped = false;
	return true;
}

WriteStructurePage::WriteStructurePage(ErdCommitWizard* parent):wxWizardPageSimple(parent) {
	m_pParentWizard = parent;
	m_mainSizer = new wxFlexGridSizer(1,0,0);
	m_mainSizer->AddGrowableCol(0);
	m_mainSizer->AddGrowableRow(1);
	m_mainSizer->SetFlexibleDirection(wxBOTH);
	m_mainSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_mainSizer->Add(new wxStaticText(this, wxID_ANY,_("Write log:")));

	m_txLog = new wxTextCtrl(this, wxID_ANY,wxT(""),wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);
	m_mainSizer->Add(m_txLog,0,wxEXPAND,5);
	m_mainSizer->Add(new wxStaticLine(this),0, wxEXPAND,2);

	wxBoxSizer* pSizer = new wxBoxSizer(wxHORIZONTAL);
	m_btnWrite = new wxButton(this, wxID_ANY,_("Write !!"));
	m_btnShowSql = new wxButton(this, wxID_ANY, _("Show SQL"));

	pSizer->Add(m_btnWrite, wxALIGN_CENTER_HORIZONTAL);
	pSizer->Add(m_btnShowSql, wxALIGN_CENTER_HORIZONTAL);

	m_mainSizer->Add(pSizer,0,wxEXPAND,0);

	SetSizer(m_mainSizer);
	m_mainSizer->Fit(this);

	m_btnWrite->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(WriteStructurePage::OnBtnWriteClick),NULL, this);
	m_btnShowSql->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(WriteStructurePage::OnBtnShowSqlClick), NULL, this);


}

WriteStructurePage::~WriteStructurePage() {
}

void WriteStructurePage::OnBtnShowSqlClick(wxCommandEvent& event) {
	CodePreviewDialog dlg(this, m_pParentWizard->GetCreateScript());
	dlg.ShowModal();
}

void WriteStructurePage::OnBtnWriteClick(wxCommandEvent& event) {
	DatabaseLayerPtr pDbLayer(NULL);
	try {
		pDbLayer = m_pParentWizard->GetSelectedDatabase()->GetDbAdapter()->GetDatabaseLayer(m_pParentWizard->GetSelectedDatabase()->GetName());
		if (pDbLayer) {
			pDbLayer->BeginTransaction();

			wxString useSql = m_pParentWizard->GetSelectedDatabase()->GetDbAdapter()->GetUseDb(m_pParentWizard->GetSelectedDatabase()->GetName());
			if (!useSql.IsEmpty()) pDbLayer->RunQuery(wxString::Format(wxT("USE %s"), m_pParentWizard->GetSelectedDatabase()->GetName().c_str()));


			pDbLayer->RunQuery(m_pParentWizard->GetCreateScript());

			pDbLayer->Commit();
			pDbLayer->Close();
			m_txLog->SetValue(_("Data structure written successfully!"));
			commited = true;
		}

	} catch (DatabaseLayerException& e) {
		if (pDbLayer) {
			pDbLayer->RollBack();
			pDbLayer->Close();
		}
		wxString errorMessage = wxString::Format(_("Error (%d): %s"), e.GetErrorCode(), e.GetErrorMessage().c_str());
		m_txLog->SetValue(errorMessage);
	} catch( ... ) {
		if (pDbLayer) {
			pDbLayer->RollBack();
			pDbLayer->Close();
		}
		m_txLog->SetValue(_("Unknown error!"));
	}
}

bool WriteStructurePage::TransferDataFromWindow() {
	if (!commited) {
		wxMessageDialog dlg(this, _("Would you like to continue without writing the database structure?"),_("Question"),wxYES_NO);
		if (dlg.ShowModal() ==  wxID_YES) {
			return true;
		} else return false;
	}
	return true;
}

bool WriteStructurePage::TransferDataToWindow() {
	commited = false;
	return true;
}

LastPage::LastPage(wxWizard* parent):wxWizardPageSimple(parent) {
	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
	mainSizer->Add(
	    new wxStaticText(this, wxID_ANY, _("Writing structure ended.\n")),
	    0,wxALL,5);
	SetSizer(mainSizer);
	mainSizer->Fit(this);
}

LastPage::~LastPage() {
}

RestorePage::RestorePage(ErdCommitWizard* parent):wxWizardPageSimple(parent) {
	m_pParentWizard = parent;
	m_mainSizer = new wxFlexGridSizer(1, 0,0);
	m_mainSizer->SetFlexibleDirection(wxBOTH);
	m_mainSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	m_mainSizer->AddGrowableCol(0);
	m_mainSizer->AddGrowableRow(4);


	m_mainSizer->Add(new wxStaticText(this, wxID_ANY, _("File for data restore:")), 0, wxEXPAND, 2);

	m_restoreFile = new wxFilePickerCtrl(this,wxID_ANY,wxT(""),_("Select file"), wxT("SQL file *.sql|*.sql"),wxDefaultPosition, wxDefaultSize, wxFLP_USE_TEXTCTRL|wxFLP_OPEN|wxFLP_FILE_MUST_EXIST);
	m_mainSizer->Add(m_restoreFile, 0, wxEXPAND, 2);
	m_mainSizer->Add(new wxStaticLine(this),0, wxEXPAND,2);

	m_mainSizer->Add(new wxStaticText(this, wxID_ANY, _("Restore log:")), 0, wxEXPAND, 2);

	m_txLog = new wxTextCtrl(this, wxID_ANY, wxT(""),wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);
	m_mainSizer->Add(m_txLog,0,wxEXPAND,2 );
	m_mainSizer->Add(new wxStaticLine(this),0, wxEXPAND,2);

	m_btnRestore = new wxButton(this,wxID_ANY,_("Restore"));
	m_mainSizer->Add(m_btnRestore,0, wxALIGN_CENTER_HORIZONTAL, 5);

	SetSizer(m_mainSizer);
	m_mainSizer->Fit(this);

	m_btnRestore->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(RestorePage::OnBtnRestoreUI), NULL, this);
	m_btnRestore->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(RestorePage::OnBtnRestoreClick), NULL, this);

}

RestorePage::~RestorePage() {
}
void RestorePage::OnBtnRestoreClick(wxCommandEvent& event)
{
	if (m_restoreFile->GetPath().IsEmpty()) return;

	DatabaseLayerPtr pDbLayer(NULL);
	Database* pDb = m_pParentWizard->GetSelectedDatabase();
	Clear();
	try {
		wxFileInputStream input(m_restoreFile->GetPath());
		wxTextInputStream text( input, wxT(";"), wxConvUTF8 );
		wxString command = wxT("");

		pDbLayer = pDb->GetDbAdapter()->GetDatabaseLayer(pDb->GetName());
		pDbLayer->BeginTransaction();

		wxString useSql = pDb->GetDbAdapter()->GetUseDb(pDb->GetName());
		if (!useSql.IsEmpty()) pDbLayer->RunQuery(wxString::Format(wxT("USE %s"), pDb->GetName().c_str()));

		while (!input.Eof()) {
			wxString line = text.ReadLine();
			//dialog.AppendText(line);
			int index = line.Find(wxT("--"));
			if (index != wxNOT_FOUND) line = line.Mid(0,index);
			command += line;
			if (line.Find(wxT(";")) != wxNOT_FOUND) {
				AppendSeparator();
				AppendComment(wxT("Run SQL command:"));
				AppendText(command);
				pDbLayer->RunQuery(command);
				AppendComment(_("Successful!"));
				command.Clear();
			}
		}
		pDbLayer->Commit();
		pDbLayer->Close();
	} catch (DatabaseLayerException& e) {
		if (pDbLayer) {
			pDbLayer->RollBack();
			pDbLayer->Close();
		}
		wxString errorMessage = wxString::Format(_("Error (%d): %s"), e.GetErrorCode(), e.GetErrorMessage().c_str());

		AppendComment(_("Fail!"));
		AppendComment(errorMessage);
		wxMessageDialog dlg(this,errorMessage,_("DB Error"),wxOK | wxCENTER | wxICON_ERROR);
		dlg.ShowModal();
	} catch( ... ) {
		if (pDbLayer) {
			pDbLayer->RollBack();
			pDbLayer->Close();
		}
		wxMessageDialog dlg(this,_("Unknown error."),_("DB Error"),wxOK | wxCENTER | wxICON_ERROR);
		dlg.ShowModal();
	}

}

void RestorePage::OnBtnRestoreUI(wxUpdateUIEvent& event)
{
	event.Enable(!m_restoreFile->GetPath().IsEmpty());
}
void RestorePage::AppendText(const wxString& txt)
{
	m_text.Append(txt + wxT("\n"));
	m_txLog->SetValue(m_text);
}

void RestorePage::Clear()
{
	m_text.clear();
	m_txLog->SetValue(m_text);
}

void RestorePage::AppendSeparator()
{
	m_text.Append(wxT("*********************************************************\n"));
	m_txLog->SetValue(m_text);
}

void RestorePage::AppendComment(const wxString& txt)
{
	m_text.Append(wxNow() + wxT(" ") + txt + wxT("\n"));
	m_txLog->SetValue(m_text);
}
