//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : sftp_item_comparator.cpp
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

#include "sftp_item_comparator.h"

/*SFTPItemComparator::SFTPItemComparator()
{
}

SFTPItemComparator::~SFTPItemComparator()
{
}

int SFTPItemComparator::Compare(wxTreeListCtrl* treelist, unsigned column, wxTreeListItem first, wxTreeListItem second)
{
    // get the item data
    MyClientData *cdOne = dynamic_cast<MyClientData *>(treelist->GetItemData(first));
    MyClientData *cdTwo = dynamic_cast<MyClientData *>(treelist->GetItemData(second));
    
    if ( column != 0 )
        return 0;
    
    if ( !cdOne || !cdTwo )
        return 0;
    
    if ( cdOne->IsFolder() && !cdTwo->IsFolder())
        return -1;
        
    else if ( !cdOne->IsFolder() && cdTwo->IsFolder() )
        return 1;
    else 
        return cdOne->GetFullName() > cdTwo->GetFullName();
}*/
