///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 22 2011)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __GUI__
#define __GUI__

#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/statusbr.h>
#include <wx/frame.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/button.h>
#include <wx/toolbar.h>
#include <wx/wxscintilla.h>
#ifdef __VISUALC__
#include <wx/link_additions.h>
#endif //__VISUALC__
#include <wx/grid.h>
#include <wx/stattext.h>
#include <wx/splitter.h>
#include <wx/dialog.h>
#include <wx/treectrl.h>
#include <wx/textctrl.h>
#include <wx/statbox.h>
#include <wx/listbox.h>
#include <wx/filepicker.h>
#include <wx/listctrl.h>
#include <wx/notebook.h>
#include <wx/statline.h>
#include <wx/combobox.h>
#include <wx/checkbox.h>
#include <wx/radiobox.h>
#include <wx/gbsizer.h>

///////////////////////////////////////////////////////////////////////////

#define wxID_RUN 1000
#define wxID_CONNECT 1001
#define wxID_CLOSE_CONNECTION 1002
#define wxID_TOOL_REFRESH 1003
#define wxID_TOOL_ERD 1004
#define wxID_Sqlite_OK 1005
#define wxID_PG_OK 1006
#define wxID_PG_CANCEL 1007
#define wxID_PG_SAVE 1008
#define wxID_PG_REMOVE 1009
#define wxID_PG_LIST 1010
#define wxID_NEW_COL 1011
#define wxID_NEW_CONSTRAIN 1012
#define wxID_DEL 1013
#define wxID_PAGE_TYPE 1014
#define wxID_TX_SIZE 1015
#define wxID_PAGE_CONSTRAINT 1016
#define wxID_ON_UPDATE 1017
#define wxID_GENERATE 1018

///////////////////////////////////////////////////////////////////////////////
/// Class _MainFrame
///////////////////////////////////////////////////////////////////////////////
class _MainFrame : public wxFrame 
{
	private:
	
	protected:
		wxMenuBar* m_menubar3;
		wxMenu* menuFile;
		wxMenu* menuHelp;
		wxStatusBar* m_statusBar1;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnExit( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAbout( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		_MainFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("DatabaseExplorer "), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxDEFAULT_FRAME_STYLE|wxMAXIMIZE|wxTAB_TRAVERSAL );
		
		~_MainFrame();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class _ThumbPane
///////////////////////////////////////////////////////////////////////////////
class _ThumbPane : public wxPanel 
{
	private:
	
	protected:
		wxBoxSizer* mainSizer;
	
	public:
		
		_ThumbPane( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL ); 
		~_ThumbPane();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class _SqlCommandPanel
///////////////////////////////////////////////////////////////////////////////
class _SqlCommandPanel : public wxPanel 
{
	private:
	
	protected:
		wxToolBar* m_toolBar3;
		wxButton* m_button34;
		wxSplitterWindow* m_splitter1;
		wxPanel* m_panel13;
		wxScintilla* m_scintillaSQL;
		wxButton* m_btnSave;
		wxButton* m_btnLoad;
		wxButton* m_btnExecute;
		wxPanel* m_panel14;
		wxGrid* m_gridTable;
		wxStaticText* m_labelStatus;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnTemplatesBtnClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnScintilaKeyDown( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnSaveClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnLoadClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnExecuteClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnGridCellRightClick( wxGridEvent& event ) { event.Skip(); }
		
	
	public:
		
		_SqlCommandPanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL ); 
		~_SqlCommandPanel();
		
		void m_splitter1OnIdle( wxIdleEvent& )
		{
			m_splitter1->SetSashPosition( 163 );
			m_splitter1->Disconnect( wxEVT_IDLE, wxIdleEventHandler( _SqlCommandPanel::m_splitter1OnIdle ), NULL, this );
		}
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class _AdapterSelectDlg
///////////////////////////////////////////////////////////////////////////////
class _AdapterSelectDlg : public wxDialog 
{
	private:
	
	protected:
		wxButton* m_btnMySql;
		wxButton* m_btnSqlite;
		wxButton* m_button24;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnMysqlClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSqliteClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnPostgresClick( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		_AdapterSelectDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Select dbAdapter"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE ); 
		~_AdapterSelectDlg();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class _DbViewerPanel
///////////////////////////////////////////////////////////////////////////////
class _DbViewerPanel : public wxPanel 
{
	private:
	
	protected:
		wxSplitterWindow* m_splitterPanels;
		wxPanel* m_panelData;
		wxToolBar* m_toolBar1;
		wxTreeCtrl* m_treeDatabases;
		wxPanel* m_panelThumb;
		wxBoxSizer* m_thmSizer;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnConncectClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnConncectUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnToolCloseClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnToolCloseUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnRefreshClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnERDClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDnDStart( wxTreeEvent& event ) { event.Skip(); }
		virtual void OnItemActivate( wxTreeEvent& event ) { event.Skip(); }
		virtual void OnItemRightClick( wxTreeEvent& event ) { event.Skip(); }
		virtual void OnItemSelectionChange( wxTreeEvent& event ) { event.Skip(); }
		
	
	public:
		
		_DbViewerPanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL ); 
		~_DbViewerPanel();
		
		void m_splitterPanelsOnIdle( wxIdleEvent& )
		{
			m_splitterPanels->SetSashPosition( 305 );
			m_splitterPanels->Disconnect( wxEVT_IDLE, wxIdleEventHandler( _DbViewerPanel::m_splitterPanelsOnIdle ), NULL, this );
		}
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class _DBSettingsDialog
///////////////////////////////////////////////////////////////////////////////
class _DBSettingsDialog : public wxDialog 
{
	private:
	
	protected:
		wxNotebook* m_notebook2;
		wxPanel* m_MySqlPanel;
		wxStaticText* m_staticText10;
		wxTextCtrl* m_txName;
		wxStaticText* m_staticText1;
		wxTextCtrl* m_txServer;
		wxStaticText* m_staticText2;
		wxTextCtrl* m_txUserName;
		wxStaticText* m_staticText3;
		wxTextCtrl* m_txPassword;
		wxButton* m_btnOK;
		wxButton* m_btnCancel;
		wxButton* m_btnSave;
		wxButton* m_btnRemove;
		wxListBox* m_listBox2;
		wxPanel* m_Sqlite;
		wxStaticText* m_staticText11;
		wxFilePickerCtrl* m_filePickerSqlite;
		wxListCtrl* m_listCtrlRecentFiles;
		wxButton* m_btnOKSqlite;
		wxButton* m_btnCancel1;
		wxPanel* m_PostgrePanel;
		wxStaticText* m_staticText101;
		wxTextCtrl* m_txPgName;
		wxStaticText* m_staticText12;
		wxTextCtrl* m_txPgServer;
		wxStaticText* m_staticText21;
		wxTextCtrl* m_txPgUserName;
		wxStaticText* m_staticText31;
		wxTextCtrl* m_txPgPassword;
		wxStaticText* m_staticText24;
		wxTextCtrl* m_txPgDatabase;
		wxButton* m_btnPgOK;
		wxButton* m_btnPgCancel;
		wxButton* m_btnPgSave;
		wxButton* m_btnPgRemove;
		wxListBox* m_listBoxPg;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnMySqlPassKeyDown( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnOkClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOKUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnCancelClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSaveClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSaveUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnRemoveClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRmoveUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnHistoryClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnHistoryDClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnHistoruUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnItemActivated( wxListEvent& event ) { event.Skip(); }
		virtual void OnItemKeyDown( wxListEvent& event ) { event.Skip(); }
		virtual void OnSqliteOkClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnPgSqlKeyDown( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnPgOkClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnPgOKUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnPgCancelClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnPgSaveClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnPgSaveUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnPgRemoveClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnPgRmoveUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnPgHistoryClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnPgHistoryDClick( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		_DBSettingsDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Connection settings"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE ); 
		~_DBSettingsDialog();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class _ErdPanel
///////////////////////////////////////////////////////////////////////////////
class _ErdPanel : public wxPanel 
{
	private:
	
	protected:
		wxToolBar* m_toolBarErd;
		wxPanel* m_wxsfPanel;
		wxBoxSizer* m_wxsfSizer;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnMouseWheel( wxMouseEvent& event ) { event.Skip(); }
		
	
	public:
		
		_ErdPanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 640,480 ), long style = wxTAB_TRAVERSAL ); 
		~_ErdPanel();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class _TableSettings
///////////////////////////////////////////////////////////////////////////////
class _TableSettings : public wxDialog 
{
	private:
	
	protected:
		wxPanel* m_panel7;
		wxPanel* m_panel8;
		wxStaticText* m_staticText7;
		wxTextCtrl* m_txTableName;
		wxPanel* m_panel9;
		wxPanel* m_panel13;
		wxListBox* m_listColumns;
		wxPanel* m_panel14;
		wxButton* m_button8;
		wxButton* m_button15;
		wxButton* m_button10;
		wxStaticLine* m_staticline2;
		wxNotebook* m_notebook3;
		wxPanel* m_pageType;
		wxStaticText* m_staticText8;
		wxTextCtrl* m_txColName;
		wxStaticText* m_staticText9;
		wxComboBox* m_comboType;
		wxStaticText* m_stSize;
		
		wxStaticText* m_staticText25;
		wxTextCtrl* m_txSize;
		wxStaticText* m_staticText2511;
		wxTextCtrl* m_txSize2;
		wxStaticText* m_staticText251;
		wxCheckBox* m_chNotNull;
		wxCheckBox* m_chAutoIncrement;
		wxPanel* m_pageConstraint;
		wxStaticText* m_staticText11;
		wxTextCtrl* m_txConstraintName;
		wxStaticText* m_staticText12;
		wxComboBox* m_comboLocalColumn;
		wxRadioBox* m_radioBox1;
		wxStaticText* m_staticText13;
		wxComboBox* m_comboRefTable;
		wxStaticText* m_staticText14;
		wxComboBox* m_comboRefColumn;
		wxRadioBox* m_radioOnDelete;
		wxRadioBox* m_radioOnUpdate;
		wxButton* m_button9;
		wxStdDialogButtonSizer* m_sdbSizer2;
		wxButton* m_sdbSizer2OK;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnListBoxClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnNewColumnClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnNewConstrainClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDeleteColumn( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnNotebookUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnPageTypeUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnColNameUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnTypeSelect( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnColTypeUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnColSizeUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnColSize2UI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnNotNullUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnAutoIncrementUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnPageConstraintUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnRefTabChange( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRefTabUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnRefColUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnDeleteUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnUpdateUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnSaveColumnClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOKClick( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		_TableSettings( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Table settings"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE ); 
		~_TableSettings();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class _CreateForeignKey
///////////////////////////////////////////////////////////////////////////////
class _CreateForeignKey : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText15;
		wxTextCtrl* m_txSrcTable;
		wxStaticText* m_staticText17;
		wxComboBox* m_cmbSrcCol;
		wxRadioBox* m_radioRelation;
		wxRadioBox* m_radioOnDelete;
		wxRadioBox* m_radioOnUpdate;
		wxStaticText* m_staticText16;
		wxTextCtrl* m_txDstTable;
		wxStaticText* m_staticText18;
		wxComboBox* m_cmbDstCol;
		
		
		wxButton* m_btnCancel;
		wxButton* m_btnOK;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnCancelClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOKClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOKUI( wxUpdateUIEvent& event ) { event.Skip(); }
		
	
	public:
		
		_CreateForeignKey( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Create foreign key"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE ); 
		~_CreateForeignKey();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class _LogDialog
///////////////////////////////////////////////////////////////////////////////
class _LogDialog : public wxDialog 
{
	private:
	
	protected:
		wxTextCtrl* m_textCtrl11;
		wxButton* m_button18;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnCloseClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCloseUI( wxUpdateUIEvent& event ) { event.Skip(); }
		
	
	public:
		
		_LogDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Database log"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 640,460 ), long style = wxDEFAULT_DIALOG_STYLE|wxMAXIMIZE_BOX|wxRESIZE_BORDER ); 
		~_LogDialog();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class _ViewSettings
///////////////////////////////////////////////////////////////////////////////
class _ViewSettings : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText19;
		wxTextCtrl* m_txName;
		wxStaticLine* m_staticline2;
		wxScintilla* m_scintilla2;
		wxButton* m_btnOK;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnOKClick( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		_ViewSettings( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("View settings"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 650,450 ), long style = wxDEFAULT_DIALOG_STYLE|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxRESIZE_BORDER ); 
		~_ViewSettings();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class _ClassGenerateDialog
///////////////////////////////////////////////////////////////////////////////
class _ClassGenerateDialog : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText30;
		wxTextCtrl* m_txPrefix;
		wxStaticText* m_staticText31;
		wxTextCtrl* m_txPostfix;
		wxStaticText* m_staticText28;
		wxDirPickerCtrl* m_dirPicker;
		wxStaticText* m_staticText34;
		wxTextCtrl* m_txVirtualDir;
		wxButton* m_btnBrowseVirtualDir;
		wxStaticText* m_staticText29;
		wxTextCtrl* m_textCtrl19;
		
		wxButton* m_button25;
		wxButton* m_button26;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnBtnBrowseClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnGenerateClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCancelClick( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		_ClassGenerateDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Class generator dialog"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE ); 
		~_ClassGenerateDialog();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class _ErdCommitDialog
///////////////////////////////////////////////////////////////////////////////
class _ErdCommitDialog : public wxDialog 
{
	private:
	
	protected:
		wxNotebook* m_notebook4;
		wxPanel* m_pnSelDatabase;
		wxTreeCtrl* m_treeConnection;
		wxPanel* m_pnBackup;
		wxStaticText* m_staticText32;
		wxFilePickerCtrl* m_fileData;
		wxStaticLine* m_staticline31;
		wxCheckBox* m_checkBox3;
		wxFilePickerCtrl* m_fileStructure;
		wxStaticLine* m_staticline3;
		wxButton* m_btnBackup;
		wxPanel* m_panel20;
		wxTextCtrl* m_textCtrl22;
		
		wxButton* m_btnWrite;
		wxButton* m_btnShowSql;
		
		wxPanel* m_panel201;
		wxFilePickerCtrl* m_fileRestore;
		wxTextCtrl* m_textCtrl221;
		wxButton* m_btnRestore;
		
		wxButton* m_btnBack;
		wxButton* m_btnNext;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnNtbPageChanging( wxNotebookEvent& event ) { event.Skip(); }
		virtual void OnStructureFileUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnBtnBackup( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnBackupUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnBtnWrite( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnWriteUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnShowSqlClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnBtnRestoreClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnBtnRestoreUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnBtnBack( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnBtnBackUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnBtnNext( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnNextUI( wxUpdateUIEvent& event ) { event.Skip(); }
		
	
	public:
		
		_ErdCommitDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("ERD commit structure"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,500 ), long style = wxDEFAULT_DIALOG_STYLE ); 
		~_ErdCommitDialog();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class _CodePreviewDialog
///////////////////////////////////////////////////////////////////////////////
class _CodePreviewDialog : public wxDialog 
{
	private:
	
	protected:
		wxScintilla* m_scintilla3;
		wxStdDialogButtonSizer* m_sdbSizer2;
		wxButton* m_sdbSizer2OK;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnOKClick( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		_CodePreviewDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("SQL Preview"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,470 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ); 
		~_CodePreviewDialog();
	
};

#endif //__GUI__
