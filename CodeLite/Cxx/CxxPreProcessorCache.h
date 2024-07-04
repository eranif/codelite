//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : CxxPreProcessorCache.h
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

#ifndef CXXPREPROCESSORCACHE_H
#define CXXPREPROCESSORCACHE_H

#include "codelite_exports.h"
#include <wx/arrstr.h>
#include <map>

class WXDLLIMPEXP_CL CxxPreProcessorCache
{
    struct CacheEntry {
        wxString filename;
        wxString project;
        wxString config;
        wxString preamble;
        wxArrayString definitions;
        time_t lastUpdated;
        CacheEntry()
            : lastUpdated(0)
        {
        }
    };

    typedef std::map<wxString, CacheEntry> Map_t;
    CxxPreProcessorCache::Map_t m_impl;

public:
    CxxPreProcessorCache();
    virtual ~CxxPreProcessorCache();

    /**
     * @brief return the preamble for a give file
     * A Preamble of a file is a list of all include statements
     * used in the file
     */
    wxString GetPreamble(const wxString& filename) const;

    /**
     * @brief clear the cache content
     */
    void Clear();

    /**
     * @brief locate Pre Processors for a given file in a specific configuraion and project
     */
    bool Find(const wxString& filename, const wxString& project, const wxString& config, wxArrayString& definitions);

    /**
     * @brief insert item to the cache
     */
    bool
    Insert(const wxString& filename, const wxString& project, const wxString& config, const wxArrayString& definitions);
};

#endif // CXXPREPROCESSORCACHE_H
