//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : clPersistenceManager.h
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

#ifndef CLPERSISTENCEMANAGER_H
#define CLPERSISTENCEMANAGER_H

#include "clIniFile.h"
#include "codelite_exports.h"

#include <wx/persist.h> // Base class: wxPersistenceManager

class WXDLLIMPEXP_SDK clPersistenceManager : public wxPersistenceManager
{
    clIniFile* m_iniFile;

public:
    clPersistenceManager();
    virtual ~clPersistenceManager();

    /**
     * @brief return the configuration object to use
     */
    virtual wxConfigBase* GetConfig() const;
};

#endif // CLPERSISTENCEMANAGER_H
