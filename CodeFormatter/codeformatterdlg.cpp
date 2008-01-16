#include "codeformatter.h"
#include "codeformatterdlg.h"
			   
CodeFormatterDlg::CodeFormatterDlg( wxWindow* parent, CodeFormatter *cf, size_t flags, const wxString &sampleCode )
: CodeFormatterBaseDlg( parent )
, m_cf(cf)
, m_sampleCode(sampleCode)
{
	m_options.SetOption(flags);
	m_buttonOK->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CodeFormatterDlg::OnOK), NULL, this);
	m_buttonHelp->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CodeFormatterDlg::OnHelp), NULL, this);
	
	// Initialise dialog
	m_textCtrlPreview->SetValue(m_sampleCode);
	InitDialog();
	GetSizer()->Fit(this);
	UpdatePreview();
}

void CodeFormatterDlg::UpdateCheckBox(wxCheckBox *obj, size_t flag)
{
	obj->SetValue(m_options.GetOptions() & flag ? true : false);
}

void CodeFormatterDlg::InitDialog()
{
	UpdateCheckBox(m_checkBoxFormatBreakBlocks,AS_BREAK_BLOCKS);
	UpdateCheckBox(m_checkBoxFormatBreakBlocksAll,AS_BREAK_BLOCKS_ALL);
	UpdateCheckBox(m_checkBoxFormatBreakElseif,AS_BREAK_ELSEIF);
	UpdateCheckBox(m_checkBoxFormatFillEmptyLines,AS_FILL_EMPTY_LINES);
	UpdateCheckBox(m_checkBoxFormatOneLineKeepBlocks,AS_ONE_LINE_KEEP_BLOCKS);
	UpdateCheckBox(m_checkBoxFormatOneLineKeepStmnt,AS_ONE_LINE_KEEP_STATEMENT);
	UpdateCheckBox(m_checkBoxFormatPadOperators,AS_PAD_OPER);
	UpdateCheckBox(m_checkBoxFormatPadParenth,AS_PAD_PARENTHESIS);
	UpdateCheckBox(m_checkBoxFormatPadParentIn,AS_PAD_PARENTHESIS_IN);
	UpdateCheckBox(m_checkBoxFormatPadParentOut,AS_PAD_PARENTHESIS_OUT);
	UpdateCheckBox(m_checkBoxFormatUnPadParent,AS_UNPAD_PARENTHESIS);
	UpdateCheckBox(m_checkBoxIndentBrackets,AS_INDENT_BRACKETS);
	UpdateCheckBox(m_checkBoxIndentLabels,AS_INDENT_LABELS);
	UpdateCheckBox(m_checkBoxIndentMaxInst,AS_MAX_INSTATEMENT_INDENT);
	UpdateCheckBox(m_checkBoxIndentMinCond,AS_MIN_COND_INDENT);
	UpdateCheckBox(m_checkBoxIndentNamespaces,AS_INDENT_NAMESPACES);
	UpdateCheckBox(m_checkBoxIndentPreprocessors,AS_INDENT_PREPROCESSORS);
	UpdateCheckBox(m_checkBoxIndetBlocks,AS_INDENT_BLOCKS);
	UpdateCheckBox(m_checkBoxIndetCase,AS_INDENT_CASE);
	UpdateCheckBox(m_checkBoxIndetClass,AS_INDENT_CASE);
	UpdateCheckBox(m_checkBoxIndetSwitch,AS_INDENT_SWITCHES);

	//update the two radio box controls
	int selection(3);	//AS_LINUX
	if(m_options.GetOptions() & AS_LINUX){
		selection = 3;
	}else if(m_options.GetOptions() & AS_GNU){
		selection =0;
	}else if(m_options.GetOptions() & AS_ANSI){
		selection = 4;
	}else if(m_options.GetOptions() & AS_JAVA){
		selection = 1;
	}else if(m_options.GetOptions() & AS_KR){
		selection = 2;
	}
	m_radioBoxPredefinedStyle->SetSelection(selection);

	selection = 2;//AS_BRACKETS_LINUX
	if(m_options.GetOptions() & AS_BRACKETS_BREAK_CLOSING){
		selection = 0;
	}else if(m_options.GetOptions() & AS_BRACKETS_ATTACH){
		selection = 1;
	}else if(m_options.GetOptions() & AS_BRACKETS_LINUX){
		selection = 2;
	}else if(m_options.GetOptions() & AS_BRACKETS_BREAK){
		selection = 3;
	}
	m_radioBoxBrackets->SetSelection(selection);
}
	
void CodeFormatterDlg::OnRadioBoxPredefinedStyle( wxCommandEvent& event )
{
	int sel = event.GetSelection();

	//remove all predefined styles
	size_t options = m_options.GetOptions();
	options &= ~(AS_ANSI);//4
	options &= ~(AS_GNU); //0
	options &= ~(AS_LINUX);//3  
	options &= ~(AS_JAVA);//1
	options &= ~(AS_KR);//2

	size_t flag = AS_LINUX;
	switch(sel){
		case 0: flag = AS_GNU; break;
		case 1: flag = AS_JAVA; break;
		case 2: flag = AS_KR; break;
		case 3: flag = AS_LINUX; break;
		case 4: flag = AS_ANSI; break;
		default: flag = AS_LINUX; break;
	}
	m_options.SetOption(options | flag);
	UpdatePreview();
}

void CodeFormatterDlg::OnRadioBoxBrackets( wxCommandEvent& event )
{
	int sel = event.GetSelection();

	//remove all predefined styles
	size_t options = m_options.GetOptions();
	options &= ~(AS_BRACKETS_BREAK_CLOSING);//0
	options &= ~(AS_BRACKETS_ATTACH); //1
	options &= ~(AS_BRACKETS_LINUX);//2
	options &= ~(AS_BRACKETS_BREAK);//3

	size_t flag = AS_LINUX;
	switch(sel){
		case 0: flag = AS_BRACKETS_BREAK_CLOSING; break;
		case 1: flag = AS_BRACKETS_ATTACH; break;
		case 2: flag = AS_BRACKETS_LINUX; break;
		case 3: flag = AS_BRACKETS_BREAK; break;
	}
	m_options.SetOption(options | flag);
	UpdatePreview();
}

void CodeFormatterDlg::OnCheckBox( wxCommandEvent& event )
{
	size_t flag(0);
	wxObject *obj = event.GetEventObject();
	if(obj == m_checkBoxFormatBreakBlocks){
		flag = AS_BREAK_BLOCKS;
	}else if(obj == m_checkBoxFormatBreakBlocksAll){
		flag = AS_BREAK_BLOCKS_ALL;
	}else if(obj == m_checkBoxFormatBreakElseif){
		flag = AS_BREAK_ELSEIF;
	}else if(obj == m_checkBoxFormatFillEmptyLines){
		flag = AS_FILL_EMPTY_LINES;
	}else if(obj == m_checkBoxFormatOneLineKeepBlocks){
		flag = AS_ONE_LINE_KEEP_BLOCKS;
	}else if(obj == m_checkBoxFormatOneLineKeepStmnt){
		flag = AS_ONE_LINE_KEEP_STATEMENT;
	}else if(obj == m_checkBoxFormatPadOperators){
		flag = AS_PAD_OPER;
	}else if(obj == m_checkBoxFormatPadParenth){
		flag = AS_PAD_PARENTHESIS;
	}else if(obj == m_checkBoxFormatPadParentIn){
		flag = AS_PAD_PARENTHESIS_IN;
	}else if(obj == m_checkBoxFormatPadParentOut){
		flag = AS_PAD_PARENTHESIS_OUT;
	}else if(obj == m_checkBoxFormatUnPadParent){
		flag = AS_UNPAD_PARENTHESIS;
	}else if(obj == m_checkBoxIndentBrackets){
		flag = AS_INDENT_BRACKETS;
	}else if(obj == m_checkBoxIndentLabels){
		flag = AS_INDENT_LABELS;
	}else if(obj == m_checkBoxIndentMaxInst){
		flag = AS_MAX_INSTATEMENT_INDENT;
	}else if(obj == m_checkBoxIndentMinCond){
		flag = AS_MIN_COND_INDENT;
	}else if(obj == m_checkBoxIndentNamespaces){
		flag = AS_INDENT_NAMESPACES;
	}else if(obj == m_checkBoxIndentPreprocessors){
		flag = AS_INDENT_PREPROCESSORS;
	}else if(obj == m_checkBoxIndetBlocks){
		flag = AS_INDENT_BLOCKS;
	}else if(obj == m_checkBoxIndetCase){
		flag = AS_INDENT_CASE;
	}else if(obj == m_checkBoxIndetClass){
		flag = AS_INDENT_CASE;
	}else if(obj == m_checkBoxIndetSwitch){
		flag = AS_INDENT_SWITCHES;
	}

	size_t options = m_options.GetOptions();
	EnableFlag(options, flag, event.IsChecked());
	m_options.SetOption(options);
	UpdatePreview();
}

void CodeFormatterDlg::EnableFlag(size_t &options, size_t flag, bool enable)
{
	if(enable){
		options |= flag;
	}else{
		options &= ~(flag);
	}
}

void CodeFormatterDlg::OnOK(wxCommandEvent &e)
{
	wxUnusedVar(e);
	//Save the options
	EndModal(wxID_OK);
}

void CodeFormatterDlg::OnHelp(wxCommandEvent &e)
{
	wxUnusedVar(e);
	static wxString helpUrl(wxT("http://astyle.sourceforge.net/astyle.html"));
	wxLaunchDefaultBrowser(helpUrl);
}

void CodeFormatterDlg::UpdatePreview()
{
	wxString output;
	m_cf->AstyleFormat(m_sampleCode, m_options.ToString(), output);
	m_textCtrlPreview->SetValue(output);
}
