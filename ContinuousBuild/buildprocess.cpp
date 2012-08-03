#include "buildprocess.h"

BuildProcess::BuildProcess()
: m_process(NULL)
, m_evtHandler(NULL)
{
}

BuildProcess::~BuildProcess()
{
	Stop();
}

bool BuildProcess::Execute(const wxString& cmd, const wxString &fileName, const wxString& workingDirectory, wxEvtHandler *evtHandler)
{
	if(m_process)
		return false;

	m_process = CreateAsyncProcess(evtHandler, cmd, IProcessCreateDefault, workingDirectory);
	if(!m_process)
		return false;

	SetFileName(fileName);
	return true;
}

void BuildProcess::Stop()
{
	if(m_process){
        delete m_process;
		m_process = NULL;
	}
	m_fileName.Clear();
}

bool BuildProcess::IsBusy()
{
	return m_process != NULL;
}

