//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : ClassGenerateDialog.cpp
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

#include "ClassGenerateDialog.h"
#include "event_notifier.h"
#include <wx/xrc/xmlres.h>
#include <wx/tokenzr.h>
#include "globals.h"
#include "cl_command_event.h"
#include "codelite_events.h"

ClassGenerateDialog::ClassGenerateDialog(
    wxWindow* parent, IDbAdapter* dbAdapter, xsSerializable* pItems, IManager* pMgr)
    : _ClassGenerateDialog(parent)
{
    m_pDbAdapter = dbAdapter;
    m_pItems = pItems;
    m_mgr = pMgr;

    m_mapTemplateFiles[wxT("DatabaseLayer (wxWidgets)")] =
        wxT("dataClass_dbl.htmp;dataClass_dbl.ctmp;viewClass_dbl.htmp;viewClass_dbl.ctmp");
    m_mapTemplateFiles[wxT("Debea (STL)")] =
        wxT("dataClass_dba.htmp;dataClass_dba.ctmp;viewClass_dba.htmp;viewClass_dba.ctmp");
    m_mapTemplateFiles[wxT("wxDebea (wxWidgets)")] =
        wxT("dataClass_wxdba.htmp;dataClass_wxdba.ctmp;viewClass_wxdba.htmp;viewClass_wxdba.ctmp");
    m_mapTemplateFiles[wxT("Generic classes (STL)")] =
        wxT("dataClass.htmp;dataClass.ctmp;viewClass.htmp;viewClass.ctmp");

    TreeItemInfo item = m_mgr->GetSelectedTreeItemInfo(TreeFileView);
    if(item.m_item.IsOk() && item.m_itemType == ProjectItem::TypeVirtualDirectory) {
        m_txVirtualDir->SetValue(
            VirtualDirectorySelectorDlg::DoGetPath(m_mgr->GetWorkspaceTree(), item.m_item, false));
    }
}

ClassGenerateDialog::~ClassGenerateDialog() { Destroy(); }

bool ClassGenerateDialog::GenerateClass(Table* pTab, const wxString& path)
{
    wxString hFileName = wxT("");
    wxString cFileName = wxT("");

    wxArrayString arrFileNames =
        wxStringTokenize(m_mapTemplateFiles[m_choiceTemplates->GetStringSelection()], wxT(";"), wxTOKEN_RET_EMPTY);

    if(pTab->IsView()) {
        hFileName = arrFileNames[2];
        cFileName = arrFileNames[3];

    } else {
        hFileName = arrFileNames[0];
        cFileName = arrFileNames[1];
    }

    wxTextFile htmpFile(m_mgr->GetInstallDirectory() + wxT("/templates/databaselayer/") + hFileName);
    wxTextFile ctmpFile(m_mgr->GetInstallDirectory() + wxT("/templates/databaselayer/") + cFileName);

    if(!htmpFile.Open() || !ctmpFile.Open()) return false;

    classTableName = pTab->GetName();
    classItemName = m_txPrefix->GetValue() + pTab->GetName() + m_txPostfix->GetValue();
    classItemDef = wxT("__") + classItemName.Upper() + wxT("_H__");
    classColName = m_txPrefix->GetValue() + pTab->GetName() + wxT("Collection") + m_txPostfix->GetValue();
    classUtilName = m_txPrefix->GetValue() + pTab->GetName() + wxT("Utils") + m_txPostfix->GetValue();

    wxString hFile;
    wxFileName fnHeaderFileName(path + wxT("/") + classItemName + wxT(".h"));

    wxString cFile;
    wxFileName fnCppFileName(path + wxT("/") + classItemName + wxT(".cpp"));

    bool suc =
        GenerateFile(pTab, htmpFile, hFile, classItemName, classItemDef, classColName, classTableName, classUtilName);
    suc &=
        GenerateFile(pTab, ctmpFile, cFile, classItemName, classItemDef, classColName, classTableName, classUtilName);

    htmpFile.Close();
    ctmpFile.Close();

    // format output files
    FormatFile(hFile, fnHeaderFileName);
    FormatFile(cFile, fnCppFileName);

    ::WriteFileWithBackup(fnCppFileName.GetFullPath(), cFile, false);
    ::WriteFileWithBackup(fnHeaderFileName.GetFullPath(), hFile, false);

    // add files to the workspace
    wxArrayString arrString;
    arrString.Add(path + wxT("/") + classItemName + wxT(".h"));
    arrString.Add(path + wxT("/") + classItemName + wxT(".cpp"));

    m_mgr->AddFilesToVirtualFolder(m_txVirtualDir->GetValue(), arrString);

    // retag workspace
    wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, XRCID("retag_workspace"));
    EventNotifier::Get()->TopFrame()->GetEventHandler()->AddPendingEvent(evt);

    return suc;
}

void ClassGenerateDialog::OnCancelClick(wxCommandEvent& event) { Destroy(); }

void ClassGenerateDialog::OnGenerateClick(wxCommandEvent& event)
{
    if(m_txVirtualDir->GetValue().IsEmpty()) {
        wxMessageBox(_("Virtual name cannot be empty"), _("CodeLite"), wxICON_WARNING | wxOK);
        m_txVirtualDir->SetFocus();
        return;
    }
    if(m_dirPicker->GetPath().IsEmpty()) {
        wxMessageBox(_("Folder name cannot be empty"), _("CodeLite"), wxICON_WARNING | wxOK);
        m_dirPicker->SetFocus();
    }

    m_textLog->Clear();

    wxString err_msg;
    wxString project = m_txVirtualDir->GetValue().BeforeFirst(wxT(':'));
    ProjectPtr proj = m_mgr->GetWorkspace()->FindProjectByName(project, err_msg);
    if(proj) {
        wxString filePath = m_dirPicker->GetPath(); // proj->GetFileName().GetPath();

        Table* pTable = wxDynamicCast(m_pItems, Table);

        if(pTable) {
            if(GenerateClass(pTable, filePath))
                m_textLog->AppendText(pTable->GetName() + _("......... Generated successfully!\n"));
            else
                m_textLog->AppendText(pTable->GetName() + _("......... Error!!!\n"));
        } else {
            SerializableList::compatibility_iterator node = m_pItems->GetFirstChildNode();
            while(node) {
                Table* pTab = wxDynamicCast(node->GetData(), Table);
                if(pTab) {
                    if(GenerateClass(pTab, filePath))
                        m_textLog->AppendText(pTab->GetName() + _("......... Generated successfully!\n"));
                    else
                        m_textLog->AppendText(pTab->GetName() + _("......... Error!!!\n"));
                }

                node = node->GetNext();
            }
        }
        wxCommandEvent e(wxEVT_COMMAND_MENU_SELECTED, XRCID("retag_workspace"));
        m_mgr->GetTheApp()->GetTopWindow()->GetEventHandler()->AddPendingEvent(e);
    }
}

bool ClassGenerateDialog::GenerateFile(Table* pTab, wxTextFile& htmpFile, wxString& hFile,
    const wxString& classItemName, const wxString& classItemDef, const wxString& classColName,
    const wxString& classTableName, const wxString& classUtilName)
{
    Constraint* pPK = NULL;
    int colCount = 0;
    int lastEditParam = 0;

    SerializableList::compatibility_iterator node = pTab->GetFirstChildNode();
    while(node) {
        Constraint* pConstr = wxDynamicCast(node->GetData(), Constraint);
        if(pConstr) {
            if(pConstr->GetType() == Constraint::primaryKey) pPK = pConstr;
        } else
            colCount++;
        node = node->GetNext();
    }
    Column* pPKCol = NULL;

    if(pPK) {
        SerializableList::compatibility_iterator node = pTab->GetFirstChildNode();
        while(node) {
            Column* pCol = wxDynamicCast(node->GetData(), Column);
            if(pCol) {
                if(pCol->GetName() == pPK->GetLocalColumn()) pPKCol = pCol;
            }
            node = node->GetNext();
        }
    }

    if((pPKCol == NULL) && (pTab->IsView() == false)) {
        m_textLog->AppendText(wxString::Format(_("Table %s has no primary key defined!\n"), pTab->GetName().c_str()));
        return false;
    }

    for(wxString str = htmpFile.GetFirstLine(); !htmpFile.Eof(); str = htmpFile.GetNextLine()) {
        if(str.Contains(wxT("%%classItemGetters%%"))) {
            SerializableList::compatibility_iterator node = pTab->GetFirstChildNode();
            while(node) {
                Column* pCol = wxDynamicCast(node->GetData(), Column);
                if(pCol) {
                    hFile << wxString::Format(wxT("\tconst %s Get%s() const {"),
                                 GetResTypeName(pCol->GetType()->GetUniversalType()).c_str(), pCol->GetName().c_str())
                          << "\n";
                    hFile << wxString::Format(wxT("\t\treturn m_%s;"), pCol->GetName().c_str()) << "\n";
                    hFile << wxString::Format(wxT("\t\t}")) << "\n";
                }
                node = node->GetNext();
            }

        } else if(str.Contains(wxT("%%classItemVariables%%"))) {
            SerializableList::compatibility_iterator node = pTab->GetFirstChildNode();
            while(node) {
                Column* pCol = wxDynamicCast(node->GetData(), Column);
                if(pCol) {
                    hFile << wxString::Format(wxT("\t%s m_%s;"),
                                 GetTypeName(pCol->GetType()->GetUniversalType()).c_str(), pCol->GetName().c_str())
                          << "\n";
                }
                node = node->GetNext();
            }

        } else if(str.Contains(wxT("%%classItemLoading%%"))) {
            SerializableList::compatibility_iterator node = pTab->GetFirstChildNode();
            while(node) {
                Column* pCol = wxDynamicCast(node->GetData(), Column);
                if(pCol) {
                    hFile << wxString::Format(wxT("\t\tm_%s = pResult->%s(wxT(\"%s\"));"), pCol->GetName().c_str(),
                                 GetResultFunction(pCol->GetType()->GetUniversalType()).c_str(),
                                 pCol->GetName().c_str())
                          << "\n";
                }
                node = node->GetNext();
            }

        } else if(str.Contains(wxT("%%classItemBindings%%"))) {
            SerializableList::compatibility_iterator node = pTab->GetFirstChildNode();
            while(node) {
                Column* pCol = wxDynamicCast(node->GetData(), Column);
                if(pCol) {
                    hFile << GetDebeaBinding(pCol) << "\n";
                }
                node = node->GetNext();
            }

        } else if(str.Contains(wxT("%%classItemAddParameters%%"))) {
            bool first = true;
            SerializableList::compatibility_iterator node = pTab->GetFirstChildNode();
            while(node) {
                Column* pCol = wxDynamicCast(node->GetData(), Column);
                if(pCol) {
                    if(first) {
                        hFile << wxString::Format(wxT("\t\t\t%s %s"),
                                     GetParamTypeName(pCol->GetType()->GetUniversalType()).c_str(),
                                     pCol->GetName().c_str())
                              << "\n";
                        first = false;
                    } else {
                        hFile << wxString::Format(wxT("\t\t\t,%s %s"),
                                     GetParamTypeName(pCol->GetType()->GetUniversalType()).c_str(),
                                     pCol->GetName().c_str())
                              << "\n";
                    }
                }
                node = node->GetNext();
            }

        } else if(str.Contains(wxT("%%classItemSetParams%%"))) {
            SerializableList::compatibility_iterator node = pTab->GetFirstChildNode();
            while(node) {
                Column* pCol = wxDynamicCast(node->GetData(), Column);
                if(pCol) {
                    hFile << wxT("\tm_") + pCol->GetName() + wxT(" = ") + pCol->GetName() + wxT(";") << "\n";
                }
                node = node->GetNext();
            }

        } else if(str.Contains(wxT("%%classColLabelFillGrid%%"))) {
            int i = 0;
            SerializableList::compatibility_iterator node = pTab->GetFirstChildNode();
            while(node) {
                Column* pCol = wxDynamicCast(node->GetData(), Column);
                if(pCol) {
                    hFile << wxT("\t\tpGrid->AppendCols(1);") << "\n";
                    hFile << wxString::Format(
                                 wxT("\t\tpGrid->SetColLabelValue(%i,wxT(\"%s\"));"), i++, pCol->GetName().c_str())
                          << "\n";
                }
                node = node->GetNext();
            }

        } else if(str.Contains(wxT("%%classColDataFillGrid%%"))) {
            int i = 0;
            SerializableList::compatibility_iterator node = pTab->GetFirstChildNode();
            while(node) {
                Column* pCol = wxDynamicCast(node->GetData(), Column);
                if(pCol) {
                    hFile << GetFillData(pCol, i++) << "\n";
                }
                node = node->GetNext();
            }

        } else if(str.Contains(wxT("%%primaryKeyHeader%%"))) {
            if(pPKCol) {
                hFile << wxString::Format(wxT("\t/*! \\brief Return %s from db on the %s base */"),
                             pPKCol->GetParentName().c_str(), pPKCol->GetName().c_str())
                      << "\n";
                hFile << wxString::Format(wxT("\tstatic %s* GetBy%s(%s %s, DatabaseLayer* pDbLayer);"),
                             classItemName.c_str(), pPKCol->GetName().c_str(),
                             GetTypeName(pPKCol->GetType()->GetUniversalType()).c_str(), pPKCol->GetName().c_str())
                      << "\n";
            }

        } else if(str.Contains(wxT("%%primaryKeyBody%%"))) {
            if(pPKCol) {
                hFile << wxString::Format(wxT("%s* %s::GetBy%s(%s %s, DatabaseLayer* pDbLayer)"), classItemName.c_str(),
                             classItemName.c_str(), pPKCol->GetName().c_str(),
                             GetTypeName(pPKCol->GetType()->GetUniversalType()).c_str(), pPKCol->GetName().c_str())
                      << "\n";
                hFile << wxT("{") << "\n";
                hFile << wxT("\tDatabaseResultSet* resSet = NULL;") << "\n";
                hFile << wxT("\tPreparedStatement* pStatement = NULL;") << "\n";
                hFile << wxT("\tif (pDbLayer){") << "\n";
                hFile << wxT("\t\tif (pDbLayer->IsOpen()){") << "\n";

                hFile << wxString::Format(wxT("\t\t\tpStatement = pDbLayer->PrepareStatement(wxT(\"SELECT * FROM %s "
                                              "WHERE %s = ?\"));"),
                             classTableName.c_str(), pPKCol->GetName().c_str())
                      << "\n";
                hFile << wxString::Format(wxT("\t\t\tpStatement->%s(1, %s);"),
                             GetAddParamFunction(pPKCol->GetType()->GetUniversalType()).c_str(),
                             pPKCol->GetName().c_str())
                      << "\n";
                hFile << wxT("\t\t\tresSet = pStatement->RunQueryWithResults();") << "\n";
                hFile << wxT("\t\t\t}") << "\n";
                hFile << wxT("\t\t}") << "\n";

                hFile << wxT("\tif (resSet){") << "\n";
                hFile << wxString::Format(wxT("\t\tif (resSet->Next()) return new %s(resSet);"), classItemName.c_str())
                      << "\n";
                hFile << wxT("\t\tpStatement->Close();") << "\n";
                hFile << wxT("\t\t}") << "\n";
                hFile << wxT("\treturn NULL;") << "\n";

                hFile << wxT("}") << "\n";
            }

        } else if(str.Contains(wxT("%%classUtilsAddParameters%%"))) {
            SerializableList::compatibility_iterator node = pTab->GetFirstChildNode();
            while(node) {
                Column* pCol = wxDynamicCast(node->GetData(), Column);
                if(pCol) {
                    hFile << wxString::Format(wxT("\t\t\t,%s %s"),
                                 GetParamTypeName(pCol->GetType()->GetUniversalType()).c_str(), pCol->GetName().c_str())
                          << "\n";
                }
                node = node->GetNext();
            }

        } else if(str.Contains(wxT("%%classUtilsAddParametersWithoutPK%%"))) {
            SerializableList::compatibility_iterator node = pTab->GetFirstChildNode();
            while(node) {
                Column* pCol = wxDynamicCast(node->GetData(), Column);
                if(pCol && (!pPKCol || (pCol->GetName() != pPKCol->GetName()))) {
                    hFile << wxString::Format(wxT("\t\t\t,%s %s"),
                                 GetParamTypeName(pCol->GetType()->GetUniversalType()).c_str(), pCol->GetName().c_str())
                          << "\n";
                }
                node = node->GetNext();
            }

        } else if(str.Contains(wxT("%%classUtilsDeleteParameters%%"))) {
            if(pPKCol)
                hFile << wxString::Format(wxT("\t\t\t,%s %s"),
                             GetParamTypeName(pPKCol->GetType()->GetUniversalType()).c_str(), pPKCol->GetName().c_str())
                      << "\n";

        } else if(str.Contains(wxT("%%classUtilsAddSetParams%%"))) {
            int i = 1;
            SerializableList::compatibility_iterator node = pTab->GetFirstChildNode();
            while(node) {
                Column* pCol = wxDynamicCast(node->GetData(), Column);
                if(pCol) {
                    hFile << wxString::Format(wxT("\t\t\tpStatement->%s(%i, %s);"),
                                 GetAddParamFunction(pCol->GetType()->GetUniversalType()).c_str(), i++,
                                 pCol->GetName().c_str())
                          << "\n";
                }
                node = node->GetNext();
            }
            lastEditParam = i;

        } else if(str.Contains(wxT("%%classUtilsEditSetParams%%"))) {
            int i = 1;
            SerializableList::compatibility_iterator node = pTab->GetFirstChildNode();
            while(node) {
                Column* pCol = wxDynamicCast(node->GetData(), Column);
                if(pCol && (!pPKCol || (pCol->GetName() != pPKCol->GetName()))) {
                    hFile << wxString::Format(wxT("\t\t\tpStatement->%s(%i, %s);"),
                                 GetAddParamFunction(pCol->GetType()->GetUniversalType()).c_str(), i++,
                                 pCol->GetName().c_str())
                          << "\n";
                }
                node = node->GetNext();
            }
            lastEditParam = i;

        } else if(str.Contains(wxT("%%classUtilsAddSetDebeaParams%%"))) {
            SerializableList::compatibility_iterator node = pTab->GetFirstChildNode();
            while(node) {
                Column* pCol = wxDynamicCast(node->GetData(), Column);
                if(pCol && (!pPKCol || (pCol->GetName() != pPKCol->GetName()))) {
                    hFile << wxT("\t\tc.m_") + pCol->GetName() + wxT(" = ") + pCol->GetName() + wxT(";") << "\n";
                }
                node = node->GetNext();
            }

        } else if(str.Contains(wxT("%%classUtilsAddDelParams%%"))) {
            if(pPKCol)
                hFile << wxString::Format(wxT("\t\t\tpStatement->%s(%i, %s);"),
                             GetAddParamFunction(pPKCol->GetType()->GetUniversalType()).c_str(), 1,
                             pPKCol->GetName().c_str())
                      << "\n";

        } else if(str.Contains(wxT("%%classUtilsAddStatement%%"))) {
            wxString cols = wxT("");
            wxString params = wxT("");
            SerializableList::compatibility_iterator node = pTab->GetFirstChildNode();
            while(node) {
                Column* pCol = wxDynamicCast(node->GetData(), Column);
                if(pCol) {
                    if(!cols.IsEmpty()) cols = cols + wxT(",");
                    cols += pCol->GetName();

                    if(!params.IsEmpty()) params += wxT(",");
                    params += wxT("?");
                }
                node = node->GetNext();
            }
            hFile << wxString::Format(wxT("\t\tPreparedStatement* pStatement = pDbLayer->PrepareStatement(wxT(\"INSERT "
                                          "INTO %s (%s) VALUES (%s)\"));"),
                         pTab->GetName().c_str(), cols.c_str(), params.c_str())
                  << "\n";

        } else if(str.Contains(wxT("%%classUtilsEditStatement%%"))) {
            wxString cols = wxT("");
            wxString params = wxT("");
            SerializableList::compatibility_iterator node = pTab->GetFirstChildNode();
            while(node) {
                Column* pCol = wxDynamicCast(node->GetData(), Column);
                if(pCol && (!pPKCol || (pCol->GetName() != pPKCol->GetName()))) {
                    if(!cols.IsEmpty()) cols = cols + wxT(",");
                    cols += pCol->GetName() + wxT(" = ?");
                }
                node = node->GetNext();
            }
            if(pPKCol)
                hFile << wxString::Format(wxT("\t\tPreparedStatement* pStatement = "
                                              "pDbLayer->PrepareStatement(wxT(\"UPDATE %s SET %s WHERE %s = ?\"));"),
                             pTab->GetName().c_str(), cols.c_str(), pPKCol->GetName().c_str())
                      << "\n";
            else
                hFile << wxT("\t\tPreparedStatement* pStatement = NULL;") << "\n";

        } else if(str.Contains(wxT("%%classUtilsEditDebeaStatement%%"))) {
            SerializableList::compatibility_iterator node = pTab->GetFirstChildNode();
            while(node) {
                Column* pCol = wxDynamicCast(node->GetData(), Column);
                if(pCol && (!pPKCol || (pCol->GetName() != pPKCol->GetName()))) {
                    hFile << wxT("\t\t\tc->setMember(c->m_") + pCol->GetName() + wxT(", ") + pCol->GetName() + wxT(");")
                          << "\n";
                }
                node = node->GetNext();
            }

        } else if(str.Contains(wxT("%%classUtilsDeleteStatement%%"))) {
            if(pPKCol)
                hFile << wxString::Format(wxT("\t\tPreparedStatement* pStatement = "
                                              "pDbLayer->PrepareStatement(wxT(\"DELETE FROM %s WHERE %s = ?\"));"),
                             pTab->GetName().c_str(), pPKCol->GetName().c_str())
                      << "\n";
            else
                hFile << wxT("\t\tPreparedStatement* pStatement = NULL;") << "\n";

        } else if(str.Contains(wxT("%%classUtilsPKSetParams%%"))) {
            if(pPKCol)
                hFile << wxString::Format(wxT("\t\t\tpStatement->%s(%i, %s);"),
                             GetAddParamFunction(pPKCol->GetType()->GetUniversalType()).c_str(), lastEditParam,
                             pPKCol->GetName().c_str())
                      << "\n";

        } else if(str.Contains(wxT("%%classUtilsCreateStatement%%"))) {
            wxStringTokenizer tknz(m_pDbAdapter->GetCreateTableSql(pTab, false), wxT("\n"), wxTOKEN_STRTOK);
            while(true) {
                wxString line = tknz.GetNextToken();
                if(!tknz.HasMoreTokens()) break; // omit last line
                hFile << wxT("\t\t\t\"") + line + wxT("\" \\") << "\n";
            }

        } else if(str.Contains(wxT("%%classUtilsDropStatement%%"))) {
            wxStringTokenizer tknz(m_pDbAdapter->GetDropTableSql(pTab), wxT("\n"), wxTOKEN_STRTOK);
            while(tknz.HasMoreTokens()) {
                hFile << wxT("\t\t\t\"") + tknz.GetNextToken() + wxT("\" \\") << "\n";
            }

        } else {
            str.Replace(wxT("%%classItemName%%"), classItemName);
            str.Replace(wxT("%%classItemDef%%"), classItemDef);
            str.Replace(wxT("%%classColName%%"), classColName);
            str.Replace(wxT("%%classTableName%%"), classTableName);
            str.Replace(wxT("%%classUtilName%%"), classUtilName);
            if(pPKCol) {
                str.Replace(wxT("%%pkType%%"), GetParamTypeName(pPKCol->GetType()->GetUniversalType()));
                str.Replace(wxT("%%pkName%%"), pPKCol->GetName());
            }
            hFile << str << "\n";
        }
    }

    return true;
}

wxString ClassGenerateDialog::GetFillData(Column* pCol, int colIndex)
{
    if(m_choiceTemplates->GetStringSelection().Contains(wxT("wxDebea"))) {
        if(pCol->GetType()->GetUniversalType() == IDbType::dbtTYPE_TEXT)
            return wxT("\t\t\tpGrid->SetCellValue(row.m_") + pCol->GetName() +
                wxString::Format(wxT(",i,%i);"), colIndex);
        if(pCol->GetType()->GetUniversalType() == IDbType::dbtTYPE_DATE_TIME)
            return wxT("\t\t\tpGrid->SetCellValue(row.m_") + pCol->GetName() +
                wxString::Format(wxT(".Format(),i,%i);"), colIndex);
        if(pCol->GetType()->GetUniversalType() == IDbType::dbtTYPE_INT)
            return wxT("\t\t\tpGrid->SetCellValue(wxString::Format(wxT(\"%i\"),row.m_") + pCol->GetName() +
                wxString::Format(wxT("),i,%i);"), colIndex);
        if(pCol->GetType()->GetUniversalType() == IDbType::dbtTYPE_FLOAT)
            return wxT("\t\t\tpGrid->SetCellValue(wxString::Format(wxT(\"%lf\"),row.m_") + pCol->GetName() +
                wxString::Format(wxT("),i,%i);"), colIndex);
        if(pCol->GetType()->GetUniversalType() == IDbType::dbtTYPE_DECIMAL)
            return wxT("\t\t\tpGrid->SetCellValue(wxString::Format(wxT(\"%lf\"),row.m_") + pCol->GetName() +
                wxString::Format(wxT("),i,%i);"), colIndex);
        if(pCol->GetType()->GetUniversalType() == IDbType::dbtTYPE_BOOLEAN)
            return wxT("\t\t\tpGrid->SetCellValue(wxString::Format(wxT(\"%i\"),row.m_") + pCol->GetName() +
                wxString::Format(wxT("),i,%i);"), colIndex);
        if(pCol->GetType()->GetUniversalType() == IDbType::dbtTYPE_OTHER)
            return wxT("\t\t\tpGrid->SetCellValue(wxT(\"some data\")") + wxString::Format(wxT(",i,%i);"), colIndex);
    } else {
        if(pCol->GetType()->GetUniversalType() == IDbType::dbtTYPE_TEXT)
            return wxT("\t\t\tpGrid->SetCellValue(row->Get") + pCol->GetName() +
                wxString::Format(wxT("(),i,%i);"), colIndex);
        if(pCol->GetType()->GetUniversalType() == IDbType::dbtTYPE_DATE_TIME)
            return wxT("\t\t\tpGrid->SetCellValue(row->Get") + pCol->GetName() +
                wxString::Format(wxT("().Format(),i,%i);"), colIndex);
        if(pCol->GetType()->GetUniversalType() == IDbType::dbtTYPE_INT)
            return wxT("\t\t\tpGrid->SetCellValue(wxString::Format(wxT(\"%i\"),row->Get") + pCol->GetName() +
                wxString::Format(wxT("()),i,%i);"), colIndex);
        if(pCol->GetType()->GetUniversalType() == IDbType::dbtTYPE_FLOAT)
            return wxT("\t\t\tpGrid->SetCellValue(wxString::Format(wxT(\"%lf\"),row->Get") + pCol->GetName() +
                wxString::Format(wxT("()),i,%i);"), colIndex);
        if(pCol->GetType()->GetUniversalType() == IDbType::dbtTYPE_DECIMAL)
            return wxT("\t\t\tpGrid->SetCellValue(wxString::Format(wxT(\"%lf\"),row->Get") + pCol->GetName() +
                wxString::Format(wxT("()),i,%i);"), colIndex);
        if(pCol->GetType()->GetUniversalType() == IDbType::dbtTYPE_BOOLEAN)
            return wxT("\t\t\tpGrid->SetCellValue(wxString::Format(wxT(\"%i\"),row->Get") + pCol->GetName() +
                wxString::Format(wxT("()),i,%i);"), colIndex);
        if(pCol->GetType()->GetUniversalType() == IDbType::dbtTYPE_OTHER)
            return wxT("\t\t\tpGrid->SetCellValue(wxT(\"some data\")") + wxString::Format(wxT(",i,%i);"), colIndex);
    }

    return wxT("");
}

wxString ClassGenerateDialog::GetAddParamFunction(IDbType::UNIVERSAL_TYPE type)
{
    if(type == IDbType::dbtTYPE_TEXT) return wxT("SetParamString");
    if(type == IDbType::dbtTYPE_DATE_TIME) return wxT("SetParamDate");
    if(type == IDbType::dbtTYPE_INT) return wxT("SetParamInt");
    if(type == IDbType::dbtTYPE_FLOAT) return wxT("SetParamDouble");
    if(type == IDbType::dbtTYPE_DECIMAL) return wxT("SetParamDouble");
    if(type == IDbType::dbtTYPE_BOOLEAN) return wxT("SetParamBool");
    if(type == IDbType::dbtTYPE_OTHER) return wxT("SetParamBlob");
    return wxT("");
}

wxString ClassGenerateDialog::GetTypeName(IDbType::UNIVERSAL_TYPE type)
{
    if(type == IDbType::dbtTYPE_TEXT) {
        if(m_choiceTemplates->GetStringSelection().Contains(wxT("wxWidgets")))
            return wxT("wxString");
        else
            return wxT("std::string");
    }
    if(type == IDbType::dbtTYPE_DATE_TIME) {
        if(m_choiceTemplates->GetStringSelection().Contains(wxT("wxWidgets")))
            return wxT("wxDateTime");
        else
            return wxT("tm");
    }
    if(type == IDbType::dbtTYPE_INT) return wxT("int");
    if(type == IDbType::dbtTYPE_FLOAT) return wxT("double");
    if(type == IDbType::dbtTYPE_DECIMAL) return wxT("double");
    if(type == IDbType::dbtTYPE_BOOLEAN) return wxT("bool");
    if(type == IDbType::dbtTYPE_OTHER) return wxT("void*");
    return wxT("");
}

wxString ClassGenerateDialog::GetResTypeName(IDbType::UNIVERSAL_TYPE type)
{
    if(type == IDbType::dbtTYPE_TEXT) {
        if(m_choiceTemplates->GetStringSelection().Contains(wxT("wxWidgets")))
            return wxT("wxString&");
        else
            return wxT("std::string&");
    }
    if(type == IDbType::dbtTYPE_DATE_TIME) {
        if(m_choiceTemplates->GetStringSelection().Contains(wxT("wxWidgets")))
            return wxT("wxDateTime&");
        else
            return wxT("tm&");
    }
    if(type == IDbType::dbtTYPE_INT) return wxT("int");
    if(type == IDbType::dbtTYPE_FLOAT) return wxT("double");
    if(type == IDbType::dbtTYPE_DECIMAL) return wxT("double");
    if(type == IDbType::dbtTYPE_BOOLEAN) return wxT("bool");
    if(type == IDbType::dbtTYPE_OTHER) return wxT("void*");
    return wxT("");
}

wxString ClassGenerateDialog::GetDebeaBinding(Column* pCol)
{
    wxString ret;

    if(pCol) {
        wxString bind, type;

        switch(pCol->GetType()->GetUniversalType()) {
        case IDbType::dbtTYPE_TEXT:
            if(m_choiceTemplates->GetStringSelection().Contains(wxT("wxWidgets"))) {
                bind = wxT("BIND_STR");
                type = wxT("wxdba::String");
            } else {
                bind = wxT("BIND_STR");
                type = wxT("dba::String");
            }
            break;
        case IDbType::dbtTYPE_DATE_TIME:
            if(m_choiceTemplates->GetStringSelection().Contains(wxT("wxWidgets"))) {
                bind = wxT("BIND_DAT");
                type = wxT("wxdba::DateTime");
            } else {
                bind = wxT("BIND_DAT");
                type = wxT("dba::DateTime");
            }
            break;
        case IDbType::dbtTYPE_INT:
            bind = wxT("BIND_INT");
            type = wxT("dba::Int");
            break;
        case IDbType::dbtTYPE_FLOAT:
            bind = wxT("BIND_FLT");
            type = wxT("dba::Double");
            break;
        case IDbType::dbtTYPE_DECIMAL:
            bind = wxT("BIND_FLT");
            type = wxT("dba::Double");
            break;
        case IDbType::dbtTYPE_BOOLEAN:
            bind = wxT("BIND_INT");
            type = wxT("dba::Bool");
            break;
        default:
            break;
        }

        ret = bind + wxT("(") + classItemName + wxT("::m_") + pCol->GetName() + wxT(", ") + type + wxT(", \"") +
            pCol->GetName() + wxT("\")");
    }

    return ret;
}

wxString ClassGenerateDialog::GetParamTypeName(IDbType::UNIVERSAL_TYPE type)
{
    if(type == IDbType::dbtTYPE_TEXT) {
        if(m_choiceTemplates->GetStringSelection().Contains(wxT("wxWidgets")))
            return wxT("const wxString&");
        else
            return wxT("const std::string&");
    }
    if(type == IDbType::dbtTYPE_DATE_TIME) {
        if(m_choiceTemplates->GetStringSelection().Contains(wxT("wxWidgets")))
            return wxT("const wxDateTime&");
        else
            return wxT("const tm&");
    }
    if(type == IDbType::dbtTYPE_INT) return wxT("int");
    if(type == IDbType::dbtTYPE_FLOAT) return wxT("double");
    if(type == IDbType::dbtTYPE_DECIMAL) return wxT("double");
    if(type == IDbType::dbtTYPE_BOOLEAN) return wxT("bool");
    if(type == IDbType::dbtTYPE_OTHER) return wxT("const void*");
    return wxT("");
}

wxString ClassGenerateDialog::GetResultFunction(IDbType::UNIVERSAL_TYPE type)
{
    if(type == IDbType::dbtTYPE_TEXT) return wxT("GetResultString");
    if(type == IDbType::dbtTYPE_DATE_TIME) return wxT("GetResultDate");
    if(type == IDbType::dbtTYPE_INT) return wxT("GetResultInt");
    if(type == IDbType::dbtTYPE_FLOAT) return wxT("GetResultDouble");
    if(type == IDbType::dbtTYPE_DECIMAL) return wxT("GetResultDouble");
    if(type == IDbType::dbtTYPE_BOOLEAN) return wxT("GetResultBool");
    if(type == IDbType::dbtTYPE_OTHER) return wxT("GetResultBlob");
    return wxT("");
}

void ClassGenerateDialog::OnBtnBrowseClick(wxCommandEvent& event)
{
    VirtualDirectorySelectorDlg dlg(this, m_mgr->GetWorkspace(), m_txVirtualDir->GetValue());
    if(dlg.ShowModal() == wxID_OK) {
        m_txVirtualDir->SetValue(dlg.GetVirtualDirectoryPath());
    }
}

void ClassGenerateDialog::FormatFile(wxString& content, const wxFileName& filename)
{
    clSourceFormatEvent evt(wxEVT_FORMAT_STRING);
    evt.SetInputString(content);
    evt.SetFileName(filename.GetFullPath());
    EventNotifier::Get()->ProcessEvent(evt);
    content = evt.GetFormattedString();
}
