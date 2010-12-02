#include "cc_colourise_page.h"

CCColourisePage::CCColourisePage( wxWindow* parent, const TagsOptionsData &data  )
	: CCColouringBasePage( parent )
{
	m_checkColourLocalVars->SetValue     (data.GetFlags() & CC_COLOUR_VARS ? true : false);
	m_checkColourProjTags->SetValue      (data.GetFlags() & CC_COLOUR_WORKSPACE_TAGS ? true : false);
	m_checkBoxClass->SetValue            (data.GetCcColourFlags() & CC_COLOUR_CLASS);
	m_checkBoxEnum->SetValue             (data.GetCcColourFlags() & CC_COLOUR_ENUM);
	m_checkBoxFunction->SetValue         (data.GetCcColourFlags() & CC_COLOUR_FUNCTION);
	m_checkBoxMacro->SetValue            (data.GetCcColourFlags() & CC_COLOUR_MACRO);
	m_checkBoxNamespace->SetValue        (data.GetCcColourFlags() & CC_COLOUR_NAMESPACE);
	m_checkBoxPrototype->SetValue        (data.GetCcColourFlags() & CC_COLOUR_PROTOTYPE);
	m_checkBoxStruct->SetValue           (data.GetCcColourFlags() & CC_COLOUR_STRUCT);
	m_checkBoxTypedef->SetValue          (data.GetCcColourFlags() & CC_COLOUR_TYPEDEF);
	m_checkBoxUnion->SetValue            (data.GetCcColourFlags() & CC_COLOUR_UNION);
	m_checkBoxEnumerator->SetValue       (data.GetCcColourFlags() & CC_COLOUR_ENUMERATOR);
	m_checkBoxMember->SetValue           (data.GetCcColourFlags() & CC_COLOUR_MEMBER);
	m_checkBoxVariable->SetValue         (data.GetCcColourFlags() & CC_COLOUR_VARIABLE);
	m_checkBoxColourMacroBlocks->SetValue(data.GetCcColourFlags() & CC_COLOUR_MACRO_BLOCKS);
	m_spinCtrlMaxItemToColour->SetValue  (data.GetMaxItemToColour() );
}

void CCColourisePage::OnColourWorkspaceUI( wxUpdateUIEvent& event )
{
	event.Enable(m_checkColourProjTags->IsChecked());
}

void CCColourisePage::Save(TagsOptionsData& data)
{
	SetColouringFlag(data, CC_COLOUR_CLASS,        m_checkBoxClass->IsChecked());
	SetColouringFlag(data, CC_COLOUR_ENUM,         m_checkBoxEnum->IsChecked());
	SetColouringFlag(data, CC_COLOUR_FUNCTION,     m_checkBoxFunction->IsChecked());
	SetColouringFlag(data, CC_COLOUR_MACRO,        m_checkBoxMacro->IsChecked());
	SetColouringFlag(data, CC_COLOUR_NAMESPACE,    m_checkBoxNamespace->IsChecked());
	SetColouringFlag(data, CC_COLOUR_PROTOTYPE,    m_checkBoxPrototype->IsChecked());
	SetColouringFlag(data, CC_COLOUR_STRUCT,       m_checkBoxStruct->IsChecked());
	SetColouringFlag(data, CC_COLOUR_TYPEDEF,      m_checkBoxTypedef->IsChecked());
	SetColouringFlag(data, CC_COLOUR_UNION,        m_checkBoxUnion->IsChecked());
	SetColouringFlag(data, CC_COLOUR_ENUMERATOR,   m_checkBoxEnumerator->IsChecked());
	SetColouringFlag(data, CC_COLOUR_VARIABLE,     m_checkBoxVariable->IsChecked());
	SetColouringFlag(data, CC_COLOUR_MEMBER,       m_checkBoxMember->IsChecked());
	SetColouringFlag(data, CC_COLOUR_MACRO_BLOCKS, m_checkBoxColourMacroBlocks->IsChecked());
	SetFlag(data, CC_COLOUR_VARS,                  m_checkColourLocalVars->IsChecked());
	SetFlag(data, CC_COLOUR_WORKSPACE_TAGS,        m_checkColourProjTags->IsChecked());
	
	data.SetMaxItemToColour( m_spinCtrlMaxItemToColour->GetValue() );
}
