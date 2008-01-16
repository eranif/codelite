#ifndef FIND_IN_FILES_DLG_H
#define FIND_IN_FILES_DLG_H

#include "wx/dialog.h"
#include "findreplacedlg.h"

class wxTextCtrl;
class wxCheckBox;
class wxButton;
class wxStaticText;
class DirPicker;
class wxComboBox;

extern const wxEventType wxEVT_FIF_FIND;
extern const wxEventType wxEVT_FIF_STOP;
extern const wxEventType wxEVT_FIF_CLOSE;

class FindInFilesDialog : public wxDialog
{
	wxEvtHandler *m_owner;

	FindReplaceData m_data;

	// Options
	wxComboBox *m_findString;
	wxCheckBox *m_matchCase;
	wxCheckBox *m_matchWholeWord;
	wxCheckBox *m_regualrExpression;
	DirPicker *m_dirPicker;
	wxComboBox *m_fileTypes;

	// Buttons
	wxButton *m_find;
	wxButton *m_stop;
	wxButton *m_cancel;
	
public:
	virtual ~FindInFilesDialog( );
	FindInFilesDialog();
	FindInFilesDialog(	wxWindow* parent, 
						const FindReplaceData& data, 
						wxWindowID id = wxID_ANY, 
						const wxString& caption = wxT("Find In Files"), 
						const wxPoint& pos = wxDefaultPosition, 
						const wxSize& size = wxSize(400, 200), 
						long style = wxDEFAULT_DIALOG_STYLE);

	// Creation
	bool Create(wxWindow* parent, 
				const FindReplaceData& data, 
				wxWindowID id = wxID_ANY, 
				const wxString& caption = wxT("Find In Files"), 
				const wxPoint& pos = wxDefaultPosition, 
				const wxSize& size = wxSize(400, 200), 
				long style = wxDEFAULT_DIALOG_STYLE
				);
 
	// Return the data 
	FindReplaceData& GetData() { return m_data; }
	void SetData(FindReplaceData &data);

	virtual bool Show();

	void SetEventOwner(wxEvtHandler *owner) { m_owner = owner; }
	wxEvtHandler *GetEventOwner() const { return m_owner; }

protected:
	void CreateGUIControls();
	void ConnectEvents();
	void OnClick(wxCommandEvent &event);
	void SendEvent(wxEventType type);
	void DoSearch();

	DECLARE_EVENT_TABLE()
	void OnClose(wxCloseEvent &event);
	void OnCharEvent(wxKeyEvent &event);
};

#endif // FIND_IN_FILES_DLG_H
