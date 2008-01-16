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

