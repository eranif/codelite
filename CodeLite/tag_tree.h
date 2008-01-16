#ifndef CODELITE_TAG_TREE_H
#define CODELITE_TAG_TREE_H

#include "smart_ptr.h"
#include "tree.h"
#include "entry.h"

typedef TreeNode<wxString, TagEntry> TagNode;

#ifdef WXMAKINGDLL_CODELITE
#    define WXDLLIMPEXP_CL WXEXPORT
#elif defined(WXUSINGDLL_CODELITE)
#    define WXDLLIMPEXP_CL WXIMPORT
#else /* not making nor using FNB as DLL */
#    define WXDLLIMPEXP_CL
#endif

/**
 * Tree representation of tags. 
 * This is basically a data structure representing the GUI symbol tree.
 *
 * \date 08-23-2006
 * \author eran
 *
 */
class WXDLLIMPEXP_CL TagTree : public Tree<wxString, TagEntry>
{
public:
	/**
	 * Construct a tree with roots' values key & data.
	 * \param key Root's key
	 * \param data Root's data
	 */
	TagTree(const wxString& key, const TagEntry& data);


	/** 
	 * Destructor
	 */
	virtual ~TagTree();

	/**
	 * Add an entry to the tree. 
	 * This functions will add all parents of the tag to the tree if non exist (or some of some exist).
	 * For example: if TagName is box::m_string, this functions will make sure that a node 'box' exists.
	 * If not, it will add it and then will add m_string to it as its child.
	 * \param tag Tag to add
	 * \return new node that was added to the tree.
	 */
	TagNode* AddEntry(TagEntry& tag);

};

typedef SmartPtr<TagTree> TagTreePtr;

#endif // CODELITE_TAG_TREE_H
