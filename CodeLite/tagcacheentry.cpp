#include "tagcacheentry.h"

static wxString GetScope(const TagEntry& tag)
{
	wxString affectedScope;
	if ( tag.IsContainer() ) {
		affectedScope << tag.GetScope() << wxT("::") << tag.GetName();
	} else {
		affectedScope << tag.GetScope();
	}
	return affectedScope;
}

TagCacheEntry::TagCacheEntry(const wxString &query, const std::vector<TagEntryPtr> &tags)
		: m_query(query)
		, m_tags(tags)
{
	// Keep a set of the relevant scopes
	for (size_t i=0; i<m_tags.size(); i++) {
		m_scopes.insert(GetScope((*m_tags.at(i))));
	}
}

TagCacheEntry::~TagCacheEntry()
{
	m_scopes.clear();
}

bool TagCacheEntry::IsRelated(TagEntryPtr tag)
{
	wxString scope = GetScope(*tag);
	return m_scopes.find(scope) != m_scopes.end();
}

bool TagCacheEntry::IsRelated(const TagEntry& tag)
{
	wxString scope = GetScope(tag);
	return m_scopes.find(scope) != m_scopes.end();
}
