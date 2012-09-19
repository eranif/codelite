//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : editoroptionsgeneralrightmarginpanel.cpp
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

#include "editoroptionsgeneralrightmarginpanel.h"
#include <wx/stc/stc.h>
#include "globals.h"

EditorOptionsGeneralRightMarginPanel::EditorOptionsGeneralRightMarginPanel( wxWindow* parent )
    : EditorOptionsGeneralRightMarginPanelBase( parent )
    , TreeBookNode<EditorOptionsGeneralRightMarginPanel>()
{
    OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
    m_radioBtnRMDisabled->SetValue(options->GetEdgeMode() == wxSTC_EDGE_NONE);
    m_radioBtnRMLine->SetValue(options->GetEdgeMode() == wxSTC_EDGE_LINE);
    m_radioBtnRMBackground->SetValue(options->GetEdgeMode() == wxSTC_EDGE_BACKGROUND);
    m_rightMarginColumn->ChangeValue( ::wxIntToString(options->GetEdgeColumn()) );
    m_rightMarginColour->SetColour(options->GetEdgeColour());
    EnableDisableRightMargin();
}

void EditorOptionsGeneralRightMarginPanel::Save(OptionsConfigPtr options)
{
    options->SetEdgeMode(m_radioBtnRMLine->GetValue()       ? wxSTC_EDGE_LINE :
                         m_radioBtnRMBackground->GetValue() ? wxSTC_EDGE_BACKGROUND
                         : wxSTC_EDGE_NONE);
    options->SetEdgeColumn( ::wxStringToInt(m_rightMarginColumn->GetValue(), 80, 0, 256) );
    options->SetEdgeColour(m_rightMarginColour->GetColour());
}

void EditorOptionsGeneralRightMarginPanel::OnRightMarginIndicator(wxCommandEvent& e)
{
    EnableDisableRightMargin();
}

void EditorOptionsGeneralRightMarginPanel::EnableDisableRightMargin()
{
    if(m_radioBtnRMDisabled->GetValue()) {
        m_rightMarginColour->Disable();
        m_rightMarginColumn->Disable();
        m_staticText1->Disable();
        m_staticText2->Disable();
    } else {
        m_rightMarginColour->Enable();
        m_rightMarginColumn->Enable();
        m_staticText1->Enable();
        m_staticText2->Enable();
    }
}
