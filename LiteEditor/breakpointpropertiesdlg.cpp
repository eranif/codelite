//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : breakpointpropertiesdlg.cpp
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

#include <wx/msgdlg.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include "breakpointpropertiesdlg.h"
#include "windowattrmanager.h"

BreakptPropertiesDlg::BreakptPropertiesDlg(wxWindow* parent)
    : BreakpointPropertiesDlgBase(parent)
{
    its_a_breakpt = true;       // Default to breakpoint for new items
    whichBreakcheck = wbc_line; // and assume it'll be a line-number type
    SetName("BreakptPropertiesDlg");
    WindowAttrManager::Load(this);
    CentreOnParent();
}

BreakptPropertiesDlg::~BreakptPropertiesDlg() {}

// Insert the data from an existing breakpoint into the dialog fields
void BreakptPropertiesDlg::EnterBPData(const BreakpointInfo& bp)
{
    // The class BreakpointInfo& b will become the new bp, so copy the old ids
    b.debugger_id = bp.debugger_id;
    b.internal_id = bp.internal_id;

    if(bp.bp_type == BP_type_watchpt) {
        its_a_breakpt = false; // UpdateUI will then tick the checkbox
        m_radioWatchtype->SetSelection(bp.watchpoint_type);
        m_textWatchExpression->SetValue(bp.watchpt_data);
    } else {
        its_a_breakpt = true;
        if(bp.memory_address.IsEmpty() == false) {
            m_checkBreakMemory->SetValue(true);
            m_textBreakMemory->Clear();
            *m_textBreakMemory << bp.memory_address;
            whichBreakcheck = wbc_memory;
        } else {
            m_textFilename->SetValue(bp.file);
            if(bp.lineno != -1) {
                m_checkLineno->SetValue(true);
                m_textLineno->Clear();
                *m_textLineno << bp.lineno;
                whichBreakcheck = wbc_line;
            } else if(!bp.function_name.IsEmpty()) {
                m_checkBreakFunction->SetValue(true);
                m_textFunctionname->SetValue(bp.function_name);
                m_checkRegex->SetValue(bp.regex == true);
                whichBreakcheck = wbc_function;
            }
        }
    }

    // We need temporarily to Disconnect this event, otherwise it's impossible to edit a watchpoint while the debugger's
    // running
    // as SetSelection(1) causes a wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGING event, which is vetoed
    m_choicebook->Disconnect(wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGING,
                             wxChoicebookEventHandler(BreakptPropertiesDlg::OnPageChanging),
                             NULL,
                             this);
    if(its_a_breakpt) {
        m_choicebook->SetSelection(0);
    } else {
        m_choicebook->SetSelection(1);
    }
    m_choicebook->Connect(wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGING,
                          wxChoicebookEventHandler(BreakptPropertiesDlg::OnPageChanging),
                          NULL,
                          this);

    m_checkDisable->SetValue(!bp.is_enabled);
    m_checkTemp->SetValue(bp.is_temp);
    m_checkTemp->Show(its_a_breakpt); // Watchpoints can't be temporary
    m_spinIgnore->SetValue(bp.ignore_number);
    m_textCond->SetValue(bp.conditions);
    m_textCommands->SetValue(bp.commandlist);
}

void BreakptPropertiesDlg::EndModal(int retCode)
{
    if(retCode != wxID_OK) {
        return wxDialog::EndModal(retCode);
    }

    long l;
    wxString contents;
    int selectedPage = m_choicebook->GetSelection();
    if(m_choicebook->GetPageText((size_t)selectedPage) == _("Watchpoint")) {
        b.bp_type = BP_type_watchpt;
        b.watchpoint_type = (WatchpointType)m_radioWatchtype->GetSelection();
        b.watchpt_data = m_textWatchExpression->GetValue();
        if(b.watchpt_data.IsEmpty()) {
            wxMessageBox(_("You don't seem to have entered a variable for the watchpoint to watch. Please try again."),
                         wxT(":/"),
                         wxICON_ERROR);
            return;
        }
    } else {

        // reset the fields to only the proper ones are copied
        b.lineno = wxNOT_FOUND;
        b.memory_address.Clear();
        b.file.Clear();
        b.function_name.Clear();

        // It's some flavour of breakpoint (assume standard for now). Only insert enabled data, in case a lineno sort is
        // now a function bp
        b.bp_type = BP_type_break;
        switch(whichBreakcheck) {
        case wbc_line:
            contents = m_textLineno->GetValue();
            if(!contents.ToLong(&l, 0)) {
                wxMessageBox(_("The breakpoint's line-number is invalid. Please try again."), _(":/"), wxICON_ERROR);
                return;
            }
            b.lineno = (int)l;
            b.file = m_textFilename->GetValue();
            break;

        case wbc_function:
            b.function_name = m_textFunctionname->GetValue();
            if(b.function_name.IsEmpty()) {
                wxMessageBox(_("You don't seem to have entered a name for the function. Please try again."),
                             _(":/"),
                             wxICON_ERROR);
                return;
            }
            b.regex = m_checkRegex->IsChecked();
            b.file = m_textFilename->GetValue();
            break;

        case wbc_memory:
            contents = m_textBreakMemory->GetValue();
            b.memory_address = contents;
            break;
        }
    }

    b.is_enabled = !m_checkDisable->GetValue();
    b.is_temp = m_checkTemp->GetValue();
    b.ignore_number = m_spinIgnore->GetValue();
    b.conditions = m_textCond->GetValue();
    b.commandlist = m_textCommands->GetValue();

    wxDialog::EndModal(retCode);
}

void BreakptPropertiesDlg::OnCheckBreakLineno(wxCommandEvent& event)
{
    if(event.IsChecked()) {
        whichBreakcheck = wbc_line;
    }
}

void BreakptPropertiesDlg::OnCheckBreakFunction(wxCommandEvent& event)
{
    if(event.IsChecked()) {
        whichBreakcheck = wbc_function;
    }
}

void BreakptPropertiesDlg::OnCheckBreakMemory(wxCommandEvent& event)
{
    if(event.IsChecked()) {
        whichBreakcheck = wbc_memory;
    }
}

void BreakptPropertiesDlg::OnBrowse(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxString newfilepath, filepath(m_textFilename->GetValue());
    if((!filepath.IsEmpty()) && wxFileName::FileExists(filepath)) {
        newfilepath = wxFileSelector(_("Select file:"), filepath.c_str());
    } else {
        newfilepath = wxFileSelector(_("Select file:"));
    }

    if(!newfilepath.IsEmpty()) {
        m_textFilename->SetValue(newfilepath);
    }
}

void BreakptPropertiesDlg::OnCheckBreakLinenoUI(wxUpdateUIEvent& event)
{
    m_checkLineno->SetValue(whichBreakcheck == wbc_line);
    m_textLineno->Enable(m_checkLineno->IsChecked() && m_checkLineno->IsEnabled());
}

void BreakptPropertiesDlg::OnCheckBreakFunctionUI(wxUpdateUIEvent& event)
{
    m_checkBreakFunction->SetValue(whichBreakcheck == wbc_function);
    m_textFunctionname->Enable(m_checkBreakFunction->IsChecked() && m_checkBreakFunction->IsEnabled());
    m_checkRegex->Enable(m_textFunctionname->IsEnabled());
}

void BreakptPropertiesDlg::OnCheckBreakMemoryUI(wxUpdateUIEvent& event)
{
    m_checkBreakMemory->SetValue(whichBreakcheck == wbc_memory);
    m_textBreakMemory->Enable(m_checkBreakMemory->IsChecked() && m_checkBreakMemory->IsEnabled());
    // Disable the filename textctrl if the memory check is ticked
    m_textFilename->Enable(!m_textBreakMemory->IsEnabled());
}

void BreakptPropertiesDlg::OnPageChanging(wxChoicebookEvent& event)
{
    if(b.debugger_id != -1) {
        wxMessageBox(
            _("Sorry, you can't change a breakpoint to a watchpoint, or vice versa, while the debugger is running"),
            _("Not possible"),
            wxICON_ERROR | wxOK);
        event.Veto();
    }
}

void BreakptPropertiesDlg::OnPageChanged(wxChoicebookEvent& event)
{
    its_a_breakpt = m_choicebook->GetPageText(m_choicebook->GetSelection()) == _("Breakpoint");
    // Watchpoints can't have conditions set direct in MI; they need to be created without, then edited to add
    m_textCond->Enable(its_a_breakpt || GetTitle().StartsWith("Properties")); // The dlg title starts with Properties when editing
    if (!GetTitle().StartsWith("Properties")) {
        wxString tip =  (its_a_breakpt ?
                            "You can add a condition to any breakpoint. The debugger will then stop only if the condition is met.\n \
The condition can be any simple or complex expression in your programming language,providing it returns a bool. However any variables that you use must be in scope.\n \
If you've previously set a condition and no longer want it, just clear this textctrl." :
                        "It is not possible to Add a conditional watchpoint. Create a normal watchpoint first, then Edit it to add the conditon"
                        );
        m_textCond->SetToolTip(tip);
    }
    m_checkTemp->Show(its_a_breakpt); // Watchpoints can't be temporary
    m_checkTemp->GetContainingSizer()->Layout();
}
