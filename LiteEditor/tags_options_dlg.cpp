//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : tags_options_dlg.cpp
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

#include "tags_options_dlg.h"

#include "AddOptionsDialog.h"
#include "ColoursAndFontsManager.h"
#include "GCCMetadata.hpp"
#include "clSingleChoiceDialog.h"
#include "cl_config.h"
#include "ctags_manager.h"
#include "globals.h"
#include "ieditor.h"
#include "lexer_configuration.h"
#include "macros.h"
#include "pluginmanager.h"
#include "pp_include.h"
#include "pptable.h"
#include "windowattrmanager.h"
#include "wx/tokenzr.h"

#include <ICompilerLocator.h>
#include <build_settings_config.h>
#include <compiler.h>
#include <wx/dirdlg.h>
#include <wx/msgdlg.h>
#include <wx/tokenzr.h>
//---------------------------------------------------------

CodeCompletionSettingsDialog::CodeCompletionSettingsDialog(wxWindow* parent, const TagsOptionsData& data)
    : TagsOptionsBaseDlg(parent)
    , m_data(data)
{
    DoSetEditEventsHandler(this);
    Centre();
    GetSizer()->Fit(this);
    SetName("CodeCompletionSettingsDialog");
    WindowAttrManager::Load(this);

    // Set default values
    bool genJsonFile = clConfig::Get().Read("GenerateCompileCommands", false);

    //------------------------------------------------------------------
    // Display and behavior
    //------------------------------------------------------------------
    m_checkDisplayFunctionTip->SetValue(m_data.GetFlags() & CC_DISP_FUNC_CALLTIP);
    m_checkDisplayTypeInfo->SetValue(m_data.GetFlags() & CC_DISP_TYPE_INFO);
    m_checkBoxBackspaceTriggers->SetValue(m_data.GetFlags() & CC_BACKSPACE_TRIGGER);

    m_spinCtrlNumberOfCCItems->ChangeValue(::wxIntToString(m_data.GetCcNumberOfDisplayItems()));
    m_checkBoxGenCompileCommandsJSON->SetValue(genJsonFile);

    //----------------------------------------------------
    // Triggering
    //----------------------------------------------------
    m_checkAutoInsertSingleChoice->SetValue(m_data.GetFlags() & CC_AUTO_INSERT_SINGLE_CHOICE ? true : false);
    m_sliderMinWordLen->SetValue(m_data.GetMinWordLen());
    GetSizer()->Fit(this);
}

CodeCompletionSettingsDialog::~CodeCompletionSettingsDialog() {}

void CodeCompletionSettingsDialog::OnButtonOk(wxCommandEvent& event)
{
    wxUnusedVar(event);
    CopyData();
    EndModal(wxID_OK);
}

void CodeCompletionSettingsDialog::CopyData()
{
    //----------------------------------------------------
    // Display and behavior
    //----------------------------------------------------
    SetFlag(CC_DISP_FUNC_CALLTIP, m_checkDisplayFunctionTip->IsChecked());
    SetFlag(CC_DISP_TYPE_INFO, m_checkDisplayTypeInfo->IsChecked());
    SetFlag(CC_BACKSPACE_TRIGGER, m_checkBoxBackspaceTriggers->IsChecked());
    m_data.SetCcNumberOfDisplayItems(::wxStringToInt(m_spinCtrlNumberOfCCItems->GetValue(), 100));
    clConfig::Get().Write("GenerateCompileCommands", this->m_checkBoxGenCompileCommandsJSON->IsChecked());

    //----------------------------------------------------
    // Colouring
    //----------------------------------------------------
    // colouring is updated on the fly. See OnColouringPropertyValueChanged() function below

    //----------------------------------------------------
    // Triggering
    //----------------------------------------------------
    SetFlag(CC_AUTO_INSERT_SINGLE_CHOICE, m_checkAutoInsertSingleChoice->IsChecked());
    m_data.SetMinWordLen(m_sliderMinWordLen->GetValue());
}

void CodeCompletionSettingsDialog::SetFlag(CodeCompletionOpts flag, bool set)
{
    if(set) {
        m_data.SetFlags(m_data.GetFlags() | flag);
    } else {
        m_data.SetFlags(m_data.GetFlags() & ~(flag));
    }
}

void CodeCompletionSettingsDialog::SetColouringFlag(CodeCompletionColourOpts flag, bool set)
{
    if(set) {
        m_data.SetCcColourFlags(m_data.GetCcColourFlags() | flag);
    } else {
        m_data.SetCcColourFlags(m_data.GetCcColourFlags() & ~(flag));
    }
}

void CodeCompletionSettingsDialog::OnAutoShowWordAssitUI(wxUpdateUIEvent& event) { event.Enable(true); }

void CodeCompletionSettingsDialog::OnButtonCancel(wxCommandEvent& event)
{
    event.Skip();
    EndModal(wxID_CANCEL);
}

void CodeCompletionSettingsDialog::DoSetEditEventsHandler(wxWindow* win)
{
    // wxTextCtrl needs some extra special handling
    if(dynamic_cast<wxStyledTextCtrl*>(win)) {
        clEditEventsHandler::Ptr_t handler(new clEditEventsHandler(dynamic_cast<wxStyledTextCtrl*>(win)));
        m_handlers.push_back(handler);
    }

    // Check the children
    wxWindowList::compatibility_iterator pclNode = win->GetChildren().GetFirst();
    while(pclNode) {
        wxWindow* pclChild = pclNode->GetData();
        this->DoSetEditEventsHandler(pclChild);
        pclNode = pclNode->GetNext();
    }
}
