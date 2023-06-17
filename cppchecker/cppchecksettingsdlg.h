//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : cppchecksettingsdlg.h
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

#ifndef CPPCHECH_SETTINGS_DLG_H_INCLUDED
#define CPPCHECH_SETTINGS_DLG_H_INCLUDED

#include "cppchecksettingsdlgbase.h"

const wxString CPPCHECK_DEFAULT_COMMAND =
    R"=(#
# Define the command to run
# The following variables are available:
# - ${cppcheck}: CodeLite will expand this to the location of your installed cppcheck
# - ${WorkspacePath}: Current workspace path
# - ${CurrentFileFullPath}: Current file, full path
#   for example: when running from the UI `Run cppcheck for project`, this will be expanded for the project files
#
# NOTE:
# - Place each argument on a separate line
# - Lines starting with # are ignored
# - Empty lines are ignored (after trim)

${cppcheck} 
    --platform=native
    --std=c++20
    --project=${WorkspacePath}/compile_commands.json
    --cppcheck-build-dir=${WorkspacePath}/.codelite/cppcheck
)=";

class CppCheckSettingsDialog : public CppCheckSettingsDialogBase
{
public:
    CppCheckSettingsDialog(wxWindow* parent);
    virtual ~CppCheckSettingsDialog();
};
#endif // CPPCHECH_SETTINGS_DLG_H_INCLUDED
