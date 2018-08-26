//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : ClassGenerateDialog.h
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

#ifndef CLASSGENERATEDIALOG_H
#define CLASSGENERATEDIALOG_H

#include "GUI.h" // Base class: _ClassGenerateDialog
#include "ErdTable.h"
#include "ErdView.h"
#include "table.h"
#include "IDbAdapter.h"
#include "wx/wxxmlserializer/XmlSerializer.h"
#include <wx/textfile.h>
#include <wx/stdpaths.h>
#include <wx/init.h>
#include <wx/hashmap.h>

#include "../Interfaces/imanager.h"
#include "../Plugin/VirtualDirectorySelectorDlg.h"
#include "../Plugin/project.h"
#include "../Plugin/workspace.h"

WX_DECLARE_STRING_HASH_MAP(wxString, TemplateMap);

/*! \brief Dialog for generating classes which are used for database acces. */
class ClassGenerateDialog : public _ClassGenerateDialog
{

public:
    /*! \brief Defautl constructor.  */
    ClassGenerateDialog(wxWindow* parent, IDbAdapter* dbAdapter, xsSerializable* pItems, IManager* pMgr);

    /*! \brief Return string with type name on UNIVERSAL TYPE base */
    wxString GetTypeName(IDbType::UNIVERSAL_TYPE type);
    /*! \brief Return string with type name in result format on UNIVERSAL TYPE base */
    wxString GetResTypeName(IDbType::UNIVERSAL_TYPE type);
    /*! \brief Return string with type name in Debea format on UNIVERSAL TYPE base */
    wxString GetDebeaBinding(Column* pCol);
    /*! \brief Return row for filling table row attribute. */
    wxString GetFillData(Column* pCol, int colIndex);
    /*! \brief Return string with type name on UNIVERSAL TYPE base */
    wxString GetResultFunction(IDbType::UNIVERSAL_TYPE type);
    /*! \brief Return string with type name in function parameter format on UNIVERSAL TYPE base */
    wxString GetParamTypeName(IDbType::UNIVERSAL_TYPE type);
    /*! \brief Return string with type name in add function format on UNIVERSAL TYPE base */
    wxString GetAddParamFunction(IDbType::UNIVERSAL_TYPE type);

    /*! \brief Function for generating classes for selected table. It call GenerateFile function two times. Fist time
     * for generate .h, second time for generate .cpp file. */
    bool GenerateClass(Table* pTab, const wxString& path);

    /*! \brief Function for generating file for selected table.  */
    bool GenerateFile(Table* pTab, wxTextFile& htmpFile, wxString& hFile, const wxString& classItemName,
        const wxString& classItemDef, const wxString& classColName, const wxString& classTableName,
        const wxString& classUtilName);
    /*! \brief Format text file */
    void FormatFile(wxString& content, const wxFileName& filename);

    virtual void OnCancelClick(wxCommandEvent& event);

    /*! \brief Function for generating classes for all tables. It call GenerateClass() function for each table. */
    virtual void OnGenerateClick(wxCommandEvent& event);
    virtual void OnBtnBrowseClick(wxCommandEvent& event);

    virtual ~ClassGenerateDialog();

protected:
    TemplateMap m_mapTemplateFiles;
    IDbAdapter* m_pDbAdapter;
    xsSerializable* m_pItems;
    IManager* m_mgr;

    wxString classTableName;
    wxString classItemName;
    wxString classItemDef;
    wxString classColName;
    wxString classUtilName;
};

#endif // CLASSGENERATEDIALOG_H
