//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : LLDBRemoteConnectReturnObject.h
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

#ifndef LLDBREMOTECONNECTRETURNOBJECT_H
#define LLDBREMOTECONNECTRETURNOBJECT_H

#include <wx/string.h>

class LLDBConnectReturnObject
{
    bool     m_pivotNeeded;
    wxString m_remoteHostName;

public:
    LLDBConnectReturnObject();
    ~LLDBConnectReturnObject();
    
    void Clear();
    
    void SetPivotNeeded(bool pivotNeeded) {
        this->m_pivotNeeded = pivotNeeded;
    }
    bool IsPivotNeeded() const {
        return m_pivotNeeded;
    }
    void SetRemoteHostName(const wxString& remoteHostName) {
        this->m_remoteHostName = remoteHostName;
    }
    const wxString& GetRemoteHostName() const {
        return m_remoteHostName;
    }
};

#endif // LLDBREMOTECONNECTRETURNOBJECT_H
