#include "cc_triggering_page.h"

CCTriggeringPage::CCTriggeringPage( wxWindow* parent, const TagsOptionsData &data  )
	: CCTriggering( parent )
{
	m_checkWordAssist->SetValue            (data.GetFlags() & CC_WORD_ASSIST ? true : false);
	m_checkAutoInsertSingleChoice->SetValue(data.GetFlags() & CC_AUTO_INSERT_SINGLE_CHOICE ? true : false);
	m_sliderMinWordLen->SetValue           (data.GetMinWordLen() );
}

void CCTriggeringPage::OnAutoShowWordAssitUI( wxUpdateUIEvent& event )
{
	event.Enable(m_checkWordAssist->IsChecked());
}

void CCTriggeringPage::Save(TagsOptionsData& data)
{
	SetFlag(data, CC_AUTO_INSERT_SINGLE_CHOICE, m_checkAutoInsertSingleChoice->IsChecked());
	SetFlag(data, CC_WORD_ASSIST,               m_checkWordAssist->IsChecked());
	data.SetMinWordLen( m_sliderMinWordLen->GetValue() );
}
