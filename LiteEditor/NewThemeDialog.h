//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : NewThemeDialog.h
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

#ifndef NEWTHEMEDIALOG_H
#define NEWTHEMEDIALOG_H
#include "syntaxhighlightbasedlg.h"
#include "lexer_configuration.h"

class NewThemeDialog : public NewThemeDialogBase
{
private:
    void DoInitialize(LexerConf::Ptr_t lexer);
    
public:
    NewThemeDialog(wxWindow* parent, LexerConf::Ptr_t lexer);
    virtual ~NewThemeDialog() = default;

    wxString GetLexerName() const {
        return m_choiceLanguage->GetStringSelection();
    }

    wxString GetThemeName() const {
        return m_textCtrlName->GetValue();
    }

    wxString GetBaseTheme() const {
        return m_choiceBaseTheme->GetStringSelection();
    }
    
protected:
    virtual void OnLexerSelected(wxCommandEvent& event);
    virtual void OnOkUI(wxUpdateUIEvent& event);
};
#endif // NEWTHEMEDIALOG_H
