//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : clTreeKeyboardInput.h
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

#ifndef CLTREEKEYBOARDINPUT_H
#define CLTREEKEYBOARDINPUT_H

#include "clTreeCtrl.h"
#include "codelite_exports.h"
#include "wx/event.h"
#include <list>
#include <vector>
#include <wx/sharedptr.h>

class WXDLLIMPEXP_SDK clTreeKeyboardInput : public wxEvtHandler
{
public:
    typedef wxSharedPtr<clTreeKeyboardInput> Ptr_t;

protected:
    clTreeCtrl* m_tree = nullptr;

protected:
    void OnSearch(wxTreeEvent& event);
    void OnClearSearch(wxTreeEvent& event);

public:
    clTreeKeyboardInput(clTreeCtrl* tree);
    virtual ~clTreeKeyboardInput();
};

#endif // CLTREEKEYBOARDINPUT_H
