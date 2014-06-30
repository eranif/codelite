//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : EditorOptionsCopyPaste.cpp
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

#include "EditorOptionsCopyPaste.h"

EditorOptionsCopyPaste::EditorOptionsCopyPaste(wxWindow* parent)
    : EditorOptionsCopyPasteBase(parent)
{
    OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
    size_t flags = options->GetOptions();
    
    int sel = 0; // Copy entire line
    if ( flags & OptionsConfig::Opt_CopyLineIfLineNotEmpty ) {
        sel = 1;
    } else if ( flags & OptionsConfig::Opt_CopyNothing ) {
        sel = 2;
    }
    m_radioBoxCopyOptions->SetSelection( sel );
    m_checkBoxDisableMultiPaste->SetValue( flags & OptionsConfig::Opt_Disable_Multipaste );
    m_checkBoxDisableMultiSelection->SetValue( flags & OptionsConfig::Opt_Disable_Multiselect );
}

EditorOptionsCopyPaste::~EditorOptionsCopyPaste()
{
}

void EditorOptionsCopyPaste::Save(OptionsConfigPtr options)
{
    size_t flags = options->GetOptions();
    flags &= ~( OptionsConfig::Opt_AlwaysCopyLine|
                OptionsConfig::Opt_CopyNothing|
                OptionsConfig::Opt_CopyLineIfLineNotEmpty|
                OptionsConfig::Opt_Disable_Multipaste|
                OptionsConfig::Opt_Disable_Multiselect);
    
    switch ( m_radioBoxCopyOptions->GetSelection() ) {
    case 1:
        flags |= OptionsConfig::Opt_CopyLineIfLineNotEmpty;
        break;
    case 2:
        flags |= OptionsConfig::Opt_CopyNothing;
        break;
    default:
    case 0:
        flags |= OptionsConfig::Opt_AlwaysCopyLine;
        break;
    }
    
    if ( m_checkBoxDisableMultiPaste->IsChecked() ) flags |= OptionsConfig::Opt_Disable_Multipaste;
    if ( m_checkBoxDisableMultiSelection->IsChecked() ) flags |= OptionsConfig::Opt_Disable_Multiselect;
    options->SetOptions( flags );
}
