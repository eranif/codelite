//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : findinfilesdlg.h
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
 #ifndef FIND_IN_FILES_DLG_H
#define FIND_IN_FILES_DLG_H

#include "wx/dialog.h"
#include "findreplacedlg.h"
#include "search_thread.h"

class wxTextCtrl;
class wxCheckBox;
class wxButton;
class wxStaticText;
class DirPicker;
class wxComboBox;
class wxChoice;

class FindInFilesDialog : public wxDialog
{
	wxEvtHandler *m_owner;

	FindReplaceData m_data;

	// Options
	wxComboBox *m_findString;
	wxCheckBox *m_matchCase;
	wxCheckBox *m_printScope;
	wxCheckBox *m_matchWholeWord;
	wxCheckBox *m_regualrExpression;
	wxCheckBox *m_fontEncoding;
	DirPicker *m_dirPicker;
	wxComboBox *m_fileTypes;
	wxChoice *m_searchResultsTab;

	// Buttons
	wxButton *m_find;
	wxButton *m_stop;
	wxButton *m_cancel;
	wxButton *m_replaceAll;

public:
	virtual ~FindInFilesDialog( );
	FindInFilesDialog();
	FindInFilesDialog(	wxWindow* parent,
						const FindReplaceData& data,
						wxWindowID id = wxID_ANY,
						const wxString& caption = wxT("Find In Files"),
						const wxPoint& pos = wxDefaultPosition,
						const wxSize& size = wxSize(450, 200),
						long style = wxDEFAULT_DIALOG_STYLE);

	// Creation
	bool Create(wxWindow* parent,
				const FindReplaceData& data,
				wxWindowID id = wxID_ANY,
				const wxString& caption = wxT("Find In Files"),
				const wxPoint& pos = wxDefaultPosition,
				const wxSize& size = wxSize(450, 200),
				long style = wxDEFAULT_DIALOG_STYLE
				);

	// Return the data
	FindReplaceData& GetData() { return m_data; }
	void SetData(FindReplaceData &data);

	virtual bool Show();

	void SetEventOwner(wxEvtHandler *owner) { m_owner = owner; }
	wxEvtHandler *GetEventOwner() const { return m_owner; }

    void SetRootDir(const wxString &rootDir);

protected:
	void CreateGUIControls();
	void ConnectEvents();
	void OnClick(wxCommandEvent &event);
	void SendEvent(wxEventType type);
	SearchData DoGetSearchData();
	void DoSearch();
	void DoSearchReplace();

	DECLARE_EVENT_TABLE()
	void OnClose(wxCloseEvent &event);
	void OnCharEvent(wxKeyEvent &event);
};

#endif // FIND_IN_FILES_DLG_H
