//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : php_open_resource_dlg.h
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

#ifndef __php_open_resource_dlg__
#define __php_open_resource_dlg__

#include <wx/propgrid/manager.h>
#include "php_ui.h"
#include <vector>
#include "PHPEntityVariable.h"
#include "PHPEntityBase.h"
#include "PHPLookupTable.h"
#include "bitmap_loader.h"

struct ResourceItem {
    enum { kRI_Invalid = -1, kRI_File, kRI_Class, kRI_Constant, kRI_Function, kRI_Member, kRI_Variable, kRI_Namespace };
    wxString displayName;
    wxFileName filename;
    int line;
    int type;

    ResourceItem()
        : line(0)
        , type(kRI_Invalid)
    {
    }

    void SetType(PHPEntityBase::Ptr_t match)
    {
        if(match->Is(kEntityTypeClass)) {
            type = kRI_Class;
        } else if(match->Is(kEntityTypeFunction)) {
            type = kRI_Function;
        } else if(match->Is(kEntityTypeVariable)) {
            if(match->Cast<PHPEntityVariable>()->IsConst()) {
                type = kRI_Constant;
            } else if(match->Cast<PHPEntityVariable>()->IsMember()) {
                type = kRI_Member;
            } else {
                type = kRI_Variable;
            }
        } else if(match->Is(kEntityTypeNamespace)) {
            type = kRI_Namespace;
        }
    }

    wxString TypeAsString() const
    {
        switch(type) {
        case ResourceItem::kRI_Namespace:
            return "Namespace";
        case ResourceItem::kRI_Class:
            return "Class";
        case ResourceItem::kRI_Constant:
            return "Constant";
        case ResourceItem::kRI_File:
            return "File";
        case ResourceItem::kRI_Function:
            return "Function";
        case ResourceItem::kRI_Member:
            return "Member";
        case ResourceItem::kRI_Invalid:
            return "Invalid";
        default:
        case ResourceItem::kRI_Variable:
            return "Variable";
        }
    }
};

typedef std::vector<ResourceItem> ResourceVector_t;

class IManager;
class OpenResourceDlg : public OpenResourceDlgBase
{
public:
    wxString m_lastFilter;
    IManager* m_mgr;
    wxTimer* m_timer;
    ResourceVector_t m_allFiles;
    ResourceVector_t m_resources;
    ResourceItem* m_selectedItem;
    PHPLookupTable m_table;

protected:
    virtual void OnDVItemActivated(wxDataViewEvent& event);

    bool IsMatchesFilter(const wxString& filter, const wxString& key);
    ResourceVector_t DoGetFiles(const wxString& filter);
    void DoGetResources(const wxString& filter);
    ResourceItem* DoGetItemData(const wxDataViewItem& item);
    int DoGetImgIdx(const ResourceItem* item);

protected:
    void DoSelectNext();
    void DoSelectPrev();
    void DoInitialize();
    void DoPopulateListCtrl(const ResourceVector_t& items);

protected:
    // Handlers for OpenResourceDlgBase events.
    void OnFilterEnter(wxCommandEvent& event);
    void OnFilterText(wxCommandEvent& event);
    void OnKeyDown(wxKeyEvent& event);

    DECLARE_EVENT_TABLE()
    void OnTimer(wxTimerEvent& event);

public:
    /** Constructor */
    OpenResourceDlg(wxWindow* parent, IManager* manager);
    OpenResourceDlg(wxWindow* parent, const ResourceVector_t& items, IManager* manager);
    virtual ~OpenResourceDlg();

    void SetSelectedItem(ResourceItem* selectedItem) { this->m_selectedItem = selectedItem; }
    ResourceItem* GetSelectedItem() { return m_selectedItem; }
};

#endif // __php_open_resource_dlg__
