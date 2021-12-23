//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : windowattrmanager.cpp
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

#include "windowattrmanager.h"

#include "editor_config.h"

#include <wx/persist.h>
#include <wx/persist/bookctrl.h>
#include <wx/persist/toplevel.h>
#include <wx/settings.h>

void WindowAttrManager::Load(wxTopLevelWindow* win)
{
    if(win->GetName().IsEmpty()) {
        return;
    }
#if 0
    // Is this object already registered?
    if(wxPersistenceManager::Get().Find(win)) {
        wxPersistenceManager::Get().Restore(win);
    } else {
        // Register and restore the object and recurse into its children
        wxPersistenceManager::Get().RegisterAndRestore(win);
    }
    DoLoad(win, win->GetName(), 0);
#endif
    if(win->GetParent()) {
        win->CentreOnParent();
    }
}

void WindowAttrManager::DoLoad(wxWindow* win, const wxString& parentName, int depth)
{
    if(!win)
        return;
    int childIndex(0);
    wxWindowList::compatibility_iterator pclNode = win->GetChildren().GetFirst();
    while(pclNode) {
        ++childIndex;
        wxWindow* pclChild = pclNode->GetData();
        if(pclChild) {
            // Load the state of a notebook
            wxBookCtrlBase* bookbase = dynamic_cast<wxBookCtrlBase*>(pclChild);
            if(bookbase) {
                // Make sure that the book control has a name (which is needed by the wxPersistenceManager)
                wxString controlName;
                if(bookbase->GetName().IsEmpty()) {
                    controlName << parentName << "_book_" << depth << "_" << childIndex;
                    bookbase->SetName(controlName);
                } else {
                    controlName = bookbase->GetName();
                }
                if(wxPersistenceManager::Get().Find(bookbase)) {
                    wxPersistenceManager::Get().Restore(bookbase);
                } else {
                    // Register and restore the object and recurse into its children
                    wxPersistenceManager::Get().RegisterAndRestore(bookbase);
                }
            }

            // Recurse into this window children
            DoLoad(pclChild, parentName, depth + 1);
        }
        // Move on to the next sibling
        pclNode = pclNode->GetNext();
    }
}
