//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : virtualdirtreectrl.cpp
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
 /////////////////////////////////////////////////////////////////////////////
// Name:        wxVirtualDirTreeCtrl.cpp
// Author:      XX
// Created:     Saturday, March 27, 2004 14:15:56
// Copyright:   XX
/////////////////////////////////////////////////////////////////////////////

//#ifdef __GNUG__
//    #pragma implementation "wxVirtualDirTreeCtrl.cpp"
//#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/wx.h"
#include <wx/dir.h>
#include <wx/busyinfo.h>
#include "virtualdirtreectrl.h"
#include "wx/xrc/xmlres.h"
#include "globals.h"
#include "bitmap_loader.h"
#include "plugin.h"

//#define __PERFORMANCE
#include "performance.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// wxVirtualDirTreeCtrl
//----------------------------------------------------------------------------

// WDR: event table for wxVirtualDirTreeCtrl

const wxEventType wxVDTC_ROOT_CHANGED = wxNewId();

BEGIN_EVENT_TABLE(wxVirtualDirTreeCtrl, wxTreeCtrl)
	EVT_TREE_ITEM_EXPANDING(wxID_ANY, wxVirtualDirTreeCtrl::OnExpanding)
END_EVENT_TABLE()

wxVirtualDirTreeCtrl::wxVirtualDirTreeCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator, const wxString& name)
	: wxTreeCtrl(parent, id, pos, size, style, validator, name)
	, _flags(wxVDTC_DEFAULT)
{
	// create an icon list for the tree ctrl
	_iconList = new wxImageList(16,16);
	MSWSetNativeTheme(this);
	
	// reset to default extension list
	ResetExtensions();
}

wxVirtualDirTreeCtrl::~wxVirtualDirTreeCtrl()
{
	// first delete all VdtcTreeItemBase items (client data)
	DeleteAllItems();
#ifdef __WXMSW__
	_cache.clear();
#endif
	// delete the icons
	delete _iconList;
}

bool wxVirtualDirTreeCtrl::SetRootPath(const wxString &root, bool notify, int flags)
{
	bool value;
	wxBusyInfo *bsy = 0;
	wxLogNull log;

	// set flags to adopt new behaviour
	_flags = flags;

	// delete all items plus root first
	DeleteAllItems();
#ifdef __WXMSW__
	_cache.clear();
#endif
	VdtcTreeItemBase *start = 0;

	// now call for icons management, the virtual
	// handler so the derived class can assign icons

	if(_iconList->GetImageCount() == 0){
		OnAssignIcons(*_iconList);
		SetImageList(_iconList);
	}

	value = ::wxDirExists(root);
	if(value)
	{
		// call virtual handler to notify the derived class
		OnSetRootPath(root);

		// create a root item
		start = OnCreateTreeItem(VDTC_TI_ROOT, root);
		if(start)
		{
			wxFileName path;
			path.AssignDir(root);

			// call the add callback and find out if this root
			// may be added (later on)

			if(OnAddRoot(*start, path))
			{
				// add this item to the tree, with info of the developer
				wxTreeItemId id = AddRoot(start->GetCaption(), start->GetIconId(), start->GetSelectedIconId(), start);

				// show a busy dialog
				if(_flags & (wxVDTC_RELOAD_ALL | wxVDTC_SHOW_BUSYDLG))
					bsy = new wxBusyInfo(_("Please wait, scanning directory..."), 0);

				// scan directory, either the smart way or not at all
				ScanFromDir(start, path, (wxVDTC_RELOAD_ALL & _flags ? -1 : VDTC_MIN_SCANDEPTH));

				// expand root when allowed
				if(!(_flags & wxVDTC_NO_EXPAND))
					Expand(id);
			}
			else
				delete start; // sorry not succeeded
		}
	}

	// delete busy info if present
	if(bsy)
		delete bsy;

	if(notify){
		wxCommandEvent e(wxVDTC_ROOT_CHANGED, GetId());
		e.SetEventObject(this);
		GetEventHandler()->ProcessEvent(e);
	}

	return value;
}

int wxVirtualDirTreeCtrl::ScanFromDir(VdtcTreeItemBase *item, const wxFileName &path, int level, bool reload)
{
	int value = 0;
	wxCHECK(item, -1);
	wxCHECK(item->IsDir() || item->IsRoot(), -1);

	wxLogNull log;

	// when we can still scan, do so
	if(level == -1 || level > 0)
	{
		// TODO: Maybe when a reload is issued, delete all items that are no longer present
		// in the tree (on disk) and check if all new items are present, else add them
		if(reload){
			DeleteChildren(item->GetId());
#ifdef __WXMSW__
            _cache.clear();
#endif
		}

		// if no items, then go iterate and get everything in this branch
		if(GetChildrenCount(item->GetId()) == 0)
		{
			VdtcTreeItemBaseArray addedItems;

			// now call handler, if allowed, scan this dir
			if(OnDirectoryScanBegin(path))
			{
				// get directories
				GetDirectories(item, addedItems, path);

				// get files
				if(!(_flags & wxVDTC_NO_FILES))
					GetFiles(item, addedItems, path);

				// call handler that can do a last thing
				// before sort and anything else
				OnDirectoryScanEnd(addedItems, path);

				// sort items
				if(addedItems.GetCount() > 0 && (_flags & wxVDTC_NO_SORT) == 0)
					SortItems(addedItems, 0, (int)addedItems.GetCount()-1);

				AddItemsToTreeCtrl(item, addedItems);

				// call handler to tell that the items are on the tree ctrl
				OnAddedItems(item->GetId());
			}
		}

		value = (int)GetChildrenCount(item->GetId());

		// go through all children of this node, pick out all
		// the dir classes, and descend as long as the level allows it
		// NOTE: Don't use the addedItems array, because some new can
		// be added or some can be deleted.

		wxTreeItemIdValue cookie = 0;
		VdtcTreeItemBase *b;

		wxTreeItemId child = GetFirstChild(item->GetId(), cookie);
		while(child.IsOk())
		{
			b = (VdtcTreeItemBase *)GetItemData(child);
			if(b && b->IsDir())
			{
				wxFileName tp = path;
				tp.AppendDir(b->GetName());
				value += ScanFromDir(b, tp, (level == -1 ? -1 : level-1), false);
			}

			child = GetNextChild(item->GetId(), cookie);
		}
	}

	return value;
}

void wxVirtualDirTreeCtrl::GetFiles(VdtcTreeItemBase *parent, VdtcTreeItemBaseArray &items, const wxFileName &path)
{
	wxUnusedVar(parent);
	wxFileName fpath;
	wxString fname;
	VdtcTreeItemBase *item;

	fpath = path;

	// no nodes present yet, we should start scanning this dir
	// scan files first in this directory, with all extensions in this array

	for(size_t i = 0; i < _extensions.Count(); i++)
	{
		wxDir fdir(path.GetFullPath());

		if(fdir.IsOpened())
		{
			bool bOk = fdir.GetFirst(&fname, _extensions[i], wxDIR_FILES | wxDIR_HIDDEN);
			while(bOk)
			{
				// TODO: Flag for double items

				item = AddFileItem(fname);
				if(item)
				{
					// fill it in, and marshall it by the user for info
					fpath.SetFullName(fname);
					if(OnAddFile(*item, fpath))
						items.Add(item);
					else
						delete item;
				}

				bOk = fdir.GetNext(&fname);
			}
		}
	}
}

void wxVirtualDirTreeCtrl::GetDirectories(VdtcTreeItemBase *parent, VdtcTreeItemBaseArray &items, const wxFileName &path)
{
	wxUnusedVar(parent);
	wxFileName fpath;
	wxString fname;
	VdtcTreeItemBase *item;

	// no nodes present yet, we should start scanning this dir
	// scan files first in this directory, with all extensions in this array

	wxDir fdir(path.GetFullPath());
	if(fdir.IsOpened())
	{
		bool bOk = fdir.GetFirst(&fname, VDTC_DIR_FILESPEC, wxDIR_DIRS | wxDIR_HIDDEN);
		while(bOk)
		{
			// TODO: Flag for double items
			item = AddDirItem(fname);
			if(item)
			{
				// fill it in, and marshall it by the user for info
				fpath = path;
				fpath.AppendDir(fname);

				if(OnAddDirectory(*item, fpath))
					items.Add(item);
				else
					delete item;
			}

			bOk = fdir.GetNext(&fname);
		}
	}
}

int wxVirtualDirTreeCtrl::OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2)
{
	// used for SortChildren, reroute to our sort routine
	VdtcTreeItemBase *a = (VdtcTreeItemBase *)GetItemData(item1),
		            *b = (VdtcTreeItemBase *)GetItemData(item2);
	if(a && b)
		return OnCompareItems(a,b);

	return 0;
}

int wxVirtualDirTreeCtrl::OnCompareItems(const VdtcTreeItemBase *a, const VdtcTreeItemBase *b)
{
	// if dir and other is not, dir has preference
	if(a->IsDir() && b->IsFile())
		return -1;
	else if(a->IsFile() && b->IsDir())
		return 1;

	// else let ascii fight it out
	return a->GetCaption().CmpNoCase(b->GetCaption());
}

void wxVirtualDirTreeCtrl::SwapItem(VdtcTreeItemBaseArray &items, int a, int b)
{
	VdtcTreeItemBase *t = items[b];
	items[b] = items[a];
	items[a] = t;
}

void wxVirtualDirTreeCtrl::SortItems(VdtcTreeItemBaseArray &items, int left, int right)
{
	VdtcTreeItemBase *a, *b;
	int i, last;

	if(left >= right)
		return;

	SwapItem(items, left, (left + right)/2);

	last = left;
	for(i = left+1; i <= right; i++)
	{
		a = items[i];
		b = items[left];
		if(a && b)
		{
			if(OnCompareItems(a, b) < 0)
				SwapItem(items, ++last, i);
		}
	}

	SwapItem(items, left, last);
	SortItems(items, left, last-1);
	SortItems(items, last+1, right);
}


void wxVirtualDirTreeCtrl::AddItemsToTreeCtrl(VdtcTreeItemBase *item, VdtcTreeItemBaseArray &items)
{
	wxCHECK2(item, return);

	// now loop through all elements on this level and add them
	// to the tree ctrl pointed out by 'id'

	VdtcTreeItemBase *t;
	wxTreeItemId id = item->GetId();
	for(size_t i = 0; i < items.GetCount(); i++)
	{
		t = items[i];
		if(t){
			wxTreeItemId newItem = AppendItem(id, t->GetCaption(), t->GetIconId(), t->GetSelectedIconId(), t);
            if (t->IsDir()) {
                SetItemHasChildren(newItem);
            }
			//keep the newly added item in the cache
#ifdef __WXMSW__
			wxString fullpath = GetFullPath(newItem).GetFullPath();
			if(fullpath.IsEmpty() == false){
					_cache[fullpath] = newItem.m_pItem;
			}
#endif
		}
	}
}

wxFileName wxVirtualDirTreeCtrl::GetRelativePath(const wxTreeItemId &id)
{
	wxFileName value;
	wxCHECK(id.IsOk(), value);

	VdtcTreeItemBase *b = (VdtcTreeItemBase *)GetItemData(id);
	wxCHECK(b, value);

	AppendPathRecursively(b, value, false);

	return value;
}

wxFileName wxVirtualDirTreeCtrl::GetFullPath(const wxTreeItemId &id)
{
	wxFileName value;
	wxCHECK(id.IsOk(), value);

	VdtcTreeItemBase *b = (VdtcTreeItemBase *)GetItemData(id);
	wxCHECK(b, value);

	AppendPathRecursively(b, value, true);

	return value;
}

wxTreeItemId wxVirtualDirTreeCtrl::GetItemByFullPath(const wxFileName &fullpath, bool scandirs)
{
#ifdef __WXMSW__
	// try the cache
	std::map< wxString, void* >::const_iterator iter = _cache.find(fullpath.GetFullPath());
	if (iter != _cache.end()) {
        return iter->second;
    }
#endif

	wxTreeItemId id = DoFindItemByPath(fullpath, scandirs);

#ifdef __WXMSW__
	if (scandirs && id.IsOk()) {
		_cache[fullpath.GetFullPath()] = id.m_pItem;
	}
#endif

	return id;
}

wxTreeItemId wxVirtualDirTreeCtrl::ExpandToPath(const wxFileName &path)
{
    wxTreeItemId item = GetItemByFullPath(path);
	if (item.IsOk()){
		if (ItemHasChildren(item)) {
			Expand(item);
		}
		SelectItem(item);
        EnsureVisible(item);
		SendCmdEvent(wxEVT_FILE_EXP_REFRESHED);
	}
	return item;
}

wxTreeItemId wxVirtualDirTreeCtrl::DoFindItemByPath(const wxFileName &path, bool scandirs)
{
    PERF_FUNCTION();

    wxString volume = path.HasVolume() ? path.GetVolume()+wxT(":\\") : wxString(wxT("/"));

    // look for volume
	wxTreeItemId curr = GetRootItem();
    if (!curr.IsOk() || GetItemText(curr) != volume) {
        // not the same volume
        if (!scandirs || !SetRootPath(volume, true, GetExtraFlags())) {
            return wxTreeItemId();
        }
        curr = GetRootItem();
    }

    // look for directories in the path
	wxFileName seekpath;
    if (path.HasVolume()) {
        seekpath.SetVolume(path.GetVolume());
    }
    bool found = true;
    for (size_t i = 0; i < path.GetDirs().GetCount() && found; i++) {
        found = false;
        wxTreeItemIdValue cookie;
        for (wxTreeItemId id = GetFirstChild(curr, cookie); id.IsOk(); id = GetNextChild(curr, cookie)) {
            VdtcTreeItemBase *ptr = (VdtcTreeItemBase *)GetItemData(id);
            // FIXME: should IsSameAs() case-sensitivity flag depend on platform?
            if (ptr->IsDir() && ptr->GetName().IsSameAs(path.GetDirs()[i], false)) {
                found = true;
                curr = id;
                seekpath.AppendDir(path.GetDirs()[i]);
                if (scandirs) {
                    // make sure child nodes are loaded from disk
                    ScanFromDir(ptr, GetFullPath(ptr->GetId()), VDTC_MIN_SCANDEPTH);
                }
                break;
            }
        }
    }
    if (found && path.HasName()) {
        // look for the final path element (should be a file but might be a directory)
        found = false;
        wxTreeItemIdValue cookie;
        for (wxTreeItemId id = GetFirstChild(curr, cookie); id.IsOk() && !found; id = GetNextChild(curr, cookie)) {
            VdtcTreeItemBase *ptr = (VdtcTreeItemBase *)GetItemData(id);
            // FIXME: should IsSameAs() case-sensitivity flag depend on platform?
            if (ptr->GetName().IsSameAs(path.GetFullName(), false)) {
                found = true;
                curr = id;
                break;
            }
        }
    }
    return found || !scandirs ? curr : wxTreeItemId();
}

bool wxVirtualDirTreeCtrl::IsRootNode(const wxTreeItemId &id)
{
	bool value = false;
	wxCHECK(id.IsOk(), value);

	VdtcTreeItemBase *b = (VdtcTreeItemBase *)GetItemData(id);
	if(b)
		value = b->IsRoot();

	return value;
}

bool wxVirtualDirTreeCtrl::IsDirNode(const wxTreeItemId &id)
{
	bool value = false;
	wxCHECK(id.IsOk(), value);

	VdtcTreeItemBase *b = (VdtcTreeItemBase *)GetItemData(id);
	if(b)
		value = b->IsDir();

	return value;
}

bool wxVirtualDirTreeCtrl::IsFileNode(const wxTreeItemId &id)
{
	bool value = false;
	wxCHECK(id.IsOk(), value);

	VdtcTreeItemBase *b = (VdtcTreeItemBase *)GetItemData(id);
	if(b)
		value = b->IsFile();

	return value;
}

/** Appends subdirs up until root. This is done by finding the root first and
    going back down to the original caller. This is faster because no copying takes place */
void wxVirtualDirTreeCtrl::AppendPathRecursively(VdtcTreeItemBase *b, wxFileName &dir, bool useRoot)
{
	wxCHECK2(b, return);

	VdtcTreeItemBase *parent = GetParent(b);
	if(parent)
		AppendPathRecursively(parent, dir, useRoot);
	else
	{
		// no parent assume top node
		if(b->IsRoot() && useRoot)
			dir.AssignDir(b->GetName());
		return;
	}

	// now we are unwinding the other way around
	if(b->IsDir())
		dir.AppendDir(b->GetName());
	else if(b->IsFile())
		dir.SetFullName(b->GetName());
};

// -- event handlers --

void wxVirtualDirTreeCtrl::OnExpanding(wxTreeEvent &event)
{

	// check for collapsing item, and scan from there
	wxTreeItemId item = event.GetItem();
	if(item.IsOk())
	{
		VdtcTreeItemBase *t = (VdtcTreeItemBase *)GetItemData(item);
		if(t && t->IsDir())
		{
			// extract data element belonging to it, and scan.
			ScanFromDir(t, GetFullPath(item), VDTC_MIN_SCANDEPTH);
		}
	}
	// be kind, and let someone else also handle this event
	event.Skip();
}

void wxVirtualDirTreeCtrl::DoReloadNode(const wxTreeItemId &item)
{
	if(item.IsOk())
	{
		VdtcTreeItemBase *t = (VdtcTreeItemBase *)GetItemData(item);
		if(t && (t->IsDir() || t->IsRoot()))
		{
			// extract data element belonging to it, and scan.
			ScanFromDir(t, GetFullPath(item), VDTC_MIN_SCANDEPTH, true);
		}
	}
}

VdtcTreeItemBase *wxVirtualDirTreeCtrl::AddFileItem(const wxString &name)
{
	// call the file item node create method
	return OnCreateTreeItem(VDTC_TI_FILE, name);
}

VdtcTreeItemBase *wxVirtualDirTreeCtrl::AddDirItem(const wxString &name)
{
	// call the dir item node create method
	return OnCreateTreeItem(VDTC_TI_DIR, name);
}


// --- virtual handlers ----

void wxVirtualDirTreeCtrl::OnAssignIcons(wxImageList &icons)
{
	BitmapLoader bmpLoader(wxT("codelite-icons.zip"));
	icons.Add(bmpLoader.LoadBitmap(wxT("mime/16/hard_disk")));          //0
	icons.Add(bmpLoader.LoadBitmap(wxT("mime/16/folder")));              //1
	icons.Add(bmpLoader.LoadBitmap(wxT("mime/16/text")));                //2
	icons.Add(bmpLoader.LoadBitmap(wxT("mime/16/cpp")));                 //3
	icons.Add(bmpLoader.LoadBitmap(wxT("mime/16/c")));                   //4
	icons.Add(bmpLoader.LoadBitmap(wxT("mime/16/h")));                   //5
	icons.Add(bmpLoader.LoadBitmap(wxT("mime/16/exe")));                 //6
	icons.Add(bmpLoader.LoadBitmap(wxT("mime/16/php")));                 //7
	icons.Add(bmpLoader.LoadBitmap(wxT("mime/16/dll")));                 //8
	icons.Add(bmpLoader.LoadBitmap(wxT("mime/16/bmp")));                 //9
	icons.Add(bmpLoader.LoadBitmap(wxT("mime/16/script")));              //10
	icons.Add(bmpLoader.LoadBitmap(wxT("mime/16/zip")));                 //11
	icons.Add(bmpLoader.LoadBitmap(wxT("mime/16/xml")));                 //12
	icons.Add(bmpLoader.LoadBitmap(wxT("mime/16/html")));                //13
	icons.Add(bmpLoader.LoadBitmap(wxT("mime/16/makefile")));            //14
	icons.Add(bmpLoader.LoadBitmap(wxT("mime/16/wxfb")));                //15
	icons.Add(bmpLoader.LoadBitmap(wxT("mime/16/cd")));                //16
	icons.Add(bmpLoader.LoadBitmap(wxT("mime/16/erd")));                //17
}

VdtcTreeItemBase *wxVirtualDirTreeCtrl::OnCreateTreeItem(int type, const wxString &name)
{
	// return a default instance, no extra info needed in this item
	return new VdtcTreeItemBase(type, name);
}

bool wxVirtualDirTreeCtrl::OnAddRoot(VdtcTreeItemBase &item, const wxFileName &name)
{
	// allow adding
	wxUnusedVar(item);
	wxUnusedVar(name);
	return true;
}

bool wxVirtualDirTreeCtrl::OnDirectoryScanBegin(const wxFileName &path)
{
	// allow all paths
	wxUnusedVar(path);
	return true;
}

bool wxVirtualDirTreeCtrl::OnAddFile(VdtcTreeItemBase &item, const wxFileName &name)
{
	// allow all files
	wxUnusedVar(item);
	wxUnusedVar(name);
	return true;
}

bool wxVirtualDirTreeCtrl::OnAddDirectory(VdtcTreeItemBase &item, const wxFileName &name)
{
	// allow all dirs
	wxUnusedVar(item);
	wxUnusedVar(name);
	return true;
}

void wxVirtualDirTreeCtrl::OnSetRootPath(const wxString &root)
{
	// do nothing here, but it can be used to start initialisation
	// based upon the setting of the root (which means a renewal from the tree)
	wxUnusedVar(root);
}

void wxVirtualDirTreeCtrl::OnAddedItems(const wxTreeItemId &parent)
{
	wxUnusedVar(parent);
	return;
}

void wxVirtualDirTreeCtrl::OnDirectoryScanEnd(VdtcTreeItemBaseArray &items, const wxFileName &path)
{
	wxUnusedVar(items);
	wxUnusedVar(path);
	return;
}



