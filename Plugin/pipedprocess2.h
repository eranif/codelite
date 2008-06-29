#ifndef __pipedprocess2__
#define __pipedprocess2__

#include "pipedprocess.h"
#include "procutils.h"

class PipedProcessTS : public PipedProcess, public IProcess {
	
	wxCriticalSection m_cs;
	bool m_isRunning;
	
public:
	PipedProcessTS(int id, const wxString &cmdLine);
	virtual ~PipedProcessTS();

public:
	virtual long Start(bool hide = true);
	bool IsRunning();
	void SetRunning(bool r){m_isRunning = r;}
	bool Write(const wxString &msg);
	bool ReadAll(wxString &msg);
};
#endif // __pipedprocess2__
