#ifndef SQLCOMMANDPANEL_H
#define SQLCOMMANDPANEL_H

#include <wx/wx.h>
#include <wx/dblayer/include/DatabaseLayerException.h>
#include "GUI.h" // Base class: _SqlCommandPanel
#include <wx/dblayer/include/DatabaseLayer.h>

#ifdef DBL_USE_MYSQL
#include <wx/dblayer/include/MysqlDatabaseLayer.h>
#endif

#include <wx/dblayer/include/DatabaseErrorCodes.h>
#include "IDbAdapter.h"

#include "Ids.h"


class SQLCommandPanel : public _SqlCommandPanel
{
protected:
	IDbAdapter* m_pDbAdapter;
	wxString    m_dbName;
	wxString    m_dbTable;
	wxString    m_cellValue;
	
public:
	SQLCommandPanel(wxWindow *parent,IDbAdapter* dbAdapter, const wxString& dbName,const wxString& dbTable);
	virtual ~SQLCommandPanel();
	virtual void OnExecuteClick(wxCommandEvent& event);
	virtual void OnScintilaKeyDown(wxKeyEvent& event);

	virtual void OnLoadClick(wxCommandEvent& event);
	virtual void OnSaveClick(wxCommandEvent& event);
	virtual void OnTeplatesLeftDown(wxMouseEvent& event);
	virtual void OnTemplatesBtnClick(wxCommandEvent& event);

	void OnPopupClick(wxCommandEvent &evt);
	void ExecuteSql();

	void OnGridCellRightClick(wxGridEvent& event);
	void OnCopyCellValue(wxCommandEvent &e);
	
	DECLARE_EVENT_TABLE()
	void OnExecuteSQL(wxCommandEvent &e);

};

#endif // SQLCOMMANDPANEL_H
