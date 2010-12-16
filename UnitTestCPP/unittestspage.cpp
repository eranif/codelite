//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : unittestspage.cpp
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

#include "workspace.h"
#include "globals.h"
#include "unittestspage.h"
#include "imanager.h"
#include <wx/msgdlg.h>

UnitTestsPage::UnitTestsPage(wxWindow* parent, TestSummary* summary, IManager *mgr )
		: UnitTestsBasePage( parent, wxID_ANY, wxDefaultPosition, wxSize(1, 1), 0 )
		, m_mgr(mgr)
{
#ifdef __WXDEBUG__
	summary->PrintSelf();
#endif

	m_progressPassed->SetMaxRange((size_t)summary->totalTests);
	m_progressFailed->SetMaxRange((size_t)summary->totalTests);
	m_progressFailed->SetFillCol(wxT("RED"));
	m_progressPassed->SetFillCol(wxT("PALE GREEN"));

	wxString msg;
	msg << summary->totalTests;
	m_staticTextTotalTests->SetLabel(msg);

	msg.clear();
	msg << summary->errorCount;
	m_staticTextFailTestsNum->SetLabel(msg);

	msg.clear();
	msg << summary->totalTests - summary->errorCount;
	m_staticTextSuccessTestsNum->SetLabel(msg);

	m_listCtrlErrors->InsertColumn(0, _("File"));
	m_listCtrlErrors->InsertColumn(1, _("Line"));
	m_listCtrlErrors->InsertColumn(2, _("Description"));

	for (size_t i=0; i<summary->errorLines.GetCount(); i++) {
		ErrorLineInfo info = summary->errorLines.Item(i);
		long row = AppendListCtrlRow(m_listCtrlErrors);
		SetColumnText(m_listCtrlErrors, row, 0, info.file);
		SetColumnText(m_listCtrlErrors, row, 1, info.line);
		SetColumnText(m_listCtrlErrors, row, 2, info.description);
	}

	m_listCtrlErrors->SetColumnWidth(0, 200);
	m_listCtrlErrors->SetColumnWidth(1, 100);
	m_listCtrlErrors->SetColumnWidth(2, wxLIST_AUTOSIZE);
}

void UnitTestsPage::UpdateFailedBar(size_t amount, const wxString& msg)
{
	m_progressFailed->Update(amount, msg);
}

void UnitTestsPage::UpdatePassedBar(size_t amount, const wxString& msg)
{
	m_progressPassed->Update(amount, msg);
}

void UnitTestsPage::OnItemActivated(wxListEvent& e)
{
	wxString file = GetColumnText(m_listCtrlErrors, e.m_itemIndex, 0);
	wxString line = GetColumnText(m_listCtrlErrors, e.m_itemIndex, 1);

	long l;
	line.ToLong(&l);

	// convert the file to absolute path
	wxString err_msg, cwd;
	wxString proj = m_mgr->GetWorkspace()->GetActiveProjectName();
	ProjectPtr p = m_mgr->GetWorkspace()->FindProjectByName(proj, err_msg);

	if(p) {
		cwd = p->GetFileName().GetPath();
	}

	wxFileName fn(file);
	fn.MakeAbsolute(cwd);

	m_mgr->OpenFile(fn.GetFullPath(), proj, l-1);
}
