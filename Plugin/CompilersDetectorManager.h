//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : CompilersDetectorManager.h
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

#ifndef COMPILERSDETECTORMANAGER_H
#define COMPILERSDETECTORMANAGER_H

#include "codelite_exports.h"
#include "compiler.h"

#include <ICompilerLocator.h>

class WXDLLIMPEXP_SDK CompilersDetectorManager
{
    ICompilerLocator::Vect_t m_detectors;
    ICompilerLocator::CompilerVec_t m_compilersFound;

public:
    CompilersDetectorManager();
    virtual ~CompilersDetectorManager();

    /**
     * @brief return true if the locator found at least one instance
     * on MinGW installed on this machine
     */
    bool FoundMinGWCompiler() const;

    void SetCompilersFound(const ICompilerLocator::CompilerVec_t& compilersFound)
    {
        this->m_compilersFound = compilersFound;
    }
    void SetDetectors(const ICompilerLocator::Vect_t& detectors) { this->m_detectors = detectors; }
    const ICompilerLocator::CompilerVec_t& GetCompilersFound() const { return m_compilersFound; }
    const ICompilerLocator::Vect_t& GetDetectors() const { return m_detectors; }
    bool Locate();
    CompilerPtr Locate(const wxString& folder);

    /**
     * @brief prompt the user to download MinGW installer
     */
    static void MSWSuggestToDownloadMinGW(bool prompt);

    void MSWFixClangToolChain(CompilerPtr compiler,
                              const ICompilerLocator::CompilerVec_t& allCompilers = ICompilerLocator::CompilerVec_t());

    wxString GetRealCXXPath(const CompilerPtr compiler) const;
};

#endif // COMPILERSDETECTORMANAGER_H
