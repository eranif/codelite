#include "sftp_item_comparator.h"

SFTPItemComparator::SFTPItemComparator()
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
}
