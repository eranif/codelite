//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : menumanager.h
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
#ifndef MENUMANAGER_H
#define MENUMANAGER_H

#include "menu_event_handlers.h"
#include "singleton.h"
#include "smart_ptr.h"
#include <map>

//-------------------------------------------------
// The menu event manager.
//-------------------------------------------------

WX_DECLARE_HASH_MAP(int, MenuEventHandlerPtr, wxIntegerHash, wxIntegerEqual, HandlesrHash);

class MenuManager : public Singleton<MenuManager>
{
    friend class Singleton<MenuManager>;
    HandlesrHash m_handlers;

public:
    // register new event handler
    void PushHandler(MenuEventHandlerPtr handler);

    MenuEventHandlerPtr GetHandler(int id);

private:
    MenuManager(void);
    virtual ~MenuManager(void);
};

#endif // MENUMANAGER_H
