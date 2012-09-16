#include "cc_colourise_page.h"
#include "globals.h"

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

#if HAS_LIBCLANG
    m_checkBoxColourMacroBlocks->SetValue(data.GetCcColourFlags() & CC_COLOUR_MACRO_BLOCKS);
#else
    m_checkBoxColourMacroBlocks->SetValue(false);
    m_checkBoxColourMacroBlocks->Enable(false);
#endif
    wxString val;
    val << data.GetMaxItemToColour();
    m_spinCtrlMaxItemToColour->SetValue(val);

    // wxFB marks the checkboxes' labels to be localised. We really don't want e.g. "class" or "enum" translated
    m_checkBoxClass->SetLabel(wxT("class"));
    m_checkBoxEnum->SetLabel(wxT("enum"));
    m_checkBoxFunction->SetLabel(wxT("function"));
    m_checkBoxMacro->SetLabel(wxT("macro"));
    m_checkBoxNamespace->SetLabel(wxT("namespace"));
    m_checkBoxPrototype->SetLabel(wxT("prototype"));
    m_checkBoxStruct->SetLabel(wxT("struct"));
    m_checkBoxTypedef->SetLabel(wxT("typedef"));
    m_checkBoxUnion->SetLabel(wxT("union"));
    m_checkBoxEnumerator->SetLabel(wxT("enumerator"));
    m_checkBoxMember->SetLabel(wxT("member"));
    m_checkBoxVariable->SetLabel(wxT("variable"));
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
    
    data.SetMaxItemToColour( ::wxStringToInt(m_spinCtrlMaxItemToColour->GetValue(), 1000, 1000, 10000) );
}
