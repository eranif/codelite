#ifndef __localstable__
#define __localstable__

#include "simpletablebase.h"
#include <wx/arrstr.h>
#include "debuggerobserver.h"
#include "debuggersettings.h"

#define LIST_LOCALS_CHILDS  600
#define QUERY_LOCALS_CHILDS 601

class LocalsTable : public LocalsTableBase
{

	DebuggerPreDefinedTypes          m_preDefTypes;
	std::map<wxString, wxTreeItemId> m_listChildItemId;
	std::map<wxString, wxTreeItemId> m_createVarItemId;
	std::map<wxString, wxTreeItemId> m_gdbIdToTreeId;
	
public:
	enum {
		Locals         = 0x00000001,
		FuncArgs       = 0x00000002,
		VariableObject = 0x00000004
	};
	
protected:
	IDebugger*    DoGetDebugger();
	wxString      DoGetGdbId(const wxTreeItemId& item);
	void          DoDeleteWatch(const wxTreeItemId& item);
	void          DoClearNonVariableObjectEntries(wxArrayString& itemsNotRemoved, size_t flags);
	void          DoRefreshItem(IDebugger *dbgr, const wxTreeItemId &item);
	void          DoRefreshItemRecursively(IDebugger *dbgr, const wxTreeItemId &item);
	wxTreeItemId  DoFindItemByGdbId(const wxString& gdbId);
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
	 * @brief callback to IDebugger::EvaluateVariableObj
	 */
	void OnEvaluateVariableObj(const DebuggerEvent& event);
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
	void Clear         ();
	void Initialize    ();
};
#endif // __localstable__
