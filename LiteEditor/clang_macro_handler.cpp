//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : clang_macro_handler.cpp
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

#include "clang_macro_handler.h"
#include "event_notifier.h"
#include "y.tab.h"
#include "cpp_scanner.h"
#include "plugin.h"
#include <wx/xrc/xmlres.h>
#include "processreaderthread.h"
#include <wx/regex.h>
#include "file_logger.h"
#include "globals.h"
#include <wx/stc/stc.h>

const wxEventType wxEVT_CMD_CLANG_MACRO_HADNLER_DELETE = XRCID("wxEVT_CMD_CLANG_MACRO_HADNLER_DELETE");

BEGIN_EVENT_TABLE(ClangMacroHandler, wxEvtHandler)
    EVT_COMMAND(wxID_ANY, wxEVT_PROC_TERMINATED, ClangMacroHandler::OnClangProcessTerminated)
    EVT_COMMAND(wxID_ANY, wxEVT_PROC_DATA_READ,  ClangMacroHandler::OnClangProcessOutput)
END_EVENT_TABLE()

ClangMacroHandler::ClangMacroHandler()
    : m_process(NULL)
    , m_editor(NULL)
{
    EventNotifier::Get()->Connect(wxEVT_EDITOR_CLOSING,      wxCommandEventHandler(ClangMacroHandler::OnEditorClosing), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_ALL_EDITORS_CLOSING, wxCommandEventHandler(ClangMacroHandler::OnAllEditorsClosing), NULL, this);
}

ClangMacroHandler::~ClangMacroHandler()
{
    EventNotifier::Get()->Disconnect(wxEVT_EDITOR_CLOSING,      wxCommandEventHandler(ClangMacroHandler::OnEditorClosing), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_ALL_EDITORS_CLOSING, wxCommandEventHandler(ClangMacroHandler::OnAllEditorsClosing), NULL, this);
}

void ClangMacroHandler::OnClangProcessOutput(wxCommandEvent& e)
{
    ProcessEventData *ped = (ProcessEventData *)e.GetClientData();
    m_output << ped->GetData();
    delete ped;
}

void ClangMacroHandler::OnClangProcessTerminated(wxCommandEvent& e)
{
    ProcessEventData *ped = (ProcessEventData *)e.GetClientData();
    delete ped;
    if( m_process ) {
        delete m_process;
        m_process = NULL;
    }

    wxString macrosAsString;
    wxArrayString lines = ::wxStringTokenize(m_output, wxT("\n\r"), wxTOKEN_STRTOK);
    for(size_t i=0; i<lines.GetCount(); i++) {
        wxString rest;
        if(lines.Item(i).StartsWith(wxT("MACRO:"), &rest)) {
            rest.Trim().Trim(false);
            macrosAsString << wxT(" ") << rest;
        }
    }

    // Process the output here...
    CL_DEBUG(wxT("ClangMacroHandler: Macros collected: %s"), macrosAsString.c_str());

    if(m_editor) {
        // Scintilla preprocessor management
        m_editor->GetSTC()->SetProperty(wxT("lexer.cpp.track.preprocessor"),  wxT("1"));
        m_editor->GetSTC()->SetProperty(wxT("lexer.cpp.update.preprocessor"), wxT("1"));
        m_editor->GetSTC()->SetKeyWords(4, macrosAsString);
        m_editor->GetSTC()->Colourise(0, wxSTC_INVALID_POSITION);
    }

    wxCommandEvent evt(wxEVT_CMD_CLANG_MACRO_HADNLER_DELETE);
    evt.SetClientData(this);
    EventNotifier::Get()->AddPendingEvent(evt);
}

void ClangMacroHandler::Cancel()
{
    m_editor = NULL;
}

void ClangMacroHandler::OnAllEditorsClosing(wxCommandEvent& e)
{
    e.Skip();
    Cancel();
}

void ClangMacroHandler::OnEditorClosing(wxCommandEvent& e)
{
    e.Skip();
    IEditor *editor = reinterpret_cast<IEditor*>(e.GetClientData());
    if(editor && editor == m_editor) {
        Cancel();
    }
}

void ClangMacroHandler::SetProcessAndEditor(IProcess* process, IEditor* editor)
{
    this->m_process = process;
    this->m_editor  = editor;

    if(m_editor) {
        m_editor->GetSTC()->SetProperty(wxT("lexer.cpp.track.preprocessor"),  wxT("0"));
        m_editor->GetSTC()->SetProperty(wxT("lexer.cpp.update.preprocessor"), wxT("0"));
        m_editor->GetSTC()->SetKeyWords(4, wxT(""));
        m_editor->GetSTC()->Colourise(0, wxSTC_INVALID_POSITION);
    }
}
