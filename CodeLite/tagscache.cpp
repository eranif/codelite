#include "tagscache.h"
#include <wx/log.h>

static void PrintMessage(const wxString &msg)
{
//	wxLogMessage(msg);
}

TagsCache::TagsCache()
		: m_maxSize       (500)
		, m_hits          (0)
		, m_totalRequests (0)
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
	if (m_cacheQueue.size() > GetMaxCacheSize()) {
		TagCacheEntryPtr deleteItem = m_cacheQueue.back();
		m_cacheQueue.pop_back();

	}
}

void TagsCache::Clear()
{
	m_hits          = 0;
	m_totalRequests = 0;
	if (m_cacheQueue.empty() == false) {
		m_cacheQueue.clear();
	}
}

TagCacheEntryPtr TagsCache::FindByQuery(const wxString& query)
{
	m_totalRequests++;
	std::list<TagCacheEntryPtr>::iterator iter = m_cacheQueue.begin();
	for (; iter != m_cacheQueue.end(); iter ++) {
		if ((*iter)->GetQueryKey() == query) {
			TagCacheEntryPtr entry = *iter;
			m_cacheQueue.erase(iter);

			// place it at the top of the queue
			m_cacheQueue.push_front(entry);
			m_hits++;
			PrintMessage(wxString::Format(wxT("[%s] %s found in cache [%d entries in cache]"), GetName().c_str(), query.c_str(), (int)m_cacheQueue.size()));
			return entry;
		}
	}
	PrintMessage(wxString::Format(wxT("[%s] %s was NOT found in cache [%d entries in cache]"), GetName().c_str(), query.c_str(), (int)m_cacheQueue.size()));
	return NULL;
}


int TagsCache::GetHitRate()
{
	if (m_totalRequests == 0) {
		return 0;
	}

	return (int)((m_hits / m_totalRequests) * 100);
}

void TagsCache::DeleteEntriesByRelation(const std::vector<std::pair<wxString, TagEntry> >& tags)
{
	int entries_deleted(0);
	std::list<TagCacheEntryPtr>::iterator iter = m_cacheQueue.begin();
	while (iter != m_cacheQueue.end()) {
		TagCacheEntryPtr entry = *iter;
		if ( IsRelated( entry, tags ) ) {
			entries_deleted++;
			iter = m_cacheQueue.erase(iter);
		} else {
			++iter;
		}
	}
	PrintMessage(wxString::Format(wxT("[%s] %d entries deleted from cache [%d entries in cache]"), GetName().c_str(), entries_deleted, (int)m_cacheQueue.size()));
}

bool TagsCache::IsRelated( TagCacheEntryPtr entry, const std::vector<std::pair<wxString, TagEntry> > & tags )
{
	for (size_t i=0; i<tags.size(); i++) {
		if ( entry->IsRelated( tags.at(i).second ) ) {
			return true;
		}
	}
	return false;
}

void TagsCache::DeleteEntriesByRelation(const TagEntry& tag)
{
	int entries_deleted(0);
	std::list<TagCacheEntryPtr>::iterator iter = m_cacheQueue.begin();
	while (iter != m_cacheQueue.end()) {
		TagCacheEntryPtr entry = *iter;
		if(entry->IsRelated(tag)){
			entries_deleted++;
			iter = m_cacheQueue.erase(iter);
		} else {
			++iter;
		}
	}
	PrintMessage(wxString::Format(wxT("[%s] %d entries deleted from cache [%d entries in cache]"), GetName().c_str(), entries_deleted, (int)m_cacheQueue.size()));

}
