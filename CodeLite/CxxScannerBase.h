//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : CxxScannerBase.h
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

#ifndef CXXSCANNERBASE_H
#define CXXSCANNERBASE_H

#include <wx/filename.h>
#include "CxxLexerAPI.h"

class CxxPreProcessor;
class WXDLLIMPEXP_CL CxxScannerBase
{
protected:
    Scanner_t m_scanner;
    wxFileName m_filename;
    size_t m_options;
    CxxPreProcessor* m_preProcessor;

public:
    CxxScannerBase(CxxPreProcessor* preProcessor, const wxFileName& filename);
    virtual ~CxxScannerBase();
    
    /**
     * @brief main loop
     */
    void Parse();
    
    /**
     * @brief
     * @param token
     */
    virtual void OnToken(CxxLexerToken& token) { wxUnusedVar(token); }
};

#endif // CXXSCANNERBASE_H
