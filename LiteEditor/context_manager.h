//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : context_manager.h
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
#ifndef CONTEXT_MANAGER_H
#define CONTEXT_MANAGER_H

#include "singleton.h"
#include "wx/string.h"
#include "map"
#include "context_base.h"
#include "wx/window.h"

class ContextManager;

class ContextManager : public Singleton<ContextManager>
{
    friend class Singleton<ContextManager>;
    std::map<wxString, ContextBasePtr> m_contextPool;

public:
    /**
     * Return new context by name
     * \param lexerName the lexer name
     * \param parent the context parent
     * \return
     */
    ContextBasePtr NewContext(clEditor *parent, const wxString &lexerName);
    ContextBasePtr NewContextByFileName(clEditor *parent, const wxFileName &fileName);
    void Initialize();
    
private:
    ContextManager();
    virtual ~ContextManager();
};
#endif // CONTEXT_MANAGER_H
