#include <wx/msgdlg.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include "breakpointpropertiesdlg.h"
#include "windowattrmanager.h"



BreakptPropertiesDlg::BreakptPropertiesDlg( wxWindow* parent )
		:	BreakpointPropertiesDlg( parent )
{
	its_a_breakpt = true; 			// Default to breakpoint for new items
	whichBreakcheck = wbc_line;	// and assume it'll be a line-number type
	WindowAttrManager::Load(this, wxT("BreakptPropertiesDlgAttr"), NULL);
}

BreakptPropertiesDlg::~BreakptPropertiesDlg()
{
	WindowAttrManager::Save(this, wxT("BreakptPropertiesDlgAttr"), NULL);
}

// Insert the data from an existing breakpoint into the dialog fields
void BreakptPropertiesDlg::EnterBPData( const BreakpointInfo &bp )
{
	// The class BreakpointInfo& b will become the new bp, so copy the old ids
	b.debugger_id = bp.debugger_id;
	b.internal_id = bp.internal_id;

	if (bp.bp_type == BP_type_watchpt) {
		its_a_breakpt = false;	// UpdateUI will then tick the checkbox
		m_radioWatchtype->SetSelection(bp.watchpoint_type);
		m_textWatchExpression->SetValue(bp.watchpt_data);
	} else {
		its_a_breakpt = true;
		if (bp.memory_address != -1) {
			m_checkBreakMemory->SetValue(true);
			m_textBreakMemory->Clear();
			*m_textBreakMemory << bp.memory_address;
		} else {
			m_textFilename->SetValue(bp.file);
			if (bp.lineno != -1) {
				m_checkLineno->SetValue(true);
				m_textLineno->Clear();
				*m_textLineno << bp.lineno;
			} else if (!bp.function_name.IsEmpty()) {
				m_checkBreakFunction->SetValue(true);
				m_textFunctionname->SetValue(bp.function_name);
				m_checkRegex->SetValue(bp.regex == true);
			}
		}
	}

	if (its_a_breakpt) {
		m_choicebook->SetSelection(0);
	} else {
		m_choicebook->SetSelection(1);
	}

	m_checkDisable->SetValue(!bp.is_enabled);
	m_checkTemp->SetValue(bp.is_temp);
	m_spinIgnore->SetValue(bp.ignore_number);
	m_textCond->SetValue(bp.conditions);
	m_textCommands->SetValue(bp.commandlist);
}

void BreakptPropertiesDlg::EndModal( int retCode )
{
	if (retCode != wxID_OK) {
		return wxDialog::EndModal(retCode);
	}

	long l;
	wxString contents;
	int selectedPage(wxNOT_FOUND);
	if (m_choicebook->GetPageText((size_t)selectedPage) == _("Watchpoint")) {
		b.bp_type = BP_type_watchpt;
		b.watchpoint_type = (WP_type)m_radioWatchtype->GetSelection();
		b.watchpt_data = m_textWatchExpression->GetValue();
	} else {
		// It's some flavour of breakpoint (assume standard for now). Only insert enabled data, in case a lineno sort is now a function bp
		b.bp_type = BP_type_break;
		switch (whichBreakcheck) {
		case wbc_line:
			contents = m_textLineno->GetValue();
			if ( ! contents.ToLong(&l, 0) ) {
				wxMessageBox(_("The breakpoint's line-number is invalid. Please try again."), _(":/"), wxICON_ERROR);
				return;
			}
			b.lineno = (int)l;
			b.file = m_textFilename->GetValue();
			break;

		case wbc_function:
			b.function_name = m_textFunctionname->GetValue();
			if (b.function_name.IsEmpty()) {
				wxMessageBox(_("You don't seem to have entered a name for the function. Please try again."), _(":/"), wxICON_ERROR);
				return;
			}
			b.regex = m_checkRegex->IsChecked();
			b.file = m_textFilename->GetValue();
			break;

		case wbc_memory:
			contents = m_textBreakMemory->GetValue();
			if ( ! contents.ToLong(&l, 0) ) {
				wxMessageBox(_("The breakpoint's memory address is invalid. Please try again."), _(":/"), wxICON_ERROR);
				return;
			}
			b.memory_address = (int)l;
			break;
		}
	}

	b.is_enabled = ! m_checkDisable->GetValue();
	b.is_temp = m_checkTemp->GetValue();
	b.ignore_number = m_spinIgnore->GetValue();
	b.conditions = m_textCond->GetValue();
	b.commandlist = m_textCommands->GetValue();

	wxDialog::EndModal(retCode);
}

void BreakptPropertiesDlg::OnCheckBreakLineno(wxCommandEvent& event)
{
	if (event.IsChecked()) {
		whichBreakcheck = wbc_line;
	}
}

void BreakptPropertiesDlg::OnCheckBreakFunction(wxCommandEvent& event)
{
	if (event.IsChecked()) {
		whichBreakcheck = wbc_function;
	}
}

void BreakptPropertiesDlg::OnCheckBreakMemory(wxCommandEvent& event)
{
	if (event.IsChecked()) {
		whichBreakcheck = wbc_memory;
	}
}

void BreakptPropertiesDlg::OnBrowse( wxCommandEvent& event )
{
	wxUnusedVar(event);
	wxString newfilepath, filepath(m_textFilename->GetValue());
	if ((!filepath.IsEmpty()) && wxFileName::FileExists(filepath)) {
		newfilepath = wxFileSelector(wxT("Select file:"), filepath.c_str());
	} else {
		newfilepath = wxFileSelector(wxT("Select file:"));
	}

	if (!newfilepath.IsEmpty()) {
		m_textFilename->SetValue(newfilepath);
	}
}

void BreakptPropertiesDlg::OnCheckBreakLinenoUI( wxUpdateUIEvent& event )
{
	m_checkLineno->SetValue(whichBreakcheck == wbc_line);
	m_textLineno->Enable(m_checkLineno->IsChecked() && m_checkLineno->IsEnabled());
}

void BreakptPropertiesDlg::OnCheckBreakFunctionUI( wxUpdateUIEvent& event )
{
	m_checkBreakFunction->SetValue(whichBreakcheck == wbc_function);
	m_textFunctionname->Enable(m_checkBreakFunction->IsChecked() && m_checkBreakFunction->IsEnabled());
	m_checkRegex->Enable(m_textFunctionname->IsEnabled());
}

void BreakptPropertiesDlg::OnCheckBreakMemoryUI( wxUpdateUIEvent& event )
{
	m_checkBreakMemory->SetValue(whichBreakcheck == wbc_memory);
	m_textBreakMemory->Enable(m_checkBreakMemory->IsChecked() && m_checkBreakMemory->IsEnabled());
	// Disable the filename textctrl if the memory check is ticked
	m_textFilename->Enable(! m_textBreakMemory->IsEnabled());
}

void BreakptPropertiesDlg::OnPageChanged(wxChoicebookEvent& event)
{
	its_a_breakpt = m_choicebook->GetPageText(m_choicebook->GetSelection()) == _("Breakpoint");
}
