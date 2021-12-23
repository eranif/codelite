//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : listctrl_improved.cpp
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

#include "listctrl_improved.h"

#include <wx/imaglist.h>
#include <wx/settings.h>

const wxEventType wxEVT_CMD_CHECKLIST_ITEM_CHECKED = 349375;

/* XPM */
static const char* Checkbox_unchecked_xpm[] = {
    /* columns rows colors chars-per-pixel */
    "16 16 11 1", "$ c #F7F7F7", "% c #FFFFFF", ". c #E7E7DE", "  c #185284", "X c #DEDEDE", "o c #E7E7DE",
    "@ c #E7E7E7", "O c #EFEFE7", "+ c #EFEFEF", "# c #F7F7EF", "& c None",
    /* pixels */
    "&&&&&&&&&&&&&&&&", "                ", " ......XooOO+.. ", " ....XXo@O++#.. ", " ...XXo@O+++#.. ",
    " ..Xoo@@++#$$.. ", " ..Xo@O++#+$$.. ", " .oo@@+++$$$$.. ", " .o@O++#+$$%%.. ", " .oO+++$$$$%%.. ",
    " .O++#$#$$%%%.. ", " .O++$$$$%%%%.. ", " .+#$#$$%%%%%.. ", " .............. ", "                ",
    "&&&&&&&&&&&&&&&&"
};

/* XPM */
static const char* Checkbox_checked_xpm[] = {
    /* columns rows colors chars-per-pixel */
    "16 16 10 1", "# c #F7F7F7", "$ c #FFFFFF", "@ c #21A521", "  c #185284", ". c #E7E7DE", "X c #E7E7E7",
    "o c #EFEFE7", "O c #EFEFEF", "+ c #F7F7EF", "& c None",
    /* pixels */
    "&&&&&&&&&&&&&&&&", "                ", " .......XooO+.. ", " .....XXooO+O.. ", " ....XXXOO@+#.. ",
    " ...XXXoO@@##.. ", " ...@XOO@@@##.. ", " ..X@@O@@@##$.. ", " .Xo@@@@@##$#.. ", " .ooO@@@##$$$.. ",
    " .oO+O@##$#$$.. ", " .O+O###$$$$$.. ", " .++###$$#$$$.. ", " .............. ", "                ",
    "&&&&&&&&&&&&&&&&"
};

BEGIN_EVENT_TABLE(ListCtrlImproved, wxListCtrl)
EVT_LEFT_DOWN(ListCtrlImproved::OnLeftDown)
END_EVENT_TABLE()

ListCtrlImproved::ListCtrlImproved(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : wxListCtrl(parent, id, pos, size, style | wxLC_REPORT)
{
    DoInitialize();
}

ListCtrlImproved::~ListCtrlImproved()
{
    for(int i = 0; i < GetItemCount(); i++) {
        wxClientData* data = (wxClientData*)GetItemData(i);
        if(data) {
            delete data;
        }
    }
}

void ListCtrlImproved::DoInitialize()
{
    m_checkedBmp = wxBitmap(Checkbox_checked_xpm);
    m_uncheckedBmp = wxBitmap(Checkbox_unchecked_xpm);

    wxImageList* imageList = new wxImageList(16, 16, true);
    imageList->Add(m_checkedBmp);
    imageList->Add(m_uncheckedBmp);
    AssignImageList(imageList, wxIMAGE_LIST_SMALL);
}

bool ListCtrlImproved::IsChecked(long item)
{
    wxListItem list_item;
    list_item.SetId(item);
    list_item.SetColumn(0);
    list_item.SetMask(wxLIST_MASK_IMAGE);
    GetItem(list_item);
    int imageId = list_item.GetImage();
    return imageId == 0;
}

void ListCtrlImproved::Check(long item, bool b)
{
    wxListItem list_item;
    list_item.SetId(item);
    list_item.SetColumn(0);
    list_item.SetMask(wxLIST_MASK_IMAGE);
    list_item.SetImage(b ? 0 : 1);
    SetItem(list_item);
}

long ListCtrlImproved::AppendRow()
{
    long item;
    GetItemCount() ? item = GetItemCount() : item = 0;

    wxListItem info;

    // Set the item display name
    info.SetColumn(0);
    info.SetId(item);

    if(GetItemCount() % 2 && HasFlag(wxLC_COLOUR_BACKGROUND))
        info.SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));

    item = InsertItem(info);
    return item;
}

wxString ListCtrlImproved::GetText(long item, long col)
{
    wxListItem list_item;
    list_item.SetId(item);
    list_item.SetColumn(col);
    list_item.SetMask(wxLIST_MASK_TEXT);
    GetItem(list_item);
    return list_item.GetText();
}

void ListCtrlImproved::SetTextColumn(long item, long col, const wxString& text)
{
    wxListItem list_item;
    list_item.SetId(item);
    list_item.SetColumn(col);
    list_item.SetImage(wxNOT_FOUND);
    list_item.SetMask(wxLIST_MASK_TEXT | wxLIST_MASK_IMAGE);
    list_item.SetText(text);
    SetItem(list_item);
}

void ListCtrlImproved::OnLeftDown(wxMouseEvent& event)
{
    wxPoint pt = event.GetPosition();

    int flags(0);
    int item = HitTest(pt, flags);
    if((item != wxNOT_FOUND) && (flags & wxLIST_HITTEST_ONITEMICON)) {
        // toggle the current line selection mode
        Check(item, !IsChecked(item));

        // Notify about the selection
        wxCommandEvent evt(wxEVT_CMD_CHECKLIST_ITEM_CHECKED);
        evt.SetEventObject(this);
        evt.SetInt(item);
        GetParent()->GetEventHandler()->AddPendingEvent(evt);
    }
    event.Skip();
}

bool ListCtrlImproved::SetItemClientData(long item, wxClientData* clientData)
{
    return SetItemPtrData(item, (wxUIntPtr)clientData);
}

void ListCtrlImproved::SetCheckboxRow(long item, bool checked)
{
    wxListItem list_item;
    list_item.SetId(item);
    list_item.SetColumn(0);
    list_item.SetImage(checked ? 0 : 1);
    list_item.SetMask(wxLIST_MASK_IMAGE);
    SetItem(list_item);
}

long ListCtrlImproved::GetSelection() const
{
    long selecteditem = wxNOT_FOUND;
    selecteditem = GetNextItem(selecteditem, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    return selecteditem;
}
