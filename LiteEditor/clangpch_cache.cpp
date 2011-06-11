#include "clangpch_cache.h"

ClangPCHCache::ClangPCHCache()
{
}

ClangPCHCache::~ClangPCHCache()
{
}

const ClangPCHEntry& ClangPCHCache::GetPCH(const wxString& filename) const
{
	static ClangPCHEntry NullCacheEntry;
	std::map<wxString, ClangPCHEntry>::const_iterator iter = m_cache.find(filename);
	if(iter == m_cache.end()) {
		return NullCacheEntry;
	}
	return iter->second;
}

void ClangPCHCache::AddPCH(const wxString& source, const wxString& pchname)
{
	std::map<wxString, ClangPCHEntry>::iterator iter = m_cache.find(source);
	if(iter != m_cache.end()) {
		m_cache.erase(iter);
	}
	
	ClangPCHEntry entry(source, pchname);
	m_cache.insert(std::make_pair<wxString, ClangPCHEntry>(source, entry));
}

void ClangPCHCache::Clear()
{
	m_cache.clear();
}

void ClangPCHCache::RemoveEntry(const wxString& source)
{
	std::map<wxString, ClangPCHEntry>::iterator iter = m_cache.find(source);
	if(iter != m_cache.end()) {
		m_cache.erase(iter);
	}
}
