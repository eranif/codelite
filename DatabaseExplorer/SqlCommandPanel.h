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

#include <map>

class SQLCommandPanel : public _SqlCommandPanel
{
protected:
	IDbAdapter*                              m_pDbAdapter;
	wxString                                 m_dbName;
	wxString                                 m_dbTable;
	wxString                                 m_cellValue;
	std::map<std::pair<int, int>, wxString > m_gridValues;
protected:
	bool IsBlobColumn(const wxString &str);
	
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
	void SetDefaultSelect();
	
	void OnGridCellRightClick(wxGridEvent& event);
	void OnCopyCellValue(wxCommandEvent &e);

	virtual void OnGridLabelRightClick(wxGridEvent& event);

	DECLARE_EVENT_TABLE()
	void OnExecuteSQL(wxCommandEvent &e);
	void OnEdit      (wxCommandEvent &e);
	void OnEditUI    (wxUpdateUIEvent &e);

};

#endif // SQLCOMMANDPANEL_H
