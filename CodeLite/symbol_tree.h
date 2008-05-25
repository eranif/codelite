//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : symbol_tree.h              
//                                                                          
// -------------------------------------------------------------------------
// A                                                                        
//              _____           _      _     _ _                            
//             /  __ \         | |    | |   (_) |                           
//             | /  \/ ___   __| | ___| |    _| |_ ___                      
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ \                     
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
 #ifndef CODELITE_SYMBOL_TREE_H
#define CODELITE_SYMBOL_TREE_H

#include "ctags_manager.h"
#include "parse_thread.h"
#include "wx/filename.h"
#include "map"

#ifdef WXMAKINGDLL_CODELITE
#    define WXDLLIMPEXP_CL WXEXPORT
#elif defined(WXUSINGDLL_CODELITE)
#    define WXDLLIMPEXP_CL WXIMPORT
#else /* not making nor using FNB as DLL */
#    define WXDLLIMPEXP_CL
#endif // WXMAKINGDLL_CODELITE

/**
 * Class MyTreeItemData, a user defined class which keeps the full name of a tree item.
 * This will allow us to quickly search the TagTree for entries using the full name as the key.
 *
 * \date 08-19-2006
 * \author Eran
 *
 */
class WXDLLIMPEXP_CL MyTreeItemData : public wxTreeItemData
{
private:
	wxString m_fileName;
	int		 m_lineno;
	

public:
    /**
     * Constructor.
     * \param filename The full name the file 
	 * \param filename line no of the symbol
     */
    MyTreeItemData(const wxString& filename, int lineno) : m_fileName(filename), m_lineno(lineno)
	{ }

	const wxString &GetFileName() const {return m_fileName;}
	int GetLineno() const {return m_lineno;}
};

/**
 * Class SymbolTree, a tree that contains the language symbols from SQLite database.
 *
 * \date 08-18-2006
 * \author Eran
 *
 */
class WXDLLIMPEXP_CL SymbolTree : public wxTreeCtrl
{
protected:
	std::map<wxString, int> m_imagesMap;
	wxTreeItemId m_globalsNode;
	wxTreeItemId m_prototypesNode;
	wxTreeItemId m_macrosNode;
	std::map<void*, bool> m_sortItems;
	std::map<wxString, bool> m_globalsKind;
	wxFileName m_fileName;
	std::map<wxString, void*> m_items;
	TagTreePtr m_tree;
	
public:
	/**
	 * Default cosntructor.
	 */
	SymbolTree();

	/**
	 * Parameterized constructor.
	 * \param parent Tree parent window
	 * \param id Window id
	 * \param pos Window position
	 * \param size Window size
	 * \param style Window style
	 */
	SymbolTree(wxWindow *parent, const wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);
	
	/**
	 * Destructor .
	 */
	virtual ~SymbolTree(void);	

	/**
	 * Create tree, usually called after constructing SymbolTree with default constructor.
	 * \param parent Tree parent window
	 * \param id Window id
	 * \param pos Window position
	 * \param size Window size
	 * \param style Window style
	 */
	virtual void Create(wxWindow *parent, const wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTR_HIDE_ROOT | wxTR_HAS_BUTTONS);
	
	/**
	 * Construct a outline tree for fileName
	 */
	void BuildTree(const wxFileName &fileName);

	/**
	 * User provided icons for the symbols tree. 
	 * The assignment is index based, in the following order:
	 *
	 * -# project
	 * -# namespace 
	 * -# globals 
	 * -# class 
	 * -# function 
	 * -# prototype_public 
	 * -# prototype_protected
	 * -# prototype_private 
	 * -# member_public 
	 * -# member_protected 
	 * -# member_private 
	 * -# typedef 
	 * -# macro
	 * -# enum
	 *
	 * \note Due to ctags limitation, only 'function' is mentioned here, there is no function_protected, function_private, etc
	 * since when coming to implementation (not prototypes!), all functions will receive 'public' icon.    
	 * \param images Image list (allocated on the heap), this class becomes the owner of this image list
	 */
	virtual void SetSymbolsImages(wxImageList *images)
	{
		AssignImageList(images);
	};

	void AddSymbols(SymbolTreeEvent& event);
	void DeleteSymbols(SymbolTreeEvent& event);
	void UpdateSymbols(SymbolTreeEvent& event);

	/**
	 * return the file name assocaited with this symbol tree
	 */
	const wxFileName &GetFilename() const {return m_fileName;}
	
	/**
	 * \brief select item by its name and select it. If multiple matches 
	 * fits 'name' the first one is selected 
	 * \param name disply name of the item to be selected (can be partial name)
	 */
	void SelectItemByName(const wxString &name);
protected:

	void GetItemChildrenRecursive(wxTreeItemId& parent, std::map<void*, bool> &deletedMap);

	/**
	 * Add an item to the gui tree.
	 * \param node Node to add 
	 */
	void AddItem(TagNode* node);

	/**
	 * Return the icon index according to item kind and access.
	 * \param kind Item kind (class, namespace etc)
	 * \param access One of $public$, $private$, $protected$, or $wxEmptyString$
	 * \return icon index
	 */
	int GetItemIconIndex(const wxString &kind, const wxString &access = wxEmptyString);

	/**
	 * Initialise the image: icon index map.
	 */
	void InitialiseSymbolMap();

	/**
	 * Sort the tree.
	 * \param nodes Vector of nodes to sort
	 */
	void SortTree(std::map<void*, bool> & nodes);

	/**
	 * Override this function in the derived class to change the sort order of the items in the tree control. 
	 * \param item1 Item one
	 * \param item2 Item two
	 * \return The function should return a negative, zero or positive value 
	 * if the first item is less than, equal to or greater than the second one.
	 */
	int OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2);

	/**
	 * Update gui item with new data
	 * \param data new data 
	 * \param key node key
	 */
	void UpdateGuiItem(TagEntry& data, const wxString& key);
	

	DECLARE_DYNAMIC_CLASS(SymbolTree)
	void OnItemActivated(wxTreeEvent &event);
	void OnTimer(wxTimerEvent &event);
};
#endif // CODELITE_SYMBOL_TREE_H
