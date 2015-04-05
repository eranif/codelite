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

#include "globals.h"
#include "editorsettingsfolding.h"

EditorSettingsFolding::EditorSettingsFolding(wxWindow* parent)
    : EditorSettingsFoldingBase(parent)
    , TreeBookNode<EditorSettingsFolding>()
{
    OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
    m_displayMargin->SetValue(options->GetDisplayFoldMargin());
    m_underlineFolded->SetValue(options->GetUnderlineFoldLine());
    m_foldPreprocessors->SetValue(options->GetFoldPreprocessor());
    m_foldCompact->SetValue(options->GetFoldCompact());
    m_foldElse->SetValue(options->GetFoldAtElse());
    m_checkBoxHighlightFolding->SetValue(options->IsHighlightFoldWhenActive());
    
    const wxString FoldStyles[] = { wxTRANSLATE("Simple"),
                                    wxTRANSLATE("Arrows"),
                                    wxTRANSLATE("Flatten Tree Square Headers"),
                                    wxTRANSLATE("Flatten Tree Circular Headers") };

    m_stringManager.AddStrings(sizeof(FoldStyles) / sizeof(wxString), FoldStyles, options->GetFoldStyle(), m_foldStyle);
}

void EditorSettingsFolding::Save(OptionsConfigPtr options)
{
    options->SetDisplayFoldMargin(m_displayMargin->GetValue());
    options->SetUnderlineFoldLine(m_underlineFolded->GetValue());
    options->SetFoldPreprocessor(m_foldPreprocessors->GetValue());
    options->SetFoldCompact(m_foldCompact->GetValue());
    options->SetFoldAtElse(m_foldElse->GetValue());
    options->SetHighlightFoldWhenActive(m_checkBoxHighlightFolding->IsChecked());
    
    // Get the foldstyle selection, unlocalised
    wxString foldStyle = m_stringManager.GetStringSelection();

    // thses 2 styles no longer exists...
    if(foldStyle == _("Arrows with Background Colour") || foldStyle == _("Simple with Background Colour"))
        foldStyle.Clear();

    if(foldStyle.IsEmpty()) {
        foldStyle = wxT("Arrows");
    }

    options->SetFoldStyle(foldStyle);
}

void EditorSettingsFolding::OnFoldingMarginUI(wxUpdateUIEvent& event) { event.Enable(m_displayMargin->IsChecked()); }
