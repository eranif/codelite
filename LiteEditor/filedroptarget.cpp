//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : filedroptarget.cpp
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
#include "filedroptarget.h"
#include "frame.h"

FileDropTarget::FileDropTarget()
    : wxFileDropTarget()
{
}

bool FileDropTarget::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString &filenames)
{
	wxUnusedVar(x);
	wxUnusedVar(y);
	for(size_t i=0; i<filenames.GetCount(); i++){
		clMainFrame::Get()->GetMainBook()->OpenFile(filenames.Item(i));
	}
	return true;
}

