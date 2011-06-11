#include "clangpch_cache.h"
#include "file_logger.h"

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

void ClangPCHCache::AddPCH(const wxString& source, const wxString& pchname, const wxArrayString &includes, const wxArrayString &includesFullPaths)
{
	std::map<wxString, ClangPCHEntry>::iterator iter = m_cache.find(source);
	if(iter != m_cache.end()) {
		m_cache.erase(iter);
	}
	
	ClangPCHEntry entry(source, pchname, includes, includesFullPaths);
	m_cache.insert(std::make_pair<wxString, ClangPCHEntry>(source, entry));
}

void ClangPCHCache::Clear()
{
	CL_DEBUG(wxT("clang PCH cache cleared!"));
	m_cache.clear();
}

void ClangPCHCache::RemoveEntry(const wxString& source)
{
	std::map<wxString, ClangPCHEntry>::iterator iter = m_cache.find(source);
	if(iter != m_cache.end()) {
		m_cache.erase(iter);
	}
}
