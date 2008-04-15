///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  5 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifdef WX_PRECOMP

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#else
#include <wx/wx.h>
#endif //WX_PRECOMP

#include "compiler_page.h"
#include "macros.h"
#include "editor_config.h"
#include "build_settings_config.h"

CompilerPage::CompilerPage( wxWindow* parent, wxString name, int id, wxPoint pos, wxSize size, int style ) 
: wxScrolledWindow( parent, id, pos, size, style )
, m_cmpname(name)
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxHORIZONTAL );
	
	wxStaticBoxSizer* sbSizer5;
	sbSizer5 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Compiler Error Pattern:") ), wxVERTICAL );
	
	m_staticText5 = new wxStaticText( this, wxID_ANY, wxT("Regex Pattern:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	sbSizer5->Add( m_staticText5, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_textErrorPattern = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer5->Add( m_textErrorPattern, 0, wxEXPAND|wxALL, 5 );
	
	m_staticText6 = new wxStaticText( this, wxID_ANY, wxT("File Index in Pattern:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	sbSizer5->Add( m_staticText6, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_textErrorFileIndex = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer5->Add( m_textErrorFileIndex, 0, wxEXPAND|wxALL, 5 );
	
	m_staticText7 = new wxStaticText( this, wxID_ANY, wxT("Line Number in Pattern:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	sbSizer5->Add( m_staticText7, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_textErrorLineNumber = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer5->Add( m_textErrorLineNumber, 0, wxEXPAND|wxALL, 5 );
	
	bSizer11->Add( sbSizer5, 1, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Compiler Warning Pattern:") ), wxVERTICAL );
	
	m_staticText51 = new wxStaticText( this, wxID_ANY, wxT("Regex Pattern:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText51->Wrap( -1 );
	sbSizer4->Add( m_staticText51, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_textWarnPattern = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer4->Add( m_textWarnPattern, 0, wxEXPAND|wxALL, 5 );
	
	m_staticText61 = new wxStaticText( this, wxID_ANY, wxT("File Index in Pattern:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText61->Wrap( -1 );
	sbSizer4->Add( m_staticText61, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_textWarnFileIndex = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer4->Add( m_textWarnFileIndex, 0, wxEXPAND|wxALL, 5 );
	
	m_staticText71 = new wxStaticText( this, wxID_ANY, wxT("Line Number in Pattern:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText71->Wrap( -1 );
	sbSizer4->Add( m_staticText71, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_textWarnLineNumber = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer4->Add( m_textWarnLineNumber, 0, wxEXPAND|wxALL, 5 );
	
	bSizer11->Add( sbSizer4, 1, wxEXPAND|wxTOP|wxBOTTOM, 5 );
	
	mainSizer->Add( bSizer11, 0, wxALL|wxEXPAND, 0 );
	
	wxStaticBoxSizer* sbSizer41;
	sbSizer41 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Tools:") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer4;
	fgSizer4 = new wxFlexGridSizer( 3, 4, 0, 0 );
	fgSizer4->AddGrowableCol( 1 );
	fgSizer4->AddGrowableCol( 3 );
	fgSizer4->SetFlexibleDirection( wxBOTH );
	fgSizer4->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText9 = new wxStaticText( this, wxID_ANY, wxT("Compiler Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText9->Wrap( -1 );
	fgSizer4->Add( m_staticText9, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCompilerName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_textCompilerName, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText11 = new wxStaticText( this, wxID_ANY, wxT("Linker Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11->Wrap( -1 );
	fgSizer4->Add( m_staticText11, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textLinkerName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_textLinkerName, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText12 = new wxStaticText( this, wxID_ANY, wxT("Shared Object Linker:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText12->Wrap( -1 );
	fgSizer4->Add( m_staticText12, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textSOLinker = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_textSOLinker, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText10 = new wxStaticText( this, wxID_ANY, wxT("Archive Tool:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText10->Wrap( -1 );
	fgSizer4->Add( m_staticText10, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textArchiveTool = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_textArchiveTool, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText14 = new wxStaticText( this, wxID_ANY, wxT("Resource Compiler:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText14->Wrap( -1 );
	fgSizer4->Add( m_staticText14, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textResourceCmp = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_textResourceCmp, 0, wxALL|wxEXPAND, 5 );
	
	sbSizer41->Add( fgSizer4, 1, wxEXPAND, 5 );
	
	mainSizer->Add( sbSizer41, 0, wxALL|wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, wxT("Object extesion:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	fgSizer2->Add( m_staticText3, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textObjectExtension = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_textObjectExtension, 1, wxALL|wxEXPAND, 5 );
	
	mainSizer->Add( fgSizer2, 0, wxALL|wxEXPAND, 0 );
	
	wxStaticBoxSizer* sbSizer51;
	sbSizer51 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Compiler Global Paths:") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer3->AddGrowableCol( 1 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText141 = new wxStaticText( this, wxID_ANY, wxT("Include Path:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText141->Wrap( -1 );
	fgSizer3->Add( m_staticText141, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlGlobalIncludePath = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_textCtrlGlobalIncludePath, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText16 = new wxStaticText( this, wxID_ANY, wxT("Libraries Path:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText16->Wrap( -1 );
	fgSizer3->Add( m_staticText16, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlGlobalLibPath = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_textCtrlGlobalLibPath, 0, wxALL|wxEXPAND, 5 );
	
	sbSizer51->Add( fgSizer3, 1, wxEXPAND, 5 );
	
	mainSizer->Add( sbSizer51, 0, wxEXPAND|wxALL, 5 );
	
	m_staticText8 = new wxStaticText( this, wxID_ANY, wxT("Switches:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText8->Wrap( -1 );
	mainSizer->Add( m_staticText8, 0, wxALL, 5 );
	
	m_listSwitches = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT );
	mainSizer->Add( m_listSwitches, 1, wxEXPAND|wxALL, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	CustomInitialize();
}

void CompilerPage::CustomInitialize()
{
	//load the compiler from the configuration file
	CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(m_cmpname);
	m_textObjectExtension->SetValue(cmp->GetObjectSuffix());
	m_textErrorPattern->SetValue(cmp->GetErrPattern());
	m_textErrorFileIndex->SetValue(cmp->GetErrFileNameIndex());
	m_textErrorLineNumber->SetValue(cmp->GetErrLineNumberIndex());
	m_textWarnPattern->SetValue(cmp->GetWarnPattern());
	m_textWarnFileIndex->SetValue(cmp->GetWarnFileNameIndex());
	m_textWarnLineNumber->SetValue(cmp->GetWarnLineNumberIndex());
	m_textArchiveTool->SetValue(cmp->GetTool(wxT("ArchiveTool")));
	m_textCompilerName->SetValue(cmp->GetTool(wxT("CompilerName")));
	m_textLinkerName->SetValue(cmp->GetTool(wxT("LinkerName")));
	m_textSOLinker->SetValue(cmp->GetTool(wxT("SharedObjectLinkerName")));
	m_textResourceCmp->SetValue(cmp->GetTool(wxT("ResourceCompiler")));
	m_textCtrlGlobalIncludePath->SetValue(cmp->GetGlobalIncludePath());
	m_textCtrlGlobalLibPath->SetValue(cmp->GetGlobalLibPath());
	
	InitSwitches();
	ConnectEvents();
}

void CompilerPage::Save()
{
	//load the compiler from the configuration file
	CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(m_cmpname);
	cmp->SetErrPattern(m_textErrorPattern->GetValue());
	cmp->SetErrFileNameIndex(m_textErrorFileIndex->GetValue());
	cmp->SetErrLineNumberIndex(m_textErrorLineNumber->GetValue());
	cmp->SetWarnPattern(m_textWarnPattern->GetValue());
	cmp->SetWarnFileNameIndex(m_textWarnFileIndex->GetValue());
	cmp->SetWarnLineNumberIndex(m_textWarnLineNumber->GetValue());
	cmp->SetObjectSuffix(m_textObjectExtension->GetValue());
	cmp->SetTool(wxT("ArchiveTool"), m_textArchiveTool->GetValue());
	cmp->SetTool(wxT("CompilerName"), m_textCompilerName->GetValue());
	cmp->SetTool(wxT("LinkerName"), m_textLinkerName->GetValue());
	cmp->SetTool(wxT("SharedObjectLinkerName"), m_textSOLinker->GetValue());
	cmp->SetTool(wxT("ResourceCompiler"), m_textResourceCmp->GetValue());
	cmp->SetGlobalIncludePath(m_textCtrlGlobalIncludePath->GetValue());
	cmp->SetGlobalLibPath(m_textCtrlGlobalLibPath->GetValue());
	
	BuildSettingsConfigST::Get()->SetCompiler(cmp);//save changes
}

void CompilerPage::AddSwitch(const wxString &name, const wxString &value, bool choose)
{
	wxListItem info;
	info.SetText(name);
	info.SetColumn(0);
	if(choose == true){
		info.SetState(wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED);
		m_selSwitchName = name;
		m_selSwitchValue = value;
	}
	long item = m_listSwitches->InsertItem(info);

	info.SetColumn(1);
	info.SetId(item);
	info.SetText(value);
	info.SetState(0);
	m_listSwitches->SetItem(info); 
}

void CompilerPage::ConnectEvents()
{
	ConnectListCtrlItemActivated(m_listSwitches, CompilerPage::OnItemActivated);
	ConnectListCtrlItemSelected(m_listSwitches, CompilerPage::OnItemSelected);
}

void CompilerPage::OnItemActivated(wxListEvent &event)
{
	//get the var name & value
	wxListItem info;
	info.m_itemId = event.m_itemIndex;
	info.m_col = 0;	//name
	info.m_mask = wxLIST_MASK_TEXT;

	if( m_listSwitches->GetItem(info) ){
		m_selSwitchName = info.m_text;
	}

	info.m_col = 1;//value
	if( m_listSwitches->GetItem(info)){
		m_selSwitchValue = info.m_text;
	}
	EditSwitch();
}

void CompilerPage::OnItemSelected(wxListEvent &event)
{
	//get the var name & value
	wxListItem info;
	info.m_itemId = event.m_itemIndex;
	info.m_col = 0;	//name
	info.m_mask = wxLIST_MASK_TEXT;

	if( m_listSwitches->GetItem(info) ){
		m_selSwitchName = info.m_text;
	}

	info.m_col = 1;//value
	if( m_listSwitches->GetItem(info)){
		m_selSwitchValue = info.m_text;
	}
}

void CompilerPage::InitSwitches()
{
	//insert columns:
	m_listSwitches->Freeze();
	m_listSwitches->ClearAll();
	m_listSwitches->InsertColumn(0, wxT("Name"));
	m_listSwitches->InsertColumn(1, wxT("Value"));

	//populate the list control
	CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(m_cmpname);
	Compiler::ConstIterator iter = cmp->SwitchesBegin();
	for(; iter != cmp->SwitchesEnd(); iter++){
		AddSwitch(iter->first, iter->second, iter == cmp->SwitchesBegin());
	}
	m_listSwitches->Thaw();
}

void CompilerPage::EditSwitch()
{
	wxString message;
	message << m_selSwitchName << wxT(" switch:");
	wxTextEntryDialog *dlg = new wxTextEntryDialog(this, message, wxT("Edit"), m_selSwitchValue);
	if(dlg->ShowModal() == wxID_OK){
		wxString newVal = dlg->GetValue();
		CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(m_cmpname);
		cmp->SetSwitch(m_selSwitchName, dlg->GetValue());
		BuildSettingsConfigST::Get()->SetCompiler(cmp);
		InitSwitches();
	}
	dlg->Destroy();
}

