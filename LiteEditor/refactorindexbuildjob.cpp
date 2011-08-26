//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : refactorindexbuildjob.cpp
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
#include <wx/sizer.h>
#include <wx/stopwatch.h>
#include <wx/progdlg.h>
#include "cppwordscanner.h"
#include <wx/filename.h>
#include "refactorindexbuildjob.h"
#include "progress_dialog.h"
#include "workspace.h"

//#define POST_NEW_STATUS(msg, value, act)
//	status = new RefactorIndexBuildJobInfo;
//	status->filename = msg;
//	status->status = value;
//	status->action = act;
//	Post(status);

RefactorIndexBuildJob::RefactorIndexBuildJob(const std::vector<wxFileName> &files, const wxChar *word)
		: m_files( files )
		, m_word(word)
{
}

RefactorIndexBuildJob::~RefactorIndexBuildJob()
{
}

void RefactorIndexBuildJob::Parse(const wxString &word, CppTokensMap &l)
{
	clProgressDlg* prgDlg = NULL;
	// Create a progress dialog
	prgDlg = new clProgressDlg (NULL, _("Gathering required information..."), wxT(""), (int)m_files.size());
	prgDlg->Update(0, _("Gathering required information..."));

	size_t i=0;
	for ( ; i<m_files.size(); i++) {

		wxFileName fn = m_files.at(i);
		CppWordScanner scanner(fn.GetFullPath().mb_str().data());

		wxString msg;
		msg << _("Parsing: ") << fn.GetFullName();
		// update the progress bar
		if (!prgDlg->Update(i, msg)){
			prgDlg->Destroy();
			l.clear();
			return;
		}

		scanner.Match(word.mb_str().data(), l);
	}
	prgDlg->Destroy();
}

void RefactorIndexBuildJob::Process(wxThread* thread)
{
	CppTokensMap l;
	Parse(m_word.c_str(), l);
}
