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

EditorOptionsGeneralIndentationPanel::EditorOptionsGeneralIndentationPanel( wxWindow* parent )
: EditorOptionsGeneralIndetationPanelBase( parent )
, TreeBookNode<EditorOptionsGeneralIndentationPanel>()
{
	OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
	m_checkBoxDisableSmartIdent->SetValue(options->GetDisableSmartIndent());
	m_indentsUsesTabs->SetValue(options->GetIndentUsesTabs());
    m_indentWidth->SetValue(options->GetIndentWidth());
    m_tabWidth->SetValue(options->GetTabWidth());
}

void EditorOptionsGeneralIndentationPanel::Save(OptionsConfigPtr options)
{
	options->SetIndentUsesTabs(m_indentsUsesTabs->IsChecked());
    options->SetIndentWidth(m_indentWidth->GetValue());
    options->SetTabWidth(m_tabWidth->GetValue());
	options->SetDisableSmartIndent(m_checkBoxDisableSmartIdent->IsChecked());
}

