//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : clSTCLineKeeper.h
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

#ifndef STCLINEKEEPER_H
#define STCLINEKEEPER_H

/**
 * @class STCLineKeeper
 * a helper class that restores STC first visible line
 */
#include "codelite_exports.h"
#include "ieditor.h"

#include <wx/stc/stc.h>

// For backward compatibility we keep this class,
// but now its just a wrapper to clEditorStateLocker
class clEditorStateLocker;
class WXDLLIMPEXP_SDK clSTCLineKeeper
{
    clEditorStateLocker* m_locker;

public:
    clSTCLineKeeper(IEditor* editor);
    clSTCLineKeeper(wxStyledTextCtrl* stc);
    virtual ~clSTCLineKeeper();
};

#endif // STCLINEKEEPER_H
