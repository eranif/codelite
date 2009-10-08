#ifndef ISTORAGE_H
#define ISTORAGE_H

#include "comment.h"
/**
 * @class ITagsStorage defined the tags storage API used by codelite
 * @author eran
 * @date 10/06/09
 * @file istorage.h
 * @brief
 */
class ITagsStorage
{
protected:
	bool m_useCache;

public:
	enum {
		OrderNone,
		OrderAsc,
		OrderDesc
	};

public:
	ITagsStorage(bool useCahe) : m_useCache (useCahe) {}
	virtual ~ITagsStorage(){};

	/**
	 * @brief enable caching
	 * @param enable
	 */
	virtual void EnableCache(bool enable) {
		m_useCache = enable;
	}

	// -------------------- API pure virtual ---------------------------

	/**
	 * @brief clear the cache
	 */
	virtual void ClearCache  () = 0;

	/**
	 * @brief return the cache hit rate
	 * @return
	 */
	virtual int GetCacheHitRate() = 0;

	/**
	 * @brief delete entries from the cache based on their relation
	 * to the tags in the tags's vector.
	 * @param tags
	 */
	virtual void  DeleteCachedEntriesByRelation(const std::vector<std::pair<wxString, TagEntry> >& tags) = 0;

	/**
	 * @brief sets the maximum items kept in the cache
	 * @param size positive number
	 */
	virtual void SetMaxCacheSize(int size) = 0;

	/**
	 * @brief return the number of items in cache
	 * @return
	 */
	virtual int GetCacheItemsCount() = 0;

	// --------------------------------------------------------------------------------------------

	/**
	 * @brief return list of tags based on scope and name
	 * @param scope the scope to search. If 'scope' is empty the scope is ommited from the search
	 * @param name
	 * @param partialNameAllowed
	 */
	virtual void GetTagsByScopeAndName(const wxString &scope, const wxString &name, bool partialNameAllowed, std::vector<TagEntryPtr> &tags) = 0;

	/**
	 * @brief return list of tags by scope. If the cache is enabled, tags will be fetched from the
	 * cache instead of accessing the disk
	 * @param scope
	 * @param tags
	 */
	virtual void GetTagsByScope(const wxString& scope, std::vector<TagEntryPtr> &tags) = 0;

	/**
	 * @brief return array of tags by kind.
	 * @param kinds array of kinds
	 * @param orderingColumn the column that the output should be ordered by (leave empty for no sorting)
	 * @param order OrderAsc, OrderDesc or use OrderNone for no ordering the results
	 * @param tags
	 */
	virtual void GetTagsByKind (const wxArrayString &kinds, const wxString &orderingColumn, int order, std::vector<TagEntryPtr> &tags) = 0;

	/**
	 * @brief return array of items by path
	 * @param path
	 * @param tags
	 */
	virtual void GetTagsByPath (const wxString &path, std::vector<TagEntryPtr> &tags) = 0;

	/**
	 * @brief return array of items by name and parent
	 * @param path
	 * @param tags
	 */
	virtual void GetTagsByNameAndParent (const wxString &name, const wxString &parent, std::vector<TagEntryPtr> &tags) = 0;

	/**
	 * @brief reutnr array of tags by kind and path
	 * @param kinds array of kind
	 * @param path
	 * @param tags  [output]
	 */
	virtual void GetTagsByKindAndPath (const wxArrayString &kinds, const wxString &path, std::vector<TagEntryPtr> &tags) = 0;

	/**
	 * @brief return tags by file and line number
	 * @param file
	 * @param line
	 * @param tags [output]
	 */
	virtual void GetTagsByFileAndLine (const wxString &file, int line, std::vector<TagEntryPtr> &tags) = 0;

	/**
	 * @brief return list by kind and scope
	 * @param scope
	 * @param kinds
	 * @param tags [output]
	 */
	virtual void GetTagsByScopeAndKind(const wxString &scope, const wxArrayString &kinds, std::vector<TagEntryPtr> &tags) = 0;

	/**
	 * @brief get list of tags by kind and file
	 * @param kind
	 * @param orderingColumn the column that the output should be ordered by (leave empty for no sorting)
	 * @param order OrderAsc, OrderDesc or use OrderNone for no ordering the results
	 * @param tags
	 */
	virtual void GetTagsByKindAndFile(const wxArrayString& kind, const wxString &fileName, const wxString &orderingColumn, int order, std::vector<TagEntryPtr> &tags) = 0;
};

#endif // ISTORAGE_H

