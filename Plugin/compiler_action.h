#ifndef COMPILER_ACTION_H
#define COMPILER_ACTION_H

#include "worker_thread.h"
#include "wx/event.h"
#include "cl_process.h"
#include "wx/timer.h"

#ifdef WXMAKINGDLL_LE_SDK
#    define WXDLLIMPEXP_LE_SDK WXEXPORT
#elif defined(WXUSINGDLL_LE_SDK)
#    define WXDLLIMPEXP_LE_SDK WXIMPORT
#else /* not making nor using FNB as DLL */
#    define WXDLLIMPEXP_LE_SDK
#endif // WXMAKINGDLL_LE_SDK

DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_LE_SDK, wxEVT_BUILD_ADDLINE, wxID_ANY)
DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_LE_SDK, wxEVT_BUILD_STARTED, wxID_ANY)
DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_LE_SDK, wxEVT_BUILD_ENDED, wxID_ANY)

/**
 * \class CompilerAction
 * \brief 
 * \author Eran
 * \date 07/22/07
 */
class WXDLLIMPEXP_LE_SDK CompilerAction : public wxEvtHandler, public ThreadRequest {
protected:
	clProcess *m_proc;
	wxEvtHandler *m_owner;
	wxTimer *m_timer;
	bool m_busy;
	bool m_stop;
	wxArrayString m_lines;

protected:	
	virtual void OnTimer(wxTimerEvent &event);
	virtual void OnProcessEnd(wxProcessEvent& event);
	virtual void PrintOutput();
	virtual void DoPrintOutput(const wxString &out, const wxString &errs);
	void CleanUp();

public:
	bool IsBusy() const { return m_busy; }
	void SetBusy(bool busy) { m_busy = busy; }
	void Stop();
	bool GetLines(wxArrayString &lines) {lines = m_lines; return true;}

public:
	//construct a compiler action 
	// \param owner the window owner for this action
	CompilerAction(wxEvtHandler *owner);
	
	/**
	 * \brief 
	 * \return 
	 */
	virtual ~CompilerAction()
	{
		delete m_timer;
	};
	
	/**
	 * \brief 
	 * \return 
	 */
	virtual void Process() = 0;

	void AppendLine(const wxString &line);
	void SendStartMsg();
	void SendEndMsg();
};
#endif 
