
#include "DbViewerPanel.h"
#include <wx/file.h>
#include <wx/textfile.h>
#include "SqlCommandPanel.h"
#include <wx/wupdlock.h>
#include <wx/busyinfo.h>
#include <wx/xrc/xmlres.h>
#include "globals.h"


const wxEventType wxEVT_EXECUTE_SQL = XRCID("wxEVT_EXECUTE_SQL");

BEGIN_EVENT_TABLE(SQLCommandPanel, _SqlCommandPanel)
	EVT_COMMAND(wxID_ANY, wxEVT_EXECUTE_SQL, SQLCommandPanel::OnExecuteSQL)
END_EVENT_TABLE()

SQLCommandPanel::SQLCommandPanel(wxWindow *parent,IDbAdapter* dbAdapter,  const wxString& dbName, const wxString& dbTable)
	: _SqlCommandPanel(parent)
{
	DbViewerPanel::InitStyledTextCtrl( m_scintillaSQL );
	m_pDbAdapter = dbAdapter;
	m_dbName = dbName;
	m_dbTable = dbTable;

	//TODO:SQL:
	//m_scintillaSQL->AddText(wxT("-- selected database ") + m_dbName);
	m_scintillaSQL->AddText(wxString::Format(wxT(" -- selected database %s\n"), m_dbName.c_str()));
	if (!dbTable.IsEmpty()) {
		m_scintillaSQL->AddText(m_pDbAdapter->GetDefaultSelect(m_dbName, m_dbTable));
		wxCommandEvent event(wxEVT_EXECUTE_SQL);
		GetEventHandler()->AddPendingEvent(event);
	}
}

SQLCommandPanel::~SQLCommandPanel()
{
	delete m_pDbAdapter;
}

void SQLCommandPanel::OnExecuteClick(wxCommandEvent& event)
{
	event.Skip();
	ExecuteSql();
}

void SQLCommandPanel::OnScintilaKeyDown(wxKeyEvent& event)
{
	if ((event.ControlDown())&&(event.GetKeyCode() == WXK_RETURN)) {
		ExecuteSql();
	}
	event.Skip();
}

void SQLCommandPanel::ExecuteSql()
{
	wxBusyInfo infoDlg(_("Executing sql..."), this);
	wxWindowUpdateLocker locker(this);
	DatabaseLayer* m_pDbLayer = m_pDbAdapter->GetDatabaseLayer(m_dbName);
	if (m_pDbLayer->IsOpen()) {
		// test for empty string
		if (this->m_scintillaSQL->GetText() != wxT("")) {
			try {
				if (!m_pDbAdapter->GetUseDb(m_dbName).IsEmpty()) m_pDbLayer->RunQuery(m_pDbAdapter->GetUseDb(m_dbName));
				// run query
				DatabaseResultSet* pResultSet = m_pDbLayer->RunQueryWithResults(this->m_scintillaSQL->GetText());
				
				// clear variables
				m_gridTable->DeleteCols(0,m_gridTable->GetNumberCols());
				m_gridTable->DeleteRows(0,m_gridTable->GetNumberRows());
				m_gridValues.clear();
				
				int rows = 0;
				int cols = pResultSet->GetMetaData()->GetColumnCount();

				// create table header
				m_gridTable->AppendCols(cols);
				for (int i = 1; i<= pResultSet->GetMetaData()->GetColumnCount(); i++) {
					m_gridTable->SetColLabelValue(i-1,pResultSet->GetMetaData()->GetColumnName(i));
				}

				// fill table data
				while (pResultSet->Next()) {
					wxString value;
					m_gridTable->AppendRows();
					for (int i = 1; i<= pResultSet->GetMetaData()->GetColumnCount(); i++) {

						switch (pResultSet->GetMetaData()->GetColumnType(i)) {
						case ResultSetMetaData::COLUMN_INTEGER:
							value = wxString::Format(wxT("%i"),pResultSet->GetResultInt(i));
							break;
							
						case ResultSetMetaData::COLUMN_STRING:
							value =  pResultSet->GetResultString(i);
							break;
							
						case ResultSetMetaData::COLUMN_UNKNOWN:
							value = pResultSet->GetResultString(i);
							break;
							
						case ResultSetMetaData::COLUMN_BLOB:
						{
							wxMemoryBuffer buffer;
							pResultSet->GetResultBlob(i, buffer);
							value = wxString::Format(wxT("BLOB (Size:%u)"), buffer.GetDataLen());
							break;
						}
						case ResultSetMetaData::COLUMN_BOOL:
							value = wxString::Format(wxT("%b"),pResultSet->GetResultBool(i));
							break;
							
						case ResultSetMetaData::COLUMN_DATE:
							value = pResultSet->GetResultDate(i).Format();
							break;
							
						case ResultSetMetaData::COLUMN_DOUBLE:
							value = wxString::Format(wxT("%f"),pResultSet->GetResultDouble(i));
							break;
							
						case ResultSetMetaData::COLUMN_NULL:
							value = wxT("NULL");
							break;
							
						default:
							value = pResultSet->GetResultString(i);
							break;
						}
						
						m_gridValues[std::make_pair<int, int>(rows,  i-1)] = value;
						
						// truncate the string to a reasonable string
						if(value.Length() > 100) {
							value = value.Mid(0, 100);
							value.Append(wxT("..."));
						}
						
						// Convert all whitespace chars into visible ones
						value.Replace(wxT("\n"), wxT("\\n"));
						value.Replace(wxT("\r"), wxT("\\r"));
						value.Replace(wxT("\t"), wxT("\\t"));
						m_gridTable->SetCellValue(value ,rows, i-1);
					}
					rows++;
				}
				
				m_pDbLayer->CloseResultSet(pResultSet);
				m_gridTable->AutoSize();
				
				// show result status
				m_labelStatus->SetLabel(wxString::Format(wxT("Result: %i rows"),rows));
				Layout();
				
			} catch (DatabaseLayerException& e) {
				wxString errorMessage = wxString::Format(_("Error (%d): %s"), e.GetErrorCode(), e.GetErrorMessage().c_str());
				wxMessageDialog dlg(this,errorMessage,wxT("DB Error"),wxOK | wxCENTER | wxICON_ERROR);
				dlg.ShowModal();
			} catch( ... ) {
				wxMessageDialog dlg(this,wxT("Unknown error."),wxT("DB Error"),wxOK | wxCENTER | wxICON_ERROR);
				dlg.ShowModal();
			}
		}

	} else
		wxMessageBox(wxT("Cant connect!"));
}

void SQLCommandPanel::OnLoadClick(wxCommandEvent& event)
{
	wxFileDialog dlg(this, wxT("Chose file"),wxT(""),wxT(""),wxT("Sql files(*.sql)|*.sql"),wxFD_OPEN|wxFD_FILE_MUST_EXIST);
	m_scintillaSQL->ClearAll();
	if (dlg.ShowModal() == wxID_OK) {
		wxTextFile file( dlg.GetPath());
		file.Open();
		if (file.IsOpened()) {
			for ( wxString str = file.GetFirstLine(); !file.Eof(); str = file.GetNextLine() ) {
				m_scintillaSQL->AddText(str);
				m_scintillaSQL->AddText(wxT("\n"));
			}
		}
	}
}

void SQLCommandPanel::OnSaveClick(wxCommandEvent& event)
{
	wxFileDialog dlg(this,wxT("Chose file"),wxT(""),wxT(""),wxT("Sql files(*.sql)|*.sql"),wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
	if (dlg.ShowModal() == wxID_OK) {

		wxFile file(dlg.GetPath(),wxFile::write);
		if (file.IsOpened()) {
			file.Write(m_scintillaSQL->GetText());
			file.Close();
		}
	}
}

void SQLCommandPanel::OnTeplatesLeftDown(wxMouseEvent& event)
{
}
void SQLCommandPanel::OnTemplatesBtnClick(wxCommandEvent& event)
{
	wxMenu menu;

	menu.Append(IDR_SQLCOMMAND_SELECT,wxT("Insert SELECT SQL template"),wxT("Insert SELECT SQL statement template into editor."));
	menu.Append(IDR_SQLCOMMAND_INSERT,wxT("Insert INSERT SQL template"),wxT("Insert INSERT SQL statement template into editor."));
	menu.Append(IDR_SQLCOMMAND_UPDATE,wxT("Insert UPDATE SQL template"),wxT("Insert UPDATE SQL statement template into editor."));
	menu.Append(IDR_SQLCOMMAND_DELETE,wxT("Insert DELETE SQL template"),wxT("Insert DELETE SQL statement template into editor."));
	menu.Connect(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&SQLCommandPanel::OnPopupClick, NULL, this);
	PopupMenu(&menu);
}
void SQLCommandPanel::OnPopupClick(wxCommandEvent& evt)
{

	if (evt.GetId() == IDR_SQLCOMMAND_SELECT) {
		m_scintillaSQL->AddText(wxT("SELECT * FROM TableName\n"));
	} else	if (evt.GetId() == IDR_SQLCOMMAND_INSERT) {
		m_scintillaSQL->AddText(wxT("INSERT INTO TableName (ColumnA, ColumnB) VALUES (1,'Test text')\n"));
	} else	if (evt.GetId() == IDR_SQLCOMMAND_UPDATE) {
		m_scintillaSQL->AddText(wxT("UPDATE TableName SET ColumnA = 2, ColumnB = 'Second text' WHERE ID = 1\n"));
	} else	if (evt.GetId() == IDR_SQLCOMMAND_DELETE) {
		m_scintillaSQL->AddText(wxT("DELETE FROM TableName WHERE ID = 1\n"));
	}
}

void SQLCommandPanel::OnExecuteSQL(wxCommandEvent& e)
{
	wxUnusedVar(e);
	ExecuteSql();
}

void SQLCommandPanel::OnGridCellRightClick(wxGridEvent& event)
{
	event.Skip();
	
	// Keep the current cell's value (taken from the map and NOT from the UI)
	std::map<std::pair<int, int>, wxString >::const_iterator iter = m_gridValues.find(std::make_pair<int, int>(event.GetRow(), event.GetCol()));
	if(iter == m_gridValues.end())
		return;

	m_cellValue = iter->second;
	
	wxMenu menu; 
	menu.Append (XRCID("db_copy_cell_value"), _("Copy value to clipboard"));
	menu.Connect(XRCID("db_copy_cell_value"), wxEVT_COMMAND_MENU_SELECTED,  wxCommandEventHandler(SQLCommandPanel::OnCopyCellValue), NULL, this);
	m_gridTable->PopupMenu( &menu );
}

void SQLCommandPanel::OnCopyCellValue(wxCommandEvent& e)
{
	e.Skip();
	if(m_cellValue.IsEmpty() == false) {
		CopyToClipboard(m_cellValue);
	}
}
