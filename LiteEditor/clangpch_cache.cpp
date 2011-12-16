#if HAS_LIBCLANG

#include <wx/dir.h>
#include "clangpch_cache.h"
#include <wx/stdpaths.h>
#include "file_logger.h"

ClangTUCache::ClangTUCache()
	: m_maxItems(20)
{
}

ClangTUCache::~ClangTUCache()
{
}

CXTranslationUnit ClangTUCache::GetPCH(const wxString& filename)
{
	std::map<wxString, ClangCacheEntry>::iterator iter = m_cache.find(filename);
	if(iter == m_cache.end()) {
		return 0;
	}
	// Remove this entry from the cache. It is up to the caller to place it back!
	
	CXTranslationUnit TU = iter->second.TU;
	m_cache.erase(iter);
	
	return TU;
}

void ClangTUCache::AddPCH(const wxString& filename, CXTranslationUnit tu)
{
	// See if we already have a cache entry for this file name
	std::map<wxString, ClangCacheEntry>::iterator iter = m_cache.find(filename);
	if(iter != m_cache.end() && iter->second.TU == tu) {
		// the entry in the cache is the same as this one
		// Just update the access-time
		iter->second.lastAccessed = time(NULL);
		return;
	}

	ClangCacheEntry entry;
	entry.TU = tu;
	entry.lastAccessed = time(NULL);
	m_cache.insert(std::make_pair<wxString, ClangCacheEntry>(filename, entry));
	
	if(m_cache.size() > m_maxItems) {
		CL_DEBUG1(wxT("clang PCH cache size reached its maximum size, removing last accessed entry"));
		time_t oldest_timestamp = time(NULL);
		// iterate over the cache and remove the oldest item from the cache
		std::map<wxString, ClangCacheEntry>::iterator it = m_cache.begin();
		wxString key_to_remove;
		for(; it != m_cache.end(); it++) {
			if(it->second.lastAccessed <= oldest_timestamp) {
				key_to_remove    = it->first;
				oldest_timestamp = it->second.lastAccessed;
			}
		}
		
		if(key_to_remove.IsEmpty() == false) {
			CL_DEBUG1(wxT("Removing entry for key: %s"), key_to_remove.c_str());
			RemoveEntry(key_to_remove);
		}
	}
}

void ClangTUCache::Clear()
{
	CL_DEBUG(wxT("clang PCH cache cleared!"));
	std::map<wxString, ClangCacheEntry>::iterator it = m_cache.begin();
	for(; it != m_cache.end(); it++) {
		if(it->second.TU)
			clang_disposeTranslationUnit(it->second.TU);
	}
	m_cache.clear();
}

void ClangTUCache::RemoveEntry(const wxString& filename)
{
	std::map<wxString, ClangCacheEntry>::iterator iter = m_cache.find(filename);
	if(iter != m_cache.end()) {
		clang_disposeTranslationUnit(iter->second.TU);
		m_cache.erase(iter);
	}
}

#endif // HAS_LIBCLANG
