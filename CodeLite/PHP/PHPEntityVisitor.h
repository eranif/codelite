//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : PHPEntityVisitor.h
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

#ifndef PHPENTITYVISITOR_H
#define PHPENTITYVISITOR_H

#include "codelite_exports.h"
#include "PHPEntityBase.h"

/**
 * @class PHPEntityVisitor
 * Accept as input a PHPEntitBase and visit all of its children
 */
class WXDLLIMPEXP_CL PHPEntityVisitor
{
public:
    PHPEntityVisitor();
    virtual ~PHPEntityVisitor();
    
    void Visit(PHPEntityBase::Ptr_t parent);
    
    /**
     * @brief called whenver an entity is visited
     * @param entity
     */
    virtual void OnEntity(PHPEntityBase::Ptr_t entity) = 0;
};

#endif // PHPENTITYVISITOR_H
