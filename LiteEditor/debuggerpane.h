//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : debuggerpane.h
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
#ifndef DEBUGGERPANE_H
#define DEBUGGERPANE_H

#include "wx/panel.h"
#include "Notebook.h"
#include <wx/imaglist.h>
#include "cl_config.h"

class DebuggerCallstackView;
class LocalsTable;
class WatchesTable;
class BreakpointDlg;
class ThreadListPanel;
class MemoryView;
class wxAuiManager;
class DebuggerAsciiViewer;
class DebugTab;
class DebuggerDisassemblyTab;

class DebuggerPaneConfig : public clConfigItem
{
    size_t m_windows;

public:
    enum eDebuggerWindows {
        None = 0x00000000,
        Locals = 0x00000001,
        Watches = 0x00000002,
        Callstack = 0x00000004,
        Memory = 0x00000008,
        AsciiViewer = 0x00000010,
        Breakpoints = 0x00000020,
        Output = 0x00000040,
        Threads = 0x00000080,
        Disassemble = 0x00000100,
        All = 0xFFFFFFFF,
    };

public:
    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON() const;

    void SetWindows(size_t windows) { this->m_windows = windows; }
    size_t GetWindows() const { return m_windows; }

    /**
     * @brief convert debugger window flag id to its visual name
     */
    wxString WindowName(eDebuggerWindows flag) const;

    void ShowDebuggerWindow(eDebuggerWindows win, bool show)
    {
        if(show) {
            m_windows |= win;
        } else {
            m_windows &= ~win;
        }
    }

    bool IsDebuggerWindowShown(eDebuggerWindows win) const { return m_windows & win; }

public:
    DebuggerPaneConfig();
    virtual ~DebuggerPaneConfig();
};

class DebuggerPane : public wxPanel
{
public:
    static const wxString LOCALS;
    static const wxString WATCHES;
    static const wxString FRAMES;
    static const wxString BREAKPOINTS;
    static const wxString THREADS;
    static const wxString MEMORY;
    static const wxString ASCII_VIEWER;
    static const wxString DEBUGGER_OUTPUT;
    static const wxString DISASSEMBLY;

private:
    Notebook* m_book;
    LocalsTable* m_localsTable;
    wxString m_caption;
    WatchesTable* m_watchesTable;
    DebuggerCallstackView* m_frameList;
    BreakpointDlg* m_breakpoints;
    ThreadListPanel* m_threads;
    MemoryView* m_memory;
    DebuggerAsciiViewer* m_asciiViewer;
    DebuggerDisassemblyTab* m_disassemble;
    bool m_initDone;
    wxAuiManager* m_mgr;
    DebugTab* m_outputDebug;
    wxStringSet_t m_visibleWindows;

private:
    void CreateGUIControls();
    void OnSettingsChanged(wxCommandEvent& event);

public:
    DebuggerPane(wxWindow* parent, const wxString& caption, wxAuiManager* mgr);
    virtual ~DebuggerPane();

    // setters/getters
    const wxString& GetCaption() const { return m_caption; }
    LocalsTable* GetLocalsTable() { return m_localsTable; }
    WatchesTable* GetWatchesTable() { return m_watchesTable; }
    DebuggerCallstackView* GetFrameListView() { return m_frameList; }
    BreakpointDlg* GetBreakpointView() { return m_breakpoints; }
    ThreadListPanel* GetThreadsView() { return m_threads; }
    MemoryView* GetMemoryView() { return m_memory; }
    DebuggerAsciiViewer* GetAsciiViewer() { return m_asciiViewer; }
    DebuggerDisassemblyTab* GetDisassemblyTab() { return m_disassemble; }
    DebugTab* GetDebugWindow() { return m_outputDebug; }

    Notebook* GetNotebook() { return m_book; }
    void SelectTab(const wxString& tabName);
    void Clear();
    void OnPageChanged(wxBookCtrlEvent& event);
};
#endif // DEBUGGERPANE_H
