//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : menumanager.cpp
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
#include "macros.h"
#include "menu_event_handlers.h"
#include "menumanager.h"

MenuManager::MenuManager(void)
{
    PushHandler(new EditHandler(wxID_COPY));
    PushHandler(new EditHandler(wxID_CUT));
    PushHandler(new EditHandler(wxID_PASTE));
    PushHandler(new EditHandler(wxID_UNDO));
    PushHandler(new EditHandler(wxID_REDO));
    PushHandler(new EditHandler(XRCID("label_current_state")));
    PushHandler(new EditHandler(wxID_SELECTALL));
    PushHandler(new EditHandler(wxID_DUPLICATE));
    PushHandler(new EditHandler(wxID_DELETE));
    PushHandler(new EditHandler(wxID_ZOOM_FIT));
    PushHandler(new EditHandler(wxID_ZOOM_IN));
    PushHandler(new EditHandler(wxID_ZOOM_OUT));
    PushHandler(new EditHandler(XRCID("delete_line")));
    PushHandler(new EditHandler(XRCID("delete_line_end")));
    PushHandler(new EditHandler(XRCID("delete_line_start")));
    PushHandler(new EditHandler(XRCID("copy_line")));
    PushHandler(new EditHandler(XRCID("cut_line")));
    PushHandler(new EditHandler(XRCID("transpose_lines")));
    PushHandler(new EditHandler(XRCID("trim_trailing")));
    PushHandler(new EditHandler(XRCID("to_upper")));
    PushHandler(new EditHandler(XRCID("to_lower")));
    PushHandler(new EditHandler(XRCID("swap_files")));
    PushHandler(new EditHandler(XRCID("move_line_down")));
    PushHandler(new EditHandler(XRCID("move_line_up")));
    PushHandler(new EditHandler(XRCID("center_line_roll")));
    PushHandler(new EditHandler(XRCID("convert_indent_to_tabs")));
    PushHandler(new EditHandler(XRCID("convert_indent_to_spaces")));
    PushHandler(new BraceMatchHandler(XRCID("select_to_brace")));
    PushHandler(new BraceMatchHandler(XRCID("match_brace")));
    PushHandler(new FindReplaceHandler(XRCID("id_find")));
    PushHandler(new FindReplaceHandler(XRCID("id_replace")));
    PushHandler(new FindReplaceHandler(XRCID("ID_QUICK_ADD_NEXT")));
    PushHandler(new FindReplaceHandler(XRCID("ID_QUICK_FIND_ALL")));
    PushHandler(new GotoHandler(XRCID("goto_linenumber")));
    PushHandler(new BookmarkHandler(XRCID("toggle_bookmark")));
    PushHandler(new BookmarkHandler(XRCID("next_bookmark")));
    PushHandler(new BookmarkHandler(XRCID("previous_bookmark")));
    PushHandler(new BookmarkHandler(XRCID("removeall_current_bookmarks")));
    PushHandler(new BookmarkHandler(XRCID("removeall_bookmarks")));
    PushHandler(new GotoDefinitionHandler(XRCID("goto_definition")));
    PushHandler(new GotoDefinitionHandler(XRCID("goto_previous_definition")));
    PushHandler(new WordWrapHandler(XRCID("word_wrap")));
    PushHandler(new FoldHandler(XRCID("toggle_fold")));
    PushHandler(new FoldHandler(XRCID("fold_topmost_in_selection")));
    PushHandler(new FoldHandler(XRCID("fold_all_in_selection")));
    PushHandler(new FoldHandler(XRCID("fold_all")));
    PushHandler(new DebuggerMenuHandler(XRCID("add_breakpoint")));
    PushHandler(new DebuggerMenuHandler(XRCID("insert_breakpoint"))); // This actually does 'toggle'
    PushHandler(new DebuggerMenuHandler(XRCID("disable_all_breakpoints")));
    PushHandler(new DebuggerMenuHandler(XRCID("enable_all_breakpoints")));
    PushHandler(new DebuggerMenuHandler(XRCID("delete_all_breakpoints")));
    PushHandler(new DebuggerMenuHandler(XRCID("insert_temp_breakpoint")));
    PushHandler(new DebuggerMenuHandler(XRCID("insert_disabled_breakpoint")));
    PushHandler(new DebuggerMenuHandler(XRCID("insert_cond_breakpoint")));
    PushHandler(new DebuggerMenuHandler(XRCID("insert_watchpoint")));
    PushHandler(new DebuggerMenuHandler(XRCID("toggle_breakpoint_enabled_status")));
    PushHandler(new DebuggerMenuHandler(XRCID("ignore_breakpoint")));
    PushHandler(new DebuggerMenuHandler(XRCID("edit_breakpoint")));
    PushHandler(new DebuggerMenuHandler(XRCID("delete_breakpoint")));
    PushHandler(new DebuggerMenuHandler(XRCID("show_breakpoint_dlg")));
}

MenuManager::~MenuManager(void)
{
}

void MenuManager::PushHandler(MenuEventHandlerPtr handler)
{
    m_handlers[handler->GetEventId()] = handler;
}

MenuEventHandlerPtr MenuManager::GetHandler(int id)
{
    HandlesrHash::iterator iter = m_handlers.find(id);
    if(iter != m_handlers.end())
        return iter->second;
    return NULL;
}
