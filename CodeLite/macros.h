//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : macros.h
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
#ifndef MACROS_H
#define MACROS_H

#include "cl_standard_paths.h"
#include <wx/stdpaths.h>
#include <wx/intl.h>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include "wxStringHash.h"

//-------------------------------------------------------
// wxWidgets Connect macros
//-------------------------------------------------------

#define ConnectChoice(ctrl, fn) \
    ctrl->Connect(ctrl->GetId(), wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(fn), NULL, this);

#define ConnectButton(ctrl, fn) \
    ctrl->Connect(ctrl->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(fn), NULL, this);

#define ConnectListBoxDClick(ctrl, fn) \
    ctrl->Connect(ctrl->GetId(), wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler(fn), NULL, this);

#define ConnectCheckBox(ctrl, fn) \
    ctrl->Connect(ctrl->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(fn), NULL, this);

#define ConnectListCtrlItemSelected(ctrl, fn) \
    ctrl->Connect(ctrl->GetId(), wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler(fn), NULL, this);

#define ConnectListCtrlItemActivated(ctrl, fn) \
    ctrl->Connect(ctrl->GetId(), wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler(fn), NULL, this);

#define ConnectKeyDown(ctrl, fn) ctrl->Connect(ctrl->GetId(), wxEVT_KEY_DOWN, wxKeyEventHandler(fn), NULL, this);

#define ConnectCharEvent(ctrl, fn) ctrl->Connect(ctrl->GetId(), wxEVT_CHAR, wxKeyEventHandler(fn), NULL, this);

#define ConnectCmdTextEntered(ctrl, fn) \
    ctrl->Connect(ctrl->GetId(), wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(fn), NULL, this);

#define ConnectCmdTextUpdated(ctrl, fn) \
    ctrl->Connect(ctrl->GetId(), wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(fn), NULL, this);

#define ConnectCombo(ctrl, fn) \
    ctrl->Connect(ctrl->GetId(), wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(fn), NULL, this);

#define ConnectCheckList(ctrl, fn) \
    ctrl->Connect(ctrl->GetId(), wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler(fn), NULL, this);

#define TrimString(str)        \
    {                          \
        str = str.Trim();      \
        str = str.Trim(false); \
    }

#define IsSourceFile(ext) \
    (ext == wxT("cpp") || ext == wxT("cxx") || ext == wxT("c") || ext == wxT("c++") || ext == wxT("cc"))

#define BoolToString(b) b ? wxT("yes") : wxT("no")

//-----------------------------------------------------
// Constants
//-----------------------------------------------------

#define clCMD_NEW  "<New...>"
#define clCMD_EDIT "<Edit...>"

// constant message
#define BUILD_START_MSG "----------Build Started--------\n"
#define BUILD_END_MSG "----------Build Ended----------\n"
#define BUILD_PROJECT_PREFIX "----------Building project:[ "
#define CLEAN_PROJECT_PREFIX "----------Cleaning project:[ "

// Find in files options
#define SEARCH_IN_WORKSPACE "<Entire Workspace>"
#define SEARCH_IN_PROJECT "<Active Project>"
#define SEARCH_IN_CURR_FILE_PROJECT "<Current File's Project>"
#define SEARCH_IN_CURRENT_FILE "<Current File>"
#define SEARCH_IN_OPEN_FILES "<Open Files>"
#define SEARCH_IN_WORKSPACE_FOLDER "<Workspace Folder>"
#define USE_WORKSPACE_ENV_VAR_SET _("<Use Defaults>")
#define USE_GLOBAL_SETTINGS _("<Use Defaults>")

// terminal macro
#ifdef __WXGTK__
#define TERMINAL_CMD                                               \
    wxString::Format(wxT("%s/codelite_xterm '$(TITLE)' '$(CMD)'"), \
                     wxFileName(clStandardPaths::Get().GetExecutablePath()).GetPath().c_str())
#elif defined(__WXMAC__)
#define TERMINAL_CMD wxString::Format(wxT("%s/OpenTerm '$(CMD)'"), clStandardPaths::Get().GetDataDir().c_str())
#else
#define TERMINAL_CMD ""
#endif

#if defined(__WXMSW__)
#define PATH_SEP wxT("\\")
#else
#define PATH_SEP wxT("/")
#endif

#define ID_MENU_CUSTOM_TARGET_FIRST 15000
#define ID_MENU_CUSTOM_TARGET_MAX 15100

#define viewAsMenuItemID 23500
#define viewAsMenuItemMaxID 23500 + 100

#define viewAsSubMenuID 23499
#define RecentFilesSubMenuID 23600
#define RecentWorkspaceSubMenuID 23650

// Useful macros
#define CHECK_PTR_RET(p) \
    if(!p) return
#define CHECK_PTR_RET_FALSE(p) \
    if(!p) return false
#define CHECK_PTR_RET_NULL(p) \
    if(!p) return NULL
#define CHECK_PTR_RET_EMPTY_STRING(p) \
    if(!p) return wxEmptyString

#define CHECK_COND_RET(p) \
    if(!(p)) return
#define CHECK_COND_RET_FALSE(p) \
    if(!(p)) return false
#define CHECK_COND_RET_NULL(p) \
    if(!(p)) return NULL
#define CHECK_COND_RET_EMPTY_STRING(p) \
    if(!(p)) return wxEmptyString

#define CHECK_ITEM_RET(item) \
    if(!item.IsOk()) return
#define CHECK_ITEM_RET_FALSE(item) \
    if(!item.IsOk()) return false
#define CHECK_ITEM_RET_NULL(item) \
    if(!item.IsOk()) return NULL
#define CHECK_ITEM_RET_EMPTY_STRING(item) \
    if(!item.IsOk()) return wxEmptyString

// PATH environment variable separator
#ifdef __WXMSW__
#define clPATH_SEPARATOR ";"
#else
#define clPATH_SEPARATOR ":"
#endif

// ----------------------------- usefule typedefs -------------------------------
typedef std::unordered_map<wxString, bool> wxStringBoolMap_t;
typedef std::unordered_map<wxString, wxString> wxStringTable_t;
typedef std::unordered_set<wxString> wxStringSet_t;
typedef wxStringTable_t wxStringMap_t; // aliases

#endif // MACROS_H
