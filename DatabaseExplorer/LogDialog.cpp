//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : LogDialog.cpp
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

#include "LogDialog.h"

LogDialog::LogDialog(wxWindow* parent):_LogDialog(parent)
{
	m_canClose = false;
	
	m_text = _(" --------------- Log starts at: ") + wxNow() + " -------------------\n";
	m_textCtrl11->SetValue(m_text);	
}

void LogDialog::AppendText(const wxString& txt)
{
	m_text.Append(txt + wxT("\n"));
	m_textCtrl11->SetValue(m_text);
}

void LogDialog::Clear()
{
	m_text.clear();
	m_textCtrl11->SetValue(m_text);
}

void LogDialog::OnCloseClick(wxCommandEvent& event)
{
	Clear();
	Destroy();
}
void LogDialog::OnCloseUI(wxUpdateUIEvent& event)
{
	event.Enable(m_canClose);
}

void LogDialog::AppendSeparator()
{
	m_text.Append("*********************************************************\n");
	m_textCtrl11->SetValue(m_text);
}

void LogDialog::AppendComment(const wxString& txt)
{
	m_text.Append(wxNow() + txt + wxT("\n"));
	m_textCtrl11->SetValue(m_text);
}

