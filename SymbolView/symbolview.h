//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : symbolview.h              
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

#ifndef __SymbolView__
#define __SymbolView__

#include <map>
#include <queue>
#include <wx/choice.h>
#include <wx/imaglist.h>
#include <wx/treectrl.h>
#include "plugin.h"
#include "dockablepane.h"
#include "windowstack.h"
#include "dropbutton.h"

class SymbolViewPlugin : public IPlugin
{
    friend class TagTreeData;

public:
    //--------------------------------------------
    //Types
    //--------------------------------------------
    /**
     * Used to indicate current "view mode" for symbol browsing
     */
    enum ViewMode
    {
        vmCurrentFile,      ///< Show symbols of current file only
        vmCurrentProject,   ///< Show symbols of current project only
        vmCurrentWorkspace, ///< Show all symbols of loaded workspace

        vmMax               ///< Sentinel value (used for size of view mode arrays)
    };

    typedef std::pair<wxString, wxString> TagKey;               ///< Identifies a tag by file path and tag key
    typedef std::pair<wxTreeCtrl*, wxTreeItemId> TreeNode;      ///< Identifies a tree node

    typedef std::multimap<wxString, TreeNode> File2TagMap;      ///< Tracks which tree nodes have tags from which files
    typedef std::multimap<wxString, TreeNode> Path2TagMap;      ///< Tracks which tree nodes are identified by which tag paths

    typedef std::pair<File2TagMap::iterator, File2TagMap::iterator> File2TagRange; ///< For querying File2TagMap by filename
    typedef std::pair<Path2TagMap::iterator, Path2TagMap::iterator> Path2TagRange; ///< For querying Path2TagMap by tag path

    /**
     * Each tag node in a tree has this data attached to it.  When created, the object attaches itself
     * to the specified node.  Also adds entries in the path->tag and file->tag maps pointing to the node.
     */
    class TagTreeData : public wxTreeItemData, public TagEntry
    {
    private:
        SymbolViewPlugin *m_view;
        SymbolViewPlugin::Path2TagMap::iterator m_pathIter;
        SymbolViewPlugin::File2TagMap::iterator m_fileIter;

    public:
        TagTreeData(SymbolViewPlugin *view, wxTreeCtrl *tree, wxTreeItemId id, const TagEntry &p)
            : TagEntry(p)
            , m_view(view)
            , m_pathIter(view->m_pathTags.insert(std::make_pair(p.Key(), TreeNode(tree, id))))
            , m_fileIter(view->m_fileTags.insert(std::make_pair(p.GetFile(), TreeNode(tree, id))))
        {
            tree->SetItemData(id, this);
        }

        ~TagTreeData()
        {
            m_view->m_pathTags.erase(m_pathIter);
            m_view->m_fileTags.erase(m_fileIter);
        }
    };

    /**
     * Subclass wxTreeCtrl so we can control the sort order of child nodes.  Also keeps ids of top-level
     * grouping nodes (globals, prototypes, macros).
     */
    class SymTree : public wxTreeCtrl
    {
        DECLARE_DYNAMIC_CLASS(SymTree)
    public:
        SymTree() { }
        SymTree(wxWindow *parent) : wxTreeCtrl(parent) { }

        int IsCtorOrDtor(const wxTreeItemId &id);
        int OnCompareItems(const wxTreeItemId &id1, const wxTreeItemId &id2);

        wxTreeItemId m_globals, m_protos, m_macros;
    };

    //--------------------------------------------
    //Constructors/Destructors
    //--------------------------------------------
    SymbolViewPlugin(IManager *manager);
    ~SymbolViewPlugin();

    //--------------------------------------------
    //Abstract methods
    //--------------------------------------------
    virtual wxToolBar *CreateToolBar(wxWindow *parent);
    virtual void CreatePluginMenu(wxMenu *pluginsMenu);
    virtual void HookPopupMenu(wxMenu *menu, MenuType type);
    virtual void UnHookPopupMenu(wxMenu *menu, MenuType type);
    virtual void UnPlug();

    //--------------------------------------------
    //Public properties
    //--------------------------------------------
    bool GetIsLinkedToEditor() const { return m_isLinkedToEditor; }
    ViewMode GetViewMode() const;

private:
    //--------------------------------------------
    //Members
    //--------------------------------------------
    wxPanel *m_symView;         ///< Container of symbol browser GUI elements
    StackButton *m_stackChoice; ///< Allows user to select a tree in the current view mode
    wxChoice *m_viewChoice;     ///< User can select a view mode
    WindowStack *m_viewStack;   ///< Shows current symbols for selected view mode
	wxBoxSizer* m_choiceSizer;	///< Sizer for the drop button & the view mode
    bool m_isLinkedToEditor;    ///< Controls whether visible symbol tree changes when active editor changes

    wxArrayString m_viewModeNames;      ///< User-visible names for view modes
    wxImageList *m_imagesList;          ///< For symbol-related icons
    std::map<wxString, int> m_image;    ///< Maps friendly name to index in m_images

    /**
     * List of all currently viewable tree nodes, indexed by the fully-qualified symbol each node
     * represents.  A symbol may appear in more than one tree.  Note that each node has a pointer
     * back to its entry in this map.
     *
     * @sa TreeTagData
     */
    Path2TagMap m_pathTags;

    /**
     * List of all currently viewable tree nodes, indexed by the path to the file each node's symbol
     * came from.  A symbol may appear in more than one tree.  Note that each node has a pointer
     * back to its entry in this map.
     *
     * @sa TreeTagData
     */
    File2TagMap m_fileTags;

    /**
     * If a scoped symbol is read from the db before its scope is, then we won't have a place to put it in
     * the tree.  Such symbols go here until they can be added.
     */
    std::queue<TagEntry> m_deferredTags;

    /**
     * During tree updates, accumulates the set of nodes that need their children sorted.  Sorting is deferred
     * till after all nodes have been updated.  The key is a specially constructed path from tree's file path
     * plus the path to the node in the tree.  (Normally I would just use a std::set<TreeNode> because that's all
     * I really need, but TreeNode doesn't appear to work as a key value -- probably an invalid operator<).
     */
    std::map<TagKey, TreeNode> m_sortNodes;

    //--------------------------------------------
    //GUI setup methods
    //--------------------------------------------
    void LoadImagesAndIndexes();
    void CreateGUIControls();
    void Connect();

    //--------------------------------------------
    //Helper methods
    //--------------------------------------------
    wxString GetSymbolsPath(IEditor *editor);
    void GetFiles(const wxFileName &path, std::multimap<wxString,wxString> &files);
    void GetPaths(const wxArrayString &files, std::multimap<wxString,wxString> &filePaths);
    wxSQLite3ResultSet GetTags(const std::multimap<wxString,wxString> &sqlopts);

    //--------------------------------------------
    //Tree-related methods
    //--------------------------------------------
    SymTree *FindSymbolTree(const wxString &path);
    wxTreeItemId GetParentForGlobalTag(SymTree *tree, const TagEntry &tag);
    void SetNodeData(wxTreeCtrl *tree, wxTreeItemId id, const TagEntry &tag);
    void SortChildren();
    int  LoadChildren(SymTree *tree, wxTreeItemId id);
    int  AddSymbol(const TagEntry &tag, const std::multimap<wxString,wxString> &filePaths);
    int  UpdateSymbol(const TagEntry &tag);
    int  DeleteSymbol(const TagEntry &tag);
    int  DeleteFileSymbols(const wxString &file);
    void AddDeferredSymbols(const std::multimap<wxString,wxString> &filePaths);
    void UpdateTrees(const wxArrayString &files, bool removeOld);
    void CreateSymbolTree(const wxString &path, WindowStack *parent);
    void ShowSymbolTree();
	bool DoActivateSelection(wxTreeCtrl *tree);

    //--------------------------------------------
    //Event handlers
    //--------------------------------------------
    void OnViewModeMouseDown(wxMouseEvent &e);
    void OnViewTypeChanged(wxCommandEvent &e);

    void OnLinkEditor(wxCommandEvent &e);
    void OnCollapseAll(wxCommandEvent &e);
    void OnGoHome(wxCommandEvent &e);

    void OnStackChoiceUI(wxUpdateUIEvent &e);
    void OnCollapseAllUI(wxUpdateUIEvent &e);
    void OnGoHomeUI(wxUpdateUIEvent &e);

    void OnNodeExpanding(wxTreeEvent &e);
    void OnNodeKeyDown(wxTreeEvent &e);
	void OnNodeDClick(wxMouseEvent &e);
    void OnNodeContextMenu(wxTreeEvent &e);

    void OnWorkspaceLoaded(wxCommandEvent &e);
    void OnWorkspaceClosed(wxCommandEvent &e);

    void OnFileRetagged(wxCommandEvent &e);
    void OnProjectFileAdded(wxCommandEvent &e);
    void OnProjectFileRemoved(wxCommandEvent &e);
    void OnProjectAdded(wxCommandEvent &e);
    void OnProjectRemoved(wxCommandEvent &e);

    void OnSymbolsUpdated(wxCommandEvent &e);
    void OnSymbolsDeleted(wxCommandEvent &e);
    void OnSymbolsAdded(wxCommandEvent &e);

    void OnActiveEditorChanged(wxCommandEvent &e);
    void OnEditorClosed(wxCommandEvent &e);
    void OnAllEditorsClosed(wxCommandEvent &e);
};


#endif //SymbolView

