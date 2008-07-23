#ifndef __tagscache__
#define __tagscache__

class TagCacheEntry;
#include <list>
#include "tagcacheentry.h"

class TagsCache {
	std::list<TagCacheEntryPtr> m_cacheQueue;
	size_t m_maxSize;
	
public:
	TagsCache();
	virtual ~TagsCache();

	void Clear();
	void DeleteByFilename(const wxString &fileName);
	TagCacheEntryPtr FindByQuery(const wxString &query);
	void AddEntry(TagCacheEntryPtr entry);
	
	// cache limit
	size_t GetMaxCacheSize() const {return m_maxSize;}
	void SetMAxCacheSize(const size_t &size) {m_maxSize = size;}
};
#endif // __tagscache__

