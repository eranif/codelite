//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : CxxUsingNamespaceCollector.h
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

#ifndef CXXUSINGNAMESPACECOLLECTOR_H
#define CXXUSINGNAMESPACECOLLECTOR_H

#include "CxxScannerBase.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_CL CxxUsingNamespaceCollector : public CxxScannerBase
{
    wxArrayString m_usingNamespaces;
    std::unordered_set<wxString>& m_visitedFiles;

private:
    void ParseUsingNamespace();

public:
    virtual void OnToken(CxxLexerToken& token);

    void SetUsingNamespaces(const wxArrayString& usingNamespaces)
    {
        this->m_usingNamespaces = usingNamespaces;
    }
    const wxArrayString& GetUsingNamespaces() const
    {
        return m_usingNamespaces;
    }
    CxxUsingNamespaceCollector(CxxPreProcessor* preProcessor, const wxFileName& filename,
                               std::unordered_set<wxString>& visitedFiles);
    virtual ~CxxUsingNamespaceCollector();
};

#endif // CXXUSINGNAMESPACECOLLECTOR_H
