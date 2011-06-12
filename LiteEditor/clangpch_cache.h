#ifndef CLANGPCHCACHE_H
#define CLANGPCHCACHE_H

#include <wx/string.h>
#include <map>
#include <set>
#include "globals.h"

class ClangPCHEntry
{
protected:
	wxString      m_source;
	wxString      m_pch;
	wxArrayString m_includes;
	wxArrayString m_includesFullPath;
	time_t        m_lastAccessed;

public:
	ClangPCHEntry()
	{}

	ClangPCHEntry(const wxString& source, const wxString &pch, const wxArrayString &includes, const wxArrayString &includesFullPath)
		: m_source(source)
		, m_pch(pch)
		, m_includes(includes)
		, m_includesFullPath(includesFullPath)
		, m_lastAccessed(time(NULL)) {
	}

	~ClangPCHEntry()
	{}

	bool NeedRegenration(const wxArrayString &includes) const {

		if(!IsValid())
			return true;

		if(includes.GetCount() != m_includes.GetCount())
			return true;

		for(size_t i=0; i<includes.GetCount(); i++) {
			if(m_includes.Index(includes.Item(i)) == wxNOT_FOUND) {
				return true;
			}
		}

		time_t pch_gen_time = GetFileModificationTime(m_pch);
		for(size_t i=0; i<m_includesFullPath.GetCount(); i++) {
			time_t inc_mod_time = GetFileModificationTime(m_includesFullPath.Item(i));
			if(inc_mod_time > pch_gen_time)
				return true;
		}
		return false;
	}

	void SetLastAccessed(time_t lastAccessed) {
		this->m_lastAccessed = lastAccessed;
	}
	time_t GetLastAccessed() const {
		return m_lastAccessed;
	}
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
	size_t                            m_maxItems;
	
public:
	ClangPCHCache();
	virtual ~ClangPCHCache();

	const ClangPCHEntry& GetPCH(const wxString &filename);
	void AddPCH(const wxString& source, const wxString& pchname, const wxArrayString &includes, const wxArrayString &includesFullPaths);
	void RemoveEntry(const wxString &source);
	void Clear();
	bool IsEmpty() const {
		return m_cache.empty();
	}

	static wxString GetCacheDirectory();
};

#endif // CLANGPCHCACHE_H
