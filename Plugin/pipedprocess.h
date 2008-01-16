#ifndef PipedProcess_H
#define PipedProcess_H

#include "wx/process.h"
#include "wx/string.h"

class PipedProcess : public wxProcess
{
	long m_pid;
	wxString m_cmd;

public:
	/**
	 * Constructs a process object. id is only used in the case you want to use wxWidgets events. 
	 * It identifies this object, or another window that will receive the event.
	 */
	PipedProcess(int id, const wxString &cmdLine);

	virtual ~PipedProcess();

	/**
	 * \return return the process id
	 */
	long GetPid();

	/**
	 * \param set the process ID
	 */
	void SetPid(long pid);

	/**
	 * Kill the process
	 */
	void Terminate();

	/** 
	 * Start the process
	 * \return the process id
	 */
	long Start(bool hide = true);
	bool HasInput(wxString &input);
	
	/**
	 * \brief read all input from the process output stream
	 * \param input
	 * \return true if some data was read, false otherwise
	 */
	bool ReadAll(wxString &input);
};

#endif // PipedProcess_H

