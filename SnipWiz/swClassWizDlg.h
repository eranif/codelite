/////////////////////////////////////////////////////////////////////////////
// Name:        swclasswizdlg.h
// Purpose:
// Author:      Frank Lichtner
// Modified by:
// Created:     02/09/2008 09:34:22
// Copyright:   2008 Frank Lichtner
// Licence:		GNU General Public Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _SWCODEWIZDLG_H_
#define _SWCODEWIZDLG_H_


/*!
 * Includes
 */

////@begin includes
#include "wx/notebook.h"
////@end includes
#include "swStringDb.h"
/*!
 * Forward declarations
 */

////@begin forward declarations
class wxNotebook;
////@end forward declarations
class SnipWiz;
/*!
 * Control identifiers
 */

////@begin control identifiers
#define SYMBOL_SWCLASSWIZDLG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_SWCLASSWIZDLG_TITLE _("Template Class Wizard ")
#define SYMBOL_SWCLASSWIZDLG_IDNAME ID_swClassWizDlg
#define SYMBOL_SWCLASSWIZDLG_SIZE wxSize(400, 300)
#define SYMBOL_SWCLASSWIZDLG_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * swCodeWizDlg class declaration
 */
class swClassWizDlg: public wxDialog
{
	DECLARE_DYNAMIC_CLASS( swClassWizDlg )
	DECLARE_EVENT_TABLE()

public:
	/// saves the generated class file
	bool SaveBufferToFile( const wxString filename, const wxString buffer, int eolType = 0 );
	/// modified flag
	bool IsModified() const { return m_modified; }
	/// returns StringDb
	swStringDb* GetStringDb() {	return m_pPlugin->GetStringDb(); }
	// Constructors
	swClassWizDlg();
	swClassWizDlg( wxWindow* parent, wxWindowID id = SYMBOL_SWCLASSWIZDLG_IDNAME, const wxString& caption = SYMBOL_SWCLASSWIZDLG_TITLE, const wxPoint& pos = SYMBOL_SWCLASSWIZDLG_POSITION, const wxSize& size = SYMBOL_SWCLASSWIZDLG_SIZE, long style = SYMBOL_SWCLASSWIZDLG_STYLE );

	// Creation
	bool Create( wxWindow* parent, wxWindowID id = SYMBOL_SWCLASSWIZDLG_IDNAME, const wxString& caption = SYMBOL_SWCLASSWIZDLG_TITLE, const wxPoint& pos = SYMBOL_SWCLASSWIZDLG_POSITION, const wxSize& size = SYMBOL_SWCLASSWIZDLG_SIZE, long style = SYMBOL_SWCLASSWIZDLG_STYLE );

	// Destructor
	~swClassWizDlg();

	// Initialises member variables
	void Init();

	// Creates the controls and sizers
	void CreateControls();

////@begin swClassWizDlg event handler declarations

	/// wxEVT_INIT_DIALOG event handler for ID_swClassWizDlg
	void OnInitDialog( wxInitDialogEvent& event );

	/// wxEVT_COMMAND_TEXT_UPDATED event handler for ID_TEXTCTRL
	void OnClassNameUpdated( wxCommandEvent& event );

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_GET_VIRT_FOLDER
	void OnGetVirtFolderClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_GET_HD_FOLDER
	void OnGetHdFolderClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_GENERATE
	void OnGenerateClick( wxCommandEvent& event );

	/// wxEVT_UPDATE_UI event handler for ID_GENERATE
	void OnGenerateUpdate( wxUpdateUIEvent& event );

	/// wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_TEMPLATES
	void OnTemplatesSelected( wxCommandEvent& event );

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_ADD_TEMPLATE
	void OnAddTemplateClick( wxCommandEvent& event );

	/// wxEVT_UPDATE_UI event handler for ID_ADD_TEMPLATE
	void OnAddTemplateUpdate( wxUpdateUIEvent& event );

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_CHANGE_TEMPLATE
	void OnChangeTemplateClick( wxCommandEvent& event );

	/// wxEVT_UPDATE_UI event handler for ID_CHANGE_TEMPLATE
	void OnChangeTemplateUpdate( wxUpdateUIEvent& event );

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_REMOVE_TEMPLATE
	void OnRemoveTemplateClick( wxCommandEvent& event );

	/// wxEVT_UPDATE_UI event handler for ID_REMOVE_TEMPLATE
	void OnRemoveTemplateUpdate( wxUpdateUIEvent& event );

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_CLEAR
	void OnClearClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_CLASS_VAR
	void OnClassVarClick( wxCommandEvent& event );

////@end swClassWizDlg event handler declarations
	/// sets the IManager pointer
	void SetManager( IManager* pManager ) { m_pManager = pManager; }
	/// sets the pugin path
	void SetPluginPath( wxString value ) { m_pluginPath = value ; }
	/// sets  current EOL
	void SetCurEol( int value ) { m_curEol = value ; }
	/// sets virtual folder
	void SetVirtualFolder( wxString value ) { m_virtualFolder = value ; }
	/// sets project path
	void SetProjectPath( wxString value ) { m_projectPath = value ; }

	// Should we show tooltips?
	static bool ShowToolTips();

////@begin swClassWizDlg member variables
	wxTextCtrl* m_pClassName;
	wxComboBox* m_pCurrentTemplate;
	wxTextCtrl* m_pH_filename;
	wxTextCtrl* m_pCPP_filename;
	wxStaticText* m_pVFolderText;
	wxTextCtrl* m_pVirtFolder;
	wxTextCtrl* m_pHdPath;
	wxButton* m_pGenerate;
	wxComboBox* m_pTemplates;
	wxNotebook* m_pFilesNotebook;
	wxTextCtrl* m_pHeaderText;
	wxTextCtrl* m_pSourceText;
	wxString m_pluginPath; // base path to template file
	int m_curEol; // current EOL
	wxString m_virtualFolder; // name of current project
	wxString m_projectPath; // path to current project
	/// Control identifiers
	enum
	{
		ID_swClassWizDlg = 10007,
		ID_NOTEBOOK1 = 10009,
		ID_PANEL = 10008,
		ID_TEXTCTRL = 10000,
		ID_CURRENT_TEMPLATE = 10040,
		ID_TEXTCTRL1 = 10002,
		ID_TEXTCTRL2 = 10003,
		ID_VIRTFOLDER_TEXT = 10014,
		ID_VIRTFOLDER = 10015,
		ID_GET_VIRT_FOLDER = 10013,
		ID_TEXTCTRL3 = 10001,
		ID_GET_HD_FOLDER = 10005,
		ID_GENERATE = 10004,
		ID_PANEL1 = 10010,
		ID_TEMPLATES = 10045,
		ID_ADD_TEMPLATE = 10046,
		ID_CHANGE_TEMPLATE = 10053,
		ID_REMOVE_TEMPLATE = 10047,
		ID_CLEAR = 10049,
		ID_CLASS_VAR = 10048,
		ID_FILES_NOTEBOOK = 10050,
		ID_TEXT_HEADER = 10051,
		ID_TEXT_SOURCE = 10052
	};
////@end swClassWizDlg member variables
	SnipWiz* m_pPlugin;			// pointer to plugin
	bool m_modified;			// modified flag
	IManager* m_pManager;		// pointer to IManager
	void RefreshTemplateList();	// regenerates template list after changes
};

#endif
// _SWCODEWIZDLG_H_
