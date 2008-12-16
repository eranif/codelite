//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : navigationmanager.cpp
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
#include "navigationmanager.h"

NavMgr::NavMgr()
		: m_cur(wxNOT_FOUND)
{
}

NavMgr::~NavMgr()
{
	Clear();
}

NavMgr *NavMgr::Get()
{
	static NavMgr theManager;
	return &theManager;
}

void NavMgr::Clear()
{
	m_cur = wxNOT_FOUND;
	m_records.clear();
}

bool NavMgr::CanNext() const
{
	int cur = m_cur;
	cur++;
	return (cur < (int)m_records.size());
}

bool NavMgr::CanPrev() const
{
	int cur = m_cur;
	cur--;
	return (cur >= 0);
}

BrowseRecord NavMgr::GetNextRecord()
{
	if (!CanNext()) {
		return BrowseRecord();
	}

	m_cur++;
	return m_records.at(m_cur);
}

BrowseRecord NavMgr::GetPrevRecord()
{
	if (!CanPrev()) {
		return BrowseRecord();
	}
	m_cur--;
	return m_records.at(m_cur);
}

void NavMgr::Push(const BrowseRecord &rec)
{
	m_records.insert(m_records.end(), rec);
	m_cur = (int)m_records.size();
}

void NavMgr::NavigateBackward(IEditor *editor, IManager *mgr)
{
	if (!CanPrev())
		return;

	// before jumping, save the current position
	if ( CanNext() == false ) {
		if (editor) {
			//keep this location as well, but make sure we dont add this twice
			BrowseRecord record = editor->CreateBrowseRecord();
			BrowseRecord last = m_records.back();
			if (!(last.filename == record.filename && last.lineno == record.lineno && last.position == record.position)) {
				//different item, we can add it
				Push(record);
				//so we dont get this location again
				GetPrevRecord();
			}
		}
	}

	BrowseRecord rec = GetPrevRecord();
	mgr->OpenFile( rec );
}
