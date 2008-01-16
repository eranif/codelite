#include "precompiled_header.h"

#include "tag_tree.h"
#include "tokenizer.h"
#ifdef __VISUALC__
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#endif 


TagTree::TagTree(const wxString& key, const TagEntry& data)
: Tree<wxString, TagEntry>(key, data)
{
}

TagTree::~TagTree()
{
}

/// Util function to help us to build a tree from tags
TagNode* TagTree::AddEntry(TagEntry& tag)
{
	// If a node with thig tag already exist, we simply updating the 
	// data 
	wxString key(tag.Key());

	TagNode* newNode = Find(key);
	if( newNode )
	{
		if( tag.IsOk() )
			newNode->SetData(tag);
		return newNode;

	}

	// To add an entry to the tree, we first must make sure
	// that all path to it exist
	wxString name = tag.GetPath();
	StringTokenizer tok(name, wxT("::"));

	wxString parentPath;
	TagNode* node = GetRoot();
	TagNode* lastFoundNode = GetRoot();
	for(int i=0; i<tok.Count()-1; i++)
	{
		parentPath += tok[i];

		// Try to find this node in the tree
		node = Find(parentPath);
		if( !node )
		{
			// Node does not exist add it, we copy key values
			// from 'tag'
			TagEntry ee;
			ee.SetPath(parentPath);
			ee.SetName(tok[i]);
			node = AddChild(parentPath, ee, lastFoundNode);
		}

		lastFoundNode = node;
		if(i < tok.Count()-2)
			parentPath += _T("::");
	}

	return AddChild(key, tag, node);
}

