//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : SvnBlameFrame.h
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

#ifndef SVNBLAMEFRAME_H
#define SVNBLAMEFRAME_H

#include "subversion2_ui.h"
#include <wx/filename.h>

class SvnBlameFrame : public SvnBlameFrameBase
{
    wxFileName m_filename;

public:
    SvnBlameFrame(wxWindow* parent, const wxFileName& filename, const wxString &content);
    virtual ~SvnBlameFrame() = default;
};
#endif // SVNBLAMEFRAME_H
