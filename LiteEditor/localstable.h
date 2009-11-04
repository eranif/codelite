#ifndef __localstable__
#define __localstable__

#include "simpletablebase.h"
#include <wx/arrstr.h>
#include "debuggerobserver.h"

class LocalsTable : public LocalsTableBase {

protected:
	void DoShowDetails (long item);
	long DoGetIdxByName(const wxString &name);

public:
	LocalsTable(wxWindow *parent);
	virtual ~LocalsTable();

public:
	virtual void OnItemActivated( wxListEvent& event );
	virtual void OnItemSelected( wxListEvent& event );

	void UpdateLocals  (const LocalVariables &locals);
	void Clear         ();
};
#endif // __localstable__
