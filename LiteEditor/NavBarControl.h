//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : NavBarControl.h
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

#ifndef NAVBARCONTROL_H
#define NAVBARCONTROL_H
#include "wxcrafter.h"

class NavBarControl : public NavBarControlBaseClass
{
public:
    NavBarControl(wxWindow* parent);
    virtual ~NavBarControl();
protected:
    virtual void OnFuncListMouseDown(wxMouseEvent& event);
    virtual void OnFunction(wxCommandEvent& event);
    virtual void OnScope(wxCommandEvent& event);
    virtual void OnScopeListMouseDown(wxMouseEvent& event);
};
#endif // NAVBARCONTROL_H
