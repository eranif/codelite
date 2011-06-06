#ifndef DBEXPLORERPANEL_H
#define DBEXPLORERPANEL_H

#include "../Interfaces/imanager.h"
#include "GUI.h" // Base class: _DbExplorerPanel
// database
#include <wx/dblayer/include/DatabaseLayer.h>

#ifdef DBL_USE_MYSQL
#include <wx/dblayer/include/MysqlDatabaseLayer.h>
#endif

#include <wx/wxsf/Thumbnail.h>

// wx classes
#include <wx/wx.h>
#include <wx/hashmap.h>
#include <wx/aui/aui.h>
#include <wx/textdlg.h>
#include <wx/txtstrm.h>

// other
#include "IDbItem.h"

#include "DnDTableShape.h"
#include "dbconnection.h"
#include "database.h"
#include "table.h"
#include "column.h"
#include "view.h"
#include "dumpclass.h"
#include "IDbAdapter.h"
#include "MySqlDbAdapter.h"
#include "dbitem.h"
#include "LogDialog.h"
#include "ClassGenerateDialog.h"

#include "Ids.h"

// plugin include
//#include "../LiteEditor/mainbook.h"


WX_DECLARE_HASH_MAP( wxString, wxTreeItemId, wxStringHash, wxStringEqual, TableHashMap );

class MainBook;
/*! \brief Main application panel. It show database tree and can open special editors. */
class DbViewerPanel : public _DbViewerPanel
{
protected:
	xsSerializable* m_pConnections;
	IDbAdapter*     m_pDbAdapter;
	wxWindow*       m_pGlobalParent;
	wxString        m_server;
	wxTreeItemId    m_selectedID;
	wxWindow*       m_pNotebook;
	IManager*       m_mgr;
	TableHashMap    m_hashTables;
	Database*       m_pEditedDatabase;
	DbConnection*   m_pEditedConnection;
	wxSFThumbnail*  m_pThumbnail;
	wxArrayString   m_pagesAdded;
	
public:
	DbViewerPanel(wxWindow *parent, wxWindow* notebook, IManager* pManager);
	virtual ~DbViewerPanel();
	void SetDbAdapter(IDbAdapter *dbAdapter) {
		m_pDbAdapter = dbAdapter;
	}

	void AddDbConnection(DbConnection* pDbCon) {
		m_pConnections->AddChild(pDbCon);
	}

	void SetServer(wxString& server) {
		m_server = server;
	}

	wxSplitterWindow *GetSplitter() {
		return m_splitterPanels;
	}

	virtual void OnConncectClick(wxCommandEvent& event);
	virtual void OnConncectUI(wxUpdateUIEvent& event);
	virtual void OnItemActivate(wxTreeEvent& event);
	virtual void OnRefreshClick(wxCommandEvent& event);
	virtual void OnItemSelectionChange(wxTreeEvent& event);
	virtual void OnERDClick(wxCommandEvent& event);
	virtual void OnDnDStart(wxTreeEvent& event);
	virtual void OnItemRightClick(wxTreeEvent& event);
	virtual void OnToolCloseClick(wxCommandEvent& event);
	virtual void OnToolCloseUI(wxUpdateUIEvent& event);

	void OnPageClose(NotebookEvent& event);
	void OnPageChange(NotebookEvent& event);
	void OnPopupClick(wxCommandEvent &evt);
	void RefreshDbView();
	static void InitStyledTextCtrl(wxScintilla *sci);

protected:

	enum PanelType {
		Sql,
		Erd
	};
	bool ImportDb(const wxString& sqlFile, Database* pDb);
	wxString CreatePanelName(Table* t, PanelType type);
	wxString CreatePanelName(View* v, PanelType type);
	wxString CreatePanelName(Database* d, PanelType type);
};

#endif // DBEXPLORERPANEL_H
