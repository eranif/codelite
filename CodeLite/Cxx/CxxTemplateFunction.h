//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : CxxTemplateFunction.h
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

#ifndef CXXTEMPLATEFUNCTION_H
#define CXXTEMPLATEFUNCTION_H

#include "codelite_exports.h"
#include "database/entry.h"
#include "CxxLexerAPI.h"

class WXDLLIMPEXP_CL CxxTemplateFunction
{
    Scanner_t m_scanner;
    Scanner_t m_sigScanner;
    wxArrayString m_list;

public:
    CxxTemplateFunction(TagEntryPtr tag);
    virtual ~CxxTemplateFunction();

public:
    /**
     * @brief parse the template definition list
     */
    virtual void ParseDefinitionList();
    const wxArrayString& GetList() const { return m_list; }
    
    /**
     * @brief can the template arguments be deduced from the function 
     * signature?
     */
    bool CanTemplateArgsDeduced();
};

#endif // CXXTEMPLATEFUNCTION_H
