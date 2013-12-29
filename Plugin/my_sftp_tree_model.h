//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : my_sftp_tree_model.h
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

#ifndef MYSFTPTREEMODEL_H
#define MYSFTPTREEMODEL_H

#if USE_SFTP
#include "sftptreemodel.h" // Base class: SFTPTreeModel

class MySFTPTreeModel : public SFTPTreeModel
{
public:
    virtual wxString GetColumnType(unsigned int col) const;

    MySFTPTreeModel();
    virtual ~MySFTPTreeModel();


};
#endif // USE_SFTP
#endif // MYSFTPTREEMODEL_H
