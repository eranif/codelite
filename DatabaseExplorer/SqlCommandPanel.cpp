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
#include "SqlCommandPanel.h"
#include "bitmap_loader.h"
#include "clKeyboardManager.h"
#include "clStatusBarMessage.h"
#include "cl_aui_tool_stickness.h"
#include "cl_defs.h"
#include "db_explorer_settings.h"
#include "editor_config.h"
#include "globals.h"
#include "imanager.h"
#include "lexer_configuration.h"
#include <algorithm>
#include <set>
#include <wx/busyinfo.h>
#include <wx/file.h>
#include <wx/textfile.h>
#include <wx/wupdlock.h>
#include <wx/xrc/xmlres.h>

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

SQLCommandPanel::SQLCommandPanel(wxWindow* parent, IDbAdapter* dbAdapter, const wxString& dbName,
                                 const wxString& dbTable)
    : _SqlCommandPanel(parent)
{
    LexerConf::Ptr_t lexerSQL = EditorConfigST::Get()->GetLexer("SQL");
    if(lexerSQL) {
        lexerSQL->Apply(m_scintillaSQL, true);

        // determine how an operator and a comment are styled
        auto lexerProperties = lexerSQL->GetLexerProperties();
        auto operatorStyle =
            std::find_if(lexerProperties.begin(), lexerProperties.end(), StyleProperty::FindByName("Operator"));
        auto commentStyle =
            std::find_if(lexerProperties.begin(), lexerProperties.end(), StyleProperty::FindByName("Comment block"));

        if(std::end(lexerProperties) != operatorStyle) {
            m_OperatorStyle = operatorStyle->second.GetId();
        }
        if(std::end(lexerProperties) != commentStyle) {
            m_CommentStyle = commentStyle->second.GetId();
        }
    } else {
        DbViewerPanel::InitStyledTextCtrl(m_scintillaSQL);
    }
    m_pDbAdapter = dbAdapter;
    m_dbName = dbName;
    m_dbTable = dbTable;

    m_editHelper.Reset(new clEditEventsHandler(m_scintillaSQL));
    m_scintillaSQL->AddText(wxString::Format(_(" -- selected database %s\n"), m_dbName.c_str()));
    if(!dbTable.IsEmpty()) {
        m_scintillaSQL->AddText(m_pDbAdapter->GetDefaultSelect(m_dbName, m_dbTable));
        wxCommandEvent event(wxEVT_EXECUTE_SQL);
        GetEventHandler()->AddPendingEvent(event);
    }

    m_toolbar = new clToolBar(this);
    auto images = m_toolbar->GetBitmapsCreateIfNeeded();
    m_toolbar->AddTool(wxID_OPEN, _("Load SQL Script"), images->Add("file_open"));
    m_toolbar->AddTool(wxID_EXECUTE, _("Execute SQL"), images->Add("execute"));
    m_toolbar->Realize();
    GetSizer()->Insert(0, m_toolbar, 0, wxEXPAND);

    // Bind events
    m_toolbar->Bind(wxEVT_TOOL, &SQLCommandPanel::OnExecuteClick, this, wxID_EXECUTE);
    m_toolbar->Bind(wxEVT_TOOL, &SQLCommandPanel::OnLoadClick, this, wxID_OPEN);
}

SQLCommandPanel::~SQLCommandPanel() { wxDELETE(m_pDbAdapter); }

void SQLCommandPanel::OnExecuteClick(wxCommandEvent& event) { ExecuteSql(); }

void SQLCommandPanel::OnScintilaKeyDown(wxKeyEvent& event)
{
    if((event.ControlDown()) && (event.GetKeyCode() == WXK_RETURN || event.GetKeyCode() == WXK_NUMPAD_ENTER)) {
        ExecuteSql();
    }
    event.Skip();
}

void SQLCommandPanel::ExecuteSql()
{
    clStatusBarMessage message(_("Executing SQL..."));

    clWindowUpdateLocker locker(this);
    std::set<int> textCols;
    std::set<int> blobCols;
    DatabaseLayerPtr m_pDbLayer = m_pDbAdapter->GetDatabaseLayer(m_dbName);
    if(m_pDbLayer->IsOpen()) {
        // build string of SQL statements with comments removed
        wxArrayString sqls = ParseSql();
        wxString sqlStmt = "";
        for(size_t i = 0; i < sqls.GetCount(); i++) {
            sqlStmt += sqls[i];
        }

        // save the history
        SaveSqlHistory(sqls);

        if(!sqls.IsEmpty()) {
            try {
                m_colsMetaData.clear();
                if(!m_pDbAdapter->GetUseDb(m_dbName).IsEmpty())
                    m_pDbLayer->RunQuery(m_pDbAdapter->GetUseDb(m_dbName));
                // run query
                DatabaseResultSet* pResultSet = m_pDbLayer->RunQueryWithResults(sqlStmt);

                m_table->ClearAll();
                if(!pResultSet) {
                    wxMessageBox(_("Unknown SQL error."), _("DB Error"), wxOK | wxICON_ERROR);
                    return;
                }

                int cols = pResultSet->GetMetaData()->GetColumnCount();
                m_colsMetaData.resize(cols);

                // create table header
                wxArrayString columns;
                for(int i = 1; i <= pResultSet->GetMetaData()->GetColumnCount(); i++) {
                    columns.Add(pResultSet->GetMetaData()->GetColumnName(i));
                    m_colsMetaData.at(i - 1) = ColumnInfo(pResultSet->GetMetaData()->GetColumnType(i),
                                                          pResultSet->GetMetaData()->GetColumnName(i));
                }
                m_table->SetColumns(columns);

                // fill table data
                std::vector<wxArrayString> data;
                while(pResultSet->Next()) {
                    data.push_back(wxArrayString());
                    wxArrayString& row = data.back();
                    wxString value;
                    for(int i = 1; i <= pResultSet->GetMetaData()->GetColumnCount(); i++) {

                        switch(pResultSet->GetMetaData()->GetColumnType(i)) {
                        case ResultSetMetaData::COLUMN_INTEGER:
                            if(m_pDbAdapter->GetAdapterType() == IDbAdapter::atSQLITE) {
                                value = pResultSet->GetResultString(i);

                            } else {
                                value = wxString::Format(wxT("%i"), pResultSet->GetResultInt(i));
                            }
                            break;

                        case ResultSetMetaData::COLUMN_STRING:
                            value = pResultSet->GetResultString(i);
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
                            value = wxString::Format(wxT("%b"), pResultSet->GetResultBool(i));
                            break;

                        case ResultSetMetaData::COLUMN_DATE: {
                            wxDateTime dt = pResultSet->GetResultDate(i);
                            if(dt.IsValid()) {
                                value = dt.Format();
                            } else {
                                value.Clear();
                            }
                        } break;

                        case ResultSetMetaData::COLUMN_DOUBLE:
                            value = wxString::Format(wxT("%f"), pResultSet->GetResultDouble(i));
                            break;

                        case ResultSetMetaData::COLUMN_NULL:
                            value = wxT("NULL");
                            break;

                        default:
                            value = pResultSet->GetResultString(i);
                            break;
                        }
                        row.Add(value);
                    }
                }
                m_pDbLayer->CloseResultSet(pResultSet);

                // Popuplate the data
                m_table->SetData(data);
                GetSizer()->Layout();
                Layout();

            } catch(DatabaseLayerException& e) {
                // for some reason an exception is thrown even if the error code is 0...
                if(e.GetErrorCode() != 0) {
                    wxString errorMessage =
                        wxString::Format(_("Error (%d): %s"), e.GetErrorCode(), e.GetErrorMessage().c_str());
                    wxMessageDialog dlg(this, errorMessage, _("DB Error"), wxOK | wxCENTER | wxICON_ERROR);
                    dlg.ShowModal();
                }

            } catch(...) {
                wxMessageDialog dlg(this, _("Unknown error."), _("DB Error"), wxOK | wxCENTER | wxICON_ERROR);
                dlg.ShowModal();
            }
        }

    } else {
        wxMessageBox(_("Cant connect!"));
    }
}

void SQLCommandPanel::OnLoadClick(wxCommandEvent& event)
{
    wxFileDialog dlg(this, _("Choose a file"), wxT(""), wxT(""), wxT("Sql files(*.sql)|*.sql"),
                     wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    m_scintillaSQL->ClearAll();
    if(dlg.ShowModal() == wxID_OK) {
        wxTextFile file(dlg.GetPath());
        file.Open();
        if(file.IsOpened()) {
            for(wxString str = file.GetFirstLine(); !file.Eof(); str = file.GetNextLine()) {
                m_scintillaSQL->AddText(str);
                m_scintillaSQL->AddText(wxT("\n"));
            }
        }
    }
}

void SQLCommandPanel::OnSaveClick(wxCommandEvent& event)
{
    wxFileDialog dlg(this, _("Chose a file"), wxT(""), wxT(""), wxT("Sql files(*.sql)|*.sql"),
                     wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if(dlg.ShowModal() == wxID_OK) {

        wxFile file(dlg.GetPath(), wxFile::write);
        if(file.IsOpened()) {
            file.Write(m_scintillaSQL->GetText());
            file.Close();
        }
    }
}

void SQLCommandPanel::OnTeplatesLeftDown(wxMouseEvent& event) {}
void SQLCommandPanel::OnTemplatesBtnClick(wxAuiToolBarEvent& event)
{
    wxMenu menu;
    menu.Append(XRCID("IDR_SQLCOMMAND_SELECT"), _("Insert SELECT SQL template"),
                _("Insert SELECT SQL statement template into editor."));
    menu.Append(XRCID("IDR_SQLCOMMAND_INSERT"), _("Insert INSERT SQL template"),
                _("Insert INSERT SQL statement template into editor."));
    menu.Append(XRCID("IDR_SQLCOMMAND_UPDATE"), _("Insert UPDATE SQL template"),
                _("Insert UPDATE SQL statement template into editor."));
    menu.Append(XRCID("IDR_SQLCOMMAND_DELETE"), _("Insert DELETE SQL template"),
                _("Insert DELETE SQL statement template into editor."));
    menu.Connect(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&SQLCommandPanel::OnPopupClick, NULL, this);

    wxAuiToolBar* auibar = dynamic_cast<wxAuiToolBar*>(event.GetEventObject());
    if(auibar) {
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

    if(evt.GetId() == XRCID("IDR_SQLCOMMAND_SELECT")) {
        text << wxT("SELECT * FROM TableName\n");

    } else if(evt.GetId() == XRCID("IDR_SQLCOMMAND_INSERT")) {
        text << wxT("INSERT INTO TableName (ColumnA, ColumnB) VALUES (1,'Test text')\n");

    } else if(evt.GetId() == XRCID("IDR_SQLCOMMAND_UPDATE")) {
        text << wxT("UPDATE TableName SET ColumnA = 2, ColumnB = 'Second text' WHERE ID = 1\n");

    } else if(evt.GetId() == XRCID("IDR_SQLCOMMAND_DELETE")) {
        text << wxT("DELETE FROM TableName WHERE ID = 1\n");
    }

    m_scintillaSQL->SetText(text);
    m_scintillaSQL->SetSelectionStart(m_scintillaSQL->GetLength() - 1);
    m_scintillaSQL->SetSelectionEnd(m_scintillaSQL->GetLength() - 1);
    m_scintillaSQL->SetFocus();
}

void SQLCommandPanel::OnExecuteSQL(wxCommandEvent& e)
{
    wxUnusedVar(e);
    ExecuteSql();
}

void SQLCommandPanel::OnCopyCellValue(wxCommandEvent& e)
{
    if(m_cellValue.IsEmpty() == false) {
        CopyToClipboard(m_cellValue);
    }
}

bool SQLCommandPanel::IsBlobColumn(const wxString& str)
{
    for(size_t i = 0; i < str.Len(); i++) {
        if(!wxIsprint(str.GetChar(i))) {
            return true;
        }
    }
    return false;
}

void SQLCommandPanel::SetDefaultSelect()
{
    m_scintillaSQL->ClearAll();
    m_scintillaSQL->AddText(wxString::Format(_(" -- selected database %s\n"), m_dbName.c_str()));
    if(!m_dbTable.IsEmpty()) {
        m_scintillaSQL->AddText(m_pDbAdapter->GetDefaultSelect(m_dbName, m_dbTable));
        CallAfter(&SQLCommandPanel::ExecuteSql);
    }
}

void SQLCommandPanel::OnHistoryToolClicked(wxAuiToolBarEvent& event)
{
    wxAuiToolBar* auibar = dynamic_cast<wxAuiToolBar*>(event.GetEventObject());
    if(auibar) {
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
        for(size_t i = 0; i < sqls.GetCount(); ++i) {
            menu.Append(wxID_HIGHEST + i, sqls.Item(i));
        }

        int pos = GetPopupMenuSelectionFromUser(menu, pt);
        if(pos == wxID_NONE)
            return;

        size_t index = pos - wxID_HIGHEST;
        if(index > sqls.GetCount())
            return;

        m_scintillaSQL->SetText(sqls.Item(index));
        CallAfter(&SQLCommandPanel::ExecuteSql);
    }
}

wxArrayString SQLCommandPanel::ParseSql() const
{
    const char SEMICOLON = ';';
    const char SPACE = ' ';

    wxMemoryBuffer styledText = m_scintillaSQL->GetStyledText(0, m_scintillaSQL->GetLength());
    auto bufSize = styledText.GetDataLen();
    char* pStyledTextBuf = static_cast<char*>(styledText.GetData());

    int startPos = 0;
    int stopPos = 0;
    wxString currStmt = "";

    char currChar;
    char currStyle;

    wxArrayString sqls;
    bool bAdded = true;

    for(size_t index = 0; index < bufSize; index += 2) {

        currChar = pStyledTextBuf[index];
        currStyle = pStyledTextBuf[index + 1];

        // eat comments
        if(m_CommentStyle == currStyle) {

            // copy the string previous to the comments
            currStmt += m_scintillaSQL->GetTextRange(startPos, stopPos);
            // replace the comments with a space
            currStmt += SPACE;
            while((m_CommentStyle == currStyle || std::isspace(currChar)) && index < bufSize) {
                index += 2;
                currChar = pStyledTextBuf[index];
                currStyle = pStyledTextBuf[index + 1];
                stopPos++;
            }
            startPos = stopPos;
        }

        // non-comment, valid character
        if(m_CommentStyle != currStyle && 0 != currChar) {
            stopPos++;
            bAdded = false;
        }

        // found an operator semi-colon to mark end of statement
        if(m_OperatorStyle == currStyle && SEMICOLON == currChar) {
            currStmt += m_scintillaSQL->GetTextRange(startPos, stopPos);

            currStmt.Trim(false);
            currStmt.Trim();
            if(currStmt.length() != 0) {
                sqls.Add(currStmt);
                currStmt.clear();
                bAdded = true;
            }
            startPos = stopPos;
            stopPos = startPos;
        }
    }

    // in case the last statement did not end in a semicolon
    if(!bAdded) {
        currStmt += m_scintillaSQL->GetTextRange(startPos, stopPos);

        currStmt.Trim(false);
        currStmt.Trim();
        if(currStmt.length() != 0) {
            sqls.Add(currStmt);
        }
    }
    return sqls;
}

void SQLCommandPanel::SaveSqlHistory(wxArrayString sqls)
{
    if(sqls.IsEmpty())
        return;

    DbExplorerSettings s;
    clConfig conf(DBE_CONFIG_FILE);
    conf.ReadItem(&s);
    const wxArrayString& history = s.GetSqlHistory();

    // Append the current history to the new sqls (exclude dups)
    for(size_t i = 0; i < history.GetCount(); ++i) {
        if(sqls.Index(history.Item(i)) == wxNOT_FOUND) {
            sqls.Add(history.Item(i));
        }
    }

    // Truncate the buffer
    while(sqls.GetCount() > 15) {
        sqls.RemoveAt(sqls.GetCount() - 1);
    }

    s.SetSqlHistory(sqls);
    conf.WriteItem(&s);
}
