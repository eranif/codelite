//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : clangpch_cache.cpp
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

#if HAS_LIBCLANG

#include "clangpch_cache.h"
#include "file_logger.h"
#include "fileutils.h"
#include <wx/dir.h>
#include <wx/log.h>
#include <wx/stdpaths.h>

ClangTUCache::ClangTUCache()
    : m_maxItems(10)
{
}

ClangTUCache::~ClangTUCache() {}

ClangCacheEntry ClangTUCache::GetPCH(const wxString& filename)
{
    std::map<wxString, ClangCacheEntry>::iterator iter = m_cache.find(filename);
    if(iter == m_cache.end()) { return ClangCacheEntry(); }
    // Remove this entry from the cache. It is up to the caller to place it back!
    ClangCacheEntry entry = iter->second;
    m_cache.erase(iter);
    return entry;
}

void ClangTUCache::AddPCH(ClangCacheEntry entry)
{
    // See if we already have a cache entry for this file name
    std::map<wxString, ClangCacheEntry>::iterator iter = m_cache.find(entry.sourceFile);
    if(iter != m_cache.end() && iter->second.TU == entry.TU) {
        // the entry in the cache is the same as this one
        // Just update the access-time
        iter->second.lastAccessed = time(NULL);
        return;
    }

    entry.lastAccessed = time(NULL);
    m_cache.insert(std::make_pair(entry.sourceFile, entry));

    if(m_cache.size() > m_maxItems) {
        CL_DEBUG1(wxT("clang PCH cache size reached its maximum size, removing last accessed entry"));
        time_t oldest_timestamp = time(NULL);
        // iterate over the cache and remove the oldest item from the cache
        std::map<wxString, ClangCacheEntry>::iterator it = m_cache.begin();
        wxString key_to_remove;
        for(; it != m_cache.end(); it++) {
            if(it->second.lastAccessed <= oldest_timestamp) {
                key_to_remove = it->first;
                oldest_timestamp = it->second.lastAccessed;
            }
        }

        if(key_to_remove.IsEmpty() == false) {
            CL_DEBUG(wxT("Removing entry for key: %s"), key_to_remove.c_str());
            RemoveEntry(key_to_remove);
        }
    }
}

void ClangTUCache::Clear()
{
    CL_DEBUG(wxT("clang PCH cache cleared!"));
    std::map<wxString, ClangCacheEntry>::iterator it = m_cache.begin();
    for(; it != m_cache.end(); it++) {
        if(it->second.TU) {
            CL_DEBUG(wxT("Deleting TU: %p"), (void*)it->second.TU);
            clang_disposeTranslationUnit(it->second.TU);
        }
    }
    m_cache.clear();

    // Clear the TU from the file system
    // if(WorkspaceST::Get()->IsOpen()) {
    //    wxString clangFileSystemCacheDir;
    //    clangFileSystemCacheDir << WorkspaceST::Get()->GetWorkspaceFileName().GetPath()
    //                            << wxFileName::GetPathSeparator()
    //                            << wxT(".clang");
    //    DeleteDirectoryContent(clangFileSystemCacheDir);
    //}
}

void ClangTUCache::RemoveEntry(const wxString& filename)
{
    std::map<wxString, ClangCacheEntry>::iterator iter = m_cache.find(filename);
    if(iter != m_cache.end()) {
        CL_DEBUG(wxT("clang_disposeTranslationUnit for TU: %p"), (void*)iter->second.TU);
        clang_disposeTranslationUnit(iter->second.TU);
        {
            wxLogNull nolog;
            clRemoveFile(iter->second.fileTU);
        }
        // it is now safe to erase the iter
        m_cache.erase(iter);
    }
}

bool ClangTUCache::Contains(const wxString& filename) const { return m_cache.find(filename) != m_cache.end(); }

wxString ClangTUCache::GetTuFileName(const wxString& sourceFile) const
{
    std::map<wxString, ClangCacheEntry>::const_iterator iter = m_cache.find(sourceFile);
    if(iter != m_cache.end()) return iter->second.fileTU;
    return wxT("");
}

void ClangTUCache::DeleteDirectoryContent(const wxString& directory)
{
    wxArrayString files;
    wxDir::GetAllFiles(directory, &files, wxT("*.TU"));
    wxLogNull nolog;

    CL_DEBUG(wxT("Clearing clang TU cache from %s"), directory.c_str());
    CL_DEBUG(wxT("Total of %d files"), (int)files.GetCount());

    for(size_t i = 0; i < files.GetCount(); i++) {
        clRemoveFile(files.Item(i));
        CL_DEBUG(wxT("Deleting .. %s"), files.Item(i).c_str());
    }
}

#endif // HAS_LIBCLANG
