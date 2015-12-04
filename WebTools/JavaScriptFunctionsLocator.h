//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : JavaScriptFunctionsLocator.h
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

#ifndef JAVASCRIPTFUNCTIONSLOCATOR_H
#define JAVASCRIPTFUNCTIONSLOCATOR_H

#include <wx/string.h>
#include "macros.h"
#include "JSLexerAPI.h"

class JavaScriptFunctionsLocator
{
    enum eState { kNormal, kScopeOperator };
    wxString m_lastIdentifier;
    wxStringSet_t m_functions;
    wxStringSet_t m_properties;
    wxStringSet_t m_keywords;
    eState m_state;
    JSScanner_t m_scanner;

protected:
    void OnToken(JSLexerToken& token);

public:
    JavaScriptFunctionsLocator(const wxFileName& filename, const wxString& content = "");
    virtual ~JavaScriptFunctionsLocator();

    wxString GetFunctionsString() const;
    wxString GetPropertiesString() const;

    void Parse();
};

#endif // JAVASCRIPTFUNCTIONSLOCATOR_H
