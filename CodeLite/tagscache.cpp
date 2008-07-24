#include "tagscache.h"
#include <wx/log.h>

TagsCache::TagsCache()
: m_maxSize(500)
{
	m_cacheQueue.clear();
}

TagsCache::~TagsCache()
{
}

void TagsCache::AddEntry(TagCacheEntryPtr entry)
{
	// add an entry at the bottom of the queue
	m_cacheQueue.push_front(entry);
	
	// incase we exceeded the cache size, delete the least accessed item from
	// cache - which is the first item in the map
	if(m_cacheQueue.size() > GetMaxCacheSize()){
		TagCacheEntryPtr deleteItem = m_cacheQueue.back();
		m_cacheQueue.pop_back();
		
		//wxLogMessage(wxString::Format(wxT("Deleted item from cache: %s"), deleteItem->GetQueryKey().c_str()));
	}
}

void TagsCache::Clear()
{ 
	if(m_cacheQueue.empty() == false){
		m_cacheQueue.clear();
	}
}

void TagsCache::DeleteByFilename(const wxString& fileName)
{
	std::list<TagCacheEntryPtr>::iterator iter = m_cacheQueue.begin();
	for(; iter != m_cacheQueue.end(); iter++){
		TagCacheEntryPtr t = (*iter);
		if(t->IsFileRelated(fileName)){
			iter = m_cacheQueue.erase(iter);
		}
	}
}

TagCacheEntryPtr TagsCache::FindByQuery(const wxString& query)
{
	std::list<TagCacheEntryPtr>::iterator iter = m_cacheQueue.begin();
	for(; iter != m_cacheQueue.end(); iter ++){
		if((*iter)->GetQueryKey() == query) {
			TagCacheEntryPtr entry = *iter;
			m_cacheQueue.erase(iter);
			
			// place it at the top of the queue
			m_cacheQueue.push_front(entry);
			return entry;
		}
	}
	return NULL;
}
