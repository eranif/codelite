//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : PHPDocVisitor.h
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

#ifndef PHPDOCVISITOR_H
#define PHPDOCVISITOR_H

#include "codelite_exports.h"
#include "PHPEntityVisitor.h" // Base class: PHPEntityVisitor
#include "PhpLexerAPI.h"
#include <vector>
#include "PHPSourceFile.h"

class WXDLLIMPEXP_CL PHPDocVisitor : public PHPEntityVisitor
{
    std::map<int, phpLexerToken> m_comments;
    PHPSourceFile& m_sourceFile;
    
public:
    PHPDocVisitor(PHPSourceFile& sourceFile, const std::vector<phpLexerToken>& comments);
    virtual ~PHPDocVisitor();

public:
    virtual void OnEntity(PHPEntityBase::Ptr_t entity);
};

#endif // PHPDOCVISITOR_H
