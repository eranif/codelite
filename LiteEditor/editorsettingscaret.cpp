//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : editorsettingscaret.cpp
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

#include "editorsettingscaret.h"

EditorSettingsCaret::EditorSettingsCaret(wxWindow* parent, OptionsConfigPtr options)
    : OptionsConfigPage(parent, options)
{
    AddProperty(_("Caret blink period"), m_options->GetCaretBlinkPeriod(), UPDATE_INT_CB(SetCaretBlinkPeriod));
    AddProperty(_("Caret width"), m_options->GetCaretWidth(), UPDATE_INT_CB(SetCaretWidth));
    AddProperty(_("Use block caret"), m_options->HasOption(OptionsConfig::Opt_UseBlockCaret),
                UPDATE_OPTION_CB(Opt_UseBlockCaret));
    AddProperty(_("Allow caret to scroll beyond end of file"), m_options->GetScrollBeyondLastLine(),
                UPDATE_BOOL_CB(SetScrollBeyondLastLine));

    AddProperty(_("Allow caret to be placed beyond the end of line"),
                m_options->HasOption(OptionsConfig::Opt_AllowCaretAfterEndOfLine),
                UPDATE_OPTION_CB(Opt_AllowCaretAfterEndOfLine));

    AddProperty(_("Caret jumps between word segments"), m_options->GetCaretUseCamelCase(),
                UPDATE_BOOL_CB(SetCaretUseCamelCase));

    AddProperty(_("Auto-adjust horizontal scrollbar width"), m_options->GetAutoAdjustHScrollBarWidth(),
                UPDATE_BOOL_CB(SetAutoAdjustHScrollBarWidth));
}
