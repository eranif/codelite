#include <wx/dir.h>
#include "clangpch_cache.h"
#include <wx/stdpaths.h>
#include "file_logger.h"

ClangPCHCache::ClangPCHCache()
	: m_maxItems(100)
{
}

ClangPCHCache::~ClangPCHCache()
{
}

const ClangPCHEntry& ClangPCHCache::GetPCH(const wxString& filename)
{
	static ClangPCHEntry NullCacheEntry;
	std::map<wxString, ClangPCHEntry>::iterator iter = m_cache.find(filename);
	if(iter == m_cache.end()) {
		return NullCacheEntry;
	}
	// update this entry access time
	iter->second.SetLastAccessed(time(NULL));
	return iter->second;
}

void ClangPCHCache::AddPCH(const wxString& source, const wxString& pchname, const wxArrayString &includes, const wxArrayString &includesFullPaths)
{
	std::map<wxString, ClangPCHEntry>::iterator iter = m_cache.find(source);
	if(iter != m_cache.end()) {
		m_cache.erase(iter);
	}

	ClangPCHEntry entry(source, pchname, includes, includesFullPaths);
	m_cache.insert(std::make_pair<wxString, ClangPCHEntry>(source, entry));
	
	if(m_cache.size() > m_maxItems) {
		CL_DEBUG1(wxT("clang PCH cache size reached its maximum size, removing last accessed entry"));
		time_t oldest_timestamp = time(NULL);
		// iterate over the cache and remove the oldest item from the cache
		std::map<wxString, ClangPCHEntry>::iterator it = m_cache.begin();
		wxString key_to_remove;
		for(; it != m_cache.end(); it++) {
			if(it->second.GetLastAccessed() <= oldest_timestamp) {
				key_to_remove = it->second.GetSource();
				oldest_timestamp = it->second.GetLastAccessed();
			}
		}
		
		if(key_to_remove.IsEmpty() == false) {
			CL_DEBUG1(wxT("Removing entry for key: %s"), key_to_remove.c_str());
			RemoveEntry(key_to_remove);
		}
	}
}

void ClangPCHCache::Clear()
{
	CL_DEBUG(wxT("clang PCH cache cleared!"));
	m_cache.clear();

	wxArrayString all_files;
	wxDir::GetAllFiles(GetCacheDirectory(), &all_files);
	for(size_t i=0; i<all_files.GetCount(); i++) {
		wxRemoveFile(all_files.Item(i));
	}
}

void ClangPCHCache::RemoveEntry(const wxString& source)
{
	std::map<wxString, ClangPCHEntry>::iterator iter = m_cache.find(source);
	if(iter != m_cache.end()) {
		wxRemoveFile(iter->second.GetPch());
		m_cache.erase(iter);
	}
}

wxString ClangPCHCache::GetCacheDirectory()
{
	wxString cacheDirectory;
	cacheDirectory << wxStandardPaths::Get().GetUserDataDir() << wxFileName::GetPathSeparator() << wxT("clang_cache");
	return cacheDirectory;
}
