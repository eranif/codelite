#include "job.h"

const wxEventType wxEVT_CMD_JOB_STATUS = wxNewEventType();
const wxEventType wxEVT_CMD_JOB_STATUS_VOID_PTR = wxNewEventType();

Job::Job(wxEvtHandler* parent)
		: m_parent(parent)
{
}

Job::~Job()
{
}

void Job::Post(int i, const wxString &message)
{
	if (m_parent) {
		wxCommandEvent e(wxEVT_CMD_JOB_STATUS);
		e.SetInt(i);
		e.SetString(message);
		m_parent->AddPendingEvent(e);
	}
}

void Job::Post(void* ptr)
{
	if (m_parent) {
		wxCommandEvent e(wxEVT_CMD_JOB_STATUS_VOID_PTR);
		e.SetClientData(ptr);
		m_parent->AddPendingEvent(e);
	}
}
