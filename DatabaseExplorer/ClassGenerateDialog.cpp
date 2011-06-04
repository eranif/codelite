#include "ClassGenerateDialog.h"
#include <wx/xrc/xmlres.h>
ClassGenerateDialog::ClassGenerateDialog(wxWindow *parent, IDbAdapter* dbAdapter, xsSerializable* pItems, IManager* pMgr):_ClassGenerateDialog(parent) {
	m_pDbAdapter = dbAdapter;
	m_pItems = pItems;	
	m_mgr = pMgr;
	
	TreeItemInfo item = m_mgr->GetSelectedTreeItemInfo( TreeFileView );
	if ( item.m_item.IsOk() && item.m_itemType == ProjectItem::TypeVirtualDirectory )
		{
			m_txVirtualDir->SetValue(VirtualDirectorySelector::DoGetPath( m_mgr->GetTree( TreeFileView ), item.m_item, false ));
		}	
	}

ClassGenerateDialog::~ClassGenerateDialog() {
	Destroy();
}

bool ClassGenerateDialog::GenerateClass(Table* pTab, const wxString& path) {
	
	wxString hFileName = wxT("");
	wxString cFileName = wxT("");

	if (pTab->IsView()){
	  	hFileName = wxT("viewClass.htmp");
		cFileName = wxT("viewClass.ctmp");		
		
	}else{
	  	hFileName = wxT("dataClass.htmp");
		cFileName = wxT("dataClass.ctmp");	
		}

	wxTextFile htmpFile(m_mgr->GetInstallDirectory() + wxT("/templates/databaselayer/") + hFileName);
	wxTextFile ctmpFile(m_mgr->GetInstallDirectory() + wxT("/templates/databaselayer/") + cFileName);	
	

	

	if (!htmpFile.Open()) return false;
	if (!ctmpFile.Open()) return false;
	wxString classTableName = pTab->GetName();
	wxString classItemName = m_txPrefix->GetValue() + pTab->GetName() + m_txPostfix->GetValue();
	wxString classItemDef = pTab->GetName().Upper() + wxT("_H");
	wxString classColName = m_txPrefix->GetValue() + pTab->GetName() + wxT("Collection")+ m_txPostfix->GetValue();
	wxString classUtilName = m_txPrefix->GetValue() + pTab->GetName() + wxT("Utils")+ m_txPostfix->GetValue();



	wxTextFile hFile(path + wxT("/") + classItemName + wxT(".h"));
	wxTextFile cFile(path + wxT("/") + classItemName + wxT(".cpp"));
	if (hFile.Exists()){
		if (!hFile.Open()) return false;
		hFile.Clear();
		}else{
		if (!hFile.Create()) return false;		
		}		
	if (cFile.Exists()){
		if (!cFile.Open()) return false;
		cFile.Clear();
		}else{
		if (!cFile.Create()) return false;		
		}	
	bool suc = GenerateFile(pTab,htmpFile,hFile, classItemName, classItemDef,classColName,classTableName, classUtilName);
	suc &= GenerateFile(pTab,ctmpFile,cFile, classItemName, classItemDef,classColName,classTableName, classUtilName);
	
	
	hFile.Write();
	hFile.Close();
	cFile.Write();
	cFile.Close();
	htmpFile.Close();
	ctmpFile.Close();
	
	wxArrayString arrString;
	arrString.Add( path + wxT("/") + classItemName + wxT(".h"));
	arrString.Add( path + wxT("/") + classItemName + wxT(".cpp"));
	
	m_mgr->AddFilesToVirtualFolder( m_txVirtualDir->GetValue(), arrString );
	
	return suc;
}

void ClassGenerateDialog::OnCancelClick(wxCommandEvent& event) {
	Destroy();
	}

void ClassGenerateDialog::OnGenerateClick(wxCommandEvent& event) {
	if( m_txVirtualDir->GetValue().IsEmpty() )
	{
		wxMessageBox( _("Virtual name cannot be empty"), _("CodeLite"), wxICON_WARNING | wxOK );
		m_txVirtualDir->SetFocus();
		return;
	}
	if (m_dirPicker->GetPath().IsEmpty())
	{
		wxMessageBox( _("Folder name cannot be empty"), _("CodeLite"), wxICON_WARNING | wxOK );
		m_dirPicker->SetFocus();
		}
	
	
	m_textCtrl19->Clear();
	
	wxString err_msg;
	wxString project = m_txVirtualDir->GetValue().BeforeFirst(wxT(':'));
	ProjectPtr proj = m_mgr->GetWorkspace()->FindProjectByName(project, err_msg);	
	if( proj )
	{	
		wxString filePath = m_dirPicker->GetPath();//proj->GetFileName().GetPath();
		
		Table* pTable = wxDynamicCast(m_pItems, Table);
		
		if (pTable){
			if (GenerateClass(pTable,filePath)) m_textCtrl19->AppendText(pTable->GetName() + wxT("......... Generated successfully!\n"));			
			else m_textCtrl19->AppendText(pTable->GetName() + wxT("......... Error!!!\n"));			 	
		}else{
			SerializableList::compatibility_iterator node = m_pItems->GetFirstChildNode();
			while( node ) {
				Table* pTab = wxDynamicCast(node->GetData(),Table);
				if (pTab){
					if (GenerateClass(pTab,filePath)) m_textCtrl19->AppendText(pTab->GetName() + wxT("......... Generated successfully!\n"));			
					else m_textCtrl19->AppendText(pTab->GetName() + wxT("......... Error!!!\n"));			 	
					}

				node = node->GetNext();
			}	
		} 
	wxCommandEvent e(wxEVT_COMMAND_MENU_SELECTED, XRCID("retag_workspace"));
	m_mgr->GetTheApp()->GetTopWindow()->GetEventHandler()->AddPendingEvent(e);
	}
}

wxString ClassGenerateDialog::GetTypeName(IDbType::UNIVERSAL_TYPE type)
{
	if (type == IDbType::dbtTYPE_TEXT) return wxT("wxString");
	if (type == IDbType::dbtTYPE_DATE_TIME) return wxT("wxDateTime");
	if (type == IDbType::dbtTYPE_INT) return wxT("int");
	if (type == IDbType::dbtTYPE_FLOAT) return wxT("double");
	if (type == IDbType::dbtTYPE_DECIMAL) return wxT("double");
	if (type == IDbType::dbtTYPE_BOOLEAN) return wxT("bool");
	if (type == IDbType::dbtTYPE_OTHER) return wxT("Object*");
	return wxT("");
}
wxString ClassGenerateDialog::GetParamTypeName(IDbType::UNIVERSAL_TYPE type)
{
	if (type == IDbType::dbtTYPE_TEXT) return wxT("const wxString&");
	if (type == IDbType::dbtTYPE_DATE_TIME) return wxT("const wxDateTime&");
	if (type == IDbType::dbtTYPE_INT) return wxT("int");
	if (type == IDbType::dbtTYPE_FLOAT) return wxT("double");
	if (type == IDbType::dbtTYPE_DECIMAL) return wxT("double");
	if (type == IDbType::dbtTYPE_BOOLEAN) return wxT("bool");
	if (type == IDbType::dbtTYPE_OTHER) return wxT("Object*");
	return wxT("");
}
wxString ClassGenerateDialog::GetResultFunction(IDbType::UNIVERSAL_TYPE type)
{
	if (type == IDbType::dbtTYPE_TEXT) return wxT("GetResultString");
	if (type == IDbType::dbtTYPE_DATE_TIME) return wxT("GetResultDate");
	if (type == IDbType::dbtTYPE_INT) return wxT("GetResultInt");
	if (type == IDbType::dbtTYPE_FLOAT) return wxT("GetResultDouble");
	if (type == IDbType::dbtTYPE_DECIMAL) return wxT("GetResultDouble");
	if (type == IDbType::dbtTYPE_BOOLEAN) return wxT("GetResultBool");
	if (type == IDbType::dbtTYPE_OTHER) return wxT("GetResultBlob");
	return wxT("");
}
//TODO:Negenerovat požaavky na ID když je auto_increment
bool ClassGenerateDialog::GenerateFile(Table* pTab, wxTextFile& htmpFile, wxTextFile& hFile, const wxString& classItemName, const wxString& classItemDef, const wxString& classColName, const wxString& classTableName, const wxString& classUtilName)
{
	Constraint* pPK = NULL;
	int colCount = 0;
	int lastEditParam = 0;
	SerializableList::compatibility_iterator node = pTab->GetFirstChildNode();
	while( node ) {
		Constraint* pConstr = wxDynamicCast(node->GetData(),Constraint);
		if (pConstr){
			if (pConstr->GetType() == Constraint::primaryKey) pPK = pConstr;		
			}else colCount++;					
		node = node->GetNext();
		}
	Column* pPKCol = NULL;
	
	if (pPK){
		SerializableList::compatibility_iterator node = pTab->GetFirstChildNode();
		while( node ) {
			Column* pCol = wxDynamicCast(node->GetData(),Column);
			if (pCol){
				if (pCol->GetName() == pPK->GetLocalColumn()) pPKCol = pCol;		
				}					
			node = node->GetNext();
			}		
		}	
	
	for ( wxString str = htmpFile.GetFirstLine(); !htmpFile.Eof(); str = htmpFile.GetNextLine() )
	{
		if (str.Contains(wxT("%%classItemGetters%%"))){
				SerializableList::compatibility_iterator node = pTab->GetFirstChildNode();
				while( node ) {
					Column* pCol = wxDynamicCast(node->GetData(),Column);
					if (pCol){
						hFile.AddLine(wxString::Format(wxT("\tconst %s Get%s() const {"), GetResTypeName(pCol->GetPType()->GetUniversalType()).c_str(), pCol->GetName().c_str()));
						hFile.AddLine(wxString::Format(wxT("\t\treturn m_%s;"), pCol->GetName().c_str()));
						hFile.AddLine(wxString::Format(wxT("\t\t}")));						
						}					
					node = node->GetNext();
					}
			
		}else if (str.Contains(wxT("%%classItemVariables%%"))){
				SerializableList::compatibility_iterator node = pTab->GetFirstChildNode();
				while( node ) {
					Column* pCol = wxDynamicCast(node->GetData(),Column);
					if (pCol){
						hFile.AddLine(wxString::Format(wxT("\t%s m_%s;"), GetTypeName(pCol->GetPType()->GetUniversalType()).c_str(), pCol->GetName().c_str()));
						}					
					node = node->GetNext();
					}		
		}else if (str.Contains(wxT("%%classItemLoading%%"))){
				SerializableList::compatibility_iterator node = pTab->GetFirstChildNode();
				while( node ) {
					Column* pCol = wxDynamicCast(node->GetData(),Column);
					if (pCol){
						hFile.AddLine(wxString::Format(wxT("\t\tm_%s = pResult->%s(wxT(\"%s\"));"),pCol->GetName().c_str(), GetResultFunction(pCol->GetPType()->GetUniversalType()).c_str(), pCol->GetName().c_str()));
						}					
					node = node->GetNext();
					}				
		}else if (str.Contains(wxT("%%classColLabelFillGrid%%"))){		
				int i = 0;
				SerializableList::compatibility_iterator node = pTab->GetFirstChildNode();
				while( node ) {
					Column* pCol = wxDynamicCast(node->GetData(),Column);
					if (pCol){
						hFile.AddLine(wxT("\tpGrid->AppendCols(1);"));
						hFile.AddLine(wxString::Format(wxT("\tpGrid->SetColLabelValue(%i,wxT(\"%s\"));"),i++,pCol->GetName().c_str()));					
						}					
					node = node->GetNext();
					}				
		}else if (str.Contains(wxT("%%classColDataFillGrid%%"))){			
				int i = 0;
				SerializableList::compatibility_iterator node = pTab->GetFirstChildNode();
				while( node ) {
					Column* pCol = wxDynamicCast(node->GetData(),Column);
					if (pCol){
						hFile.AddLine(GetFillData(pCol, i++));
						}					
					node = node->GetNext();
					}				
		}else if (str.Contains(wxT("%%primaryKeyHeader%%"))){			
				if (pPKCol){
						hFile.AddLine(wxString::Format(wxT("\t/*! \\brief Return %s from db on the %s base */"),pPKCol->GetParentName().c_str(),pPKCol->GetName().c_str()));
						hFile.AddLine(wxString::Format(wxT("\tstatic %s* GetBy%s(%s %s, DatabaseLayer* pDbLayer);"),classItemName.c_str(),pPKCol->GetName().c_str(), GetTypeName(pPKCol->GetPType()->GetUniversalType()).c_str(),pPKCol->GetName().c_str()));
					}				
		}else if (str.Contains(wxT("%%primaryKeyBody%%"))){			
				if (pPKCol){
						hFile.AddLine(wxString::Format(wxT("%s* %s::GetBy%s(%s %s, DatabaseLayer* pDbLayer)"),classItemName.c_str(),classItemName.c_str(),pPKCol->GetName().c_str(), GetTypeName(pPKCol->GetPType()->GetUniversalType()).c_str(),pPKCol->GetName().c_str()));
						hFile.AddLine(wxT("{"));
						hFile.AddLine(wxT("\tDatabaseResultSet* resSet = NULL;"));
						hFile.AddLine(wxT("\tPreparedStatement* pStatement = NULL;"));
						hFile.AddLine(wxT("\tif (pDbLayer){"));
						hFile.AddLine(wxT("\t\tif (pDbLayer->IsOpen()){"));
						
						hFile.AddLine(wxString::Format(wxT("\t\t\tpStatement = pDbLayer->PrepareStatement(wxT(\"SELECT * FROM %s WHERE %s = ?\"));"),classTableName.c_str(), pPKCol->GetName().c_str()));
						hFile.AddLine(wxString::Format(wxT("\t\t\tpStatement->SetParamInt(1, %s);"),pPKCol->GetName().c_str()));			
						hFile.AddLine(wxT("\t\t\tresSet = pStatement->RunQueryWithResults();"));
						hFile.AddLine(wxT("\t\t\t}"));
						hFile.AddLine(wxT("\t\t}"));			
	
						hFile.AddLine(wxT("\tif (resSet){"));
						hFile.AddLine(wxString::Format(wxT("\t\tif (resSet->Next()) return new %s(resSet);"),classItemName.c_str()));	
						hFile.AddLine(wxT("\t\tpStatement->Close();"));	
						hFile.AddLine(wxT("\t\t}"));	
						hFile.AddLine(wxT("\treturn NULL;"));
			
						
						hFile.AddLine(wxT("}"));
				}			
		}else if (str.Contains(wxT("%%classUtilsAddParameters%%"))){
				SerializableList::compatibility_iterator node = pTab->GetFirstChildNode();
				while( node ) {
					Column* pCol = wxDynamicCast(node->GetData(),Column);
					if (pCol){
						hFile.AddLine(wxString::Format(wxT("\t\t\t,%s %s"), GetParamTypeName(pCol->GetPType()->GetUniversalType()).c_str(), pCol->GetName().c_str()));
						}					
					node = node->GetNext();
					}		
		}else if (str.Contains(wxT("%%classUtilsDeleteParameters%%"))){
				if (pPKCol) hFile.AddLine(wxString::Format(wxT("\t\t\t,%s %s"), GetParamTypeName(pPKCol->GetPType()->GetUniversalType()).c_str(), pPKCol->GetName().c_str()));
		}else if (str.Contains(wxT("%%classUtilsAddSetParams%%"))){
				int i = 1;
				SerializableList::compatibility_iterator node = pTab->GetFirstChildNode();
				while( node ) {
					Column* pCol = wxDynamicCast(node->GetData(),Column);
					if (pCol){
						hFile.AddLine(wxString::Format(wxT("\t\t\t\tpStatement->%s(%i, %s);"), GetAddParamFunction(pCol->GetPType()->GetUniversalType()).c_str(),i++,pCol->GetName().c_str()));
						}					
					node = node->GetNext();
					}	
				lastEditParam = i;
		}else if (str.Contains(wxT("%%classUtilsAddDelParams%%"))){
				if (pPKCol) hFile.AddLine(wxString::Format(wxT("\t\t\t\tpStatement->%s(%i, %s);"), GetAddParamFunction(pPKCol->GetPType()->GetUniversalType()).c_str(),1,pPKCol->GetName().c_str()));
		}else if (str.Contains(wxT("%%classUtilsAddStatement%%"))){
				wxString cols = wxT("");
				wxString params = wxT("");
				SerializableList::compatibility_iterator node = pTab->GetFirstChildNode();
				while( node ) {
					Column* pCol = wxDynamicCast(node->GetData(),Column);
					if (pCol){						
						if (!cols.IsEmpty()) cols = cols + wxT(",");
						cols += pCol->GetName();

						if (!params.IsEmpty()) params += wxT(",");
						params += wxT("?");
						}					
					node = node->GetNext();
					}	
				
				hFile.AddLine(wxString::Format(wxT("\t\tPreparedStatement* pStatement = pDbLayer->PrepareStatement(wxT(\"INSERT INTO %s (%s) VALUES (%s)\"));"),pTab->GetName().c_str(), cols.c_str(), params.c_str()));
			
		}else if (str.Contains(wxT("%%classUtilsEditStatement%%"))){
				wxString cols = wxT("");
				wxString params = wxT("");
				SerializableList::compatibility_iterator node = pTab->GetFirstChildNode();
				while( node ) {
					Column* pCol = wxDynamicCast(node->GetData(),Column);
					if (pCol){						
						if (!cols.IsEmpty()) cols = cols + wxT(",");
						cols += pCol->GetName() + wxT(" = ?");
						}					
					node = node->GetNext();
					}	
				if (pPKCol)	hFile.AddLine(wxString::Format(wxT("\t\tPreparedStatement* pStatement = pDbLayer->PrepareStatement(wxT(\"UPDATE %s SET %s WHERE %s = ?\"));"),pTab->GetName().c_str(), cols.c_str(), pPKCol->GetName().c_str()));
				else hFile.AddLine(wxT("\t\tPreparedStatement* pStatement = NULL;"));
		}else if (str.Contains(wxT("%%classUtilsDeleteStatement%%"))){
				if (pPKCol)	hFile.AddLine(wxString::Format(wxT("\t\tPreparedStatement* pStatement = pDbLayer->PrepareStatement(wxT(\"DELETE FROM %s WHERE %s = ?\"));"),pTab->GetName().c_str(), pPKCol->GetName().c_str()));
				else hFile.AddLine(wxT("\t\tPreparedStatement* pStatement = NULL;"));
		}else if (str.Contains(wxT("%%classUtilsPKSetParams%%"))){
				if (pPKCol) hFile.AddLine(wxString::Format(wxT("\t\t\t\tpStatement->%s(%i, %s);"), GetAddParamFunction(pPKCol->GetPType()->GetUniversalType()).c_str(),lastEditParam,pPKCol->GetName().c_str()));
		}else{
			str.Replace(wxT("%%classItemName%%"),classItemName);
			str.Replace(wxT("%%classItemDef%%"),classItemDef);
			str.Replace(wxT("%%classColName%%"),classColName);
			str.Replace(wxT("%%classTableName%%"),classTableName);
			str.Replace(wxT("%%classUtilName%%"), classUtilName);
			hFile.AddLine(str);			
		}	
	}
	
	return true;
}

wxString ClassGenerateDialog::GetResTypeName(IDbType::UNIVERSAL_TYPE type)
{
	if (type == IDbType::dbtTYPE_TEXT) return wxT("wxString&");
	if (type == IDbType::dbtTYPE_DATE_TIME) return wxT("wxDateTime");
	if (type == IDbType::dbtTYPE_INT) return wxT("int");
	if (type == IDbType::dbtTYPE_FLOAT) return wxT("double");
	if (type == IDbType::dbtTYPE_DECIMAL) return wxT("double");
	if (type == IDbType::dbtTYPE_BOOLEAN) return wxT("bool");
	if (type == IDbType::dbtTYPE_OTHER) return wxT("Object*");
	return wxT("");
}

wxString ClassGenerateDialog::GetFillData(Column* pCol, int colIndex)
{
	if (pCol->GetPType()->GetUniversalType() == IDbType::dbtTYPE_TEXT) return wxT("\t\tpGrid->SetCellValue(wxString::Format(wxT(\"%s\"),current->Get")+ pCol->GetName() + wxString::Format(wxT("().c_str()),i,%i);"),colIndex);
	if (pCol->GetPType()->GetUniversalType() == IDbType::dbtTYPE_DATE_TIME) return wxT("\t\tpGrid->SetCellValue(current->Get")+ pCol->GetName() + wxString::Format(wxT("().Format(),i,%i);"),colIndex);
	if (pCol->GetPType()->GetUniversalType() == IDbType::dbtTYPE_INT) return wxT("\t\tpGrid->SetCellValue(wxString::Format(wxT(\"%i\"),current->Get")+ pCol->GetName() + wxString::Format(wxT("()),i,%i);"),colIndex);
	if (pCol->GetPType()->GetUniversalType() == IDbType::dbtTYPE_FLOAT) return wxT("\t\tpGrid->SetCellValue(wxString::Format(wxT(\"%f\"),current->Get")+ pCol->GetName() + wxString::Format(wxT("()),i,%i);"),colIndex);
	if (pCol->GetPType()->GetUniversalType() == IDbType::dbtTYPE_DECIMAL) return wxT("\t\tpGrid->SetCellValue(wxString::Format(wxT(\"%d\"),current->Get")+ pCol->GetName() + wxString::Format(wxT("()),i,%i);"),colIndex);
	if (pCol->GetPType()->GetUniversalType() == IDbType::dbtTYPE_BOOLEAN) return wxT("\t\tpGrid->SetCellValue(wxString::Format(wxT(\"%i\"),current->Get")+ pCol->GetName() + wxString::Format(wxT("()),i,%i);"),colIndex);
	if (pCol->GetPType()->GetUniversalType() == IDbType::dbtTYPE_OTHER) return wxT("\t\tpGrid->SetCellValue(wxT(\"some data\")") + wxString::Format(wxT(",i,%i);"),colIndex);
	return wxT("");
}

wxString ClassGenerateDialog::GetAddParamFunction(IDbType::UNIVERSAL_TYPE type)
{
	if (type == IDbType::dbtTYPE_TEXT) return wxT("SetParamString");
	if (type == IDbType::dbtTYPE_DATE_TIME) return wxT("SetParamDate");
	if (type == IDbType::dbtTYPE_INT) return wxT("SetParamInt");
	if (type == IDbType::dbtTYPE_FLOAT) return wxT("SetParamDouble");
	if (type == IDbType::dbtTYPE_DECIMAL) return wxT("SetParamDouble");
	if (type == IDbType::dbtTYPE_BOOLEAN) return wxT("SetParamBool");
	if (type == IDbType::dbtTYPE_OTHER) return wxT("SetParamBlob");
	return wxT("");
}
void ClassGenerateDialog::OnBtnBrowseClick(wxCommandEvent& event)
{
	VirtualDirectorySelector dlg(this, m_mgr->GetWorkspace(), m_txVirtualDir->GetValue());
	if ( dlg.ShowModal() == wxID_OK )
	{
		m_txVirtualDir->SetValue( dlg.GetVirtualDirectoryPath() );
	}
}

