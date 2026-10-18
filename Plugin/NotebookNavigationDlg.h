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

#include <wx/weakref.h>

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

/// A modeless dialog: create it with `new` and call Show(). When the user picks a tab, it selects that tab in the
/// book and destroys itself. It also destroys itself when it is cancelled (Escape, or another window gets the focus)
class WXDLLIMPEXP_SDK NotebookNavigationDlg : public NotebookNavigationDlgBase
{
    // Weak references: the book may be destroyed while the dialog is shown
#if !MAINBOOK_AUIBOOK
    wxWeakRef<Notebook> m_notebook;
#endif
    wxWeakRef<clAuiBook> m_aui_notebook;
    int m_selection = wxNOT_FOUND;
    bool m_dismissed = false;

public:
#if !MAINBOOK_AUIBOOK // In this case Notebook == clAuiBook
    NotebookNavigationDlg(wxWindow* parent, Notebook* book);
#endif
    NotebookNavigationDlg(wxWindow* parent, clAuiBook* book);
    ~NotebookNavigationDlg() override;
    /// Select the chosen tab in the book and destroy the dialog
    void CloseDialog();
    /// Destroy the dialog without changing the selection of the book
    void CancelDialog();

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
    /// Hide and destroy the dialog. With `apply`, select the chosen tab first. Only the first call does something
    void Dismiss(bool apply);
    void OnActivate(wxActivateEvent& event);
    void OnCharHook(wxKeyEvent& event);
    void OnCloseWindow(wxCloseEvent& event);

    template <typename Book>
    void Initialise(Book* book);
};
#endif // NOTEBOOKNAVIGATIONDLG_H
