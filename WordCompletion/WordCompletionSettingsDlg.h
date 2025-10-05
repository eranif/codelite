//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : WordCompletionSettingsDlg.h
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

#ifndef WORDCOMPLETIONSETTINGSDLG_H
#define WORDCOMPLETIONSETTINGSDLG_H
#include "UI.h"

class WordCompletionSettingsDlg : public WordCompletionSettingsBaseDlg
{
    bool m_modified;
public:
    WordCompletionSettingsDlg(wxWindow* parent);
    virtual ~WordCompletionSettingsDlg() = default;

protected:
    virtual void OnOk(wxCommandEvent& event);
    virtual void OnOkUI(wxUpdateUIEvent& event);
    virtual void OnValueChanged(wxPropertyGridEvent& event);
};
#endif // WORDCOMPLETIONSETTINGSDLG_H
