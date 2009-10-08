#ifndef __tagscache__
#define __tagscache__

class TagCacheEntry;
#include <list>
#include "tagcacheentry.h"

class TagsCache
{
	std::list<TagCacheEntryPtr> m_cacheQueue;
	size_t                      m_maxSize;
	double                      m_hits;
	double                      m_totalRequests;
	wxString                    m_name;

protected:
	bool IsRelated( TagCacheEntryPtr entry, const std::vector<std::pair<wxString, TagEntry> > & tags );
public:
	TagsCache();
	virtual ~TagsCache();

	void             Clear                  ();
	TagCacheEntryPtr FindByQuery            (const wxString &query);
	void             AddEntry               (TagCacheEntryPtr entry);

	// For performance reasons, we use the input vector as it seen by the ctags manager
	void             DeleteEntriesByRelation(const std::vector<std::pair<wxString, TagEntry> >& tags);
	void             DeleteEntriesByRelation(const TagEntry& tag);

	// cache limit
	size_t GetMaxCacheSize() const {
		return m_maxSize;
	}

	void   SetMaxCacheSize(const size_t &size) {
		m_maxSize = size;
	}

	/**
	 * @brief return the cache hit rate
	 * @return
	 */
	int GetHitRate();

	void SetName(const wxString& name) {
		this->m_name = name;
	}
	const wxString& GetName() const {
		return m_name;
	}

	int GetItemCount() {
		return (int)m_cacheQueue.size();
	}
};
#endif // __tagscache__
