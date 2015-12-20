//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : SqlCommandPanel.cpp
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


#include "DbViewerPanel.h"
#include <wx/file.h>
#include <wx/textfile.h>
#include "SqlCommandPanel.h"
#include <wx/wupdlock.h>
#include <wx/busyinfo.h>
#include <wx/xrc/xmlres.h>
#include "cl_defs.h"
#include "globals.h"
#include <set>
#include "cl_aui_tool_stickness.h"
#include "lexer_configuration.h"
#include "editor_config.h"
#include "db_explorer_settings.h"
#include <algorithm>

#if CL_USE_NATIVEBOOK
#ifdef __WXGTK20__
// We need this ugly hack to workaround a gtk2-wxGTK name-clash
// See http://trac.wxwidgets.org/ticket/10883
#define GSocket GlibGSocket
#include <gtk/gtk.h>
#undef GSocket
#endif
#endif

const wxEventType wxEVT_EXECUTE_SQL = XRCID("wxEVT_EXECUTE_SQL");

BEGIN_EVENT_TABLE(SQLCommandPanel, _SqlCommandPanel)
    EVT_COMMAND(wxID_ANY, wxEVT_EXECUTE_SQL, SQLCommandPanel::OnExecuteSQL)
END_EVENT_TABLE()

SQLCommandPanel::SQLCommandPanel(wxWindow *parent,IDbAdapter* dbAdapter,  const wxString& dbName, const wxString& dbTable)
    : _SqlCommandPanel(parent)
{
    LexerConf::Ptr_t lexerSQL = EditorConfigST::Get()->GetLexer("SQL");
    if ( lexerSQL ) {
        lexerSQL->Apply(m_scintillaSQL, true);

    } else {
        DbViewerPanel::InitStyledTextCtrl( m_scintillaSQL );

    }
    m_pDbAdapter = dbAdapter;
    m_dbName = dbName;
    m_dbTable = dbTable;

    wxTheApp->Connect(wxID_SELECTALL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SQLCommandPanel::OnEdit),   NULL, this);
    wxTheApp->Connect(wxID_COPY,      wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SQLCommandPanel::OnEdit),   NULL, this);
    wxTheApp->Connect(wxID_PASTE,     wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SQLCommandPanel::OnEdit),   NULL, this);
    wxTheApp->Connect(wxID_CUT,       wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SQLCommandPanel::OnEdit),   NULL, this);
    wxTheApp->Connect(wxID_UNDO,      wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SQLCommandPanel::OnEdit),   NULL, this);
    wxTheApp->Connect(wxID_REDO,      wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SQLCommandPanel::OnEdit),   NULL, this);

    m_scintillaSQL->AddText(wxString::Format(wxT(" -- selected database %s\n"), m_dbName.c_str()));
    if (!dbTable.IsEmpty()) {
        m_scintillaSQL->AddText(m_pDbAdapter->GetDefaultSelect(m_dbName, m_dbTable));
        wxCommandEvent event(wxEVT_EXECUTE_SQL);
        GetEventHandler()->AddPendingEvent(event);
    }

#if CL_USE_NATIVEBOOK
    gtk_widget_show_all(this->m_widget);
#endif
}

SQLCommandPanel::~SQLCommandPanel()
{
    wxTheApp->Disconnect(wxID_SELECTALL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SQLCommandPanel::OnEdit),   NULL, this);
    wxTheApp->Disconnect(wxID_COPY,      wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SQLCommandPanel::OnEdit),   NULL, this);
    wxTheApp->Disconnect(wxID_PASTE,     wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SQLCommandPanel::OnEdit),   NULL, this);
    wxTheApp->Disconnect(wxID_CUT,       wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SQLCommandPanel::OnEdit),   NULL, this);
    wxTheApp->Disconnect(wxID_UNDO,      wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SQLCommandPanel::OnEdit),   NULL, this);
    wxTheApp->Disconnect(wxID_REDO,      wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SQLCommandPanel::OnEdit),   NULL, this);
    delete m_pDbAdapter;
}

void SQLCommandPanel::OnExecuteClick(wxCommandEvent& event)
{
    ExecuteSql();
}

void SQLCommandPanel::OnScintilaKeyDown(wxKeyEvent& event)
{
    if ( (event.ControlDown()) && (event.GetKeyCode() == WXK_RETURN || event.GetKeyCode() == WXK_NUMPAD_ENTER)) {
        ExecuteSql();
    }
    event.Skip();
}

void SQLCommandPanel::ExecuteSql()
{
    wxBusyInfo infoDlg(_("Executing sql..."), wxTheApp->GetTopWindow());
    
    clWindowUpdateLocker locker(this);
    std::set<int> textCols;
    std::set<int> blobCols;
    DatabaseLayerPtr m_pDbLayer = m_pDbAdapter->GetDatabaseLayer(m_dbName);
    if (m_pDbLayer->IsOpen()) {
        // test for empty command string
        wxArrayString arrCmdLines = wxStringTokenize( m_scintillaSQL->GetText(), wxT("\n"), wxTOKEN_STRTOK );
        int cmdLines = 0;
        for( size_t i = 0; i < arrCmdLines.GetCount(); i++) {
            if( ! arrCmdLines[i].Trim(false).StartsWith( wxT("--") ) ) cmdLines++;
        }
        
        // save the history
        SaveSqlHistory();
        
        if ( cmdLines > 0 ) {
            try {
                m_colsMetaData.clear();
                if (!m_pDbAdapter->GetUseDb(m_dbName).IsEmpty()) m_pDbLayer->RunQuery(m_pDbAdapter->GetUseDb(m_dbName));
                // run query
                DatabaseResultSet* pResultSet = m_pDbLayer->RunQueryWithResults(m_scintillaSQL->GetText());

                // clear variables
                if(m_gridTable->GetNumberCols()) {
                    m_gridTable->DeleteCols(0, m_gridTable->GetNumberCols());

                }

                if(m_gridTable->GetNumberRows()) {
                    m_gridTable->DeleteRows(0, m_gridTable->GetNumberRows());
                }

                m_gridValues.clear();

                if( !pResultSet ) {
                    wxMessageBox( _("Unknown SQL error."), _("DB Error"), wxOK | wxICON_ERROR );
                    return;
                }

                int rows = 0;
                int cols = pResultSet->GetMetaData()->GetColumnCount();
                m_colsMetaData.resize(cols);
                
                // create table header
                m_gridTable->AppendCols(cols);
                for (int i = 1; i<= pResultSet->GetMetaData()->GetColumnCount(); i++) {
                    m_gridTable->SetColLabelValue(i-1,pResultSet->GetMetaData()->GetColumnName(i));
                    m_colsMetaData.at(i-1) = ColumnInfo(pResultSet->GetMetaData()->GetColumnType(i), pResultSet->GetMetaData()->GetColumnName(i) );
                }

                m_gridTable->BeginBatch();
                // fill table data
                while (pResultSet->Next()) {
                    wxString value;
                    m_gridTable->AppendRows();
                    for (int i = 1; i<= pResultSet->GetMetaData()->GetColumnCount(); i++) {

                        switch (pResultSet->GetMetaData()->GetColumnType(i)) {
                        case ResultSetMetaData::COLUMN_INTEGER:
                            if(m_pDbAdapter->GetAdapterType() == IDbAdapter::atSQLITE) {
                                value = pResultSet->GetResultString(i);

                            } else {
                                value = wxString::Format(wxT("%i"),pResultSet->GetResultInt(i));
                            }
                            break;

                        case ResultSetMetaData::COLUMN_STRING:
                            value =  pResultSet->GetResultString(i);
                            break;

                        case ResultSetMetaData::COLUMN_UNKNOWN:
                            value = pResultSet->GetResultString(i);
                            break;

                        case ResultSetMetaData::COLUMN_BLOB: {
                            if(textCols.find(i) != textCols.end()) {
                                // this column should be displayed as TEXT rather than BLOB
                                value = pResultSet->GetResultString(i);

                            } else if(blobCols.find(i) != blobCols.end()) {
                                // this column should be displayed as BLOB
                                wxMemoryBuffer buffer;
                                pResultSet->GetResultBlob(i, buffer);
                                value = wxString::Format(wxT("BLOB (Size:%u)"), buffer.GetDataLen());

                            } else {
                                // first time
                                wxString strCol = pResultSet->GetResultString(i);
                                if(IsBlobColumn(strCol)) {
                                    blobCols.insert(i);
                                    wxMemoryBuffer buffer;
                                    pResultSet->GetResultBlob(i, buffer);
                                    value = wxString::Format(wxT("BLOB (Size:%u)"), buffer.GetDataLen());

                                } else {
                                    textCols.insert(i);
                                    value = strCol;
                                }
                            }
                            break;
                        }
                        case ResultSetMetaData::COLUMN_BOOL:
                            value = wxString::Format(wxT("%b"),pResultSet->GetResultBool(i));
                            break;

                        case ResultSetMetaData::COLUMN_DATE: {
                            wxDateTime dt = pResultSet->GetResultDate(i);
                            if(dt.IsValid()) {
                                value = dt.Format();
                            } else {
                                value.Clear();
                            }
                        }
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

                        m_gridValues[std::make_pair(rows,  i-1)] = value;

                        // truncate the string to a reasonable string
                        if(value.Length() > 100) {
                            value = value.Mid(0, 100);
                            value.Append(wxT("..."));
                        }

                        // Convert all whitespace chars into visible ones
                        value.Replace(wxT("\n"), wxT("\\n"));
                        value.Replace(wxT("\r"), wxT("\\r"));
                        value.Replace(wxT("\t"), wxT("\\t"));
                        m_gridTable->SetCellValue(rows, i-1, value);
                    }
                    rows++;
                }

                m_gridTable->EndBatch();

                m_pDbLayer->CloseResultSet(pResultSet);

                // show result status
                m_labelStatus->SetLabel(wxString::Format(_("Result: %i rows"),rows));
                Layout();

                GetParent()->Layout();

            } catch (DatabaseLayerException& e) {
                // for some reason an exception is thrown even if the error code is 0...
                if(e.GetErrorCode() != 0) {
                    wxString errorMessage = wxString::Format(_("Error (%d): %s"), e.GetErrorCode(), e.GetErrorMessage().c_str());
                    wxMessageDialog dlg(this,errorMessage,_("DB Error"),wxOK | wxCENTER | wxICON_ERROR);
                    dlg.ShowModal();
                }

            } catch( ... ) {
                wxMessageDialog dlg(this,_("Unknown error."),_("DB Error"),wxOK | wxCENTER | wxICON_ERROR);
                dlg.ShowModal();

            }
        }

    } else {
        wxMessageBox(_("Cant connect!"));
    }
}

void SQLCommandPanel::OnLoadClick(wxCommandEvent& event)
{
    wxFileDialog dlg(this, _("Choose a file"),wxT(""),wxT(""),wxT("Sql files(*.sql)|*.sql"),wxFD_OPEN|wxFD_FILE_MUST_EXIST);
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
    wxFileDialog dlg(this,_("Chose a file"),wxT(""),wxT(""),wxT("Sql files(*.sql)|*.sql"),wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
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
void SQLCommandPanel::OnTemplatesBtnClick(wxAuiToolBarEvent& event)
{
    wxMenu menu;
    menu.Append(XRCID("IDR_SQLCOMMAND_SELECT"),_("Insert SELECT SQL template"),_("Insert SELECT SQL statement template into editor."));
    menu.Append(XRCID("IDR_SQLCOMMAND_INSERT"),_("Insert INSERT SQL template"),_("Insert INSERT SQL statement template into editor."));
    menu.Append(XRCID("IDR_SQLCOMMAND_UPDATE"),_("Insert UPDATE SQL template"),_("Insert UPDATE SQL statement template into editor."));
    menu.Append(XRCID("IDR_SQLCOMMAND_DELETE"),_("Insert DELETE SQL template"),_("Insert DELETE SQL statement template into editor."));
    menu.Connect(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&SQLCommandPanel::OnPopupClick, NULL, this);

    wxAuiToolBar* auibar = dynamic_cast<wxAuiToolBar*>(event.GetEventObject());
    if ( auibar ) {
        clAuiToolStickness ts(auibar, event.GetToolId());
        wxRect rect = auibar->GetToolRect(event.GetId());
        wxPoint pt = auibar->ClientToScreen(rect.GetBottomLeft());
        pt = ScreenToClient(pt);
        PopupMenu(&menu, pt);
    }
}

void SQLCommandPanel::OnPopupClick(wxCommandEvent& evt)
{
    wxString text = m_scintillaSQL->GetText();
    text.Trim().Trim(false);

    text.Append(wxT("\n"));

    if (evt.GetId() == XRCID("IDR_SQLCOMMAND_SELECT")) {
        text << wxT("SELECT * FROM TableName\n");

    } else	if (evt.GetId() == XRCID("IDR_SQLCOMMAND_INSERT")) {
        text << wxT("INSERT INTO TableName (ColumnA, ColumnB) VALUES (1,'Test text')\n");

    } else	if (evt.GetId() == XRCID("IDR_SQLCOMMAND_UPDATE")) {
        text <<  wxT("UPDATE TableName SET ColumnA = 2, ColumnB = 'Second text' WHERE ID = 1\n");

    } else	if (evt.GetId() == XRCID("IDR_SQLCOMMAND_DELETE")) {
        text << wxT("DELETE FROM TableName WHERE ID = 1\n");
    }

    m_scintillaSQL->SetText(text);
    m_scintillaSQL->SetSelectionStart(m_scintillaSQL->GetLength() - 1);
    m_scintillaSQL->SetSelectionEnd  (m_scintillaSQL->GetLength() - 1);
    m_scintillaSQL->SetFocus();
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
    if(m_cellValue.IsEmpty() == false) {
        CopyToClipboard(m_cellValue);
    }
}

void SQLCommandPanel::OnEdit(wxCommandEvent& e)
{
    if(wxWindow::FindFocus() == m_scintillaSQL) {
        switch(e.GetId()) {
        case wxID_REDO:
            if(m_scintillaSQL->CanRedo()) {
                m_scintillaSQL->Redo();
            }
            break;

        case wxID_UNDO:
            if(m_scintillaSQL->CanUndo()) {
                m_scintillaSQL->Undo();
            }
            break;

        case wxID_CUT:
            if((m_scintillaSQL->GetSelectionStart() - m_scintillaSQL->GetSelectionEnd()) != 0) {
                m_scintillaSQL->Cut();
            }
            break;
        case wxID_COPY:
            if((m_scintillaSQL->GetSelectionStart() - m_scintillaSQL->GetSelectionEnd()) != 0) {
                m_scintillaSQL->Copy();
            }
            break;
        case wxID_PASTE:
            if(m_scintillaSQL->CanPaste()) {
                m_scintillaSQL->Paste();
            }
            break;
        case wxID_SELECTALL:
            m_scintillaSQL->SelectAll();
            break;
        }
    } else {
        e.Skip();

    }
}

void SQLCommandPanel::OnEditUI(wxUpdateUIEvent& e)
{
    wxUnusedVar(e);
//	if(wxWindow::FindFocus() == m_scintillaSQL) {
//		bool enable = false;
//		switch(e.GetId()) {
//		case wxID_REDO:
//			enable = m_scintillaSQL->CanRedo();
//			break;
//
//		case wxID_UNDO:
//			enable = m_scintillaSQL->CanUndo();
//			break;
//
//		case wxID_COPY:
//		case wxID_CUT:
//			enable = ((m_scintillaSQL->GetSelectionStart() - m_scintillaSQL->GetSelectionEnd()) != 0);
//			break;
//
//		case wxID_PASTE:
//			enable = m_scintillaSQL->CanPaste();
//			break;
//
//		case wxID_SELECTALL:
//			enable = m_scintillaSQL->GetLength() > 0;
//			break;
//		default:
//			break;
//		}
//		e.Enable(enable);
//		e.StopPropagation();
//	} else {
//		e.Skip();
//	}
}

void SQLCommandPanel::OnGridLabelRightClick(wxGridEvent& event)
{
    event.Skip();
}

bool SQLCommandPanel::IsBlobColumn(const wxString &str)
{
    for(size_t i=0; i<str.Len(); i++) {
        if(!wxIsprint(str.GetChar(i))) {
            return true;
        }
    }
    return false;
}

void SQLCommandPanel::SetDefaultSelect()
{
    m_scintillaSQL->ClearAll();
    m_scintillaSQL->AddText(wxString::Format(wxT(" -- selected database %s\n"), m_dbName.c_str()));
    if (!m_dbTable.IsEmpty()) {
        m_scintillaSQL->AddText(m_pDbAdapter->GetDefaultSelect(m_dbName, m_dbTable));
        CallAfter( &SQLCommandPanel::ExecuteSql );
    }
}

void SQLCommandPanel::OnHistoryToolClicked(wxAuiToolBarEvent& event)
{
    wxAuiToolBar* auibar = dynamic_cast<wxAuiToolBar*>(event.GetEventObject());
    if ( auibar ) {
        clAuiToolStickness ts(auibar, event.GetToolId());
        wxRect rect = auibar->GetToolRect(event.GetId());
        wxPoint pt = auibar->ClientToScreen(rect.GetBottomLeft());
        pt = ScreenToClient(pt);
        
        DbExplorerSettings settings;
        clConfig conf(DBE_CONFIG_FILE);
        conf.ReadItem(&settings);
        settings.GetRecentFiles();
        
        wxArrayString sqls = settings.GetSqlHistory();
        wxMenu menu;
        for(size_t i=0; i<sqls.GetCount(); ++i) {
            menu.Append(wxID_HIGHEST+i, sqls.Item(i));
        }
        
        int pos = GetPopupMenuSelectionFromUser(menu, pt);
        if ( pos == wxID_NONE )
            return;
        
        size_t index = pos - wxID_HIGHEST;
        if ( index > sqls.GetCount() )
            return;
        
        m_scintillaSQL->SetText( sqls.Item(index) );
        CallAfter( &SQLCommandPanel::ExecuteSql );
    }
}

wxArrayString SQLCommandPanel::ParseSql(const wxString& sql) const
{
    // filter out comments
    wxString noCommentsSql;
    wxArrayString lines = ::wxStringTokenize(sql, "\n", wxTOKEN_STRTOK);
    for(size_t i=0; i<lines.GetCount(); ++i) {
        lines.Item(i).Trim().Trim(false);
        if ( lines.Item(i).StartsWith("--") ) {
            continue;
        }
        noCommentsSql << lines.Item(i) << "\n";
    }
    
    // Split by semi-colon
    wxArrayString tmpSqls = ::wxStringTokenize(noCommentsSql, ";", wxTOKEN_STRTOK);
    wxArrayString sqls;
    for(size_t i=0; i<tmpSqls.GetCount(); ++i) {
        
        wxString sql = tmpSqls.Item(i);
        sql.Trim().Trim(false);
        if ( sql.IsEmpty() )
            continue;
            
        sql.Replace("\n", " ");
        sql.Replace("\r", "");
        sqls.Add( sql );
    }
    return sqls;
}

void SQLCommandPanel::SaveSqlHistory()
{
    wxArrayString sqls = ParseSql( m_scintillaSQL->GetText() );
    if ( sqls.IsEmpty() )
        return;
    
    DbExplorerSettings s;
    clConfig conf(DBE_CONFIG_FILE);
    conf.ReadItem( &s );
    const wxArrayString &history = s.GetSqlHistory();
    
    // Append the current history to the new sqls (exclude dups)
    for(size_t i=0; i<history.GetCount(); ++i) {
        if ( sqls.Index(history.Item(i) ) == wxNOT_FOUND ) {
            sqls.Add( history.Item(i) );
        }
    }

    // Truncate the buffer
    while( sqls.GetCount() > 15 ) {
        sqls.RemoveAt(sqls.GetCount()-1);
    }
    
    s.SetSqlHistory( sqls );
    conf.WriteItem( &s );
}
