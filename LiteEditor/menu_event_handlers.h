//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : menu_event_handlers.h
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
#ifndef MENU_EVENT_HANDLERS_H
#define MENU_EVENT_HANDLERS_H

#include <memory>
#include <wx/event.h>
#include <wx/xrc/xmlres.h>

/**
 * The interface for menu event handler classes
 */
class MenuEventHandler
{
protected:
    int m_id;

public:
    MenuEventHandler(int id)
        : m_id(id){};
    virtual ~MenuEventHandler(){};

    // handle an event
    virtual void ProcessCommandEvent(wxWindow* owner, wxCommandEvent& event) = 0;
    virtual void ProcessUpdateUIEvent(wxWindow* owner, wxUpdateUIEvent& event) = 0;

    // Setters/Getters
    const int& GetEventId() const { return m_id; }
    void SetEventId(const int& id) { m_id = id; }
};

using MenuEventHandlerPtr = std::shared_ptr<MenuEventHandler>;

//-----------------------------------------------------------------
// Define here a class per event/group
//-----------------------------------------------------------------

//------------------------------------
// Common edit tasks
//------------------------------------
class EditHandler : public MenuEventHandler
{
public:
    EditHandler(int id)
        : MenuEventHandler(id){};
    virtual ~EditHandler(){};

public:
    virtual void ProcessCommandEvent(wxWindow* owner, wxCommandEvent& event);
    virtual void ProcessUpdateUIEvent(wxWindow* owner, wxUpdateUIEvent& event);
};

//------------------------------------
// Brace matching
//------------------------------------
class BraceMatchHandler : public MenuEventHandler
{
public:
    BraceMatchHandler(int id)
        : MenuEventHandler(id){};
    virtual ~BraceMatchHandler(){};

public:
    virtual void ProcessCommandEvent(wxWindow* owner, wxCommandEvent& event);
    virtual void ProcessUpdateUIEvent(wxWindow* owner, wxUpdateUIEvent& event);
};

//------------------------------------
// Find / Repalce
//------------------------------------
class FindReplaceHandler : public MenuEventHandler
{
public:
    FindReplaceHandler(int id)
        : MenuEventHandler(id){};
    virtual ~FindReplaceHandler(){};

public:
    virtual void ProcessCommandEvent(wxWindow* owner, wxCommandEvent& event);
    virtual void ProcessUpdateUIEvent(wxWindow* owner, wxUpdateUIEvent& event);
};

//------------------------------------
// Goto
//------------------------------------
class GotoHandler : public MenuEventHandler
{
public:
    GotoHandler(int id)
        : MenuEventHandler(id){};
    virtual ~GotoHandler(){};

public:
    virtual void ProcessCommandEvent(wxWindow* owner, wxCommandEvent& event);
    virtual void ProcessUpdateUIEvent(wxWindow* owner, wxUpdateUIEvent& event);
};

//------------------------------------
// Bookmarks
//------------------------------------
class BookmarkHandler : public MenuEventHandler
{
public:
    BookmarkHandler(int id)
        : MenuEventHandler(id){};
    virtual ~BookmarkHandler(){};

public:
    virtual void ProcessCommandEvent(wxWindow* owner, wxCommandEvent& event);
    virtual void ProcessUpdateUIEvent(wxWindow* owner, wxUpdateUIEvent& event);
};

//------------------------------------
// Go to definition
//------------------------------------
class GotoDefinitionHandler : public MenuEventHandler
{
public:
    GotoDefinitionHandler(int id)
        : MenuEventHandler(id){};
    virtual ~GotoDefinitionHandler(){};

public:
    virtual void ProcessCommandEvent(wxWindow* owner, wxCommandEvent& event);
    virtual void ProcessUpdateUIEvent(wxWindow* owner, wxUpdateUIEvent& event);
};

//-------------------------------------------------
// Word wrap
//-------------------------------------------------
class WordWrapHandler : public MenuEventHandler
{
public:
    WordWrapHandler(int id)
        : MenuEventHandler(id){};
    virtual ~WordWrapHandler(){};

public:
    virtual void ProcessCommandEvent(wxWindow* owner, wxCommandEvent& event);
    virtual void ProcessUpdateUIEvent(wxWindow* owner, wxUpdateUIEvent& event);
};

//-------------------------------------------------
// Folding
//-------------------------------------------------
class FoldHandler : public MenuEventHandler
{
public:
    FoldHandler(int id)
        : MenuEventHandler(id){};
    virtual ~FoldHandler(){};

public:
    virtual void ProcessCommandEvent(wxWindow* owner, wxCommandEvent& event);
    virtual void ProcessUpdateUIEvent(wxWindow* owner, wxUpdateUIEvent& event);
};

//-------------------------------------------------
// Debugger handler
//-------------------------------------------------
class DebuggerMenuHandler : public MenuEventHandler
{
public:
    DebuggerMenuHandler(int id)
        : MenuEventHandler(id){};
    virtual ~DebuggerMenuHandler(){};

public:
    virtual void ProcessCommandEvent(wxWindow* owner, wxCommandEvent& event);
    virtual void ProcessUpdateUIEvent(wxWindow* owner, wxUpdateUIEvent& event);
};

#endif // MENU_EVENT_HANDLERS_H
