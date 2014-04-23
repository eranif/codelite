#include "LLDBSettingDialog.h"
#include "LLDBProtocol/LLDBSettings.h"
#include "windowattrmanager.h"

LLDBSettingDialog::LLDBSettingDialog(wxWindow* parent)
    : LLDBSettingDialogBase(parent)
    , m_modified(false)
{
    LLDBSettings settings;
    settings.Load();
    
    m_pgPropArraySize->SetValue( (int)settings.GetMaxArrayElements() );
    m_pgPropCallStackSize->SetValue( (int)settings.GetMaxCallstackFrames() );
    m_pgPropRaiseCodeLite->SetValue( settings.IsRaiseWhenBreakpointHit() );
    m_pgPropDebugUser->SetChoiceSelection( settings.IsDebugAsSuperuser() ? 1 : 0);
    m_stcTypes->SetText( settings.GetTypes() );
    m_stcTypes->SetModified(false);
    WindowAttrManager::Load(this, "LLDBSettingDialog");
}

LLDBSettingDialog::~LLDBSettingDialog()
{
    WindowAttrManager::Save(this, "LLDBSettingDialog");
}

void LLDBSettingDialog::Save()
{
    LLDBSettings settings;
    settings.Load();
    settings.SetMaxArrayElements( m_pgPropArraySize->GetValue().GetInteger() );
    settings.SetMaxCallstackFrames( m_pgPropCallStackSize->GetValue().GetInteger() );
    settings.EnableFlag( kLLDBOptionRaiseCodeLite, m_pgPropRaiseCodeLite->GetValue().GetBool() );
    settings.EnableFlag( kLLDBOptionDebugAsSuperuser, m_pgPropDebugUser->GetChoiceSelection() == 1);
    settings.SetTypes( m_stcTypes->GetText() );
    settings.Save();
}

void LLDBSettingDialog::OnOKUI(wxUpdateUIEvent& event)
{
    event.Enable( m_modified || m_stcTypes->IsModified() );
}

void LLDBSettingDialog::OnGeneralValueChanged(wxPropertyGridEvent& event)
{
    event.Skip();
    m_modified = true;
}
