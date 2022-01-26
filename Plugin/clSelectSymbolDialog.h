//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : clSelectSymbolDialog.h
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

#ifndef CLSELECTSYMBOLDIALOG_H
#define CLSELECTSYMBOLDIALOG_H

#include "clSelectSymbolDialogBase.h"
#include "cl_command_event.h"
#include "codelite_exports.h"

#include <vector>
#include <wx/sharedptr.h>

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_SYMBOL_SELECTED, clCommandEvent);

struct WXDLLIMPEXP_SDK clSelectSymbolDialogEntry {
    wxString name;
    wxBitmap bmp;
    wxString help;
    wxClientData* clientData;

    clSelectSymbolDialogEntry()
        : bmp(wxNullBitmap)
        , clientData(NULL)
    {
    }

    typedef std::vector<clSelectSymbolDialogEntry> List_t;
};

class WXDLLIMPEXP_SDK clSelectSymbolDialog : public clSelectSymbolDialogBase
{
public:
    clSelectSymbolDialog(wxWindow* parent, const clSelectSymbolDialogEntry::List_t& entries);
    virtual ~clSelectSymbolDialog();

protected:
    void AddSymbol(const wxString& name, const wxBitmap& bmp = wxNullBitmap, const wxString& help = "",
                   wxClientData* clientData = NULL);
    void Initialise(const clSelectSymbolDialogEntry::List_t& entries);

public:
    /**
     * @brief return the selected item client data
     */
    wxClientData* GetSelection() const;

protected:
    virtual void OnItemActivated(wxDataViewEvent& event);
    virtual void OnOKUI(wxUpdateUIEvent& event);
};
#endif // CLSELECTSYMBOLDIALOG_H
