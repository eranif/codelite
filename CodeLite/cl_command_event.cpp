//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : cl_command_event.cpp
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

#include "cl_command_event.h"

clCommandEvent::clCommandEvent(wxEventType commandType, int winid)
    : wxCommandEvent(commandType, winid)
{
}

wxEvent* clCommandEvent::Clone() const
{
    clCommandEvent* new_event = new clCommandEvent(*this);
    return new_event;
}

void clCommandEvent::SetClientObject(wxClientData* clientObject) { m_ptr = clientObject; }

wxClientData* clCommandEvent::GetClientObject() const { return m_ptr.get(); }

// ---------------------------------
// Code Completion event
// ---------------------------------
wxEvent* clCodeCompletionEvent::Clone() const
{
    clCodeCompletionEvent* new_event = new clCodeCompletionEvent(*this);
    return new_event;
}

clCodeCompletionEvent::clCodeCompletionEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}

// ------------------------------------------------
// clColourEvent
// ------------------------------------------------

clColourEvent::clColourEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}

// ------------------------------------------------
// clBuildEvent
// ------------------------------------------------

clBuildEvent::clBuildEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}

// ------------------------------------------------------------------
// clDebugEvent
// ------------------------------------------------------------------
clDebugEvent::clDebugEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}

//-------------------------------------------------------------------------
// clNewProjectEvent
//-------------------------------------------------------------------------

clNewProjectEvent::clNewProjectEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}

// --------------------------------------------------------------
// Compiler event
// --------------------------------------------------------------

clCompilerEvent::clCompilerEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}

// --------------------------------------------------------------
// Source formatting event
// --------------------------------------------------------------

clSourceFormatEvent::clSourceFormatEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}

//-------------------------------------------------------------------------
// clContextMenuEvent
//-------------------------------------------------------------------------

clContextMenuEvent::clContextMenuEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}

//-------------------------------------------------------------------------
// clExecuteEvent
//-------------------------------------------------------------------------

clExecuteEvent::clExecuteEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}

//------------------------------------------------------------------------
// clProjectSettingsEvent
//------------------------------------------------------------------------

clProjectSettingsEvent::clProjectSettingsEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}

//------------------------------------------------------------------------
// clFindEvent
//------------------------------------------------------------------------

clFindEvent::clFindEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}

//------------------------------------------------------------------------
// clFindInFilesEvent
//------------------------------------------------------------------------

clFindInFilesEvent::clFindInFilesEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}

//------------------------------------------------------------------------
// clParseEvent
//------------------------------------------------------------------------

clParseEvent::clParseEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}

//-------------------------------------------------------------------
// clProcessEvent
//-------------------------------------------------------------------

clProcessEvent::clProcessEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}

// --------------------------------------------------------------
// Compiler event
// --------------------------------------------------------------

clEditorConfigEvent::clEditorConfigEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}

// --------------------------------------------------------------
// Goto Event
// --------------------------------------------------------------
clGotoEvent::clGotoEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}

// --------------------------------------------------------------
// Source control event
// --------------------------------------------------------------
clSourceControlEvent::clSourceControlEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}

wxEvent* clSourceControlEvent::Clone() const { return new clSourceControlEvent(*this); }

// --------------------------------------------------------------
// Recent workspace event
// --------------------------------------------------------------
clEditorEvent::clEditorEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}

//// --------------------------------------------------------------
// Recent workspace event
// --------------------------------------------------------------
clRecentWorkspaceEvent::clRecentWorkspaceEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}

wxEvent* clRecentWorkspaceEvent::Clone() const { return new clRecentWorkspaceEvent(*this); }

///----------------------------------------------------------------------------------
/// clLanguageServerEvent
///----------------------------------------------------------------------------------

clLanguageServerEvent::clLanguageServerEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}

wxEvent* clLanguageServerEvent::Clone() const { return new clLanguageServerEvent(*this); }
