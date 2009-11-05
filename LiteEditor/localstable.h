#ifndef __localstable__
#define __localstable__

#include "simpletablebase.h"
#include <wx/arrstr.h>
#include "debuggerobserver.h"
#include "debuggersettings.h"

class LocalsTable : public LocalsTableBase {

	std::vector<DebuggerCmdData> m_dbgCmds;
public:
	static wxString GetRealType (const wxString &gdbType);

protected:
	void     DoShowDetails (long item);
	long     DoGetIdxByName(const wxString &name);

public:
	LocalsTable(wxWindow *parent);
	virtual ~LocalsTable();

public:
	virtual void OnItemActivated( wxListEvent& event );
	virtual void OnItemSelected( wxListEvent& event );

	void UpdateLocals  (const LocalVariables &locals);
	void UpdateFuncArgs(const LocalVariables &args  );
	void Clear         ();
	void Initialize    ();
};
#endif // __localstable__
