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
    : m_cur(0)
{
}

NavMgr::~NavMgr() { Clear(); }

NavMgr* NavMgr::Get()
{
    static NavMgr theManager;
    return &theManager;
}

void NavMgr::Clear()
{
    m_cur = 0;
    m_jumps.clear();
}

bool NavMgr::ValidLocation(const BrowseRecord& rec) const
{
    // ATTN: lineno == 1 implies a file was just opened, but before the find-and-select has happened
    // TODO: don't allow records for non-source files (*.diff, *.i, etc)
    return (!rec.filename.IsEmpty()) && (rec.lineno > 1);
}

bool NavMgr::CanNext() const { return m_cur + 1 < m_jumps.size(); }

bool NavMgr::CanPrev() const { return m_cur > 0; }

BrowseRecord NavMgr::GetNext() { return CanNext() ? m_jumps[++m_cur] : BrowseRecord(); }

BrowseRecord NavMgr::GetPrev() { return CanPrev() ? m_jumps[--m_cur] : BrowseRecord(); }

void NavMgr::AddJump(const BrowseRecord& from, const BrowseRecord& to)
{
    if(ValidLocation(from)) {
        // keep previous location only if it's not at position 0, and it is not equal to from
        if((m_cur > 0) && (!((m_jumps[m_cur].filename == from.filename) && (m_jumps[m_cur].lineno == from.lineno)))) {
            m_cur++;
        }
        m_jumps.resize(m_cur);
        m_jumps.push_back(from);
    }
    if(ValidLocation(to)) {
        // only add if there's an actual jump
        if((!m_jumps.empty()) &&
           (!((m_jumps[m_cur].filename == to.filename) && (m_jumps[m_cur].lineno == to.lineno)))) {
            m_cur++;
            m_jumps.resize(m_cur);
            m_jumps.push_back(to);
        }
    }
}

bool NavMgr::NavigateBackward(IManager* mgr) { return CanPrev() && mgr->OpenFile(GetPrev()); }

bool NavMgr::NavigateForward(IManager* mgr) { return CanNext() && mgr->OpenFile(GetNext()); }
