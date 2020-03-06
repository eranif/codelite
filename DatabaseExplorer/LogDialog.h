//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : LogDialog.h
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

#ifndef LOGDIALOG_H
#define LOGDIALOG_H

#include "GUI.h" // Base class: _LogDialog
#include <wx/utils.h> 

class LogDialog : public _LogDialog {

public:
	LogDialog(wxWindow* parent);
	virtual ~LogDialog();

	virtual void OnCloseClick(wxCommandEvent& event);
	virtual void OnCloseUI(wxUpdateUIEvent& event);
	
	/*! \brief Enable close button. */
	void EnableClose(bool enable){ m_canClose = enable; }
	/*! \brief Clear text field */
	void Clear();
	/*! \brief Add text line */
	void AppendText(const wxString& txt);
	/*! \brief Add comment line */
	void AppendComment(const wxString& txt);
	/*! \brief Add separator. */
	void AppendSeparator();
	
protected:
	bool m_canClose;
	wxString m_text;
};

#endif // LOGDIALOG_H
