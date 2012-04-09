#ifndef EVENTNOTIFIER_H
#define EVENTNOTIFIER_H

#include <wx/event.h>
#include <wx/frame.h>
#include "../CodeLite/codelite_exports.h"

class WXDLLIMPEXP_SDK EventNotifier : public wxEvtHandler
{
	bool _eventsDiabled;

private:
	EventNotifier();
	virtual ~EventNotifier();

public:
	static EventNotifier* Get();
	static void Release();

	void DisableEvents(bool b) {
		_eventsDiabled = b;
	}

	bool IsEventsDiabled() const {
		return _eventsDiabled;
	}
	
	wxFrame* TopFrame();
	bool SendCommandEvent(int eventId, void *clientData);
	void PostCommandEvent(int eventId, void *clientData);
	bool SendCommandEvent(int eventId, void *clientData, const wxString &s);
};

#endif // EVENTNOTIFIER_H
