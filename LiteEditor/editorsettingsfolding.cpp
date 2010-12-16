//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : editorsettingsfolding.cpp
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

#include "editorsettingsfolding.h"

EditorSettingsFolding::EditorSettingsFolding( wxWindow* parent )
		: EditorSettingsFoldingBase( parent )
		, TreeBookNode<EditorSettingsFolding>()
{
	OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
	m_displayMargin->SetValue(options->GetDisplayFoldMargin());
	m_underlineFolded->SetValue(options->GetUnderlineFoldLine());
	m_foldPreprocessors->SetValue(options->GetFoldPreprocessor());
	m_foldCompact->SetValue(options->GetFoldCompact());
	m_foldElse->SetValue(options->GetFoldAtElse());
	m_foldStyle->SetStringSelection( options->GetFoldStyle() );
	m_colourPicker->SetColour(options->GetFoldBgColour());
}


void EditorSettingsFolding::Save(OptionsConfigPtr options)
{
	options->SetDisplayFoldMargin(m_displayMargin->GetValue());
	options->SetUnderlineFoldLine(m_underlineFolded->GetValue());
	options->SetFoldPreprocessor(m_foldPreprocessors->GetValue());
	options->SetFoldCompact(m_foldCompact->GetValue());
	options->SetFoldAtElse(m_foldElse->GetValue());
	options->SetFoldStyle(m_foldStyle->GetStringSelection());
	options->SetFoldBgColour(m_colourPicker->GetColour());
}

void EditorSettingsFolding::OnFoldColourUI(wxUpdateUIEvent& e)
{
	e.Enable( m_displayMargin->IsChecked() && (m_foldStyle->GetStringSelection() == _("Arrows with Background Colour") || m_foldStyle->GetStringSelection() == _("Simple with Background Colour")) );
}

void EditorSettingsFolding::OnFoldingMarginUI(wxUpdateUIEvent& event)
{
	event.Enable(m_displayMargin->IsChecked());
}
