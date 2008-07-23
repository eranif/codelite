#include "tagcacheentry.h"

TagCacheEntry::TagCacheEntry(const wxString &query, const std::vector<TagEntryPtr> &tags)
		: m_query(query)
		, m_tags(tags)
{
	// populate the files list for this query
	for(size_t i=0; i<m_tags.size(); i++){
		TagEntryPtr t = m_tags.at(i);
		if(m_files.Index(t->GetFile()) == wxNOT_FOUND){
			// add it
			m_files.Add(t->GetFile());
		}
	}
}

TagCacheEntry::~TagCacheEntry()
{
}

bool TagCacheEntry::IsFileRelated(const wxString& fileName) const
{
	return m_files.Index(fileName) != wxNOT_FOUND;
}
