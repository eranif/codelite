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
	CXTranslationUnit TU;
	time_t            lastAccessed;
};

class ClangTUCache
{
protected:
	std::map<wxString, ClangCacheEntry> m_cache;
	size_t                              m_maxItems;
	
public:
	ClangTUCache();
	virtual ~ClangTUCache();

	CXTranslationUnit GetPCH(const wxString &filename);
	void AddPCH(const wxString& filename, CXTranslationUnit tu);
	void RemoveEntry(const wxString &filename);
	void Clear();
	bool IsEmpty() const {
		return m_cache.empty();
	}
};
#endif // HAS_LIBCLANG
#endif // CLANGPCHCACHE_H
