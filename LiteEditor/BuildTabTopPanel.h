//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : BuildTabTopPanel.h
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

#ifndef BUILDTABTOPPANEL_H
#define BUILDTABTOPPANEL_H
#include "cl_defs.h"
#include "clToolBar.h"

class NewBuildTab;
class BuildTabTopPanel : public wxPanel
{
private:
    NewBuildTab* m_buildTab;
    clToolBar* m_toolbar;

public:
    BuildTabTopPanel(wxWindow* parent);
    virtual ~BuildTabTopPanel();

protected:
    virtual void OnAutoScrollUI(wxUpdateUIEvent& event);
    virtual void OnAutoScroll(wxCommandEvent& event);
    virtual void OnPaste(wxCommandEvent& event);
    virtual void OnPasteUI(wxUpdateUIEvent& event);
    virtual void OnCopyBuildOutput(wxCommandEvent& event);
    virtual void OnCopyBuildOutputUI(wxUpdateUIEvent& event);
    virtual void OnClearBuildOutput(wxCommandEvent& event);
    virtual void OnClearBuildOutputUI(wxUpdateUIEvent& event);
    virtual void OnSaveBuildOutput(wxCommandEvent& event);
    virtual void OnSaveBuildOutputUI(wxUpdateUIEvent& event);
    virtual void OnToolPinCommandToolClicked(wxCommandEvent& event);
};

#endif // BUILDTABTOPPANEL_H
