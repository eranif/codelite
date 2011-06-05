#ifndef DBSETTINGDIALOG_H
#define DBSETTINGDIALOG_H

#include "GUI.h" // Base class: _DBSettingsDialog
#include <wx/wx.h>
#include "DbViewerPanel.h"

#ifdef DBL_USE_MYSQL
#include "MySqlDbAdapter.h"
#endif

#ifdef DBL_USE_SQLITE
#include "SqliteDbAdapter.h"
#endif

#ifdef DBL_USE_POSTGRES
#include "PostgreSqlDbAdapter.h"
#endif

#include "MysqlConnectionHistory.h"
#include "PostgresConnectionHistory.h"


/*! \brief Dialog for connection to the database server. It should be change everytime, if new database server typ is add. */
class DbSettingDialog : public _DBSettingsDialog
{

public:
	DbSettingDialog(DbViewerPanel *parent, wxWindow* pWindowParent);
	virtual ~DbSettingDialog();

protected:


	virtual void OnCancelClick(wxCommandEvent& event);
	virtual void OnOkClick(wxCommandEvent& event);
	virtual void OnSqliteOkClick(wxCommandEvent& event);
	virtual void OnHistoruUI(wxUpdateUIEvent& event);
	virtual void OnHistoryClick(wxCommandEvent& event);
	virtual void OnHistoryDClick(wxCommandEvent& event);
	virtual void OnRemoveClick(wxCommandEvent& event);
	virtual void OnRmoveUI(wxUpdateUIEvent& event);
	virtual void OnSaveClick(wxCommandEvent& event);
	virtual void OnSaveUI(wxUpdateUIEvent& event);
	virtual void OnOKUI(wxUpdateUIEvent& event);
	virtual void OnMySqlPassKeyDown(wxKeyEvent& event);

	virtual void OnPgCancelClick(wxCommandEvent& event);
	virtual void OnPgOkClick(wxCommandEvent& event);
	virtual void OnPgRemoveClick(wxCommandEvent& event);
	virtual void OnPgSaveClick(wxCommandEvent& event);
	virtual void OnPgHistoryClick(wxCommandEvent& event);
	virtual void OnPgHistoryDClick(wxCommandEvent& event);
	virtual void OnPgOKUI(wxUpdateUIEvent& event);
	virtual void OnPgRmoveUI(wxUpdateUIEvent& event);
	virtual void OnPgSaveUI(wxUpdateUIEvent& event);
	virtual void OnPgSqlKeyDown(wxKeyEvent& event);
	virtual void OnItemActivated(wxListEvent& event);
	virtual void OnItemKeyDown(wxListEvent& event);



	void LoadHistory();

	DbViewerPanel *m_pParent;
	MysqlConnectionHistory *m_pHistory;
	PostgresConnectionHistory *m_pPgHistory;
};

#endif // DBSETTINGDIALOG_H
