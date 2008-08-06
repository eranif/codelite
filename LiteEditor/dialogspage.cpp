#include "editor_config.h"
#include "dialogspage.h"

DialogsPage::DialogsPage( wxWindow* parent )
:
DialogsPageBase( parent )
{
	Initialize();
}

void DialogsPage::Initialize()
{
	long adjustCpuNumber(0);
	long replaceWrapAround(0);
	long findNextWrapAround(0);
	
	EditorConfigST::Get()->GetLongValue(wxT("AdjustCPUNumber"), adjustCpuNumber);
	EditorConfigST::Get()->GetLongValue(wxT("ReplaceWrapAroundAnswer"), replaceWrapAround);
	EditorConfigST::Get()->GetLongValue(wxT("FindNextWrapAroundAnswer"), findNextWrapAround);
	
	int idx;
	
	// the value stored is 0 / 1
	idx = m_checkListAnswers->Append(wxT("Adjust number of build processes according to CPU"));
	m_checkListAnswers->Check((unsigned int)idx, adjustCpuNumber ? true : false);
	
	// the value stored is wxID_OK / wxID_NO
	idx = m_checkListAnswers->Append(wxT("Wrap around 'Replace' operation"));
	m_checkListAnswers->Check((unsigned int)idx, replaceWrapAround == wxID_OK);
	
	// the value stored is wxID_OK / wxID_NO
	idx = m_checkListAnswers->Append(wxT("Wrap around 'Find' operation"));
	m_checkListAnswers->Check((unsigned int)idx, findNextWrapAround == wxID_OK);
}

void DialogsPage::Save()
{
	EditorConfigST::Get()->SaveLongValue(wxT("AdjustCPUNumber"), m_checkListAnswers->IsChecked(0) ? 1 : 0);
	EditorConfigST::Get()->SaveLongValue(wxT("ReplaceWrapAroundAnswer"), m_checkListAnswers->IsChecked(1) ? wxID_OK : wxID_NO);
	EditorConfigST::Get()->SaveLongValue(wxT("FindNextWrapAroundAnswer"), m_checkListAnswers->IsChecked(2) ? wxID_OK : wxID_NO);
}
