//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : svn_console.cpp
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

#include "Notebook.h"
#include "editor_config.h"
#include "environmentconfig.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "globals.h"
#include "lexer_configuration.h"
#include "processreaderthread.h"
#include "subversion2.h"
#include "subversion_strings.h"
#include "svn_console.h"
#include <wx/app.h>
#include <wx/aui/framemanager.h>
#include <wx/regex.h>
#include <wx/settings.h>
#include <wx/textdlg.h>
#include <wx/tokenzr.h>
#include <wx/xrc/xmlres.h>
#include "ColoursAndFontsManager.h"

//-------------------------------------------------------------
SvnConsole::SvnConsole(wxStyledTextCtrl* stc, Subversion2* plugin)
    : m_sci(stc)
    , m_process(NULL)
    , m_plugin(plugin)
    , m_inferiorEnd(0)
{
    m_sci->SetLexer(wxSTC_LEX_NULL);
    m_sci->StyleClearAll();
    m_sci->SetUndoCollection(false); // dont allow undo/redo
    DoInitializeFontsAndColours();

    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &SvnConsole::OnReadProcessOutput, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &SvnConsole::OnProcessEnd, this);
}

SvnConsole::~SvnConsole() {}

void SvnConsole::OnReadProcessOutput(clProcessEvent& event)
{
    m_output.Append(event.GetOutput());

    wxString s = event.GetOutput().Lower();
    if(m_currCmd.printProcessOutput) AppendText(event.GetOutput());

    static wxRegEx reUsername("username[ \t]*:", wxRE_DEFAULT | wxRE_ICASE);
    wxArrayString lines = wxStringTokenize(s, wxT("\n"), wxTOKEN_STRTOK);
    if(!lines.IsEmpty() && lines.Last().StartsWith(wxT("password for '"))) {
        m_output.Clear();
        wxString pass = wxGetPasswordFromUser(event.GetOutput(), wxT("Subversion"));
        if(!pass.IsEmpty() && m_process) {
            m_process->WriteToConsole(pass);
        }
    } else if(!lines.IsEmpty() && reUsername.IsValid() && reUsername.Matches(lines.Last())) {
        // Prompt the user for "Username:"
        wxString username = ::wxGetTextFromUser(event.GetOutput(), "Subversion");
        if(!username.IsEmpty() && m_process) {
            m_process->Write(username + "\n");
        }
    }
}

void SvnConsole::OnProcessEnd(clProcessEvent& event)
{
    wxDELETE(m_process);
    if(m_currCmd.handler) {
        // command ended successfully, invoke the "success" callback
        m_currCmd.handler->Process(m_output);
        AppendText(wxT("-----\n"));
        delete m_currCmd.handler;
    }

    // do we have more commands to process?
    if(!m_queue.empty()) {
        DoProcessNextCommand();

    } else {
        // Do some cleanup
        m_output.Clear();
        m_url.Clear();
        m_currCmd.clean();
    }
}

void SvnConsole::ExecuteURL(const wxString& cmd, const wxString& url, SvnCommandHandler* handler,
                            bool printProcessOutput)
{
    DoExecute(cmd, handler, wxT(""), printProcessOutput);
    m_url = url;
}

void SvnConsole::Execute(const wxString& cmd, const wxString& workingDirectory, SvnCommandHandler* handler,
                         bool printProcessOutput, bool showConsole)
{
    DoExecute(cmd, handler, workingDirectory, printProcessOutput, showConsole);
}

void SvnConsole::AppendText(const wxString& text)
{
    // Make sure that the carete is at the end
    m_sci->SetSelectionEnd(m_sci->GetLength());
    m_sci->SetSelectionStart(m_sci->GetLength());
    m_sci->SetCurrentPos(m_sci->GetLength());

    // Append the text
    m_sci->AppendText(text);

    m_sci->SetSelectionEnd(m_sci->GetLength());
    m_sci->SetSelectionStart(m_sci->GetLength());
    m_sci->SetCurrentPos(m_sci->GetLength());

    m_sci->EnsureCaretVisible();
    m_inferiorEnd = m_sci->GetLength();
}

void SvnConsole::Clear()
{
    m_sci->ClearAll();
    m_inferiorEnd = 0;

    DoInitializeFontsAndColours();
}

void SvnConsole::Stop()
{
    if(m_process) {
        delete m_process;
        m_process = NULL;
    }
    AppendText(_("Aborted.\n"));
    AppendText(wxT("--------\n"));
}

bool SvnConsole::IsRunning() { return m_process != NULL; }

bool SvnConsole::IsEmpty() { return m_sci->GetText().IsEmpty(); }

void SvnConsole::EnsureVisible() { m_plugin->EnsureVisible(); }

void SvnConsole::DoProcessNextCommand()
{
    // If another process is running, we try again when the process is termianted
    if(m_process) {
        return;
    }

    if(m_queue.empty()) return;

    // Remove the command from the queue
    SvnConsoleCommand* command = m_queue.front();
    m_queue.pop_front();
    m_output.Clear();

    m_currCmd.clean();
    m_currCmd = *command;

    // Delete it
    wxDELETE(command);

    EnsureVisible();

    // Print the command?
    AppendText("[" + m_currCmd.workingDirectory + "] " + m_currCmd.cmd + wxT("\n"));

    // Wrap the command in the OS Shell
    wxString cmdShell(m_currCmd.cmd);

    // Apply the environment variables before executing the command
    wxStringMap_t om;
    om.insert(std::make_pair("LC_ALL", "C"));

    bool useOverrideMap = m_plugin->GetSettings().GetFlags() & SvnUsePosixLocale;
    EnvSetter env(m_plugin->GetManager()->GetEnv(), useOverrideMap ? &om : NULL);

    clDEBUG() << "Running svn command:" << cmdShell << clEndl;

    m_process = CreateAsyncProcess(this, cmdShell,
                                   m_currCmd.showConsole ? IProcessCreateConsole : IProcessCreateWithHiddenConsole,
                                   m_currCmd.workingDirectory);
    if(!m_process) {
        AppendText(_("Failed to launch Subversion client.\n"));
        return;
    }
    m_sci->SetFocus();
}

void SvnConsole::DoExecute(const wxString& cmd, SvnCommandHandler* handler, const wxString& workingDirectory,
                           bool printProcessOutput, bool showConsole)
{
    SvnConsoleCommand* consoleCommand = new SvnConsoleCommand();
    consoleCommand->cmd = cmd.c_str();
    consoleCommand->handler = handler;
    consoleCommand->printProcessOutput = printProcessOutput;
    consoleCommand->workingDirectory = workingDirectory.c_str();
    consoleCommand->showConsole = showConsole;
    m_queue.push_back(consoleCommand);

    DoProcessNextCommand();
}

void SvnConsole::OnCharAdded(wxStyledTextEvent& event)
{
    if(event.GetKey() == '\n') {
        // an enter was inserted
        // take the last inserted line and send it to the m_process
        wxString line = m_sci->GetTextRange(m_inferiorEnd, m_sci->GetLength());
        line.Trim();

        if(m_process) {
            m_process->Write(line);
        }
    }
    event.Skip();
}

void SvnConsole::OnUpdateUI(wxStyledTextEvent& event)
{
    // if(m_sci->GetCurrentPos() < m_inferiorEnd) {
    //	m_sci->SetCurrentPos(m_inferiorEnd);
    //	m_sci->SetSelectionStart(m_inferiorEnd);
    //	m_sci->SetSelectionEnd(m_inferiorEnd);
    //	m_sci->EnsureCaretVisible();
    //}
    event.Skip();
}

void SvnConsole::OnKeyDown(wxKeyEvent& event) { event.Skip(); }

void SvnConsole::DoInitializeFontsAndColours()
{
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("text");
    if(lexer) { lexer->Apply(m_sci); }
}
