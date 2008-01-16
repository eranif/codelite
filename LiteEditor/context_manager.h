#ifndef CONTEXT_MANAGER_H
#define CONTEXT_MANAGER_H

#include "singleton.h"
#include "wx/string.h"
#include "map"
#include "context_base.h"
#include "wx/window.h"

class ContextManager;

class ContextManager : public Singleton<ContextManager> {
	friend class Singleton<ContextManager>;
	std::map<wxString, ContextBasePtr> m_contextPool;

public:
	/**
	 * Return new context by name
	 * \param lexerName the lexer name
	 * \param parent the context parent
	 * \return 
	 */
	ContextBasePtr NewContext(wxWindow *parent, const wxString &lexerName);

private:
	ContextManager();
	virtual ~ContextManager();
};
#endif // CONTEXT_MANAGER_H
