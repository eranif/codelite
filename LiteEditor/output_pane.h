//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : output_pane.h
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
#ifndef OUTPUT_PANE_H
#define OUTPUT_PANE_H

#include <wx/panel.h>

#include "shelltab.h"
#include "cl_command_event.h"

class FindResultsTab;
#if CL_USE_NEW_BUILD_TAB
class NewBuildTab;
#else
class BuildTab;
#endif
class ReplaceInFilesPanel;
class ShellTab;
class TaskPanel;
class OutputPaneBook;
class FindUsageTab;

/**
 * \ingroup LiteEditor
 * This class represents the default bottom pane in the editor
 *
 * \date 04-14-2007
 *
 * \author Eran
 *
 * \par license
 * This code is absolutely free to use and modify. The code is provided "as is" with
 * no expressed or implied warranty. The author accepts no liability if it causes
 * any damage to your computer, causes your pet to fall ill, increases baldness
 * or makes your car start emitting strange noises when you start it up.
 * This code has no bugs, just undocumented features!
 *
 */
class OutputPane : public wxPanel
{
public:
    static const wxString FIND_IN_FILES_WIN;
    static const wxString BUILD_WIN;
    static const wxString OUTPUT_WIN;
    static const wxString OUTPUT_DEBUG;
    static const wxString REPLACE_IN_FILES;
    static const wxString TASKS;
    static const wxString TRACE_TAB;
    static const wxString SHOW_USAGE;

private:
    wxString m_caption;
    wxLog* m_logTargetOld;

    Notebook* m_book;
    FindResultsTab* m_findResultsTab;
    ReplaceInFilesPanel* m_replaceResultsTab;

#if CL_USE_NEW_BUILD_TAB
    NewBuildTab* m_buildWin;
#else
    BuildTab* m_buildWin;
#endif

    ShellTab* m_outputWind;
    TaskPanel* m_taskPanel;
    FindUsageTab* m_showUsageTab;
    bool m_buildInProgress;

protected:
    void CreateGUIControls();
    void OnEditorFocus(wxCommandEvent& e);
    void OnBuildStarted(clBuildEvent& e);
    void OnBuildEnded(clBuildEvent& e);

public:
    /**
     * Constructor
     * \param parent parent window for this pane
     * \param caption the caption
     */
    OutputPane(wxWindow* parent, const wxString& caption);

    /**
     * Destructor
     */
    virtual ~OutputPane();

    Notebook* GetNotebook() { return m_book; }
    const wxString& GetCaption() const { return m_caption; }

    FindResultsTab* GetFindResultsTab() { return m_findResultsTab; }
    ReplaceInFilesPanel* GetReplaceResultsTab() { return m_replaceResultsTab; }
    NewBuildTab* GetBuildTab() { return m_buildWin; }
    ShellTab* GetOutputWindow() { return m_outputWind; }
    FindUsageTab* GetShowUsageTab() { return m_showUsageTab; }
};

#endif // OUTPUT_PANE_H
