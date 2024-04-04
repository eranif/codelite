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
    PushHandler(std::make_shared<EditHandler>(wxID_COPY));
    PushHandler(std::make_shared<EditHandler>(wxID_CUT));
    PushHandler(std::make_shared<EditHandler>(wxID_PASTE));
    PushHandler(std::make_shared<EditHandler>(wxID_UNDO));
    PushHandler(std::make_shared<EditHandler>(wxID_REDO));
    PushHandler(std::make_shared<EditHandler>(XRCID("label_current_state")));
    PushHandler(std::make_shared<EditHandler>(wxID_SELECTALL));
    PushHandler(std::make_shared<EditHandler>(wxID_DUPLICATE));
    PushHandler(std::make_shared<EditHandler>(wxID_DELETE));
    PushHandler(std::make_shared<EditHandler>(wxID_ZOOM_FIT));
    PushHandler(std::make_shared<EditHandler>(wxID_ZOOM_IN));
    PushHandler(std::make_shared<EditHandler>(wxID_ZOOM_OUT));
    PushHandler(std::make_shared<EditHandler>(XRCID("delete_line")));
    PushHandler(std::make_shared<EditHandler>(XRCID("delete_line_end")));
    PushHandler(std::make_shared<EditHandler>(XRCID("delete_line_start")));
    PushHandler(std::make_shared<EditHandler>(XRCID("copy_line")));
    PushHandler(std::make_shared<EditHandler>(XRCID("cut_line")));
    PushHandler(std::make_shared<EditHandler>(XRCID("transpose_lines")));
    PushHandler(std::make_shared<EditHandler>(XRCID("trim_trailing")));
    PushHandler(std::make_shared<EditHandler>(XRCID("to_upper")));
    PushHandler(std::make_shared<EditHandler>(XRCID("to_lower")));
    PushHandler(std::make_shared<EditHandler>(XRCID("swap_files")));
    PushHandler(std::make_shared<EditHandler>(XRCID("move_line_down")));
    PushHandler(std::make_shared<EditHandler>(XRCID("move_line_up")));
    PushHandler(std::make_shared<EditHandler>(XRCID("center_line_roll")));
    PushHandler(std::make_shared<EditHandler>(XRCID("convert_indent_to_tabs")));
    PushHandler(std::make_shared<EditHandler>(XRCID("convert_indent_to_spaces")));
    PushHandler(std::make_shared<BraceMatchHandler>(XRCID("select_to_brace")));
    PushHandler(std::make_shared<BraceMatchHandler>(XRCID("match_brace")));
    PushHandler(std::make_shared<FindReplaceHandler>(XRCID("id_find")));
    PushHandler(std::make_shared<FindReplaceHandler>(XRCID("id_replace")));
    PushHandler(std::make_shared<FindReplaceHandler>(XRCID("ID_QUICK_ADD_NEXT")));
    PushHandler(std::make_shared<FindReplaceHandler>(XRCID("ID_QUICK_FIND_ALL")));
    PushHandler(std::make_shared<GotoHandler>(XRCID("goto_linenumber")));
    PushHandler(std::make_shared<BookmarkHandler>(XRCID("toggle_bookmark")));
    PushHandler(std::make_shared<BookmarkHandler>(XRCID("next_bookmark")));
    PushHandler(std::make_shared<BookmarkHandler>(XRCID("previous_bookmark")));
    PushHandler(std::make_shared<BookmarkHandler>(XRCID("removeall_current_bookmarks")));
    PushHandler(std::make_shared<BookmarkHandler>(XRCID("removeall_bookmarks")));
    PushHandler(std::make_shared<GotoDefinitionHandler>(XRCID("goto_definition")));
    PushHandler(std::make_shared<GotoDefinitionHandler>(XRCID("goto_previous_definition")));
    PushHandler(std::make_shared<WordWrapHandler>(XRCID("word_wrap")));
    PushHandler(std::make_shared<FoldHandler>(XRCID("toggle_fold")));
    PushHandler(std::make_shared<FoldHandler>(XRCID("fold_topmost_in_selection")));
    PushHandler(std::make_shared<FoldHandler>(XRCID("fold_all_in_selection")));
    PushHandler(std::make_shared<FoldHandler>(XRCID("fold_all")));
    PushHandler(std::make_shared<DebuggerMenuHandler>(XRCID("add_breakpoint")));
    PushHandler(std::make_shared<DebuggerMenuHandler>(XRCID("insert_breakpoint"))); // This actually does 'toggle'
    PushHandler(std::make_shared<DebuggerMenuHandler>(XRCID("disable_all_breakpoints")));
    PushHandler(std::make_shared<DebuggerMenuHandler>(XRCID("enable_all_breakpoints")));
    PushHandler(std::make_shared<DebuggerMenuHandler>(XRCID("delete_all_breakpoints")));
    PushHandler(std::make_shared<DebuggerMenuHandler>(XRCID("insert_temp_breakpoint")));
    PushHandler(std::make_shared<DebuggerMenuHandler>(XRCID("insert_disabled_breakpoint")));
    PushHandler(std::make_shared<DebuggerMenuHandler>(XRCID("insert_cond_breakpoint")));
    PushHandler(std::make_shared<DebuggerMenuHandler>(XRCID("insert_watchpoint")));
    PushHandler(std::make_shared<DebuggerMenuHandler>(XRCID("toggle_breakpoint_enabled_status")));
    PushHandler(std::make_shared<DebuggerMenuHandler>(XRCID("ignore_breakpoint")));
    PushHandler(std::make_shared<DebuggerMenuHandler>(XRCID("edit_breakpoint")));
    PushHandler(std::make_shared<DebuggerMenuHandler>(XRCID("delete_breakpoint")));
    PushHandler(std::make_shared<DebuggerMenuHandler>(XRCID("show_breakpoint_dlg")));
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
