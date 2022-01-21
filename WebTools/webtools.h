//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : webtools.h
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

#ifndef __WebTools__
#define __WebTools__

#include "CSSCodeCompletion.h"
#include "XMLCodeCompletion.h"
#include "cl_command_event.h"
#include "ieditor.h"
#include "plugin.h"

#include <wx/timer.h>

class NodeDebuggerPane;
class NodeJSWorkspaceView;
class WebTools : public IPlugin
{
    friend class JavaScriptSyntaxColourThread;

    XMLCodeCompletion::Ptr_t m_xmlCodeComplete;
    CSSCodeCompletion::Ptr_t m_cssCodeComplete;
    wxTimer* m_timer;

    /// Node.js
    bool m_clangOldFlag;
    NodeDebuggerPane* m_nodejsCliDebuggerPane = nullptr;
    wxString m_savePerspective;

protected:
    void OnWorkspaceClosed(clWorkspaceEvent& event);
    void OnWorkspaceLoaded(clWorkspaceEvent& event);
    void OnEditorChanged(wxCommandEvent& event);
    void DoRefreshColours(const wxString& filename);
    void OnFileLoaded(clCommandEvent& event);
    void OnEditorContextMenu(clContextMenuEvent& event);
    void OnFileSaved(clCommandEvent& event);
    void OnThemeChanged(wxCommandEvent& event);
    void OnCodeComplete(clCodeCompletionEvent& event);
    void OnSettings(wxCommandEvent& event);
    void OnTimer(wxTimerEvent& event);
    void OnCommentLine(wxCommandEvent& e);
    void OnCommentSelection(wxCommandEvent& e);
    void OnNodeJSCliDebuggerStarted(clDebugEvent& event);
    void OnNodeJSDebuggerStopped(clDebugEvent& event);
    void OnIsDebugger(clDebugEvent& event);
    void OnNodeCommandCompleted(clProcessEvent& event);

public:
    bool IsJavaScriptFile(const wxString& filename);
    bool IsJavaScriptFile(const wxFileName& filename);
    bool IsJavaScriptFile(IEditor* editor);
    bool IsHTMLFile(IEditor* editor);
    bool IsCSSFile(IEditor* editor);
    bool InsideJSComment(IEditor* editor);
    bool InsideJSString(IEditor* editor);
    void EnsureAuiPaneIsVisible(const wxString& paneName, bool update);

public:
    WebTools(IManager* manager);
    ~WebTools();

    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    virtual void CreateToolBar(clToolBar* toolbar);
    virtual void CreatePluginMenu(wxMenu* pluginsMenu);
    virtual void UnPlug();
};

#endif // WebTools
