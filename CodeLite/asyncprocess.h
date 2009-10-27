#ifndef I_PROCESS_H
#define I_PROCESS_H

#include <wx/string.h>
#include <wx/event.h>

/**
 * @class IProcess
 * @author eran
 * @date 10/09/09
 * @file i_process.h
 * @brief
 */
class IProcess
{
protected:
	wxEvtHandler *m_parent;
	int           m_pid;

public:
	IProcess(wxEvtHandler *parent) : m_parent(parent), m_pid(-1) {}
	virtual ~IProcess() {}

	// Read from process stdout - return immediately if no data is available
	virtual bool Read(wxString& buff) = 0;

	// Write to the process stdin
	virtual bool Write(const wxString& buff) = 0;

	// Return true if the process is still alive
	virtual bool IsAlive() = 0;

	// Clean the process resources and kill the process if it is
	// still alive
	virtual void Cleanup() = 0;
	
	void SetPid(const int& pid) {
		this->m_pid = pid;
	}
	const int& GetPid() const {
		return m_pid;
	}
};

// Help method
IProcess* CreateAsyncProcess(wxEvtHandler *parent, const wxString& cmd, const wxString &workingDir = wxEmptyString);
#endif // I_PROCESS_H
