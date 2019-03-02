//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : codelitediff.h
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

#ifndef __CodeLiteDiff__
#define __CodeLiteDiff__

#include "plugin.h"
#include "cl_command_event.h"
#include <wx/filename.h>

class CodeLiteDiff : public IPlugin
{
    wxFileName m_leftFile;

protected:
    void OnNewDiff(wxCommandEvent& e);
    void OnNewDiffFolder(wxCommandEvent& e);
    void OnTabContextMenu(clContextMenuEvent& event);
    void OnDiff(wxCommandEvent& event);
    void DoClear();
    wxFileName SaveEditorToTmpfile(IEditor* editor) const;

public:
    CodeLiteDiff(IManager* manager);
    ~CodeLiteDiff();

    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    virtual void CreateToolBar(clToolBar* toolbar);
    virtual void CreatePluginMenu(wxMenu* pluginsMenu);
    virtual void UnPlug();
};

#endif // CodeLiteDiff
