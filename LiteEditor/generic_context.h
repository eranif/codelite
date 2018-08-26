//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : generic_context.h
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
#ifndef CONTEXT_GENERIC_H
#define CONTEXT_GENERIC_H

#include "context_base.h"

class clEditor;

/**
 * \ingroup LiteEditor
 * \brief the basic editor from which complicated editors derives from (e.g. ContextCpp)
 *
 * \version 1.0
 * first version
 *
 * \date 04-30-2007
 *
 * \author Eran
 *
 *
 */
class ContextGeneric : public ContextBase
{
public:
    //---------------------------------------
    // ctors-dtor
    //---------------------------------------
    ContextGeneric(clEditor* container, const wxString& name);
    ContextGeneric()
        : ContextBase(wxT("Text")){};
    ContextGeneric(const wxString& name)
        : ContextBase(name){};

    virtual ~ContextGeneric();
    virtual ContextBase* NewInstance(clEditor* container);

    //---------------------------------------
    // Operations
    //---------------------------------------
    virtual void ApplySettings();
    
    void ProcessIdleActions();
};
#endif // CONTEXT_GENERIC_H
