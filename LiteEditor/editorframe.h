//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : editorframe.h
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

#ifndef EDITORFRAME_H
#define EDITORFRAME_H

#include "cl_command_event.h"
#include "wxcrafter.h"
#include <list>

class Notebook;
class MyMenuBar;
wxDECLARE_EVENT(wxEVT_DETACHED_EDITOR_CLOSED, clCommandEvent);

class clEditor;
class QuickFindBar;
class EditorFrame : public EditorFrameBase
{
public:
    typedef std::list<EditorFrame*> List_t;

protected:
    virtual void OnCloseWindow(wxCloseEvent& event);
    QuickFindBar* m_findBar;
    MyMenuBar* m_myMenuBar;
    Notebook* m_book;

protected:
    virtual void OnFindUI(wxUpdateUIEvent& event);
    virtual void OnFind(wxCommandEvent& event);
    virtual void OnClose(wxCommandEvent& event);
    virtual void OnCloseUI(wxUpdateUIEvent& event);

    void OnPageClosed(wxBookCtrlEvent& event);
    void OnPageClosing(wxBookCtrlEvent& event);

    clEditor* GetEditor(size_t index) const;
    void DoCloseEditor(clEditor* editor);

public:
    EditorFrame(wxWindow* parent, clEditor* editor, size_t notebookStyle);
    virtual ~EditorFrame();
    void GetAllEditors(std::vector<clEditor*>& editors);
    clEditor* GetEditor() const;
};
#endif // EDITORFRAME_H
