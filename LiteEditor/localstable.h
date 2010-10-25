#ifndef __localstable__
#define __localstable__

#include "simpletablebase.h"
#include <wx/arrstr.h>
#include "debuggerobserver.h"
#include "debuggersettings.h"

#define LIST_LOCALS_CHILDS  600
#define QUERY_LOCALS_CHILDS 601

class LocalsTable : public DebuggerTreeListCtrlBase
{

	DebuggerPreDefinedTypes m_preDefTypes;
	bool                    m_resolveLocals;
	
protected:
	void          DoClearNonVariableObjectEntries(wxArrayString& itemsNotRemoved, size_t flags);
	void          DoUpdateLocals  (const LocalVariables& locals, size_t kind);

	// Events
	void          OnItemExpanding(wxTreeEvent& event);

public:
	LocalsTable(wxWindow *parent);
	virtual ~LocalsTable();

public:
	/**
	 * @brief callback to IDebugger::CreateVariableObject
	 * @param event
	 */
	void OnCreateVariableObj  (const DebuggerEvent& event);
	/**
	 * @brief callback to IDebugger::ListChildren
	 */
	void OnListChildren       (const DebuggerEvent& event);
	/**
	 * @brief called to IDEbugger::UpdateVariableObject
	 */
	void OnVariableObjUpdate  (const DebuggerEvent& event);

	void UpdateLocals  (const LocalVariables& locals);
	void UpdateFuncArgs(const LocalVariables& args);
	void Initialize    ();
};
#endif // __localstable__
