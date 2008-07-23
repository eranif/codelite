#ifndef __tagscache__
#define __tagscache__

class TagCacheEntry;
#include <map>
#include "tagcacheentry.h"

class TagsCache {
	std::map<wxString, TagCacheEntryPtr> m_cache;
	
public:
	TagsCache();
	virtual ~TagsCache();

	void Clear();
	void DeleteByFilename(const wxString &fileName);
	TagCacheEntryPtr FindByQuery(const wxString &query);
	void AddEntry(TagCacheEntryPtr entry);
	
};
#endif // __tagscache__
