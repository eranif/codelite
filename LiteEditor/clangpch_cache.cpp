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

ClangCacheEntry ClangTUCache::GetPCH(const wxString& filename)
{
	std::map<wxString, ClangCacheEntry>::iterator iter = m_cache.find(filename);
	if(iter == m_cache.end()) {
		return ClangCacheEntry();
	}
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
	m_cache.insert(std::make_pair<wxString, ClangCacheEntry>(entry.sourceFile, entry));
	
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
            CL_DEBUG(wxT("Deleting TU: %x"), (void*)it->second.TU);
			clang_disposeTranslationUnit(it->second.TU);
        }
	}
	m_cache.clear();
}

void ClangTUCache::RemoveEntry(const wxString& filename)
{
	std::map<wxString, ClangCacheEntry>::iterator iter = m_cache.find(filename);
	if(iter != m_cache.end()) {
        CL_DEBUG(wxT("clang_disposeTranslationUnit for TU: %x"), (void*)iter->second.TU);
		clang_disposeTranslationUnit(iter->second.TU);
		m_cache.erase(iter);
		
		wxLogNull nolog;
		wxRemoveFile(iter->second.fileTU);
	}
}

bool ClangTUCache::Contains(const wxString& filename) const
{
    return m_cache.find(filename) != m_cache.end();
}

wxString ClangTUCache::GetTuFileName(const wxString& sourceFile) const
{
	std::map<wxString, ClangCacheEntry>::const_iterator iter = m_cache.find(sourceFile);
	if(iter != m_cache.end())
		return iter->second.fileTU;
	return wxT("");
}

#endif // HAS_LIBCLANG


