//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : LLDBSettingDialog.cpp
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

#include "ColoursAndFontsManager.h"
#include "LLDBProtocol/LLDBSettings.h"
#include "LLDBSettingDialog.h"
#include "globals.h"
#include "windowattrmanager.h"

LLDBSettingDialog::LLDBSettingDialog(wxWindow* parent)
    : LLDBSettingDialogBase(parent)
    , m_modified(false)
{
    LLDBSettings settings;
    settings.Load();

    // keep the full path
    m_pgPropDebugServer->SetAttribute("ShowFullPath", 1);

    m_pgPropArraySize->SetValue((int)settings.GetMaxArrayElements());
    m_pgPropCallStackSize->SetValue((int)settings.GetMaxCallstackFrames());
    m_pgPropRaiseCodeLite->SetValue(settings.IsRaiseWhenBreakpointHit());
    m_pgShowThreadNames->SetValue(settings.HasFlag(kLLDBOptionShowThreadNames));
    m_pgShowFileNamesOnly->SetValue(settings.HasFlag(kLLDBOptionShowFileNamesOnly));
    m_pgPropProxyPort->SetValue(settings.GetProxyPort());
    m_pgPropProxyIP->SetValue(settings.GetProxyIp());
    m_pgPropProxyType->SetChoiceSelection(settings.IsUsingRemoteProxy() ? 1 : 0);
    m_pgPropDebugServer->SetValue(settings.GetDebugserver());
    m_stcTypes->SetText(settings.GetTypes());
    m_stcTypes->SetModified(false);
    SetName("LLDBSettingDialog");
    auto lexer = ColoursAndFontsManager::Get().GetLexer("text");
    if(lexer) {
        lexer->Apply(m_stcTypes);
    }
    ::clSetDialogBestSizeAndPosition(this);
}

LLDBSettingDialog::~LLDBSettingDialog() {}

void LLDBSettingDialog::Save()
{
    LLDBSettings settings;
    settings.Load();
    settings.SetMaxArrayElements(m_pgPropArraySize->GetValue().GetInteger());
    settings.SetMaxCallstackFrames(m_pgPropCallStackSize->GetValue().GetInteger());
    settings.EnableFlag(kLLDBOptionRaiseCodeLite, m_pgPropRaiseCodeLite->GetValue().GetBool());
    settings.EnableFlag(kLLDBOptionShowThreadNames, m_pgShowThreadNames->GetValue().GetBool());
    settings.EnableFlag(kLLDBOptionShowFileNamesOnly, m_pgShowFileNamesOnly->GetValue().GetBool());
    settings.SetUseRemoteProxy(m_pgPropProxyType->GetChoiceSelection() == 1 ? true : false);
    settings.SetProxyIp(m_pgPropProxyIP->GetValue().GetString());
    settings.SetProxyPort(m_pgPropProxyPort->GetValue().GetInteger());
    settings.SetTypes(m_stcTypes->GetText());
    settings.SetDebugserver(m_pgPropDebugServer->GetValue().GetString());
    settings.Save();
    m_modified = false;
    m_stcTypes->SetModified(false);
}

void LLDBSettingDialog::OnOKUI(wxUpdateUIEvent& event) { event.Enable(m_modified || m_stcTypes->IsModified()); }

void LLDBSettingDialog::OnGeneralValueChanged(wxPropertyGridEvent& event)
{
    event.Skip();
    m_modified = true;
}

void LLDBSettingDialog::OnAdvancedValueChanged(wxPropertyGridEvent& event)
{
    event.Skip();
    m_modified = true;
}

void LLDBSettingDialog::OnApply(wxCommandEvent& event) { Save(); }
