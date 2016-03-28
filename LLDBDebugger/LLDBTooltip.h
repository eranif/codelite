//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : LLDBTooltip.h
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

#ifndef LLDBTOOLTIP_H
#define LLDBTOOLTIP_H

#include "UI.h"
#include "LLDBProtocol/LLDBVariable.h"
#include "LLDBProtocol/LLDBEvent.h"
#include "clResizableTooltip.h"

class LLDBPlugin;

// ----------------------------------------------------------
// ----------------------------------------------------------

class LLDBTooltip : public clResizableTooltip
{
    LLDBPlugin *m_plugin;
    std::map<int, wxTreeItemId> m_itemsPendingExpansion;
    
private:
    void DoCleanup();
    LLDBVariableClientData* ItemData( const wxTreeItemId & item ) const;
    void DoAddVariable(const wxTreeItemId& parent, LLDBVariable::Ptr_t variable);
    
public:
    LLDBTooltip(LLDBPlugin* plugin);
    virtual ~LLDBTooltip();

    void Show(const wxString &displayName, LLDBVariable::Ptr_t variable);

protected:
    virtual void OnItemExpanding(wxTreeEvent& event);

    // LLDB events
    void OnLLDBVariableExpanded(LLDBEvent &event);
};
#endif // LLDBTOOLTIP_H
