//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : compiler_action.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
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
DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_LE_SDK, wxEVT_BUILD_STARTED_NOCLEAN, wxID_ANY)

// ====================================================================
// Build information
// ====================================================================
class BuildInfo
{
	wxString m_project;
	wxString m_configuration;
	bool m_projectOnly;
	int m_kind;
	bool m_cleanLog;

public:
	enum {
		Build,
		Clean
	};

public:
	BuildInfo(const wxString &project, const wxString &configuration, bool projectOnly, int kind)
			: m_project(project)
			, m_configuration(configuration)
			, m_projectOnly(projectOnly)
			, m_kind(kind)
			, m_cleanLog(true) {
	}

	~BuildInfo() {};

	//----------------------------------------
	// Setters/Getters
	//----------------------------------------

	void SetConfiguration(const wxString& configuration) {
		this->m_configuration = configuration;
	}
	void SetProject(const wxString& project) {
		this->m_project = project;
	}

	const wxString& GetConfiguration() const {
		return m_configuration;
	}
	const wxString& GetProject() const {
		return m_project;
	}

	void SetProjectOnly(const bool& projectOnly) {
		this->m_projectOnly = projectOnly;
	}

	const bool& GetProjectOnly() const {
		return m_projectOnly;
	}

	void SetKind(const int& kind) {
		this->m_kind = kind;
	}
	const int& GetKind() const {
		return m_kind;
	}

	void SetCleanLog(const bool& cleanLog) {
		this->m_cleanLog = cleanLog;
	}
	
	const bool& GetCleanLog() const {
		return m_cleanLog;
	}

};

/**
 * \class CompilerAction
 * \brief
 * \author Eran
 * \date 07/22/07
 */
class WXDLLIMPEXP_LE_SDK CompilerAction : public wxEvtHandler, public ThreadRequest
{
protected:
	clProcess *m_proc;
	wxEvtHandler *m_owner;
	wxTimer *m_timer;
	bool m_busy;
	bool m_stop;
	wxArrayString m_lines;
	BuildInfo m_info;
	
protected:
	virtual void OnTimer(wxTimerEvent &event);
	virtual void OnProcessEnd(wxProcessEvent& event);
	virtual void PrintOutput();
	virtual void DoPrintOutput(const wxString &out, const wxString &errs);
	void CleanUp();

public:
	bool IsBusy() const {
		return m_busy;
	}
	void SetBusy(bool busy) {
		m_busy = busy;
	}
	void Stop();
	bool GetLines(wxArrayString &lines) {
		lines = m_lines;
		return true;
	}

public:
	//construct a compiler action
	// \param owner the window owner for this action
	CompilerAction(wxEvtHandler *owner, const BuildInfo &buildInfo);

	/**
	 * \brief
	 * \return
	 */
	virtual ~CompilerAction() {
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
