//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : clImageViewer.h
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

#ifndef CLIMAGEVIEWER_H
#define CLIMAGEVIEWER_H
#include "wxcrafter_plugin.h"

#include <wx/filename.h>

class WXDLLIMPEXP_SDK clImageViewer : public clImageViewerBase
{
    wxFileName m_filename;
    wxBitmap m_bitmap;

public:
    clImageViewer(wxWindow* parent, const wxFileName& filename);
    virtual ~clImageViewer();
    void DoLayout();
};

#endif // CLIMAGEVIEWER_H
