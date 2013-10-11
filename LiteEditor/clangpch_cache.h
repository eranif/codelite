//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : clangpch_cache.h
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

#ifndef CLANGPCHCACHE_H
#define CLANGPCHCACHE_H

#if HAS_LIBCLANG

#include <wx/string.h>
#include <map>
#include <set>
#include "globals.h"
#include <clang-c/Index.h>

struct ClangCacheEntry
{
public:
	CXTranslationUnit TU;
	time_t            lastAccessed;
	wxString          fileTU;
	wxString          sourceFile;
	time_t            lastReparse;
	
public:
	
	ClangCacheEntry() : TU(NULL), lastAccessed(0), lastReparse(0) {}
	ClangCacheEntry(const ClangCacheEntry &rhs) {
		*this = rhs;
	}
	
	void operator=(const ClangCacheEntry &rhs) {
		this->TU           = rhs.TU;
		this->lastAccessed = rhs.lastAccessed;
		this->fileTU       = rhs.fileTU;
		this->sourceFile   = rhs.sourceFile;
		this->lastReparse  = rhs.lastReparse;
	}
	
	bool IsOk() const {
		return TU != NULL;
	}
};

class ClangTUCache
{
protected:
	std::map<wxString, ClangCacheEntry> m_cache;
	size_t                              m_maxItems;
	
public:
	ClangTUCache();
	virtual ~ClangTUCache();

	ClangCacheEntry GetPCH(const wxString &filename);
	void AddPCH(ClangCacheEntry entry);
	void RemoveEntry(const wxString &filename);
	void Clear();
    bool Contains(const wxString &filename) const;
	wxString GetTuFileName(const wxString &sourceFile) const;
	bool IsEmpty() const {
		return m_cache.empty();
	}
	
	static void DeleteDirectoryContent(const wxString &directory);
};
#endif // HAS_LIBCLANG
#endif // CLANGPCHCACHE_H
