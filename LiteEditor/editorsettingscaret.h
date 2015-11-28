//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : editorsettingscaret.h
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

#ifndef __editorsettingscaret__
#define __editorsettingscaret__

#include "editor_options_caret.h"
#include "treebooknodebase.h"

class EditorSettingsCaret : public EditorSettingsCaretBase, public TreeBookNode<EditorSettingsCaret>
{
public:
    /** Constructor */
    EditorSettingsCaret( wxWindow* parent );
    void Save(OptionsConfigPtr options);
protected:
    virtual void OnCaretWidthUI(wxUpdateUIEvent& event);
};

#endif // __editorsettingscaret__
