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
