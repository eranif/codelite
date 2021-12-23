//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : includepathlocator.h
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

#ifndef GCCMETADATA_H
#define GCCMETADATA_H

#include "asyncprocess.h"
#include "codelite_exports.h"

#include <wx/arrstr.h>

// Collect metadata about compiler
// - Compiler name + version
// - Target
// - Search paths
// this class supports both the GCC and CLANG compilers
class WXDLLIMPEXP_SDK GCCMetadata
{
    wxArrayString m_searchPaths;
    wxString m_target;
    wxString m_name;
    wxString m_basename;
    wxArrayString m_macros;

private:
    wxString RunCommand(const wxString& command, const wxString& working_directory = wxEmptyString,
                        clEnvList_t* env = nullptr);
    static void GetMetadataFromCache(const wxString& tool, const wxString& rootDir, bool is_cygwin, GCCMetadata* md);
    void DoLoad(const wxString& tool, const wxString& rootDir, bool is_cygwin = false);

public:
    GCCMetadata(const wxString& basename = "GCC");
    ~GCCMetadata();

    /**
     * @brief locate compiler paths by querying the compiler itself
     * If the include paths reported by the compiler starting with "/"
     * and we are on Windows, replace "/" with rootDir
     * @example
     * Locate("C:\\msys64", "C:\\msys64\\user\\bin\\g++", paths)
     *
     * @param tool the compiler to use
     * @param rootDir the compiler installation folder (the root folder). Needed for Windows, ignored elsewhere
     */
    void Load(const wxString& tool, const wxString& rootDir, bool is_cygwin = false);

    // accessors
    const wxString& GetName() const { return m_name; }
    const wxArrayString& GetSearchPaths() const { return m_searchPaths; }
    const wxString& GetTarget() const { return m_target; }
    const wxArrayString& GetMacros() const { return m_macros; }
};
#endif // GCCMETADATA_H
