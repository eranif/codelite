//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : buidltab.h
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
#ifndef __errorstab__
#define __errorstab__

#include <vector>
#include "buidltab.h"


class ErrorsTab : protected OutputTabWindow
{
	friend class BuildTab;

private:
	BuildTab *m_bt;
	std::map<int,int> m_lineMap;

	void ClearLines();
	bool IsShowing(int line);
	void AppendLine(int line);
	void MarkLine(int line);

	void OnRedisplayLines(wxCommandEvent   &e);
	void OnClearAll      (wxCommandEvent   &e);
	void OnRepeatOutput  (wxCommandEvent   &e);
	void OnClearAllUI    (wxUpdateUIEvent  &e);
	void OnRepeatOutputUI(wxUpdateUIEvent  &e);
	void OnMouseDClick   (wxScintillaEvent &e);
	DECLARE_EVENT_TABLE()

public:
	ErrorsTab(BuildTab *bt, wxWindow *parent, wxWindowID id, const wxString &name);
	~ErrorsTab();
};

#endif // __errorstab__
