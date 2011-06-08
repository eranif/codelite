#include "DbViewerPanel.h"
#include "DbSettingDialog.h"
#include "SqlCommandPanel.h"
#include "ErdPanel.h"
#include "AdapterSelectDlg.h"
#include <wx/wupdlock.h>

#include <wx/wfstream.h>
#include <wx/imaglist.h>

DbViewerPanel::DbViewerPanel(wxWindow *parent, wxWindow* notebook, IManager* pManager)
	: _DbViewerPanel(parent)
{
	m_pNotebook = notebook;
	m_pGlobalParent = parent;
	m_mgr = pManager;

	m_pDbAdapter = NULL;
	m_pConnections = new xsSerializable();

	m_pThumbnail = new wxSFThumbnail(m_panelThumb);
	m_thmSizer->Add(m_pThumbnail, 1, wxEXPAND, 0);
	m_thmSizer->Layout();
	
	// replace the icons...
	m_toolBar1->FindById(wxID_CONNECT)->SetNormalBitmap(pManager->GetStdIcons()->LoadBitmap(wxT("db-explorer/16/connect")));
	m_toolBar1->FindById(wxID_CLOSE_CONNECTION)->SetNormalBitmap(pManager->GetStdIcons()->LoadBitmap(wxT("db-explorer/16/disconnect")));
	m_toolBar1->FindById(wxID_TOOL_REFRESH)->SetNormalBitmap(pManager->GetStdIcons()->LoadBitmap(wxT("db-explorer/16/database_refresh")));
	m_toolBar1->Realize();
	
	Layout();


	m_mgr->GetTheApp()->Connect(wxEVT_COMMAND_BOOK_PAGE_CHANGED,NotebookEventHandler(DbViewerPanel::OnPageChange), NULL, this);
	m_mgr->GetTheApp()->Connect(wxEVT_COMMAND_BOOK_PAGE_CLOSING,NotebookEventHandler(DbViewerPanel::OnPageClose), NULL, this);
}

DbViewerPanel::~DbViewerPanel()
{
	m_mgr->GetTheApp()->Disconnect(wxEVT_COMMAND_BOOK_PAGE_CHANGED,NotebookEventHandler(DbViewerPanel::OnPageChange), NULL, this);
	m_mgr->GetTheApp()->Disconnect(wxEVT_COMMAND_BOOK_PAGE_CLOSING,NotebookEventHandler(DbViewerPanel::OnPageClose), NULL, this);
	delete m_pDbAdapter;
}

void DbViewerPanel::OnConncectClick(wxCommandEvent& event)
{
	DbSettingDialog dlg(this, m_mgr->GetTheApp()->GetTopWindow());
	dlg.ShowModal();
	RefreshDbView();
}

void DbViewerPanel::OnConncectUI(wxUpdateUIEvent& event)
{
	event.Enable( !m_pConnections->HasChildren() );
}

void DbViewerPanel::OnItemActivate(wxTreeEvent& event)
{
	DbItem* item = (DbItem*) m_treeDatabases->GetItemData(event.GetItem());
	if (item) {
		wxMouseState cState = wxGetMouseState();
		
		wxString pagename;
		if (Table* tab =  wxDynamicCast(item->GetData(), Table)) {
			tab->RefreshChildren();
			if( cState.ControlDown() ) {
				pagename = CreatePanelName(tab, DbViewerPanel::Erd);
				m_mgr->AddEditorPage(new ErdPanel(m_pNotebook,tab->GetDbAdapter()->Clone(),m_pConnections, (Table*) tab->Clone() ), pagename);
			} else {
				wxWindowUpdateLocker locker(m_mgr->GetEditorPaneNotebook());
				pagename = CreatePanelName(tab, DbViewerPanel::Sql);
				m_mgr->AddEditorPage(new SQLCommandPanel(m_pNotebook,tab->GetDbAdapter()->Clone(),tab->GetParentName(),tab->GetName()), pagename);

			}
		}

		if (View* pView = wxDynamicCast(item->GetData(), View)) {
			pagename = CreatePanelName(pView, DbViewerPanel::Sql);
			m_mgr->AddEditorPage(new SQLCommandPanel(m_pNotebook,pView->GetDbAdapter()->Clone(),pView->GetParentName(),pView->GetName()), pagename);
		}

		if (Database* db = wxDynamicCast(item->GetData(), Database)) {
			if( cState.ControlDown() ) {
				db->RefreshChildrenDetails();
				pagename = CreatePanelName(db, DbViewerPanel::Erd);
				m_mgr->AddEditorPage(new ErdPanel(m_pNotebook,db->GetDbAdapter()->Clone(),m_pConnections,(Database*)db->Clone()), pagename);
			} else {
				pagename = CreatePanelName(db, DbViewerPanel::Sql);
				m_mgr->AddEditorPage(new SQLCommandPanel(m_pNotebook,db->GetDbAdapter()->Clone(),db->GetName(),wxT("")), pagename);
			}
		}
		m_pagesAdded.Add(pagename);
	}
}

void DbViewerPanel::OnRefreshClick(wxCommandEvent& event)
{

	DbItem* data = (DbItem*) m_treeDatabases->GetItemData(m_treeDatabases->GetSelection());
	if (data) {
		DbConnection* pCon = wxDynamicCast(data->GetData(), DbConnection);
		if (pCon) pCon->RefreshChildren();
		Database* pDb = wxDynamicCast(data->GetData(),Database);
		if (pDb) pDb->RefreshChildren(false);
		Table* pTab = wxDynamicCast(data->GetData(), Table);
		if (pTab) pTab->RefreshChildren();

	}

	RefreshDbView();
}
void DbViewerPanel::RefreshDbView()
{
	// clear items from tree
	m_treeDatabases->DeleteAllItems();
	// create imageList for icons
	wxImageList *pImageList = new wxImageList(16, 16, true,3);
	pImageList->Add(m_mgr->GetStdIcons()->LoadBitmap(wxT("toolbars/16/standard/file_open"))); // folder icon
	pImageList->Add(wxBitmap(Grid_xpm)); // table icon
	pImageList->Add(m_mgr->GetStdIcons()->LoadBitmap(wxT("toolbars/16/search/find")));        // view icon
	pImageList->Add(wxBitmap(database_xpm)); // database

	m_treeDatabases->AssignImageList(pImageList);

	wxTreeItemId totalRootID = m_treeDatabases->AddRoot(wxString::Format(wxT("Databases")),-1);

	// ---------------- load connections ----------------------------
	SerializableList::compatibility_iterator connectionNode = m_pConnections->GetFirstChildNode();
	while(connectionNode) {
		DbConnection* pDbCon = (DbConnection*) wxDynamicCast(connectionNode->GetData(),DbConnection);
		if (pDbCon) {
			wxTreeItemId rootID = m_treeDatabases->AppendItem(totalRootID,
			                      wxString::Format(wxT("Databases (%s)"),pDbCon->GetServerName().c_str()),
			                      3,
			                      3,
			                      new DbItem(pDbCon));

			// ----------------------- load databases -------------------------------
			SerializableList::compatibility_iterator dbNode = pDbCon->GetFirstChildNode();
			while(dbNode) {
				Database* pDatabase = wxDynamicCast(dbNode->GetData(), Database);
				if (pDatabase) {
					//wxTreeItemId dbID = m_treeDatabases->AppendItem(rootID,pDatabase->getName(),-1,-1, new DbItem(pDatabase,NULL));//new DbDatabase(db->getName()));
					wxTreeItemId dbID = m_treeDatabases->AppendItem(rootID,
					                    pDatabase->GetName(),
					                    3,
					                    3,
					                    new DbItem(pDatabase));
					m_treeDatabases->Expand(rootID);
					wxTreeItemId idFolder = m_treeDatabases->AppendItem(dbID, wxT("Tables"),0,0,NULL);
					//m_treeDatabases->Expand(dbID);

					// ----------------------------- load tables ----------------------------------
					SerializableList::compatibility_iterator tabNode = pDatabase->GetFirstChildNode();
					while(tabNode) {
						Table* pTable = wxDynamicCast(tabNode->GetData(), Table);
						if (pTable) {
							//wxTreeItemId tabID = m_treeDatabases->AppendItem(idFolder,pTable->getName(),1,-1,new DbItem(NULL,pTable)); //NULL);
							wxTreeItemId tabID = m_treeDatabases->AppendItem(idFolder,
							                     pTable->GetName(),
							                     1,
							                     1,
							                     new DbItem(pTable)); //NULL);
						}
						tabNode = tabNode->GetNext();
					}
					// ----------------------------------------------------------------------------


					idFolder = m_treeDatabases->AppendItem(dbID, wxT("Views"),0,0,NULL);
					//m_treeDatabases->Expand(dbID);

					// ----------------------------- load views ----------------------------------
					tabNode = pDatabase->GetFirstChildNode();
					while(tabNode) {
						View* pView = wxDynamicCast(tabNode->GetData(),View);
						if (pView) {
							//wxTreeItemId tabID = m_treeDatabases->AppendItem(idFolder,pTable->getName(),1,-1,new DbItem(NULL,pTable)); //NULL);
							wxTreeItemId tabID = m_treeDatabases->AppendItem(idFolder,
							                     pView->GetName(),
							                     2,
							                     2,
							                     new DbItem(pView)); //NULL);
						}
						tabNode = tabNode->GetNext();
					}
					// ----------------------------------------------------------------------------

				}
				dbNode = dbNode->GetNext();
			}
			// -----------------------------------------------------------------------
		}
		connectionNode = connectionNode->GetNext();
	}

}

void DbViewerPanel::OnItemSelectionChange(wxTreeEvent& event)
{
	wxUnusedVar(event);
}

void DbViewerPanel::OnERDClick(wxCommandEvent& event)
{
	AdapterSelectDlg dlg(m_mgr->GetTheApp()->GetTopWindow(), m_pNotebook, m_mgr, m_pConnections);
	dlg.ShowModal();
}

void DbViewerPanel::OnDnDStart(wxTreeEvent& event)
{
	ShapeList lstDnD;
	lstDnD.DeleteContents(true);
	DbItem* item = (DbItem*) m_treeDatabases->GetItemData(event.GetItem());

	ErdPanel* pPanel = wxDynamicCast(m_mgr->GetActivePage(),ErdPanel);

	if (item != NULL) {
		if (pPanel) {
			Table* table = wxDynamicCast(item->GetData(),Table);
			if (table) {
				if (table->GetChildrenList().GetCount() == 0) table->RefreshChildren();
				table = (Table*) table->Clone();
				wxSFShapeBase *pShape = new dndTableShape(table);
				lstDnD.Append(pShape);
				pPanel->getCanvas()->DoDragDrop(lstDnD);
			}

			View* view = wxDynamicCast(item->GetData(),View);
			if (view) {
				view = (View*) view->Clone();
				wxSFShapeBase *pShape = new dndTableShape(view);
				lstDnD.Append(pShape);
				pPanel->getCanvas()->DoDragDrop(lstDnD);
			}
		}
		//delete pShape;
	}
}
void DbViewerPanel::OnItemRightClick(wxTreeEvent& event)
{
	//TODO:LANG:nekolikrat
	m_pEditedDatabase = NULL;
	m_pEditedConnection = NULL;
	m_selectedID = event.GetItem();

	DbItem* item = (DbItem*) m_treeDatabases->GetItemData(m_selectedID);
	wxMenu menu;
	int c = 0;
	if (item) {
		Database* db = wxDynamicCast(item->GetData(),Database);
		if (db) {
			menu.Append(IDR_DBVIEWER_ADD_TABLE,wxT("Add table"),wxT("Run SQL command for creating Table"));
			menu.Append(IDR_DBVIEWER_DROP_DATABASE, wxT("Drop database"), wxT("Run SQL command for deleting Database"));
			c++;
			c++;
			menu.AppendSeparator();
			menu.Append(IDR_DBVIEWER_ERD_DB, wxT("Create ERD from DB"),wxT("Create ERD diagram from database"));
			menu.Append(IDR_DBVIEWER_CLASS_DB, wxT("Create classes from DB"), wxT("Create c++ classes for selected database"));
			c++;
			c++;
			menu.AppendSeparator();
			menu.Append(IDR_DBVIEWER_IMPORT_DATABASE, wxT("Import database from file"), wxT("Run SQL commands stored in *.sql file"));
			menu.Append(IDR_DBVIEWER_EXPORT_DATABASE, wxT("Export database to file"), wxT("Export database CREATE SQL statements into *.sql file"));
			c++;
			c++;

			menu.AppendSeparator();
			menu.Append(IDR_DBVIEWER_DUMP_DATABASE, wxT("Dump data to file"), wxT("Dump data from database into .sql file"));
			c++;

			m_pEditedDatabase = db;
		}

		DbConnection* con = wxDynamicCast(item->GetData(), DbConnection);
		if (con) {
			menu.Append(IDR_DBVIEWER_ADD_DATABASE, wxT("Add database"),wxT("Run SQL command for create DB"));

			c++;

			m_pEditedConnection = con;
		}

		Table* tab = wxDynamicCast(item->GetData(), Table);
		if (tab) {
			menu.Append(IDR_DBVIEWER_DROP_TABLE,wxT("Drop table"),wxT("Run SQL command for deleting Table"));
			c++;
			menu.AppendSeparator();
			menu.Append(IDR_DBVIEWER_ERD_TABLE, wxT("Create ERD from Table"),wxT("Create ERD diagram from table"));
			menu.Append(IDR_DBVIEWER_CLASS_TABLE, wxT("Create classes from Table"), wxT("Create c++ classes for selected table"));
			c++;
			c++;
		}
	}

	if ( c > 0 ) {
		menu.Connect(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&DbViewerPanel::OnPopupClick, NULL, this);
		PopupMenu(&menu);
	}


}
void DbViewerPanel::OnToolCloseClick(wxCommandEvent& event)
{
	// Close the active connection (there is only one)
	// getting selected item data
	wxTreeItemIdValue cookie;
	DbItem* data = (DbItem*) m_treeDatabases->GetItemData(m_treeDatabases->GetFirstChild(m_treeDatabases->GetRootItem(), cookie));
	if (data) {
		DbConnection* pCon = wxDynamicCast(data->GetData(), DbConnection);
		if (pCon) {
			wxMessageDialog dlg(this,wxT("Close connection?"),wxT("Close"),wxYES_NO);
			if (dlg.ShowModal() == wxID_YES) {
				m_pConnections->GetChildrenList().DeleteContents(true);
				m_pConnections->GetChildrenList().DeleteObject(pCon);
				RefreshDbView();
			}
		}
	}
	
	// loop over the editor open pages and close all DbExplorer related
	for(size_t i=0; i<m_pagesAdded.Count(); i++) {
		m_mgr->ClosePage(m_pagesAdded.Item(i));
	}
	
	m_pagesAdded.Clear();
}

void DbViewerPanel::OnToolCloseUI(wxUpdateUIEvent& event)
{
	event.Enable(m_pConnections->HasChildren());
}

void DbViewerPanel::OnPopupClick(wxCommandEvent& evt)
{
	if( !m_selectedID.IsOk() ) return;

	try {
		if (evt.GetId() == IDR_DBVIEWER_ADD_DATABASE) {
			if (m_pEditedConnection) {
				//TODO:LANG:
				wxString dbName = wxGetTextFromUser(wxT("Database name"), wxT("Add database"));
				if (!dbName.IsEmpty()) {
					DatabaseLayer* pDbLayer = m_pEditedConnection->GetDbAdapter()->GetDatabaseLayer(wxT(""));
					wxString sql = m_pEditedConnection->GetDbAdapter()->GetCreateDatabaseSql(dbName);
					if (!sql.empty()) {

						pDbLayer->RunQuery(sql);
						pDbLayer->Close();
						delete pDbLayer;
						//TODO:LANG:
						wxMessageBox(wxT("Database created successfully"));

						m_pEditedConnection->RefreshChildren();
						RefreshDbView();
					} else {
						//TODO:LANG:
						wxMessageDialog dlg(this, wxT("Can't create new db in this database engine!"), wxT("Error"),wxOK|wxICON_ERROR);
						dlg.ShowModal();
					}
				}
			}
		} else if (evt.GetId() == IDR_DBVIEWER_DROP_DATABASE) {
			DbItem* data = (DbItem*) m_treeDatabases->GetItemData(m_selectedID);
			if (data) {
				Database* pDb = (Database*) wxDynamicCast(data->GetData(),Database);
				if (pDb) {
					wxString dropSQL = pDb->GetDbAdapter()->GetDropDatabaseSql(pDb);
					if (!dropSQL.IsEmpty()) {
						//TODO:LANG:
						wxMessageDialog dlg(this, wxString::Format(wxT("Remove database '%s'?"),pDb->GetName().c_str()),wxT("Drop database"),wxYES_NO);
						if (dlg.ShowModal() == wxID_YES) {
							DatabaseLayer* pDbLayer = pDb->GetDbAdapter()->GetDatabaseLayer(wxT(""));
							pDbLayer->RunQuery(dropSQL);
							pDbLayer->Close();
							delete pDbLayer;
							//TODO:LANG:
							wxMessageBox(wxT("Database dropped successfully"));

							DbConnection* pCon = wxDynamicCast(pDb->GetParent(), DbConnection);
							if (pCon) pCon->RefreshChildren();
							RefreshDbView();
						}
					}
				}
			}
		} else if (evt.GetId() == IDR_DBVIEWER_ERD_TABLE) {
			DbItem* data = (DbItem*) m_treeDatabases->GetItemData(m_selectedID);
			if (data) {
				Table* pTab = (Table*) wxDynamicCast(data->GetData(),Table);
				if (pTab) {
					pTab->RefreshChildren();
					wxString pagename;
					pagename = CreatePanelName(pTab, DbViewerPanel::Erd);
					m_mgr->AddEditorPage(new ErdPanel(m_pNotebook,pTab->GetDbAdapter()->Clone(),m_pConnections, (Table*) pTab->Clone() ), pagename);
					m_pagesAdded.Add(pagename);
				}
			}
		} else if (evt.GetId() == IDR_DBVIEWER_ERD_DB) {
			DbItem* data = (DbItem*) m_treeDatabases->GetItemData(m_selectedID);
			if (data) {
				Database* pDb = (Database*) wxDynamicCast(data->GetData(),Database);
				if (pDb) {
					pDb->RefreshChildrenDetails();
					wxString pagename;
					pagename = CreatePanelName(pDb, DbViewerPanel::Erd);
					m_mgr->AddEditorPage(new ErdPanel(m_pNotebook,pDb->GetDbAdapter()->Clone(),m_pConnections, (Database*) pDb->Clone() ), pagename);
					m_pagesAdded.Add(pagename);
				}
			}
		} else if (evt.GetId() == IDR_DBVIEWER_CLASS_DB) {
			DbItem* data = (DbItem*) m_treeDatabases->GetItemData(m_selectedID);
			if (data) {
				Database* pDb = (Database*) wxDynamicCast(data->GetData(),Database);
				if (pDb) {
					pDb = (Database*) pDb->Clone();
					pDb->RefreshChildren(true);
					pDb->RefreshChildrenDetails();
					ClassGenerateDialog dlg(m_mgr->GetTheApp()->GetTopWindow(), pDb->GetDbAdapter(), (Database*) pDb->Clone(),m_mgr);
					dlg.ShowModal();
				}
			}
		} else if (evt.GetId() == IDR_DBVIEWER_CLASS_TABLE) {
			DbItem* data = (DbItem*) m_treeDatabases->GetItemData(m_selectedID);
			if (data) {
				Table* pTab = (Table*) wxDynamicCast(data->GetData(),Table);
				if (pTab) {
					pTab->RefreshChildren();
					ClassGenerateDialog dlg(GetParent(), pTab->GetDbAdapter(), (Table*) pTab->Clone(), m_mgr);
					dlg.ShowModal();
				}
			}
		} else if (evt.GetId() ==  IDR_DBVIEWER_DROP_TABLE) {
			DbItem* data = (DbItem*) m_treeDatabases->GetItemData(m_selectedID);
			if (data) {
				Table* pTab = (Table*) wxDynamicCast(data->GetData(),Table);
				if (pTab) {
					//TODO:LANG:
					wxMessageDialog dlg(this, wxString::Format(wxT("Remove table '%s'?"),pTab->GetName().c_str()),wxT("Drop table"),wxYES_NO);
					if (dlg.ShowModal() == wxID_YES) {
						DatabaseLayer* pDbLayer = pTab->GetDbAdapter()->GetDatabaseLayer(pTab->GetParentName());
						pDbLayer->RunQuery(pTab->GetDbAdapter()->GetDropTableSql(pTab));
						pDbLayer->Close();
						delete pDbLayer;
						//TODO:LANG:
						wxMessageBox(wxT("Table dropped successfully"));

						Database* pDb = wxDynamicCast(pTab->GetParent(), Database);
						if (pDb) pDb->RefreshChildren(false);
						RefreshDbView();
					}
				}
			}
		} else if (evt.GetId() == IDR_DBVIEWER_IMPORT_DATABASE) {
			DbItem* data = (DbItem*) m_treeDatabases->GetItemData(m_selectedID);
			if (data) {
				Database* pDb = (Database*) wxDynamicCast(data->GetData(),Database);
				if (pDb) {
					//TODO:LANG:
					wxFileDialog dlg(this, wxT("Import database from SQL file ..."), wxGetCwd(), wxT(""), wxT("SQL Files (*.sql)|*.sql"), wxOPEN | wxFILE_MUST_EXIST);
					if(dlg.ShowModal() == wxID_OK) {
						ImportDb(dlg.GetPath(), pDb);
					}
					pDb->RefreshChildren(false);
				}
			}
			RefreshDbView();
		} else if (evt.GetId() == IDR_DBVIEWER_DUMP_DATABASE) {
			DbItem* data = (DbItem*) m_treeDatabases->GetItemData(m_selectedID);
			if (data) {
				Database* pDb = (Database*) wxDynamicCast(data->GetData(),Database);
				if (pDb) {
					pDb->RefreshChildrenDetails();
					//TODO:LANG:
					wxFileDialog dlg(this, wxT("Dump data into file ..."),wxT(""), pDb->GetName() + wxT(".sql"),wxT("SQL files (*.sql)|*.sql"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
					if( dlg.ShowModal() == wxID_OK ) {
						DumpClass* dump = new DumpClass(pDb->GetDbAdapter(),pDb, dlg.GetPath());
						dump->DumpData();
						wxMessageBox(wxT("Data was saved to ") + dlg.GetPath() );
					}
				}

			}
		} else if (evt.GetId() == IDR_DBVIEWER_EXPORT_DATABASE) {
			DbItem* data = (DbItem*) m_treeDatabases->GetItemData(m_selectedID);
			if (data) {
				Database* pDb = (Database*) wxDynamicCast(data->GetData(),Database);
				if (pDb) {
					pDb->RefreshChildrenDetails();

					wxFileDialog dlg(this, wxT("Export database..."), wxGetCwd(), wxT(""), wxT("SQL Files (*.sql)|*.sql"), wxSAVE | wxFD_OVERWRITE_PROMPT);

					if(dlg.ShowModal() == wxID_OK) {
						// CreateStructure
						wxString retStr = wxT("-- SQL script created by wxDbExplorer\n\n ");
						SerializableList::compatibility_iterator tabNode = pDb->GetFirstChildNode();
						while(tabNode) {
							Table* tab = wxDynamicCast(tabNode->GetData(),Table);
							if (tab) {
								retStr.append(pDb->GetDbAdapter()->GetCreateTableSql(tab,true));
							}
							tabNode = tabNode->GetNext();
						}

						tabNode = pDb->GetFirstChildNode();
						while(tabNode) {
							View* view = wxDynamicCast(tabNode->GetData(),View);
							if (view) {
								retStr.append(pDb->GetDbAdapter()->GetCreateViewSql(view,true));
							}
							tabNode = tabNode->GetNext();
						}

						tabNode = pDb->GetFirstChildNode();
						while(tabNode) {
							Table* tab = wxDynamicCast(tabNode->GetData(),Table);
							if (tab) {
								retStr.append(pDb->GetDbAdapter()->GetAlterTableConstraintSql(tab));
							}
							tabNode = tabNode->GetNext();
						}

						DumpClass dump(pDb->GetDbAdapter(), pDb, dlg.GetPath());
						dump.DumpData();


						wxTextFile file(dlg.GetPath());
						if (!file.Exists()) file.Create();
						file.Open();
						if (file.IsOpened()) {
							file.InsertLine(retStr,0);
							file.Write();
							file.Close();
						}

						wxMessageBox(wxString::Format(wxT("The database has been exported to '%s'."), dlg.GetPath().GetData()), wxT("wxDbExplorer"));
					}

				}
			}
		} else {
			wxMessageBox(wxT("Sorry, requested feature isn't implemented yet. "),wxT("Sorry"));
		}
	} catch (DatabaseLayerException& e) {
		wxString errorMessage = wxString::Format(_("Error (%d): %s"), e.GetErrorCode(), e.GetErrorMessage().c_str());
		wxMessageDialog dlg(this,errorMessage,wxT("DB Error"),wxOK | wxCENTER | wxICON_ERROR);
		dlg.ShowModal();
	} catch( ... ) {
		wxMessageDialog dlg(this,wxT("Unknown error."),wxT("DB Error"),wxOK | wxCENTER | wxICON_ERROR);
		dlg.ShowModal();
	}
}

bool DbViewerPanel::ImportDb(const wxString& sqlFile, Database* pDb)
{
	DatabaseLayer* pDbLayer = NULL;
	LogDialog dialog( this );//TODO:Doresit parenta
	dialog.Show();

	try {
		wxFileInputStream input(sqlFile);
		wxTextInputStream text( input );
		text.SetStringSeparators(wxT(";"));
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
			command.append(line);
			if (line.Find(wxT(";")) != wxNOT_FOUND) {
				dialog.AppendSeparator();
				dialog.AppendComment(wxT("Run SQL command:"));
				dialog.AppendText(command);
				pDbLayer->RunQuery(command);
				dialog.AppendComment(wxT("Successful!"));
				command.clear();
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

		dialog.AppendComment(wxT("Fail!"));
		dialog.AppendComment(errorMessage);
		wxMessageDialog dlg(this,errorMessage,wxT("DB Error"),wxOK | wxCENTER | wxICON_ERROR);
		dlg.ShowModal();
	} catch( ... ) {
		if (pDbLayer) {
			pDbLayer->RollBack();
			pDbLayer->Close();
		}
		wxMessageDialog dlg(this,wxT("Unknown error."),wxT("DB Error"),wxOK | wxCENTER | wxICON_ERROR);
		dlg.ShowModal();
	}
	dialog.EnableClose(true);
	dialog.ShowModal();


	return false;
}

void DbViewerPanel::OnPageChange(NotebookEvent& event)
{
	ErdPanel* pPanel = wxDynamicCast(m_mgr->GetActivePage(),ErdPanel);
	if (pPanel)	m_pThumbnail->SetCanvas(pPanel->getCanvas());

	event.Skip();
}

void DbViewerPanel::OnPageClose(NotebookEvent& event)
{
	ErdPanel* pPanel = wxDynamicCast(m_mgr->GetPage(event.GetSelection()),ErdPanel);
	if (pPanel)	{
		m_pThumbnail->SetCanvas(NULL);
		event.Skip();
	}
}

wxString DbViewerPanel::CreatePanelName(Database* d, PanelType type)
{
	if( type == DbViewerPanel::Sql ) {
		return wxT("SQL [") + d->GetName() + wxT("]");
	} else
		return wxT("ERD [") + d->GetName() + wxT("]");
}

wxString DbViewerPanel::CreatePanelName(Table* t, PanelType type)
{
	if( type == DbViewerPanel::Sql ) {
		return wxT("SQL [") + t->GetParentName() + wxT(":") + t->GetName() + wxT("]");
	} else
		return wxT("ERD [") + t->GetParentName() + wxT(":") + t->GetName() +  wxT("]");
}

wxString DbViewerPanel::CreatePanelName(View* v, PanelType type)
{
	if( type == DbViewerPanel::Sql ) {
		return wxT("SQL [") + v->GetParentName() + wxT(":") + v->GetName() + wxT("]");
	} else
		return wxT("ERD [") + v->GetParentName() + wxT(":") + v->GetName() +  wxT("]");
}
void DbViewerPanel::InitStyledTextCtrl(wxScintilla *sci)
{
	sci->SetLexer( wxSCI_LEX_SQL );
	sci->SetKeyWords(0, wxT("select insert into delete update from drop create alter where values order by desc asc show table column tables columns limit as in exists view join left inner set") );
	wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	font.SetFamily(wxTELETYPE);

	for(int i=0; i<wxSCI_STYLE_MAX; i++)
		sci->StyleSetFont( wxSCI_STYLE_MAX, font);

	sci->StyleSetBold( wxSCI_C_WORD, true );
	sci->StyleSetForeground( wxSCI_C_WORD, *wxBLUE );
	sci->StyleSetForeground( wxSCI_C_STRING, *wxRED );
	sci->StyleSetForeground( wxSCI_C_STRINGEOL, *wxRED );
	sci->StyleSetForeground( wxSCI_C_PREPROCESSOR, wxColour( 49, 106, 197 ) );
	sci->StyleSetForeground( wxSCI_C_COMMENT, wxColour( 0, 128, 0 ) );
	sci->StyleSetForeground( wxSCI_C_COMMENTLINE, wxColour( 0, 128, 0 ) );
	sci->StyleSetForeground( wxSCI_C_COMMENTDOC, wxColour( 0, 128, 0 ) );
	sci->StyleSetForeground( wxSCI_C_COMMENTLINEDOC, wxColour( 0, 128, 0 ) );
	sci->StyleSetForeground( wxSCI_C_NUMBER, *wxGREEN );

	sci->SetSelBackground( true, wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) );
	sci->SetSelForeground( true, wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHTTEXT ) );

	sci->SetCaretWidth( 2 );

	sci->SetTabIndents(true);
	sci->SetBackSpaceUnIndents(true);
	sci->SetUseTabs( false );
	sci->SetTabWidth( 4 );
	sci->SetIndent( 4 );

	// markers
	sci->MarkerDefine(wxSCI_MARKNUM_FOLDER,        wxSCI_MARK_BOXPLUS);
	sci->MarkerSetBackground( wxSCI_MARKNUM_FOLDER, wxColour( wxT("DARK GREY") ) );
	sci->MarkerSetForeground( wxSCI_MARKNUM_FOLDER, wxColour( wxT("WHITE") ) );

	sci->MarkerDefine(wxSCI_MARKNUM_FOLDEROPEN,    wxSCI_MARK_BOXMINUS);
	sci->MarkerSetBackground( wxSCI_MARKNUM_FOLDEROPEN, wxColour( wxT("DARK GREY") ) );
	sci->MarkerSetForeground( wxSCI_MARKNUM_FOLDEROPEN, wxColour( wxT("WHITE") ) );

	sci->MarkerDefine(wxSCI_MARKNUM_FOLDERSUB,     wxSCI_MARK_VLINE);
	sci->MarkerSetBackground( wxSCI_MARKNUM_FOLDERSUB, wxColour( wxT("DARK GREY") ) );
	sci->MarkerSetForeground( wxSCI_MARKNUM_FOLDERSUB, wxColour( wxT("WHITE") ) );

	sci->MarkerDefine(wxSCI_MARKNUM_FOLDEREND,     wxSCI_MARK_EMPTY);
	sci->MarkerSetBackground( wxSCI_MARKNUM_FOLDEREND, wxColour( wxT("DARK GREY") ) );
	sci->MarkerSetForeground( wxSCI_MARKNUM_FOLDEREND, wxColour( wxT("WHITE") ) );

	sci->MarkerDefine(wxSCI_MARKNUM_FOLDEROPENMID, wxSCI_MARK_BOXMINUS);
	sci->MarkerSetBackground( wxSCI_MARKNUM_FOLDEROPENMID, wxColour( wxT("DARK GREY") ) );
	sci->MarkerSetForeground( wxSCI_MARKNUM_FOLDEROPENMID, wxColour( wxT("WHITE") ) );

	sci->MarkerDefine(wxSCI_MARKNUM_FOLDERMIDTAIL, wxSCI_MARK_TCORNER);
	sci->MarkerSetBackground( wxSCI_MARKNUM_FOLDERMIDTAIL, wxColour( wxT("DARK GREY") ) );
	sci->MarkerSetForeground( wxSCI_MARKNUM_FOLDERMIDTAIL, wxColour( wxT("WHITE") ) );

	sci->MarkerDefine(wxSCI_MARKNUM_FOLDERTAIL,    wxSCI_MARK_LCORNER);
	sci->MarkerSetBackground( wxSCI_MARKNUM_FOLDERTAIL, wxColour( wxT("DARK GREY") ) );
	sci->MarkerSetForeground( wxSCI_MARKNUM_FOLDERTAIL, wxColour( wxT("WHITE") ) );

	sci->SetFoldFlags( wxSCI_FOLDFLAG_LINEAFTER_CONTRACTED );
}
