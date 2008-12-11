#include "tagcacheentry.h"

TagCacheEntry::TagCacheEntry(const wxString &query, const std::vector<TagEntryPtr> &tags)
		: m_query(query)
		, m_tags(tags)
{
	// populate the files list for this query
	for (size_t i=0; i<m_tags.size(); i++) {
		TagEntryPtr t = m_tags.at(i);
		if (m_files.Index(t->GetFile()) == wxNOT_FOUND) {

			// normalize the file name
			wxString file_name = NormalizeFileName(t->GetFile());
			// add it
			m_files.Add(file_name);
		}
	}
}

TagCacheEntry::~TagCacheEntry()
{
	m_files.Clear();
}

bool TagCacheEntry::IsFileRelated(const wxString& fileName)
{
	wxString file_name = NormalizeFileName(fileName);
	return m_files.Index(file_name) != wxNOT_FOUND;
}

wxString TagCacheEntry::NormalizeFileName(const wxString& fileName)
{
	// normalize the file name
	wxString file_name(fileName);
	file_name.MakeLower();
	file_name.Replace(wxT("\\"), wxT("/"));
	file_name.Replace(wxT("//"), wxT("/"));
	file_name.Trim().Trim(false);
	return file_name;
}

bool TagCacheEntry::IsFileStartsWith(const wxString& fileNamePrefix)
{
	wxString prefix = NormalizeFileName(fileNamePrefix);
	for(size_t i=0; i<m_files.GetCount(); i++){
		if(m_files.Item(i).StartsWith(fileNamePrefix)){
			return true;
		}
	}
	return false;
}
