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
    explicit MenuEventHandler(int id)
        : m_id(id)
    {
    }
    virtual ~MenuEventHandler() = default;

    // handle an event
    virtual void ProcessCommandEvent(wxWindow* owner, wxCommandEvent& event) = 0;
    virtual void ProcessUpdateUIEvent(wxWindow* owner, wxUpdateUIEvent& event) = 0;

    // Setters/Getters
    int GetEventId() const { return m_id; }
    void SetEventId(int id) { m_id = id; }
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
    explicit EditHandler(int id)
        : MenuEventHandler(id)
    {
    }
    ~EditHandler() override = default;

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
    explicit BraceMatchHandler(int id)
        : MenuEventHandler(id)
    {
    }
    ~BraceMatchHandler() override = default;

public:
    virtual void ProcessCommandEvent(wxWindow* owner, wxCommandEvent& event);
    virtual void ProcessUpdateUIEvent(wxWindow* owner, wxUpdateUIEvent& event);
};

//------------------------------------
// Find / Replace
//------------------------------------
class FindReplaceHandler : public MenuEventHandler
{
public:
    explicit FindReplaceHandler(int id)
        : MenuEventHandler(id)
    {
    }
    ~FindReplaceHandler() override = default;

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
    explicit GotoHandler(int id)
        : MenuEventHandler(id)
    {
    }
    ~GotoHandler() override = default;

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
    explicit BookmarkHandler(int id)
        : MenuEventHandler(id)
    {
    }
    ~BookmarkHandler() override = default;

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
    explicit GotoDefinitionHandler(int id)
        : MenuEventHandler(id)
    {
    }
    ~GotoDefinitionHandler() override = default;

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
    explicit WordWrapHandler(int id)
        : MenuEventHandler(id)
    {
    }
    ~WordWrapHandler() override = default;

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
    explicit FoldHandler(int id)
        : MenuEventHandler(id)
    {
    }
    ~FoldHandler() override = default;

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
    explicit DebuggerMenuHandler(int id)
        : MenuEventHandler(id)
    {
    }
    ~DebuggerMenuHandler() override = default;

public:
    virtual void ProcessCommandEvent(wxWindow* owner, wxCommandEvent& event);
    virtual void ProcessUpdateUIEvent(wxWindow* owner, wxUpdateUIEvent& event);
};

#endif // MENU_EVENT_HANDLERS_H
