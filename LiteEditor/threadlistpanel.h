//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : threadlistpanel.h
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
#ifndef __threadlistpanel__
#define __threadlistpanel__

#include "ThreadListPanelBase.h"
#include "debugger.h"

class ThreadListPanel : public ThreadListBasePanel
{
protected:
    ThreadEntryArray m_threads;

protected:
    // Handlers for ThreadListBasePanel events.
    void OnItemActivated(wxDataViewEvent& event);
    bool IsTheSame(const ThreadEntryArray& threads1, const ThreadEntryArray& threads2);

public:
    ThreadListPanel(wxWindow* parent);
    virtual ~ThreadListPanel();
    void PopulateList(const ThreadEntryArray& threads);
    void Clear();
};

#endif // __threadlistpanel__
