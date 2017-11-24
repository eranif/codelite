//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C)); Eran Ifrah
// file name            : codelite_events.h
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

#ifndef CODELITE_EVENTS_H
#define CODELITE_EVENTS_H

#include "codelite_exports.h"
#include "cl_command_event.h"
#include "clFileSystemEvent.h"

// ------------------------------------------------------------------------
//
// Plugins events
//  will not change when we move an event up or down in this file...
// ------------------------------------------------------------------------

// clientData is NULL
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_INIT_DONE, wxCommandEvent);

// wxCommandEvent::GetString() will return the node name modified
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_EDITOR_CONFIG_CHANGED, wxCommandEvent);

// wxCommandEvent::GetString() will return the workspace fullpath
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_WORKSPACE_LOADED, wxCommandEvent);

// The build configuration was changed
// use event.GetString() to get the selected configuration name
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_WORKSPACE_CONFIG_CHANGED, wxCommandEvent);

// clientData is NULL
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_WORKSPACE_CLOSED, wxCommandEvent);

// A workspace closing has started
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_WORKSPACE_CLOSING, wxCommandEvent);

// clientData is NULL
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_FILE_VIEW_INIT_DONE, wxCommandEvent);

// clientData is NULL
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_FILE_VIEW_REFRESHED, wxCommandEvent);

// clientData is NULL
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_FILE_EXP_INIT_DONE, wxCommandEvent);

// clientData is NULL
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_FILE_EXP_REFRESHED, wxCommandEvent);

// clientData is the wxTreeItemId*
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CMD_FILE_EXP_ITEM_EXPANDING, wxCommandEvent);

// an attempt to open a file using double click / ENTER was made
// Use clCommandEvent.GetFileName() to get the file name
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_TREE_ITEM_FILE_ACTIVATED, clCommandEvent);

// File(s) were added to the project
// Event type: clCommandEvent
// Use: clCommandEvent::GetStrings() to get the list of files added
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_PROJ_FILE_ADDED, clCommandEvent);

// File(s) were removed from the project
// Event type: clCommandEvent
// Use: clCommandEvent::GetStrings() to get the list of files removed
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_PROJ_FILE_REMOVED, clCommandEvent);

// Project has been removed from the workspace
// Event type: clCommandEvent
// Use: clCommandEvent::GetStrings() to get the name of the removed project
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_PROJ_REMOVED, clCommandEvent);

// Project has been added to the workspace
// Event type: clCommandEvent
// Use: clCommandEvent::GetString() to get the name of project
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_PROJ_ADDED, clCommandEvent);

// the following 2 events are used as "transaction"
// the first event indicates that any "wxEVT_FILE_SAVED" event sent from this point
// is due to build process which is about to starte
// the later event, indicates the end of that transaction
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_FILE_SAVE_BY_BUILD_END, wxCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_FILE_SAVE_BY_BUILD_START, wxCommandEvent);

// Sent when a file has been saved.
// Event type: clCommandEvent
// Use: clCommandEvent::GetString() or clCommandEvent::GetFileName() to get the name of file
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_FILE_SAVED, clCommandEvent);

// A file was renamed
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_FILE_RENAMED, clFileSystemEvent);

// A file was deleted
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_FILE_DELETED, clFileSystemEvent);

// A folder was deleted
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_FOLDER_DELETED, clFileSystemEvent);

// A file was "Save as" by the user
// This event can also be fired if the user selected "Duplicate Tab"
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_FILE_SAVEAS, clFileSystemEvent);

// clientData is list of files which have been retagged (std::vector<wxFileName>*)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_FILE_RETAGGED, wxCommandEvent);

// The active editor was changed
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_ACTIVE_EDITOR_CHANGED, wxCommandEvent);

// clientData is closing editor (IEditor*)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_EDITOR_CLOSING, wxCommandEvent);

// clientData is NULL
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_ALL_EDITORS_CLOSING, wxCommandEvent);

// clientData is NULL
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_ALL_EDITORS_CLOSED, wxCommandEvent);

// This event is sent when the user clicks inside an editor
// this event can not be Veto()
// clientData is NULL. You may query the clicked editor by calling to
// IManager::GetActiveEditor()
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_EDITOR_CLICKED, wxCommandEvent);

// User dismissed the Editor's settings dialog with
// Apply or OK (Settings | Editor)
// clientData is NULL
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_EDITOR_SETTINGS_CHANGED, wxCommandEvent);

// This event is sent from plugins to the application to tell it to reload
// any open files (and re-tag them as well)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED, wxCommandEvent);

// Same as wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED
// just without prompting the user
// this event only reload code files without
// any reload to the workspace / project
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED_NOPROMPT, wxCommandEvent);

// Sent by the project settings dialogs to indicate that
// the project configurations are saved
// Event type: clProjectSettingsEvent
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CMD_PROJ_SETTINGS_SAVED, clProjectSettingsEvent);

// A user requested to execute the active project
// Event type:
// clExecuteEvent.
// Get the target to run by using clExecuteEvent::GetTargetName
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CMD_EXECUTE_ACTIVE_PROJECT, clExecuteEvent);

// A user requested to stop the previously executed program
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CMD_STOP_EXECUTED_PROGRAM, clExecuteEvent);

// codelite sends this event to query plugins incase there is a program running
// use evet.SetAnswer(true) to indicate that the plugin has launched an executable
// it is mainly used for displaying the 'Stop' button in the toolbar as active/disabled
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CMD_IS_PROGRAM_RUNNING, clExecuteEvent);

// ----------------------------------------------------------------------
// Build Events
// ----------------------------------------------------------------------

// Event type: clBuildEvent
// Notify that a build has been started
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_BUILD_STARTED, clBuildEvent);

// Event type: clBuildEvent
// Notify that a build has been ended
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_BUILD_ENDED, clBuildEvent);

// Event type: clBuildEvent
// sent by the application to the plugins to indicate that a
// build process is about to start. by handling this event
// and by avoid calling event.Skip() codelite will NOT start
// the build process
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_BUILD_STARTING, clBuildEvent);

// Event type: clBuildEvent
// return the project clean command
// set the new clean command using: event.SetCommand( "new-clean-command" );
// avoid calling event.Skip() to indicate that a new command was provided
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_GET_PROJECT_CLEAN_CMD, clBuildEvent);

// Event type: clBuildEvent
// return the project build command
// set the new build command using: event.SetCommand( "new-build-command" );
// avoid calling event.Skip() to indicate that a new command was provided
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_GET_PROJECT_BUILD_CMD, clBuildEvent);

// Event type: clBuildEvent
// the below two events are sent by the application to the plugins to query whether a
// given project and build configuration are handled by the plugin.
// the first event is whether the makefile file is generated by the plugin
// and the second is to know whether the build process is also being
// handled by the plugin.
// the return answer is done by simply avoid calling event.Skip() (which will result in ProcessEvent() == true)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_GET_IS_PLUGIN_MAKEFILE, clBuildEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_GET_IS_PLUGIN_BUILD, clBuildEvent);

// Event type: clBuildEvent
// These events allows the plugins to concatenate a string
// to the compilation/link line of the default build system
// By using the event.SetCommand()/event.GetCommand()
// Note, that the since all multiple plugins
// might be interesting with this feature, it is recommened
// to use it like this:
// wxString content = event.GetCommand();
// content << wxT(" -DMYMACRO ");
// event.SetCommand( content );
// event.Skip();
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_GET_ADDITIONAL_COMPILEFLAGS, clBuildEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_GET_ADDITIONAL_LINKFLAGS, clBuildEvent);

// Evnet type: clBuildEvent
// Sent to the plugins to request to export the makefile
// for the project + configuration
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_PLUGIN_EXPORT_MAKEFILE, clBuildEvent);

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------

/**
 * Debug related events
 */
// sent when the debugger is about to start
// clientData is a pointer to a DebuggerStartupInfo structure
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_DEBUG_STARTING, clDebugEvent);

// sent right after the debugger started; program is not running yet
// clientData is a pointer to a DebuggerStartupInfo structure
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_DEBUG_STARTED, clDebugEvent);

// sent just before the debugger stops
// clientData is NULL
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_DEBUG_ENDING, clDebugEvent);

// sent after the debugger stopped
// clientData is NULL
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_DEBUG_ENDED, clDebugEvent);

// set when the editor gains or loses
// the control over the debugger
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_DEBUG_EDITOR_LOST_CONTROL, wxCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_DEBUG_EDITOR_GOT_CONTROL, wxCommandEvent);

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
// Code completion events
// Events sent here are of type 'clCodeCompletionEvent'
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------

// User hit Ctrl-Space in the editor
// let the plugins a chance to handle this
// event.
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CC_CODE_COMPLETE, clCodeCompletionEvent);

// User selected an entry from the code completion box. call event.Skip(false)
// if you wish to perform something unique instead of the default "insert selection into editor"
// The selected string can be retrieved by calling: event.GetWord()
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CCBOX_SELECTION_MADE, clCodeCompletionEvent);

// Code completion box is about to be shown. Let the plugins access the list before it is shown
// to the user. Call event.Skip(false) if you modify the list. The modified list should be set
// via the event.SetEntries();
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CCBOX_SHOWING, clCodeCompletionEvent);

/// User asked for "word completion" (non context code completion event)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CC_WORD_COMPLETE, clCodeCompletionEvent);

// A function calltip is requesed
// clientData is set to the client data set by the user
// the plugin returns the tooltip to the IDE using the:
// evt.SetTooltip(..) method
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CC_CODE_COMPLETE_FUNCTION_CALLTIP, clCodeCompletionEvent);

// The code completion box has been dismissed
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CC_CODE_COMPLETE_BOX_DISMISSED, clCodeCompletionEvent);

// User has requested to display the current files' outline
// Use m_mgr->GetActiveEditor() to get the active editor
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CC_SHOW_QUICK_OUTLINE, clCodeCompletionEvent);

// User is hovering a text, display the typeinfo
// IEditor* editor = dynamic_cast<IEditor*>(evt.GetEditor());
// Hover position is set in the evt.GetPosition()
// To pass a new tooltip, just call event.SetTooltip(...)
// CodeLite will display the tooltip if a non empty string is passed. Simple markup is allowed (<br> <hr> etc)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CC_TYPEINFO_TIP, clCodeCompletionEvent);

// Send a clCodeCompletionEvent
// Codelite is about to show the completion box for language keywords
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CC_CODE_COMPLETE_LANG_KEYWORD, clCodeCompletionEvent);

// Send clCodeCompletionEvent
// User clicked on hyper link inside the editor
// If the plugin wants to handle it, simple avoid calling event.Skip()
// Use event.GetString() to get the hyperlink text
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CC_JUMP_HYPER_LINK, clCodeCompletionEvent);

// User requested to perform a raw search for a symbol
// Use evt.GetWord() to get the searched string
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CC_FIND_SYMBOL, clCodeCompletionEvent);

// Event type: clCodeCompletionEvent
// Sent by codelite to generate documentation block for class or function
// after the user has typed "/**" followed by ENTER
// Use event.SetTooltip(...) to provide the doxygen block comment
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CC_GENERATE_DOXY_BLOCK, clCodeCompletionEvent);

// An update to the navigation bar is required
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CC_UPDATE_NAVBAR, clCodeCompletionEvent);

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
// Code completion events - END
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------

/////////////////////////////////////////////////
// Project management events
/////////////////////////////////////////////////

// User selected an option to create a new workspace
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CMD_CREATE_NEW_WORKSPACE, clCommandEvent);

// Event type: clCommandEvent
// A workspace file was selected by the user.
// use event.GetFileName() to get the file name
// If the plugin wishes to process this file, it should call
// event.Skip(false) on the event
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CMD_OPEN_WORKSPACE, clCommandEvent);

// Event type: clCommandEvent
// User requested to close the workspace.
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CMD_CLOSE_WORKSPACE, clCommandEvent);

// Event type: clCommandEvent
// This event is sent by codelite to the plugins to query whether a
// a custom workspace is opened (i.e. a worksapce which is completely managed
// by the plugin) this allows codelite to enable menu items which otherwise
// will be left disabled
// to return a true or false reply to codelite, use
// evt.SetAnswer( true / false )
// The workspace name should also returned in the evt.SetString(..)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CMD_IS_WORKSPACE_OPEN, clCommandEvent);

// User has requested a retagging for the workspace
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CMD_RETAG_WORKSPACE, wxCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CMD_RETAG_WORKSPACE_FULL, wxCommandEvent);

// codelite has requested a complete list of the workspace files.
// The result should be wxArrayString that contains a list of files
// in their ABSOLUTE path.
// The wxArrayString IS OWNED by codelite
// and the plugin should NOT attempt to delete it
// <code>
// if(I_want_to_handle_this_event) {
// 		wxArrayString *files = (wxArrayString *) event.GetClientData();
// 		<fill the files* array ...>
// } else {
// 		event.Skip();
// }
// </code>
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CMD_GET_WORKSPACE_FILES, wxCommandEvent);
// Same as the above event, however you should return
// a list of the current active project files
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CMD_GET_ACTIVE_PROJECT_FILES, wxCommandEvent);
// Same as the above event, however you should return
// a list of the current files' project files list
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CMD_GET_CURRENT_FILE_PROJECT_FILES, wxCommandEvent);

/////////////////////////////////////////////////
// Search events
/////////////////////////////////////////////////

// User requested to open the resource dialog
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CMD_OPEN_RESOURCE, wxCommandEvent);

// codelite is about to display the editor's context menu
// A plugin can override the default menu display by catching this event and
// handling it differently
// event.GetEventObject() holds a pointer to the editor triggered
// the menu
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CMD_EDITOR_CONTEXT_MENU, wxCommandEvent);

// Event type: clCommandEvent
// Sent by the Find IN Files dialog when the dialog is dismissed
// The information passed in the event contains the selected 'file mask'
// used for the search. Use event.GetString() to get it
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CMD_FIND_IN_FILES_DISMISSED, clCommandEvent);

// Event type: clCommandEvent
// Sent by CodeLite before showing the FindInFiles dialog
// A plugin can append search directories to the FindInFiles dialog by appending them
// to the event: clCommandEvent::GetStrings().Add(...)
// Paths added by these event are not persistent and will be removed next time the dialog is shown
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CMD_FIND_IN_FILES_SHOWING, clCommandEvent);

/////////////////////////////////////////////////////////

// Sent when the parser thread has completed a tagging request
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CMD_RETAG_COMPLETED, wxCommandEvent);

// send a wxNotifyEvent about page closing.
// This event can be vetoed
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_NOTIFY_PAGE_CLOSING, wxNotifyEvent);

// send an wxCommandEvent indicating that the mainbook page has changed.
// Unlike 'wxEVT_ACTIVE_EDITOR_CHANGED' this event is only sent when the
// 'page' is not of type IEditor
// use event.GetClientData() to get a pointer to the wxWindow*
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CMD_PAGE_CHANGED, wxCommandEvent);

// Triggers a workspace view tree rebuild (useful when user has modified the workspace / project structure and he want
// to reflect the changes in the tree view)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_REBUILD_WORKSPACE_TREE, wxCommandEvent);

// Sent when user has changed the active project
// Event type: clProjectSettingsEvent
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_ACTIVE_PROJECT_CHANGED, clProjectSettingsEvent);

// This event is fired by CodeLite before the find bar is shown
// The handler can pass a wxStyledTextCtrl class pointer to be associated with the 
// find bar by using the clFindEvent::SetCtrl() method
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_FINDBAR_ABOUT_TO_SHOW, clFindEvent);

// A plugin *must* send this event to ask the find bar to release any window associated with it
// This event should be used with  'wxEVT_FINDBAR_ABOUT_TO_SHOW'. If this event is not sent when the window
// is destroyed - it might result in a crash
// The window pointer is passed using event.SetCtrl()
// If the editor managed by the find-bar is the same as event.GetCtrl() -> the find-bar will un-refernce it
// but *IT DOES NOT FREE ITS MEMORY*
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_FINDBAR_RELEASE_EDITOR, clFindEvent);

// Instruct codelite to build a project only ( no deps )
// the project name is passed in the wxCommandEvent::GetString
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CMD_BUILD_PROJECT_ONLY, wxCommandEvent);
// Instruct codelite to clean a project only ( no deps )
// the project name is passed in the wxCommandEvent::GetString
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CMD_CLEAN_PROJECT_ONLY, wxCommandEvent);

// User changed the default theme color of codelite
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CL_THEME_CHANGED, wxCommandEvent);

// Sent from the CodeFormatter plugin when a file indentation was completed
// event.GetString() will return the name of the indented file
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CODEFORMATTER_INDENT_COMPLETED, wxCommandEvent);

// Sent from the CodeFormatter plugin when a file indentation is about to begin
// event.GetString() will return the name of the indented file
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CODEFORMATTER_INDENT_STARTING, wxCommandEvent);

// Sent when codelite's main frame gained the focus
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CODELITE_MAINFRAME_GOT_FOCUS, wxCommandEvent);

// User clicked on a project item in the treeview
// the event.GetString() contains the project name that was clicked
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_PROJECT_TREEITEM_CLICKED, wxCommandEvent);

// user has deleted all the breakpoints using the "Breakpoints" table
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CODELITE_ALL_BREAKPOINTS_DELETED, wxCommandEvent);

// User right click while holding the code-navigation modifier key (default to Shift)
// send a clCodeCompletionEvent
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CC_SHOW_QUICK_NAV_MENU, clCodeCompletionEvent);

// Event type: clCommandEvent
// User requested to reload the workspace
// simply avoid calling event.Skip() to indicate to codelite that this event
// was handled by a plugin
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CMD_RELOAD_WORKSPACE, clCommandEvent);

// Event type: clColourEvent
// Sent by codelite whenever it needs to colour a single tab
// avoid calling event.Skip() to notify codelite that the plugin wants to place a sepcial
// colour. The colours (*plural*) should be passed using the 'event.SetFgColour()' and 'event.SetBgColour()'
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_COLOUR_TAB, clColourEvent);

// Event type: clCommandEvent
// Sent by codelite before it starts building the "Workspsace View" tree view.
// User may provide a different image list by placing it inside the event
// event.SetClientData() member and by calling event.Skip(false)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_WORKSPACE_VIEW_BUILD_STARTING, clCommandEvent);

// Event type: clColourEvent
// Sent by codelite adding a project item to the file-view ("Workspace View"). Use event.GetString() to get the project
// name
// User may alter the following attributes:
// . Project item background colour
// . Project item text colour
// . Project icon
// The image id is returned as INT event.SetInt()
// Avoid calling event.Skip() to notify codelite that a new attributes were provided
// IMPORTANT:
// codelite will not check the validity of the icon index. Its up to the plugin to provide a proper
// index image. One can use the event wxEVT_WORKSPACE_VIEW_BUILD_STARTING to replace the default
// image list with its own list
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_WORKSPACE_VIEW_CUSTOMIZE_PROJECT, clColourEvent);

// Event type: clColourEvent
// Sent by codelite whenever it needs the pen colour for the tab area
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_GET_TAB_BORDER_COLOUR, clColourEvent);

// ---------------------------------------------------------------------
// Debugger events
// The following events are firing the clDebugEvent class
// If a plugin wishes to override codelite's default debugger (gdb)
// it simply needs to connect the event and avoid calling 'event.Skip();
//----------------------------------------------------------------------
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_DBG_UI_START,
    clDebugEvent); // Start. This event is fired when a debug session is starting. The plugin should also set the
                   // "feaures" field to indicate which features are available by the debugger
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_DBG_UI_CONTINUE, clDebugEvent);  // Continue
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_DBG_UI_STOP, clDebugEvent);      // Stop the debugger
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_DBG_UI_STEP_IN, clDebugEvent);   // Step into function
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_DBG_UI_STEP_I, clDebugEvent);   // Step into instruction
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_DBG_UI_STEP_OUT, clDebugEvent);  // Step out of current frame
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_DBG_UI_NEXT, clDebugEvent);      // Next line
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_DBG_UI_NEXT_INST, clDebugEvent); // Next instruction
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_DBG_UI_INTERRUPT, clDebugEvent); // Interrupt the debugger execution
wxDECLARE_EXPORTED_EVENT(
    WXDLLIMPEXP_CL, wxEVT_DBG_UI_SHOW_CURSOR, clDebugEvent); // Set the focus to the current debugger file/line
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_DBG_UI_RESTART, clDebugEvent); // Restart the debug session
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_DBG_IS_RUNNING, clDebugEvent); // Use evet.SetAnswer() method to reply
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_DBG_UI_TOGGLE_BREAKPOINT,
    clDebugEvent); // Toggle breakpoint. Use event.GetFileName() / event.GetInt() for the file:line
// Can CodeLite interact with the debugger? use event.SetAnswer(true);
// Note: by avoid calling Skip() CodeLite will assume that the plugin is controlling the debug session
// and it will use the event.IsAnswer() as the answer to the question to : CanDbgInteract()
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_DBG_CAN_INTERACT, clDebugEvent);
// Provide a tooltip for the expression under the caret. user event.GetString() to get the expression
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_DBG_EXPR_TOOLTIP, clDebugEvent);

// This event is sent by codelite to all plugins to determine whether a plugin is actually a debugger.
// A plugin should *always* call event.Skip() when handling this event. If the plugin is actually a debugger
// plugin, it should add itself like this: event.GetStrings().Add("<the-debugger-name")
// This string is later will be availe for codelite to display it in various dialogs (e.g. Quick Debug, project settings
// etc)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_DBG_IS_PLUGIN_DEBUGGER, clDebugEvent);

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_DBG_UI_QUICK_DEBUG,
    clDebugEvent); // User clicked on the 'Quick Debug' button. Event type is clDebugEvent
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_DBG_UI_CORE_FILE,
    clDebugEvent); // User selected to debug a core file. Event type is clDebugEvent
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_DBG_UI_ATTACH_TO_PROCESS,
    clDebugEvent); // Attach to process. Use clDebugEvent::GetInt() to get the process ID
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_DBG_UI_DELTE_ALL_BREAKPOINTS, clDebugEvent);   // Delete all breakpoints
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_DBG_UI_ENABLE_ALL_BREAKPOINTS, clDebugEvent);  // Enable all breakpoints
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_DBG_UI_DISABLE_ALL_BREAKPOINTS, clDebugEvent); // Disable all breakpoints

// -------------------Debugger events end------------------------------------------------
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CMD_OPEN_PROJ_SETTINGS,
    clCommandEvent); // clCommandEvent. Use event.GetString() to get the project name

// Workspace reload started
// event type: clCommandEvent
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_WORKSPACE_RELOAD_STARTED, clCommandEvent);

// Workspace reload is done
// event type: clCommandEvent
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_WORKSPACE_RELOAD_ENDED, clCommandEvent);

// event type: clNewProjectEvent
// Use this event to add new templates / categories to the wizard
// If you don't call event.Skip() codelite will assume that the plugin
// is replacing the dialog with its own and will do nothing
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_NEW_PROJECT_WIZARD_SHOWING, clNewProjectEvent);

// event type: clNewProjectEvent
// User clicked on the 'Finish' button of the new project wizard dialog
// call event.Skip( false ) if the plugin wants to handle the new project, otherwise
// call event.Skip( true ) for codelite to run the default behvior
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_NEW_PROJECT_WIZARD_FINISHED, clNewProjectEvent);

// --------------------------------------------------------------
// Compiler events
// --------------------------------------------------------------

// The compiler list was updated (e.g. a compiler was deleted, renamed etc)
// Event type: clCompilerEvent
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_COMPILER_LIST_UPDATED, clCompilerEvent);

// ++++--------------

// User clicked on the 'Save All Editors' button
// Editors will be saved, however, if a plugin has a tab which is not an editor
// save it as well
// event type: clCommandEvent
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_SAVE_ALL_EDITORS, clCommandEvent);

//----------------------------------------------------------------------
// Source Code Format events
//----------------------------------------------------------------------

// Format an input string
// Event type: clSourceFormatEvent
// Use clSourceFormatEvent.SetInputString() / GetFormattedString()
// to get and set the string to format
// Note that the plugin should be "smart" enough to indent the file
// by its type
// Call event.Skip(false) to mark this event as completed
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_FORMAT_STRING, clSourceFormatEvent);

// Format a file
// Event type: clSourceFormatEvent
// Use event.SetFileName() to pass the file name to be formatted
// Note that the plugin should be "smart" enough to indent the file
// by its type
// Call event.Skip(false) to mark this event as completed
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_FORMAT_FILE, clSourceFormatEvent);

//----------------------------------------------------------------------
// Context menu events
//----------------------------------------------------------------------

// codelite is about to show the context menu for an editor
// Event type: clContextMenuEvent. You can use event.GetEditor() to get the editor
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CONTEXT_MENU_EDITOR, clContextMenuEvent);

// codelite is about to show the context menu for the editor margin
// Event type: clContextMenuEvent. You can use event.GetEditor() to get the editor
// Calling event.Skip(false) will cancel the menu. A plugin may also
// alter the context menu
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CONTEXT_MENU_EDITOR_MARGIN, clContextMenuEvent);

// A context menu for a folder is about to be shown, you may alter the menu using event.GetMenu()
// Use event.GetPath() to get the path of the folder
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CONTEXT_MENU_FOLDER, clContextMenuEvent);

// A context menu for a virtual folder is about to be shown, you may alter the menu using event.GetMenu()
// Use event.GetPath() to get the virtual folder path
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CONTEXT_MENU_VIRTUAL_FOLDER, clContextMenuEvent);

// A context menu for a file is about to be shown, you may alter the menu using event.GetMenu()
// use event.GetStrings() to get the list of files selected (since there can be multiple items, we use
// event.GetStrings() a not event.GetFileName())
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CONTEXT_MENU_FILE, clContextMenuEvent);

// A context menu for a project is about to be shown, you may alter the menu using event.GetMenu()
// use event.GetFileName() to get the project file (if any). event.GetString() will return the project name
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CONTEXT_MENU_PROJECT, clContextMenuEvent);

// A context menu for a file is about to be shown, you may alter the menu using event.GetMenu()
// use event.GetFileName() to get the workspace file (if any). event.GetString() will return the workspace name
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CONTEXT_MENU_WORKSPACE, clContextMenuEvent);

// A context menu for the active tab label header is about to be shown. You can get the active page
// by calling clGetManager()->GetActivePage()
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CONTEXT_MENU_TAB_LABEL, clContextMenuEvent);

// User modified the colours and font of the IDE
// event type: clCommandEvent
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CMD_COLOURS_FONTS_UPDATED, clCommandEvent);

// File has been loaded into the IDE
// User: clCommandEvnet::GetFileName() to get the file name
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_FILE_LOADED, clCommandEvent);

// Sent when codelite is about to set the main frame's title.
// A plugin may alter the frame title by calling event.SetString(..)
// To get the current frame title, a plugin may call event.GetString()
// Event type: clCommandEvent
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CL_FRAME_TITLE, clCommandEvent);

// Event: clCommandEvent
// CodeLite is about to save a file. Use event.GetFilename() to get the file name
// This event can be vetoed by calling event.Veto()
// Note that this event is sent for editor files (i.e. it is not sent for internal files like project file being saved
// etc) only for editors (hence the name)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_BEFORE_EDITOR_SAVE, clCommandEvent);

// Editor has been modified. Use event.GetFilename() to get the file name of the editor
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_EDITOR_MODIFIED, clCommandEvent);

// Event: clCommandEvent
// Sent when clang code completion encountered an error
// use: event.GetString() to get the error message
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CLANG_CODE_COMPLETE_MESSAGE, clCommandEvent);

// Event: clCommandEvent
// CodeLite is going down. This event can not be vetoed
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_GOING_DOWN, clCommandEvent);

// Event: clCommandEvent
// Use GetString() to get the new project name
// Use GetOldName() to get the old project name
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_PROJ_RENAMED, clCommandEvent);

// An editor is being instantiated
// codelite will send a clCommandEvent with the editor pointer
// set in the event.GetEventObject() member
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_EDITOR_INITIALIZING, clCommandEvent);

// Event: clFileSystemEvent
// Sent when the file system was modified externally (typically, this event is sent
// after git pull, svn update etc)
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_FILE_SYSTEM_UPDATED, clFileSystemEvent);

// Event: clFileSystemEvent
// List of files were modified by the replace-in-files operation (this event will only include
// the files that were modified on the file system directly and have no open editor)
// Use clFileSystemEvent::GetStrings to get list of modified files
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_FILES_MODIFIED_REPLACE_IN_FILES, clFileSystemEvent);

// Event: clCommandEvent
// Sent when CodeLite requires to store the current workspace session
// Call event.Skip(false) to instruct codelite to skip the default session-save
// action (this is useful if the session is managed by an external plugin)
// A good example for this is the PHP plugin which manages its own session
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_SAVE_SESSION_NEEDED, clCommandEvent);

// Event: clCommandEvent
// User modified its environment variables
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_ENVIRONMENT_VARIABLES_MODIFIED, clCommandEvent);

// Event: clCommandEvent
// user dropped a folder on the window
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_DND_FOLDER_DROPPED, clCommandEvent);

// Event: clCommandEvent
// user dropped a file on the window
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_DND_FILE_DROPPED, clCommandEvent);

// Event: clCommandEvent
// a codelite restart is required
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_RESTART_CODELITE, clCommandEvent);

// Event: clCommandEvent
// Toggle workspace view tab. Use event.IsSelected() to test whether we should hide/show the tab
// In anycase, you should not destroy the window, just hide it
// The tab name is set in the event.GetString()
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_SHOW_WORKSPACE_TAB, clCommandEvent);

// Event: clCommandEvent
// Toggle output view tab. Use event.IsSelected() to test whether we should hide/show the tab
// In anycase, you should not destroy the window, just hide it
// The tab name is set in the event.GetString()
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_SHOW_OUTPUT_TAB, clCommandEvent);

// Event: clCommandEvent
// Sent by CodeLite when the active editor is _not_ an IEditor instance
// but something else (probably provided by the plugins)
// and CodeLite has no way of telling if the "Save" icon should be enabled or not
// In case the window in the question is modified and owned by the plugin, the plugin should:
//     1. Call event.SetAnswer(true)
//     2. Call event.Skip(false);
//
// In case the window in the question is not owned by the plugin, the plugin should:
//     1. Call event.Skip();
//
// Use: event.GetClientData() to get the pointer to the window page
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_PAGE_MODIFIED_UPDATE_UI, clCommandEvent);

// Send a clEditorConfig event. This event is sent whenever an editor requires
// to update its properties (tabs vs spaces, EOL style etc)
// A plugin may capture the event and send back EditorConfig data by doing the following:
// Call: 
// 1. event.Skip(false)
// 2. Set the data using event.SetEditorConfig(...) method
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_EDITOR_CONFIG_LOADING, clEditorConfigEvent);

// Sent when PHP settings have been saved to the file system
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_PHP_SETTINGS_CHANGED, clCommandEvent);

// Goto Anything item was selected
// Use: clCommandEvent::GetString to get the selected item
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_GOTO_ANYTHING_SELECTED, clGotoEvent);

// Goto Anything dialog is about to be shown
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_GOTO_ANYTHING_SHOWING, clGotoEvent);

// This event is sent to allow plugins and other modules to sort the entries before
// they are displayed to the user
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_GOTO_ANYTHING_SORT_NEEDED, clGotoEvent);

// Build configuration for the workspace has changed
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_WORKSPACE_BUILD_CONFIG_CHANGED, clCommandEvent);

// The navigation bar is about show the scope drop down menu
// Build the menu using clContextMenuEvent::GetMenu()
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_NAVBAR_SCOPE_MENU_SHOWING, clContextMenuEvent);

// User selected an entry from the scope drop down menu. GetString() contains the selected string
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_NAVBAR_SCOPE_MENU_SELECTION_MADE, clCommandEvent);

// A marker was added/removed from an editor. clCommandEvent::GetFileName() will return the relevant file name
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_MARKER_CHANGED, clCommandEvent);


#endif // CODELITE_EVENTS_H
