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
	
	m_notebook1 = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_panel1 = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizerError;
	bSizerError = new wxBoxSizer( wxVERTICAL );
	
	m_staticText161 = new wxStaticText( m_panel1, wxID_ANY, wxT("Compiler Errors Pattern:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText161->Wrap( -1 );
	m_staticText161->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	bSizerError->Add( m_staticText161, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer41;
	fgSizer41 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer41->AddGrowableCol( 1 );
	fgSizer41->SetFlexibleDirection( wxBOTH );
	fgSizer41->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText5 = new wxStaticText( m_panel1, wxID_ANY, wxT("Regex Pattern:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	fgSizer41->Add( m_staticText5, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_textErrorPattern = new wxTextCtrl( m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer41->Add( m_textErrorPattern, 0, wxEXPAND|wxALL, 5 );
	
	m_staticText6 = new wxStaticText( m_panel1, wxID_ANY, wxT("File Index in Pattern:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	fgSizer41->Add( m_staticText6, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_textErrorFileIndex = new wxTextCtrl( m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer41->Add( m_textErrorFileIndex, 0, wxEXPAND|wxALL, 5 );
	
	m_staticText7 = new wxStaticText( m_panel1, wxID_ANY, wxT("Line Number in Pattern:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	fgSizer41->Add( m_staticText7, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_textErrorLineNumber = new wxTextCtrl( m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer41->Add( m_textErrorLineNumber, 0, wxEXPAND|wxALL, 5 );
	
	bSizerError->Add( fgSizer41, 0, wxEXPAND, 5 );
	
	m_staticline1 = new wxStaticLine( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizerError->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	bSizer11->Add( bSizerError, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizerWarnings;
	bSizerWarnings = new wxBoxSizer( wxVERTICAL );
	
	m_staticText17 = new wxStaticText( m_panel1, wxID_ANY, wxT("Compiler Warnings Pattern:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText17->Wrap( -1 );
	m_staticText17->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	bSizerWarnings->Add( m_staticText17, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer5;
	fgSizer5 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer5->AddGrowableCol( 1 );
	fgSizer5->SetFlexibleDirection( wxBOTH );
	fgSizer5->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText51 = new wxStaticText( m_panel1, wxID_ANY, wxT("Regex Pattern:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText51->Wrap( -1 );
	fgSizer5->Add( m_staticText51, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_textWarnPattern = new wxTextCtrl( m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer5->Add( m_textWarnPattern, 0, wxEXPAND|wxALL, 5 );
	
	m_staticText61 = new wxStaticText( m_panel1, wxID_ANY, wxT("File Index in Pattern:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText61->Wrap( -1 );
	fgSizer5->Add( m_staticText61, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_textWarnFileIndex = new wxTextCtrl( m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer5->Add( m_textWarnFileIndex, 0, wxEXPAND|wxALL, 5 );
	
	m_staticText71 = new wxStaticText( m_panel1, wxID_ANY, wxT("Line Number in Pattern:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText71->Wrap( -1 );
	fgSizer5->Add( m_staticText71, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_textWarnLineNumber = new wxTextCtrl( m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer5->Add( m_textWarnLineNumber, 0, wxEXPAND|wxALL, 5 );
	
	bSizerWarnings->Add( fgSizer5, 1, wxEXPAND, 5 );
	
	bSizer11->Add( bSizerWarnings, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	m_staticline2 = new wxStaticLine( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer7->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );
	
	m_staticText18 = new wxStaticText( m_panel1, wxID_ANY, wxT("Global Paths:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText18->Wrap( -1 );
	m_staticText18->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	bSizer7->Add( m_staticText18, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer3->AddGrowableCol( 1 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText141 = new wxStaticText( m_panel1, wxID_ANY, wxT("Include Path:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText141->Wrap( -1 );
	fgSizer3->Add( m_staticText141, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlGlobalIncludePath = new wxTextCtrl( m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_textCtrlGlobalIncludePath, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText16 = new wxStaticText( m_panel1, wxID_ANY, wxT("Libraries Path:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText16->Wrap( -1 );
	fgSizer3->Add( m_staticText16, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlGlobalLibPath = new wxTextCtrl( m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_textCtrlGlobalLibPath, 0, wxALL|wxEXPAND, 5 );
	
	bSizer7->Add( fgSizer3, 1, wxEXPAND, 5 );
	
	bSizer11->Add( bSizer7, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );
	
	m_staticline3 = new wxStaticLine( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer8->Add( m_staticline3, 0, wxEXPAND | wxALL, 5 );
	
	m_staticText19 = new wxStaticText( m_panel1, wxID_ANY, wxT("Misc:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText19->Wrap( -1 );
	m_staticText19->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	bSizer8->Add( m_staticText19, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText3 = new wxStaticText( m_panel1, wxID_ANY, wxT("Objects extesion:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	fgSizer2->Add( m_staticText3, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textObjectExtension = new wxTextCtrl( m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_textObjectExtension, 1, wxALL|wxEXPAND, 5 );
	
	bSizer8->Add( fgSizer2, 0, wxALL|wxEXPAND, 0 );
	
	bSizer11->Add( bSizer8, 0, wxEXPAND, 5 );
	
	bSizer4->Add( bSizer11, 0, wxALL|wxEXPAND, 0 );
	
	m_panel1->SetSizer( bSizer4 );
	m_panel1->Layout();
	bSizer4->Fit( m_panel1 );
	m_notebook1->AddPage( m_panel1, wxT("General"), true );
	m_panel2 = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer4;
	fgSizer4 = new wxFlexGridSizer( 3, 2, 0, 0 );
	fgSizer4->AddGrowableCol( 1 );
	fgSizer4->AddGrowableCol( 3 );
	fgSizer4->SetFlexibleDirection( wxBOTH );
	fgSizer4->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText9 = new wxStaticText( m_panel2, wxID_ANY, wxT("Compiler Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText9->Wrap( -1 );
	fgSizer4->Add( m_staticText9, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCompilerName = new wxTextCtrl( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_textCompilerName, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText11 = new wxStaticText( m_panel2, wxID_ANY, wxT("Linker Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11->Wrap( -1 );
	fgSizer4->Add( m_staticText11, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textLinkerName = new wxTextCtrl( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_textLinkerName, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText12 = new wxStaticText( m_panel2, wxID_ANY, wxT("Shared Object Linker:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText12->Wrap( -1 );
	fgSizer4->Add( m_staticText12, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textSOLinker = new wxTextCtrl( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_textSOLinker, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText10 = new wxStaticText( m_panel2, wxID_ANY, wxT("Archive Tool:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText10->Wrap( -1 );
	fgSizer4->Add( m_staticText10, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textArchiveTool = new wxTextCtrl( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_textArchiveTool, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText14 = new wxStaticText( m_panel2, wxID_ANY, wxT("Resource Compiler:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText14->Wrap( -1 );
	fgSizer4->Add( m_staticText14, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textResourceCmp = new wxTextCtrl( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_textResourceCmp, 0, wxALL|wxEXPAND, 5 );
	
	bSizer9->Add( fgSizer4, 1, wxEXPAND, 5 );
	
	m_panel2->SetSizer( bSizer9 );
	m_panel2->Layout();
	bSizer9->Fit( m_panel2 );
	m_notebook1->AddPage( m_panel2, wxT("Tools"), false );
	m_panel3 = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText8 = new wxStaticText( m_panel3, wxID_ANY, wxT("Switches:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText8->Wrap( -1 );
	bSizer3->Add( m_staticText8, 0, wxALL, 5 );
	
	m_listSwitches = new wxListCtrl( m_panel3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT );
	bSizer3->Add( m_listSwitches, 1, wxEXPAND|wxALL, 5 );
	
	m_panel3->SetSizer( bSizer3 );
	m_panel3->Layout();
	bSizer3->Fit( m_panel3 );
	m_notebook1->AddPage( m_panel3, wxT("Switches"), false );
	
	mainSizer->Add( m_notebook1, 1, wxEXPAND | wxALL, 5 );
	
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

