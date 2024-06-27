//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : PHPRefactoring.h
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

#ifndef PHPREFACTORING_H
#define PHPREFACTORING_H

#include "PHP/PHPEntityBase.h"
#include "PHPSetterGetterEntry.h"

class IEditor;
class PHPRefactoring
{
    PHPRefactoring();
protected:
    bool FindByName(const PHPEntityBase::List_t& entries, const wxString &name) const;
    
public:
    virtual ~PHPRefactoring();
    static PHPRefactoring& Get();
};

#endif // PHPREFACTORING_H
