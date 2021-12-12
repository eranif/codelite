//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : variable.h
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

#ifndef VARIABLE_H
#define VARIABLE_H

#include "codelite_exports.h"
#include "list"
#include "string"
#include <stdio.h>

class WXDLLIMPEXP_CL Variable
{
public:
    std::string m_name;
    bool m_isTemplate;
    std::string m_templateDecl;
    bool m_isPtr;
    std::string m_type;      // as in 'int a;' -> type=int
    std::string m_typeScope; // as in 'std::string a;' -> typeScope = std, type=string
    std::string m_pattern;
    std::string m_completeType;
    std::string m_starAmp;
    int m_lineno;
    bool m_isConst;
    std::string m_rightSideConst;
    std::string m_defaultValue; // used mainly for function arguments with default values foo(int = 0);
    std::string m_arrayBrackets;
    bool m_isEllipsis;
    bool m_isBasicType;
    bool m_isVolatile;
    bool m_isAuto;
    bool m_enumInTypeDecl; // e.g. enum MyENum e

public:
    Variable();
    virtual ~Variable();

    // copy ctor
    Variable(const Variable& src);

    // operator =
    Variable& operator=(const Variable& src);

    // clear the class content
    void Reset();

    // print the variable to stdout
    void Print();
};

typedef std::list<Variable> VariableList;
#endif // VARIABLE_H
