#ifndef CLANGPCHCACHE_H
#define CLANGPCHCACHE_H

#include <wx/string.h>
#include <map>
#include <set>
#include "globals.h"

class ClangPCHEntry
{
protected:
	wxString                   m_source;
	wxString                   m_pch;

public:
	ClangPCHEntry()
	{}
	
	ClangPCHEntry(const wxString& source, const wxString &pch)
		: m_source(source)
		, m_pch(pch) {
	}

	~ClangPCHEntry()
	{}
	
	bool IsValid() const {
		return !m_source.IsEmpty();
	}
	const wxString &GetSource() const {
		return m_source;
	}
	void SetPch(const wxString& pch) {
		this->m_pch = pch;
	}
	void SetSource(const wxString& source) {
		this->m_source = source;
	}
	const wxString& GetPch() const {
		return m_pch;
	}
};

class ClangPCHCache
{
protected:
	std::map<wxString, ClangPCHEntry> m_cache;
	
public:
	ClangPCHCache();
	virtual ~ClangPCHCache();
	
	const ClangPCHEntry& GetPCH(const wxString &filename) const;
	void AddPCH(const wxString &source, const wxString &pchname);
	void RemoveEntry(const wxString &source);
	void Clear();
	bool IsEmpty() const {
		return m_cache.empty();
	}
};

#endif // CLANGPCHCACHE_H
