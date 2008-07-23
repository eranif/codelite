#ifndef __tagcacheentry__
#define __tagcacheentry__

#include <vector>
#include <wx/string.h>
#include "smart_ptr.h"
#include "entry.h"

class TagCacheEntry
{
	wxString m_query;
	std::vector<TagEntryPtr> m_tags;
	wxArrayString m_files;
	
public:
	TagCacheEntry(const wxString& query, const std::vector<TagEntryPtr> &tags);
	~TagCacheEntry();

	const wxString &GetQueryKey() const {
		return m_query;
	}
	
	const std::vector<TagEntryPtr> &GetTags() const {
		return m_tags;
	}

	const wxArrayString& GetFiles() const {
		return m_files;
	}
	
	bool IsFileRelated(const wxString &fileName) const;
};

typedef SmartPtr<TagCacheEntry> TagCacheEntryPtr;

#endif // __tagcacheentry__
