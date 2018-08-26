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
#include "wxcrafter_plugin.h"
#include "codelite_exports.h"
#include "Notebook.h"

class WXDLLIMPEXP_SDK NotebookNavigationDlg : public NotebookNavigationDlgBase
{
    Notebook* m_book;
    int m_selection;

public:
    NotebookNavigationDlg(wxWindow* parent, Notebook* book);
    virtual ~NotebookNavigationDlg();
    void CloseDialog();

    int GetSelection() const { return m_selection; }

protected:
    virtual void OnItemActivated(wxDataViewEvent& event);
    virtual void OnKeyDown(wxKeyEvent& event);
    virtual void OnKeyUp(wxKeyEvent& event);
};
#endif // NOTEBOOKNAVIGATIONDLG_H
