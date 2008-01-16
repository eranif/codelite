#ifndef INTERACTIVPROCESS_H
#define INTERACTIVPROCESS_H

#include "wx/event.h"
#include "wx/timer.h"
#include "pipedprocess.h"

class InteractiveProcess : public wxEvtHandler
{
protected:
	PipedProcess *m_proc;
	wxTimer *m_timer;
	bool m_busy;
	bool m_stop;
	wxCriticalSection m_cs;
	bool m_inUse;
	bool m_canUse;

protected:
	void OnTimer(wxTimerEvent &e);
	void OnProcessEnd(wxProcessEvent &e);
	void CleanUp();

public:
	bool IsBusy() const { return m_busy; }
	void SetBusy(bool busy) { m_busy = busy; }
	void StopProcess();

	//construct a compiler action 
	// \param owner the window owner for this action
	InteractiveProcess() 
	: m_proc(NULL)
	, m_busy(false)
	, m_stop(false)
	, m_canUse(false)
	{
		m_timer = new wxTimer(this);
	}
	
	/**
	 * \brief 
	 * \return 
	 */
	virtual ~InteractiveProcess(){
		delete m_timer;
	}
	
	/**
	 * write command to the process
	 */
	bool Write(const wxString &cmd);

	/**
	 * read from the process output stream
	 */
	bool ReadLine(wxString &ostr, int timeout);
	
	/**
	 * \brief read all stdout & stderr from the process
	 * \param output
	 * \return 
	 */
	bool ReadAllData(wxString &output);
};

#endif //INTERACTIVPROCESS_H
