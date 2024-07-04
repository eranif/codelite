//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : tag_tree.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#ifndef CODELITE_TAG_TREE_H
#define CODELITE_TAG_TREE_H

#include "codelite_exports.h"
#include "database/entry.h"
#include "tree.h"

#include <wx/string.h>
#include <memory>

typedef TreeNode<wxString, TagEntry> TagNode;

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

using TagTreePtr = std::shared_ptr<TagTree>;

#endif // CODELITE_TAG_TREE_H
