#ifndef WXTREETRAVERSER_H
#define WXTREETRAVERSER_H

#include "precompiled_header.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK wxTreeTraverser
{
protected:
	wxTreeCtrl *m_tree;
	
protected:
	void DoTraverse(const wxTreeItemId &item);
	
public:
	wxTreeTraverser(wxTreeCtrl *tree);
	virtual ~wxTreeTraverser();
	
	/**
	 * @brief traverse the tree starting with given 'item'
	 * If item is invalid, the traverse will start from the root item
	 * @param item
	 */
	virtual void Traverse(const wxTreeItemId &item = wxTreeItemId());
	
	/**
	 * @brief this method is called by the traverser for every item in the tree
	 * By default this method does nothing. Override it to do something meaningfull
	 * @param item current item
	 */
	virtual void OnItem(const wxTreeItemId& item);
};

#endif // WXTREETRAVERSER_H
