//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : Notebook.h
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

#ifndef NOTEBOOK_H
#define NOTEBOOK_H

#include "cl_defs.h"

#include <wx/aui/auibook.h>

#if CL_USE_NATIVEBOOK

#include "clNativeNotebook.hpp"
typedef clNativeNotebook Notebook;
#else // !CL_USE_NATIVEBOOK

#include "clGenericNotebook.hpp"
typedef clGenericNotebook Notebook;

#endif // CL_USE_NATIVEBOOK

class WXDLLIMPEXP_SDK clAuiBook : public wxAuiNotebook
{
public:
    clAuiBook(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize, long style = 0);
    ~clAuiBook() override;
    size_t GetAllTabs(clTabInfo::Vec_t& tabs);
    clTabHistory::Ptr_t GetHistory() const { return m_history; }
    void MoveActivePage(int newIndex);

protected:
    void OnPageClosed(wxAuiNotebookEvent& event);
    void OnPageChanged(wxAuiNotebookEvent& event);

private:
    clTabHistory::Ptr_t m_history;
};

#endif // NOTEBOOK_H
