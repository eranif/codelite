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

EditorOptionsGeneralIndentationPanel::EditorOptionsGeneralIndentationPanel( wxWindow* parent )
    : EditorOptionsGeneralIndetationPanelBase( parent )
    , TreeBookNode<EditorOptionsGeneralIndentationPanel>()
{
    OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
    m_checkBoxDisableSmartIdent->SetValue(options->GetDisableSmartIndent());
    m_indentsUsesTabs->SetValue(options->GetIndentUsesTabs());
    m_indentWidth->SetValue( ::wxIntToString(options->GetIndentWidth()) ) ;
    m_tabWidth->SetValue( ::wxIntToString(options->GetTabWidth()));
}

void EditorOptionsGeneralIndentationPanel::Save(OptionsConfigPtr options)
{
    options->SetIndentUsesTabs(m_indentsUsesTabs->IsChecked());
    options->SetIndentWidth( ::wxStringToInt(m_indentWidth->GetValue(), 4, 1, 20) );
    options->SetTabWidth( ::wxStringToInt(m_tabWidth->GetValue(), 4, 1, 20) );
    options->SetDisableSmartIndent(m_checkBoxDisableSmartIdent->IsChecked());
}
