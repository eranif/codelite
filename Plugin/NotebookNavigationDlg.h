//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : NotebookNavigationDlg.h
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

#ifndef NOTEBOOKNAVIGATIONDLG_H
#define NOTEBOOKNAVIGATIONDLG_H

#include "Notebook.h"
#include "clAuiBook.hpp"
#include "clGenericNotebook.hpp"
#include "clTab.h"
#include "cl_defs.h"
#include "globals.h"
#include "wxcrafter_plugin.hpp"

struct TabData {
    wxString label;
    wxBitmap bmp;
    wxFileName filename;
    int index;
    bool isFile;

    TabData()
        : index(wxNOT_FOUND)
        , isFile(false)
    {
    }
};

class WXDLLIMPEXP_SDK NotebookNavigationDlg : public NotebookNavigationDlgBase
{
#if !MAINBOOK_AUIBOOK
    Notebook* m_notebook = nullptr;
#endif
    clAuiBook* m_aui_notebook = nullptr;
    int m_selection = wxNOT_FOUND;

public:
#if !MAINBOOK_AUIBOOK // In this case Notebook == clAuiBook
    NotebookNavigationDlg(wxWindow* parent, Notebook* book);
#endif
    NotebookNavigationDlg(wxWindow* parent, clAuiBook* book);
    ~NotebookNavigationDlg() override;
    void CloseDialog();

    int GetSelection() const { return m_selection; }

protected:
    void OnItemActivated(wxDataViewEvent& event) override;
    void OnKeyDown(wxKeyEvent& event) override;
    void OnKeyUp(wxKeyEvent& event) override;

    void SelectNext();
    void SelectPrev();

    void OnHotKeyNext(wxKeyEvent& event);
    void OnHotKeyPrev(wxKeyEvent& event);
    void FinalizeCtor();

    template <typename Book>
    void Initialise(Book* book);
};
#endif // NOTEBOOKNAVIGATIONDLG_H
