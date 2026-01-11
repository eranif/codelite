//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : EditDlg.h
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

#ifndef EDITDLG_H
#define EDITDLG_H

#include "lexer_configuration.h"
#include "wxcrafter_plugin.h"

class WXDLLIMPEXP_SDK EditDlg : public EditDlgBase
{
public:
    EditDlg(wxWindow* parent, const wxString& text, LexerConf::Ptr_t lexer = nullptr);
    virtual ~EditDlg();

    wxString GetText() const { return m_stc10->GetText(); }
};

/**
 * @brief Displays a dialog to prompt the user to edit a string with syntax highlighting.
 *
 * Opens an EditDlg modal dialog pre-filled with the initial value. If the user confirms
 * the dialog (clicks OK), the edited text is returned; otherwise, an empty string is returned.
 *
 * @param initialValue The initial text content to display in the editor dialog.
 * @param parent The parent window for the modal dialog, or nullptr for no parent.
 * @param lexer A shared pointer to a LexerConf object for syntax highlighting configuration (currently unused in the
 * implementation).
 *
 * @return A wxString containing the user's edited text if the dialog was confirmed (wxID_OK),
 *         or an empty string (wxEmptyString) if the dialog was cancelled.
 */
WXDLLIMPEXP_SDK wxString clGetStringFromUser(const wxString& initialValue,
                                             wxWindow* parent = nullptr,
                                             LexerConf::Ptr_t lexer = nullptr);
#endif // EDITDLG_H
