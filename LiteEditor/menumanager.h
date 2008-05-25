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
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ \                     
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

#include "singleton.h"
#include "menu_event_handlers.h"
#include "smart_ptr.h"
#include <map>

//-------------------------------------------------
// The menu event manager.
//-------------------------------------------------

class MenuManager : public Singleton<MenuManager>
{
	friend class Singleton<MenuManager>;
	std::map<int, MenuEventHandlerPtr> m_handlers;
public:

	// register new event handler
	void PushHandler(MenuEventHandlerPtr handler){
		m_handlers[handler->GetEventId()] = handler;
	};

	MenuEventHandlerPtr GetHandler(int id) {
		std::map<wxEventType, MenuEventHandlerPtr>::iterator iter = m_handlers.find(id);
		if( iter != m_handlers.end() )
			return iter->second;
		return NULL;
	};

private:
	MenuManager(void);
	virtual ~MenuManager(void);
};

#endif // MENUMANAGER_H

