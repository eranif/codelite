//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : plugin.h
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
#ifndef PLUGIN_H
#define PLUGIN_H

// Windows headers define it
#ifdef __WXMSW__
#ifdef Yield
#undef Yield
#endif
#endif

#include <wx/app.h>
#include "imanager.h"
#include "wx/toolbar.h"
#include <wx/pen.h>
#include "cl_aui_tb_are.h"
#include "cl_defs.h"
#if USE_AUI_TOOLBAR
#include <wx/aui/auibar.h>
#endif
#include "wx/event.h"
#include "wx/notebook.h"
#include "plugindata.h"
#include "plugin_version.h"

#ifdef _WIN32
#define STDCALL __stdcall
#define EXPORT  __declspec(dllexport)
#else
#define STDCALL
#define EXPORT
#endif

class IManager;

/**
 * Possible popup menu
 */
enum MenuType {
    MenuTypeFileExplorer = 0,
    MenuTypeFileView_Workspace,
    MenuTypeFileView_Project,
    MenuTypeFileView_Folder,
    MenuTypeFileView_File,
    MenuTypeEditor
};

// ------------------------------------------------------------------------
// 
// Plugins events
// 
//  We use #define and not enum because to make sure that the event numbers
//  will not change when we move an event up or down in this file...
// ------------------------------------------------------------------------

//clientData is NULL
#define wxEVT_INIT_DONE 3450

// wxCommandEvent::GetString() will return the node name modified
#define wxEVT_EDITOR_CONFIG_CHANGED 3451

// wxCommandEvent::GetString() will return the workspace fullpath
#define wxEVT_WORKSPACE_LOADED  3452
    
// The build configuration was changed
// use event.GetString() to get the selected configuration name
#define wxEVT_WORKSPACE_CONFIG_CHANGED 3453
    
//clientData is NULL
#define wxEVT_WORKSPACE_CLOSED 3454

//clientData is NULL
#define wxEVT_FILE_VIEW_INIT_DONE 3455

//clientData is NULL
#define wxEVT_FILE_VIEW_REFRESHED 3456

//clientData is NULL
#define wxEVT_FILE_EXP_INIT_DONE 3457

//clientData is NULL
#define wxEVT_FILE_EXP_REFRESHED 3458

//clientData is the wxTreeItemId*
#define wxEVT_CMD_FILE_EXP_ITEM_EXPANDING 3459

// an attempt to open a file using double click / ENTER was made
// on an item in the 'File Explorer' OR from the 'Workspace' tree
// clientData is the full path of the file (wxString*)
#define wxEVT_TREE_ITEM_FILE_ACTIVATED 3460

// Event type: clCommandEvent
// Use: clCommandEvent::GetStrings() to get the list of files added
#define wxEVT_PROJ_FILE_ADDED 3461

//clientData is list of files which have been removed (wxArrayString*)
#define wxEVT_PROJ_FILE_REMOVED 3462

//clientData is the project name (wxString*)
#define wxEVT_PROJ_REMOVED 3463

//clientData is the project name (wxString*)
#define wxEVT_PROJ_ADDED 3464

// the following 2 events are used as "transaction"
// the first event indicates that any "wxEVT_FILE_SAVED" event sent from this point
// is due to build process which is about to starte
// the later event, indicates the end of that transaction
#define wxEVT_FILE_SAVE_BY_BUILD_END 3465
#define wxEVT_FILE_SAVE_BY_BUILD_START 3466

// Sent when a file has been saved.
// use event.GetString() to get the file name
#define wxEVT_FILE_SAVED 3467

// clientData is list of files which have been retagged (std::vector<wxFileName>*)
#define wxEVT_FILE_RETAGGED 3468

// clientData is wxArrayString*: Item(0) = oldName
//                               Item(1) = newName
#define wxEVT_FILE_RENAMED 3469

//clientData is active editor (IEditor*)
#define wxEVT_ACTIVE_EDITOR_CHANGED 3470

//clientData is closing editor (IEditor*)
#define wxEVT_EDITOR_CLOSING 3471

//clientData is NULL
#define wxEVT_ALL_EDITORS_CLOSING 3472

//clientData is NULL
#define wxEVT_ALL_EDITORS_CLOSED 3473

// This event is sent when the user clicks inside an editor
// this event can not be Veto()
// clientData is NULL. You may query the clicked editor by calling to
// IManager::GetActiveEditor()
#define wxEVT_EDITOR_CLICKED 3474

// User dismissed the Editor's settings dialog with
// Apply or OK (Settings | Editor)
// clientData is NULL
#define wxEVT_EDITOR_SETTINGS_CHANGED 3475

// This event is sent from plugins to the application to tell it to reload
// any open files (and re-tag them as well)
#define wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED 3476

// Same as wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED
// just without prompting the user
// this event only reload code files without
// any reload to the workspace / project
#define wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED_NOPROMPT 3477

// Sent by the project settings dialogs to indicate that
// the project configurations are saved
// clientData is the project name (wxString*)
// event.GetString() returns the selected configuration
#define wxEVT_CMD_PROJ_SETTINGS_SAVED 3478

// A user requested to execute the active project
#define wxEVT_CMD_EXECUTE_ACTIVE_PROJECT 3479
    
// A user requested to stop the previously executed program
#define wxEVT_CMD_STOP_EXECUTED_PROGRAM 3480
    
// codelite sends this event to query plugins incase there is a program running
// use evet.SetInt(1) to indicate that the plugin has launched an executable
// it is mainly used for displaying the 'Stop' button in the toolbar as active/disabled
#define wxEVT_CMD_IS_PROGRAM_RUNNING 3481

// ----------------------------------------------------------------------
// Build Events
// ----------------------------------------------------------------------

// Event type: clBuildEvent
// Notify that a build has been started
#define wxEVT_BUILD_STARTED 3482

// Event type: clBuildEvent
// Notify that a build has been ended
#define wxEVT_BUILD_ENDED 3483

// Event type: clBuildEvent
// sent by the application to the plugins to indicate that a
// build process is about to start. by handling this event
// and by avoid calling event.Skip() codelite will NOT start
// the build process
#define wxEVT_BUILD_STARTING 3484

// Event type: clBuildEvent
// return the project clean command
// set the new clean command using: event.SetCommand( "new-clean-command" );
// avoid calling event.Skip() to indicate that a new command was provided
#define wxEVT_GET_PROJECT_CLEAN_CMD 3485

// Event type: clBuildEvent
// return the project build command
// set the new build command using: event.SetCommand( "new-build-command" );
// avoid calling event.Skip() to indicate that a new command was provided
#define wxEVT_GET_PROJECT_BUILD_CMD 3486

// Event type: clBuildEvent
// the below two events are sent by the application to the plugins to query whether a
// given project and build configuration are handled by the plugin.
// the first event is whether the makefile file is generated by the plugin
// and the second is to know whether the build process is also being
// handled by the plugin.
// the return answer is done by simply avoid calling event.Skip() (which will result in ProcessEvent() == true)
#define wxEVT_GET_IS_PLUGIN_MAKEFILE 3487
#define wxEVT_GET_IS_PLUGIN_BUILD 3488

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
#define wxEVT_GET_ADDITIONAL_COMPILEFLAGS 3495
#define wxEVT_GET_ADDITIONAL_LINKFLAGS 3496

// Evnet type: clBuildEvent
// Sent to the plugins to request to export the makefile
// for the project + configuration
#define wxEVT_PLUGIN_EXPORT_MAKEFILE 3497

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------

/**
 * Debug related events
 */
// sent when the debugger is about to start
// clientData is a pointer to a DebuggerStartupInfo structure
#define wxEVT_DEBUG_STARTING 3489

// sent right after the debugger started; program is not running yet
// clientData is a pointer to a DebuggerStartupInfo structure
#define wxEVT_DEBUG_STARTED 3490

// sent just before the debugger stops
// clientData is NULL
#define wxEVT_DEBUG_ENDING 3491

// sent after the debugger stopped
// clientData is NULL
#define wxEVT_DEBUG_ENDED 3492

// set when the editor gains or loses
// the control over the debugger
#define wxEVT_DEBUG_EDITOR_LOST_CONTROL 3493
#define wxEVT_DEBUG_EDITOR_GOT_CONTROL 3494

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
// Code completion events
// Events sent here are of type 'clCodeCompletionEvent'
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------

// User hit Ctrl-Space in the editor
// let the plugins a chance to handle this
// event.
#define wxEVT_CC_CODE_COMPLETE 3498
    
//clientData is the selected word (wxString*)
#define wxEVT_CCBOX_SELECTION_MADE 3499


// A tooltip is requested for the selected entry in the completion box
// clientData is set to the client data set by the user
// the plugin returns the tooltip to the IDE using the:
// evt.SetTooltip(..) method
// Use evt.GetTagEntry() to retrieve the tag 
#define wxEVT_CC_CODE_COMPLETE_TAG_COMMENT 3500

// A function calltip is requesed
// clientData is set to the client data set by the user
// the plugin returns the tooltip to the IDE using the:
// evt.SetTooltip(..) method
#define wxEVT_CC_CODE_COMPLETE_FUNCTION_CALLTIP 3501

// The code completion box has been dismissed
#define wxEVT_CC_CODE_COMPLETE_BOX_DISMISSED 3502

// User has requested to display the current files' outline
// Use m_mgr->GetActiveEditor() to get the active editor
#define wxEVT_CC_SHOW_QUICK_OUTLINE 3503

// User is hovering a text, display the typeinfo
// IEditor* editor = dynamic_cast<IEditor*>(evt.GetEditor());
// Hover position is set in the evt.GetPosition()
#define wxEVT_CC_TYPEINFO_TIP 3504
    
// Send a clCodeCompletionEvent
// Codelite is about to show the completion box for language keywords
#define wxEVT_CC_CODE_COMPLETE_LANG_KEYWORD 3505

// User requested to perform a raw search for a symbol
// Use evt.GetWord() to get the searched string
#define wxEVT_CC_FIND_SYMBOL 3518

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
// Code completion events - END
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------

/////////////////////////////////////////////////
// Project management events
/////////////////////////////////////////////////

// User selected an option to create a new workspace
#define wxEVT_CMD_CREATE_NEW_WORKSPACE 3506

// User selected an option to create a new project
#define wxEVT_CMD_CREATE_NEW_PROJECT 3507
    
// a _plugin_ sends this event to codelite to request adding 
// a workspace to the recetly used list
// The workspace path is sent in the evt.SetString()/GetString() 
#define wxEVT_CODELITE_ADD_WORKSPACE_TO_RECENT_LIST 3508

// Event type: clCommandEvent
// User requested to open a workspace
// By default codelite will open a dialog to open a workspace with the '.workspace' suffix.
// If a plugin wishes to offer a user a dialog with other extension (for example: .myworkspace)
// it should handle this event and open the dialog itself and then based on the selected
// file extension it should decide what to do: pass the selected file to codelite (assuming user selected the
// standard .workspace file, or handle it by itself).
// to pass the selection to codelite, simply set it in the evt.SetFileName(...) function
// If the plugin wishes to handle the file by itself, it should avoid calling evt.Skip()
#define wxEVT_CMD_OPEN_WORKSPACE 3509

// Event type: clCommandEvent
// User requested to close the workspace.
#define wxEVT_CMD_CLOSE_WORKSPACE 3510

// Event type: clCommandEvent
// This event is sent by codelite to the plugins to query whether a
// a custom workspace is opened (i.e. a worksapce which is completely managed
// by the plugin) this allows codelite to enable menu items which otherwise
// will be left disabled
// to return a true or false reply to codelite, use
// evt.SetAnswer( true / false )
// The workspace name should also returned in the evt.SetString(..) 
#define wxEVT_CMD_IS_WORKSPACE_OPEN 3511

// User has requested a retagging for the workspace
#define wxEVT_CMD_RETAG_WORKSPACE 3512
#define wxEVT_CMD_RETAG_WORKSPACE_FULL 3513 

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
#define wxEVT_CMD_GET_WORKSPACE_FILES 3514
// Same as the above event, however you should return
// a list of the current active project files
#define wxEVT_CMD_GET_ACTIVE_PROJECT_FILES 3515
// Same as the above event, however you should return
// a list of the current files' project files list
#define wxEVT_CMD_GET_CURRENT_FILE_PROJECT_FILES 3516

/////////////////////////////////////////////////
// Search events
/////////////////////////////////////////////////

// User requested to open the resource dialog
#define wxEVT_CMD_OPEN_RESOURCE 3517

// codelite is about to display the editor's context menu
// A plugin can override the default menu display by catching this event and
// handling it differently
// event.GetEventObject() holds a pointer to the editor triggered
// the menu
#define wxEVT_CMD_EDITOR_CONTEXT_MENU 3519

// codelite is about to display the editor's *left margin* context menu
// A plugin can override the default menu display by catching this event and
// handling it differently
// event.GetEventObject() holds a pointer to the editor triggered
// the menu
#define wxEVT_CMD_EDITOR_MARGIN_CONTEXT_MENU 3520

// codelite is requesting for the find-in-files file masking.
// the format should be:
// *.a;*.b
// and should be placed at:
// event.SetString("*.a;*.b");
// The plugin can also control what codelite will do with its masking by
// setting the event.SetInt() with the following values (the plugin can use
// bitwise OR):
// 0x00000001 -> append the plugins' masking to the default masking
// 0x00000002 -> prepend the plugins' masking to default masking
// 0x00000004 -> replace default masking with the one provided by the plugin
// 0x00000008 -> select the plugin's masking by default
#define wxEVT_CMD_GET_FIND_IN_FILES_MASK 3521

////////////////////////////////////////////////////////
// Debugger events
////////////////////////////////////////////////////////

// User is hovering a text, display the debugger tip for this variable
// User m_mgr->GetActiveEditor() to get the current editor Or
// IEditor* editor = dynamic_cast<IEditor*>(evt.GetEventObject());
// Hover position is set in the evt.GetInt()
// NOT IMPLEMENTED YET
#define wxEVT_CMD_DEBUGGER_TIP 3522

// codelite needs to know wether the user can interact with the
// debugger
#define wxEVT_CMD_DEBUGGER_CAN_INTERACT 3523

/////////////////////////////////////////////////////////

// Send dwell end event to the plugins to notify them
// to dismiss any displayed tooltip
// event.GetEventObject() holds a pointer to the editor
#define wxEVT_CMD_EDITOR_TIP_DWELL_END 3524

// Sent when the parser thread has completed a tagging request
#define wxEVT_CMD_RETAG_COMPLETED 3525

// send a wxNotifyEvent about page closing.
// This event can be vetoed
#define wxEVT_NOTIFY_PAGE_CLOSING 3526

// send an wxCommandEvent indicating that the mainbook page has changed.
// Unlike 'wxEVT_ACTIVE_EDITOR_CHANGED' this event is only sent when the
// 'page' is not of type IEditor
// use event.GetClientData() to get a pointer to the wxWindow*
#define wxEVT_CMD_PAGE_CHANGED 3527

// Triggers a workspace view tree rebuild (useful when user has modified the workspace / project structure and he want 
// to reflect the changes in the tree view)
#define wxEVT_REBUILD_WORKSPACE_TREE 3529
    
// Sent when user has changed the active project
// call event.GetString() to get the name of the project
#define wxEVT_ACTIVE_PROJECT_CHANGED 3530
    
// This event is fired by codelite when the find-bar is requested to be shown
// the default for the find bar is not to be shown if it has no window associated with it
// The Window is passed using the event.GetClientData()
// The Window *MUST* be of type wxStyledTextCtrl
#define wxEVT_FINDBAR_ABOUT_TO_SHOW 3531
    
// A plugin *must* send this event to ask the find bar to release any window associated with it
// This event should be used with  'wxEVT_FINDBAR_ABOUT_TO_SHOW'. If this event is not sent when the window
// is destroyed - it might result in a crash
// The window pointer is passed using event.GetClientData()
// If the editor managed by the find-bar is the same as event.GetClientData() -> the find-bar will un-refernce it 
// but *IT DOES NOT FREE ITS MEMORY*
#define wxEVT_FINDBAR_RELEASE_EDITOR 3532
    
// Instruct codelite to build a project only ( no deps )
// the project name is passed in the wxCommandEvent::GetString
#define wxEVT_CMD_BUILD_PROJECT_ONLY 3533
// Instruct codelite to clean a project only ( no deps )
// the project name is passed in the wxCommandEvent::GetString
#define wxEVT_CMD_CLEAN_PROJECT_ONLY 3534
    
// User changed the default theme color of codelite
#define wxEVT_CL_THEME_CHANGED 3535
    
// Sent from the CodeFormatter plugin when a file indentation was completed
// event.GetString() will return the name of the indented file
#define wxEVT_CODEFORMATTER_INDENT_COMPLETED 3536
    
// Sent from the CodeFormatter plugin when a file indentation is about to begin
// event.GetString() will return the name of the indented file
#define wxEVT_CODEFORMATTER_INDENT_STARTING 3537
    
// Sent when codelite's main frame gained the focus
#define wxEVT_CODELITE_MAINFRAME_GOT_FOCUS 3538
    
// User clicked on a project item in the treeview
// the event.GetString() contains the project name that was clicked
#define wxEVT_PROJECT_TREEITEM_CLICKED 3539
    
// user has deleted all the breakpoints using the "Breakpoints" table
#define wxEVT_CODELITE_ALL_BREAKPOINTS_DELETED 3540

// User right click while holding the code-navigation modifier key (default to Shift)
// send a clCodeCompletionEvent
#define wxEVT_CC_SHOW_QUICK_NAV_MENU 3541

// Event type: clCommandEvent
// User requested to reload the workspace
// simply avoid calling event.Skip() to indicate to codelite that this event 
// was handled by a plugin
#define wxEVT_CMD_RELOAD_WORKSPACE 3542

// Event type: clColourEvent
// Sent by codelite whenever it needs to colour a single tab
// avoid calling event.Skip() to notify codelite that the plugin wants to place a sepcial
// colour. The colours (*plural*) should be passed using the 'event.SetFgColour()' and 'event.SetBgColour()'
#define wxEVT_COLOUR_TAB 3543

// Event type: clCommandEvent
// Sent by codelite before it starts building the "Workspsace View" tree view.
// User may provide a different image list by placing it inside the event
// event.SetClientData() member and avoid calling event.Skip() (so codelite will know that
// a new wxImageList is provided)
#define wxEVT_WORKSPACE_VIEW_BUILD_STARTING 3544

// Event type: clColourEvent
// Sent by codelite adding a project item to the file-view ("Workspace View"). Use event.GetString() to get the project name
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
#define wxEVT_WORKSPACE_VIEW_CUSTOMIZE_PROJECT 3545

// Event type: clColourEvent
// Sent by codelite whenever it needs the pen colour for the tab area
#define wxEVT_GET_TAB_BORDER_COLOUR 3546

// ---------------------------------------------------------------------
// Debugger events
// The following events are firing the clDebugEvent class
// If a plugin wishes to override codelite's default debugger (gdb)
// it simply needs to connect the event and avoid calling 'event.Skip();
//----------------------------------------------------------------------
#define wxEVT_DBG_UI_START_OR_CONT     3547 // Start the debugger or continue
#define wxEVT_DBG_UI_STOP              3549 // Stop the debugger
#define wxEVT_DBG_UI_STEP_IN           3550 // Step into function
#define wxEVT_DBG_UI_STEP_OUT          3551 // Step out of current frame
#define wxEVT_DBG_UI_NEXT              3552 // Next line
#define wxEVT_DBG_UI_NEXT_INST         3553 // Next instruction
#define wxEVT_DBG_UI_INTERRUPT         3554 // Interrupt the debugger execution
#define wxEVT_DBG_UI_SHOW_CURSOR       3555 // Set the focus to the current debugger file/line
#define wxEVT_DBG_UI_RESTART           3556 // Restart the debug session
#define wxEVT_DBG_IS_RUNNING           3557 // Use evet.SetAnswer() method to reply
#define wxEVT_DBG_UI_TOGGLE_BREAKPOINT 3558 // Toggle breakpoint. Use event.GetFileName() / event.GetInt() for the file:line

// -------------------Debugger events end------------------------------------------------
#define wxEVT_CMD_OPEN_PROJ_SETTINGS   3580 // clCommandEvent. Use event.GetString() to get the project name

//------------------------------------------------------------------
//each plugin must implement this interface
//------------------------------------------------------------------
/**
 * @class IPlugin
 * @author Eran
 * @date 05/07/08
 * @file plugin.h
 * @brief The interface that defines a plugin for CodeLite. each plugin must implement the pure virtual methods of this
 * interface. The plugin gains access to CodeLite data by using the m_mgr member
 * @sa  IManager
 */
class IPlugin : public wxEvtHandler
{
protected:
    wxString m_shortName;
    wxString m_longName;
    IManager *m_mgr;

public:
    IPlugin(IManager *manager) : m_mgr(manager) {}
    virtual ~IPlugin() {}

    //-----------------------------------------------
    //The interface
    //-----------------------------------------------
    /**
     * @brief return the plugin's short name
     * @return
     */
    virtual const wxString &GetShortName() const {
        return m_shortName;
    }
    /**
     * @brief return the plugin's long and more descriptive name
     * @return
     */
    virtual const wxString &GetLongName() const {
        return m_longName;
    }

    /**
     * When LiteEditor loads all the plugins, this function is called. If toolbar
     * is provided by the plugin, the Plugin Manager will palce it in the appropriate
     * place on the toolbar pane.
     * \param parent toolbar parent, usually this is the main frame
     * \return toolbar or NULL
     */
    virtual clToolBar *CreateToolBar(wxWindow *parent) = 0;

    /**
     * Every plugin can place a sub menu in the 'Plugins' Menu at the menu bar
     * \param pluginsMenu
     */
    virtual void CreatePluginMenu(wxMenu *pluginsMenu) = 0;

    /**
     * \brief Call the plugin "shutdown" function
     */
    virtual void UnPlug() = 0;

    /**
     * Override this method to allow the plugin to
     * hook the popup menu by adding its entries.
     * \param menu menu to hook
     * \param type menu type
     * \sa  MenuType
     */
    virtual void HookPopupMenu(wxMenu *menu, MenuType type) {
        wxUnusedVar(type);
        wxUnusedVar(menu);
    };

    /**
     * @brief load image file from /path/to/install/plugins/resources/
     * @param name file name (name+extension)
     * @return Bitmap of wxNullBitmap if no match was found
     */
    virtual wxBitmap LoadBitmapFile(const wxString &name, wxBitmapType type = wxBITMAP_TYPE_PNG) {
        wxBitmap bmp;
#ifdef __WXGTK__
        wxString pluginsDir(PLUGINS_DIR, wxConvUTF8);
#else
        wxString pluginsDir(m_mgr->GetInstallDirectory() + wxT( "/plugins" ));
#endif
        wxString basePath(pluginsDir + wxT("/resources/"));

        bmp.LoadFile(basePath + name, type);
        if (bmp.IsOk()) {
            return bmp;
        }
        return wxNullBitmap;
    }


    /**
     * @brief allow the plugins to hook a tab in the project settings
     * @param notebook the parent
     * @param configName the associated configuration name
     */
    virtual void HookProjectSettingsTab(wxBookCtrlBase *notebook, const wxString &projectName, const wxString &configName) {
        wxUnusedVar( notebook );
        wxUnusedVar( projectName );
        wxUnusedVar( configName );
    }

    /**
     * @brief Unhook any tab from the project settings dialog
     * @param notebook the parent
     * @param configName the associated configuration name
     */
    virtual void UnHookProjectSettingsTab(wxBookCtrlBase *notebook, const wxString &projectName, const wxString &configName) {
        wxUnusedVar( notebook );
        wxUnusedVar( projectName );
        wxUnusedVar( configName );
    }
};

#define CHECK_CL_SHUTDOWN(){\
        if(m_mgr->IsShutdownInProgress())\
            return;\
    }

//Every dll must contain at least this function
typedef IPlugin*    (*GET_PLUGIN_CREATE_FUNC)            (IManager*);
typedef PluginInfo  (*GET_PLUGIN_INFO_FUNC)              ();
typedef int         (*GET_PLUGIN_INTERFACE_VERSION_FUNC) ();
#endif //PLUGIN_H
