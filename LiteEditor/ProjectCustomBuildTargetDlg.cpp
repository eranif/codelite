//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : ProjectCustomBuildTargetDlg.cpp
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

#include "ProjectCustomBuildTargetDlg.h"

#include "globals.h"

const wxString ProjectCustomBuildTargetDlg::CUSTOM_TARGET_BUILD = wxT("Build");
const wxString ProjectCustomBuildTargetDlg::CUSTOM_TARGET_CLEAN = wxT("Clean");
const wxString ProjectCustomBuildTargetDlg::CUSTOM_TARGET_REBUILD = wxT("Rebuild");
const wxString ProjectCustomBuildTargetDlg::CUSTOM_TARGET_COMPILE_SINGLE_FILE = wxT("Compile Single File");
const wxString ProjectCustomBuildTargetDlg::CUSTOM_TARGET_PREPROCESS_FILE = wxT("Preprocess File");

ProjectCustomBuildTargetDlg::ProjectCustomBuildTargetDlg(wxWindow* parent,
                                                         const wxString& targetName,
                                                         const wxString& targetCommand)
    : ProjectCustomBuildTragetDlgBase(parent)
{
    m_textCtrlCommand->ChangeValue(targetCommand);
    m_textCtrlTargetName->ChangeValue(targetName);
    SetName("ProjectCustomBuildTargetDlg");
    clSetSmallDialogBestSizeAndPosition(*this);
}

void ProjectCustomBuildTargetDlg::OnEditTargetNameUI(wxUpdateUIEvent& event)
{
    event.Enable(!IsPredefinedTarget(GetTargetName()));
}

bool ProjectCustomBuildTargetDlg::IsPredefinedTarget(const wxString& name)
{
    return name == CUSTOM_TARGET_BUILD || name == CUSTOM_TARGET_CLEAN || name == CUSTOM_TARGET_REBUILD ||
           name == CUSTOM_TARGET_COMPILE_SINGLE_FILE || name == CUSTOM_TARGET_PREPROCESS_FILE;
}
