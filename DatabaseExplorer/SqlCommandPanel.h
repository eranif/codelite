//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : SqlCommandPanel.h
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

#ifndef SQLCOMMANDPANEL_H
#define SQLCOMMANDPANEL_H

#include "GUI.h" // Base class: _SqlCommandPanel
#include "IDbAdapter.h"
#include "clEditorEditEventsHandler.h"
#include "clToolBar.h"

#include <map>
#include <wx/dblayer/include/DatabaseErrorCodes.h>
#include <wx/dblayer/include/DatabaseLayer.h>
#include <wx/dblayer/include/DatabaseLayerException.h>
#include <wx/wx.h>

// ----------------------------------------------------------------
class ColumnInfo
{
    int m_type;
    wxString m_name;

public:
    typedef std::vector<ColumnInfo> Vector_t;

public:
    ColumnInfo()
        : m_type(0)
    {
    }

    ColumnInfo(int type, const wxString& name)
        : m_type(type)
        , m_name(name)
    {
    }

    virtual ~ColumnInfo() {}

    void SetName(const wxString& name) { this->m_name = name; }
    void SetType(int type) { this->m_type = type; }
    const wxString& GetName() const { return m_name; }
    int GetType() const { return m_type; }
};

// ----------------------------------------------------------------
class SQLCommandPanel : public _SqlCommandPanel
{

    int m_OperatorStyle;
    int m_CommentStyle;

protected:
    virtual void OnHistoryToolClicked(wxAuiToolBarEvent& event);
    IDbAdapter* m_pDbAdapter;
    wxString m_dbName;
    wxString m_dbTable;
    wxString m_cellValue;
    std::map<std::pair<int, int>, wxString> m_gridValues;
    ColumnInfo::Vector_t m_colsMetaData;
    clEditEventsHandler::Ptr_t m_editHelper;
    clToolBarGeneric* m_toolbar;

protected:
    bool IsBlobColumn(const wxString& str);
    wxArrayString ParseSql() const;
    void SaveSqlHistory(wxArrayString sqls);

public:
    SQLCommandPanel(wxWindow* parent, IDbAdapter* dbAdapter, const wxString& dbName, const wxString& dbTable);
    virtual ~SQLCommandPanel();
    virtual void OnExecuteClick(wxCommandEvent& event);
    virtual void OnScintilaKeyDown(wxKeyEvent& event);

    virtual void OnLoadClick(wxCommandEvent& event);
    virtual void OnSaveClick(wxCommandEvent& event);
    virtual void OnTeplatesLeftDown(wxMouseEvent& event);
    virtual void OnTemplatesBtnClick(wxAuiToolBarEvent& event);

    void OnPopupClick(wxCommandEvent& evt);
    void ExecuteSql();
    void SetDefaultSelect();
    void OnCopyCellValue(wxCommandEvent& e);
    DECLARE_EVENT_TABLE()
    void OnExecuteSQL(wxCommandEvent& e);
};

#endif // SQLCOMMANDPANEL_H
