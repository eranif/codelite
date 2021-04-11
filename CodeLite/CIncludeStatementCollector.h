//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : CIncludeStatementCollector.h
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

#ifndef CXXPREPROCESSORSCANNERBASE_H
#define CXXPREPROCESSORSCANNERBASE_H

#include "CxxLexerAPI.h"
#include "CxxScannerBase.h"
#include "codelite_exports.h"
#include <unordered_set>
#include <wx/filename.h>

class WXDLLIMPEXP_CL CxxPreProcessor;
class WXDLLIMPEXP_CL CIncludeStatementCollector : public CxxScannerBase
{
    std::unordered_set<wxString>& m_visitedFiles;

public:
    CIncludeStatementCollector(CxxPreProcessor* pp, const wxFileName& filename,
                               std::unordered_set<wxString>& visitedFiles);
    virtual ~CIncludeStatementCollector();
    void OnToken(CxxLexerToken& token) override;
};

#endif // CXXPREPROCESSORSCANNERBASE_H
