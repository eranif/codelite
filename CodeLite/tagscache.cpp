#include "tagscache.h"

TagsCache::TagsCache()
{
}

TagsCache::~TagsCache()
{
}

void TagsCache::AddEntry(TagCacheEntryPtr entry)
{
}

void TagsCache::Clear()
{
}

void TagsCache::DeleteByFilename(const wxString& fileName)
{
	std::map<wxString, TagCacheEntryPtr>::iterator iter = m_cache.begin();
	for(; iter != m_cache.end(); iter++){
		TagCacheEntryPtr t = iter->second;
		if(t->IsFileRelated(fileName)){
			m_cache.erase(iter);
			iter = m_cache.begin();
		}
	}
}

TagCacheEntryPtr TagsCache::FindByQuery(const wxString& query)
{
	std::map<wxString, TagCacheEntryPtr>::iterator iter = m_cache.find(query);
	if (iter != m_cache.end()) {
		return iter->second;
	}
	return NULL;
}

