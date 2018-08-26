//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : windowstack.cpp
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
#include "drawingutils.h"
#include "windowstack.h"
#include <wx/dcbuffer.h>
#include <wx/wupdlock.h>

WindowStack::WindowStack(wxWindow* parent, wxWindowID id)
    : wxSimplebook(parent, id)
{
    // Disable the events by capturing them and not calling 'Skip()'
    Bind(wxEVT_NOTEBOOK_PAGE_CHANGED, [](wxBookCtrlEvent& event) { wxUnusedVar(event); });
    Bind(wxEVT_NOTEBOOK_PAGE_CHANGING, [](wxBookCtrlEvent& event) { wxUnusedVar(event); });
}

WindowStack::~WindowStack() {}

void WindowStack::Select(wxWindow* win)
{
#ifndef __WXOSX__
    wxWindowUpdateLocker locker(this);
#endif
    int index = FindPage(win);
    if(index == wxNOT_FOUND) { return; }
    ChangeSelection(index);
}

void WindowStack::Clear() { DeleteAllPages(); }

bool WindowStack::Remove(wxWindow* win)
{
#ifndef __WXOSX__
    wxWindowUpdateLocker locker(this);
#endif

    int index = FindPage(win);
    if(index == wxNOT_FOUND) { return false; }
    return RemovePage(index);
}

bool WindowStack::Delete(wxWindow* win)
{
#ifndef __WXOSX__
    wxWindowUpdateLocker locker(this);
#endif

    int index = FindPage(win);
    if(index == wxNOT_FOUND) { return false; }
    return DeletePage(index);
}

bool WindowStack::Add(wxWindow* win, bool select)
{
#ifndef __WXOSX__
    wxWindowUpdateLocker locker(this);
#endif
    if(!win || Contains(win)) { return false; }
    win->Reparent(this);
    AddPage(win, "", select, wxNOT_FOUND);
    return true;
}

bool WindowStack::Contains(wxWindow* win) { return FindPage(win) != wxNOT_FOUND; }

int WindowStack::FindPage(wxWindow* page) const
{
    for(size_t i = 0; i < GetPageCount(); ++i) {
        if(GetPage(i) == page) { return static_cast<int>(i); }
    }
    return wxNOT_FOUND;
}

wxWindow* WindowStack::GetSelected() const
{
    int index = GetSelection();
    if(index == wxNOT_FOUND) { return NULL; }
    return GetPage(index);
}
