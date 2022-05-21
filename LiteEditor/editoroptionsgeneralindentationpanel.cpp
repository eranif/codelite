//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : editoroptionsgeneralindentationpanel.cpp
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

#include "editoroptionsgeneralindentationpanel.h"

#include "globals.h"

EditorOptionsGeneralIndentationPanel::EditorOptionsGeneralIndentationPanel(wxWindow* parent, OptionsConfigPtr options)
    : OptionsConfigPage(parent, options)
{
    AddHeader(_("Indentation settings"));
    AddProperty(_("Disable Smart Indentation"), m_options->GetDisableSmartIndent(),
                UPDATE_BOOL_CB(SetDisableSmartIndent));
    AddProperty(_("Use tabs"), m_options->GetIndentUsesTabs(), UPDATE_BOOL_CB(SetIndentUsesTabs));
    AddProperty(_("Columns per indentation level"), m_options->GetIndentWidth(), UPDATE_INT_CB(SetIndentWidth));
    AddProperty(_("Tab width"), m_options->GetTabWidth(), UPDATE_INT_CB(SetTabWidth));
}
