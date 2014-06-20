//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : listctrlpanel.h
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
#ifndef DebuggerCallstackView_H
#define DebuggerCallstackView_H

#include "listctrlpanelbase.h"
#include "debugger.h"
#include "cl_command_event.h"

/** Implementing ListCtrlPanelBase */
class DebuggerCallstackView : public ListCtrlPanelBase
{
protected:
    // Handlers for ListCtrlPanelBase events.
    void OnCopyBacktrace(wxCommandEvent &event);
    void OnUpdateBacktrace(clCommandEvent &e);
    
public:
    virtual void OnMenu(wxDataViewEvent& event);
    virtual void OnItemActivated(wxDataViewEvent& event);
    /** Constructor */
    DebuggerCallstackView( wxWindow* parent );
    void Update(const StackEntryArray &stackArr);
    void SetCurrentLevel(const int level);
    void Clear();

private:
    int m_currLevel;
    StackEntryArray m_stack;
    DebuggerBtImgList m_images;
};

#endif // DebuggerCallstackView_H
