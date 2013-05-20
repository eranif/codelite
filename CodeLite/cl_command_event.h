#ifndef CLCOMMANDEVENT_H
#define CLCOMMANDEVENT_H

#include <wx/event.h>
#include <wx/sharedptr.h>
#include "codelite_exports.h"

/// a wxCommandEvent that takes ownership of the clientData
class WXDLLIMPEXP_CL clCommandEvent : public wxCommandEvent
{
    wxSharedPtr<wxClientData> m_ptr;

public:
    clCommandEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    clCommandEvent(const clCommandEvent& event);
    virtual ~clCommandEvent();

    // Hides wxCommandEvent::Set{Get}ClientObject
    void SetClientObject(wxClientData* clientObject) ;

    wxClientData *GetClientObject() const ;

    virtual wxEvent *Clone() const;
};

typedef void (wxEvtHandler::*clCommandEventFunction)(clCommandEvent&);
#define clCommandEventHandler(func) \
    wxEVENT_HANDLER_CAST(clCommandEventFunction, func)

#endif // CLCOMMANDEVENT_H
