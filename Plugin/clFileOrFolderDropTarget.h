//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : clFileOrFolderDropTarget.h
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

#ifndef CLFILEORFOLDERDROPTARGET_H
#define CLFILEORFOLDERDROPTARGET_H

#include "codelite_exports.h"

#include <wx/dnd.h> // Base class: wxFileDropTarget
#include <wx/event.h>

/**
 * @class clFileOrFolderDropTarget
 * @brief a drop target class for files or folder
 * This class translates the "OnDrop" function into codelite events:
 * wxEVT_DND_FOLDER_DROPPED and wxEVT_DND_FILE_DROPPED that will be sent to the eventSink object
 * The list of files or folders is set in the event.GetString() method
 * of the event
 */
class WXDLLIMPEXP_SDK clFileOrFolderDropTarget : public wxFileDropTarget
{
    wxEvtHandler* m_sink;

public:
    clFileOrFolderDropTarget(wxEvtHandler* eventSink);
    virtual ~clFileOrFolderDropTarget();
    virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames);
};

#endif // CLFILEORFOLDERDROPTARGET_H
