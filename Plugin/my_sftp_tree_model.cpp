//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : my_sftp_tree_model.cpp
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

#include "my_sftp_tree_model.h"
#if USE_SFTP

MySFTPTreeModel::MySFTPTreeModel()
{
}

MySFTPTreeModel::~MySFTPTreeModel()
{
}

wxString MySFTPTreeModel::GetColumnType(unsigned int col) const
{
    wxUnusedVar(col);
#ifdef __WXGTK__
    return "string";
#else
    return SFTPTreeModel::GetColumnType(col);
#endif
}

#endif // USE_SFTP
