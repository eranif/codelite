#ifndef __tagcacheentry__
#define __tagcacheentry__

#include <vector>
#include <wx/string.h>
#include "smart_ptr.h"
#include "entry.h"
#include <set>

class TagCacheEntry
{
	wxString                 m_query;
	std::vector<TagEntryPtr> m_tags;
	std::set<wxString>       m_scopes;

public:
	TagCacheEntry(const wxString& query, const std::vector<TagEntryPtr> &tags);
	~TagCacheEntry();

	const wxString &GetQueryKey() const {
		return m_query;
	}

	const std::vector<TagEntryPtr> &GetTags() const {
		return m_tags;
	}

	/**
	 * @brief return true if this cache entry is related to the input tag
	 * the 'relation' is determined by the tag's sccope
	 * @param tag
	 * @return
	 */
	bool IsRelated       (TagEntryPtr tag);
	bool IsRelated       (const TagEntry& tag);
};

typedef SmartPtr<TagCacheEntry> TagCacheEntryPtr;

#endif // __tagcacheentry__
