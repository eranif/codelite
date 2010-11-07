#ifndef BUILDPROCESS_H
#define BUILDPROCESS_H

#include "asyncprocess.h"

class BuildProcess
{
	IProcess*     m_process;
	wxEvtHandler* m_evtHandler;
	wxString      m_fileName;

public:
	BuildProcess();
	virtual ~BuildProcess();

	bool Execute(const wxString &cmd, const wxString &fileName, const wxString &workingDirectory, wxEvtHandler *evtHandler);
	void Stop();
	bool IsBusy();

	void SetFileName(const wxString& fileName) {
		this->m_fileName = fileName;
	}
	const wxString& GetFileName() const {
		return m_fileName;
	}

	int GetPid() const {
		if(m_process) {
			return m_process->GetPid();
		}
		return wxNOT_FOUND;
	}
};

#endif // BUILDPROCESS_H
