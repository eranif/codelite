//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : event_notifier.h
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

#ifndef EVENTNOTIFIER_H
#define EVENTNOTIFIER_H

#include <wx/event.h>
#if wxUSE_GUI
#include <wx/frame.h>
#endif
#include "codelite_exports.h"

class WXDLLIMPEXP_CL EventNotifier : public wxEvtHandler
{
    bool _eventsDiabled;

private:
    EventNotifier();
    virtual ~EventNotifier();

public:
    static EventNotifier* Get();
    static void Release();
    
    virtual void AddPendingEvent(const wxEvent& event);
    virtual bool ProcessEvent(wxEvent &event);
    
    void DisableEvents(bool b) {
        _eventsDiabled = b;
    }

    bool IsEventsDiabled() const {
        return _eventsDiabled;
    }
#if wxUSE_GUI
    wxFrame* TopFrame();
#endif
    bool SendCommandEvent(int eventId, void *clientData);
    void PostCommandEvent(int eventId, void *clientData);
    bool SendCommandEvent(int eventId, void *clientData, const wxString &s);
    
    // --------------------------------------------------------
    // About the following functions:
    // the below functions are meant to provide an easy 
    // way to fire codelite's plugins events without the need
    // to create the event on the stack.
    // Post* are meant as Asnyc event ("AddPendingEvent")
    // While Notify* are synchronous event ("ProcessEvent")
    //---------------------------------------------------------
    
    /**
     * @brief post a wxEVT_FILE_SAVED event
     */
    void PostFileSavedEvent( const wxString &filename );
    
    /**
     * @brief post a wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED_NOPROMPT or wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED
     * @param prompt 
     */
    void PostReloadExternallyModifiedEvent( bool prompt = true );
    
    /**
     * @brief post a wxEVT_PROJ_FILE_REMOVED event
     */
    void PostFileRemovedEvent( const wxArrayString &files );
    
    /**
     * @brief send a wxEVT_WORKSPACE_RELOAD_STARTED event (sync event)
     */
    void NotifyWorkspaceReloadStartEvet(const wxString &workspaceFile);
    
    /**
     * @brief send a wxEVT_WORKSPACE_RELOAD_ENDED event (sync event)
     */
    void NotifyWorkspaceReloadEndEvent(const wxString &workspaceFile);
};

#endif // EVENTNOTIFIER_H
