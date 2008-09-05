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
 #include "menumanager.h"
#include "menu_event_handlers.h"
#include "macros.h"


MenuManager::MenuManager(void)
{
	PushHandler(new EditHandler(wxID_COPY));
	PushHandler(new EditHandler(wxID_CUT));
	PushHandler(new EditHandler(wxID_PASTE));
	PushHandler(new EditHandler(wxID_UNDO));
	PushHandler(new EditHandler(wxID_REDO));
	PushHandler(new EditHandler(wxID_SELECTALL));
	PushHandler(new EditHandler(wxID_DUPLICATE));
	PushHandler(new EditHandler(wxID_DELETE));
	PushHandler(new EditHandler(XRCID("delete_line")));
	PushHandler(new EditHandler(XRCID("delete_line_end")));
	PushHandler(new EditHandler(XRCID("delete_line_start")));
	PushHandler(new EditHandler(XRCID("transpose_lines")));
	PushHandler(new EditHandler(XRCID("to_upper")));
	PushHandler(new EditHandler(XRCID("to_lower")));
	PushHandler(new EditHandler(XRCID("swap_files")));
	PushHandler(new BraceMatchHandler(XRCID("select_to_brace")));
	PushHandler(new BraceMatchHandler(XRCID("match_brace")));
	PushHandler(new FindReplaceHandler(wxID_FIND));
	PushHandler(new FindReplaceHandler(wxID_REPLACE));
	PushHandler(new FindReplaceHandler(XRCID("find_next")));
	PushHandler(new FindReplaceHandler(XRCID("find_next_at_caret")));
	PushHandler(new FindReplaceHandler(XRCID("find_previous_at_caret")));
	PushHandler(new FindReplaceHandler(XRCID("find_previous")));
	PushHandler(new GotoHandler(XRCID("goto_linenumber")));
	PushHandler(new BookmarkHandler(XRCID("toggle_bookmark")));
	PushHandler(new BookmarkHandler(XRCID("next_bookmark")));
	PushHandler(new BookmarkHandler(XRCID("previous_bookmark")));
	PushHandler(new BookmarkHandler(XRCID("removeall_bookmarks")));
	PushHandler(new GotoDefinitionHandler(XRCID("goto_definition")));
	PushHandler(new GotoDefinitionHandler(XRCID("goto_previous_definition")));
	PushHandler(new ViewAsHandler(viewAsMenuItemID));
	PushHandler(new WordWrapHandler(XRCID("word_wrap")));
	PushHandler(new FoldHandler(XRCID("toggle_fold")));
	PushHandler(new FoldHandler(XRCID("fold_all")));
	PushHandler(new DebuggerMenuHandler(XRCID("insert_breakpoint")));
	PushHandler(new DebuggerMenuHandler(XRCID("delete_breakpoint")));
}

MenuManager::~MenuManager(void)
{
}
