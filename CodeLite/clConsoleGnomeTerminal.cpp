#include "clConsoleGnomeTerminal.h"

#include "dirsaver.h"
#include "file_logger.h"
#include "fileutils.h"
#include "procutils.h"

#include <wx/tokenzr.h>
#include <wx/utils.h>

bool clConsoleGnomeTerminal::FindProcessByCommand(const wxString& name, wxString& tty, long& pid)
{
    clDEBUG() << "FindProcessByCommand is called";
    tty.Clear();
    pid = wxNOT_FOUND;

    // Run "ps -A -o pid,tty,command" to locate the terminal ID
    wxString psCommand;
    wxArrayString arrOutput;
    psCommand << "ps -A -o pid,tty,command";

    ProcUtils::SafeExecuteCommand(psCommand, arrOutput);

    for(size_t i = 0; i < arrOutput.GetCount(); ++i) {
        wxString curline = arrOutput.Item(i).Trim().Trim(false);
        wxArrayString tokens = ::wxStringTokenize(curline, " ", wxTOKEN_STRTOK);
        if(tokens.GetCount() < 3) {
            continue;
        }

        // replace tabs with spaces
        curline.Replace("\t", " ");

        // remove any duplicate spaces
        while(curline.Replace("  ", " ")) {}

        wxString tmp_pid = curline.BeforeFirst(' ');
        curline = curline.AfterFirst(' ');

        wxString tmp_tty = curline.BeforeFirst(' ');
        curline = curline.AfterFirst(' ');

        wxString command = curline; // the remainder
        command.Trim().Trim(false);

        if(command == name) {
            // we got our match
            tmp_tty = tmp_tty.AfterLast('/');
            tmp_tty.Prepend("/dev/pts/");
            tty = tmp_tty;
            tmp_pid.Trim().Trim(false).ToCLong(&pid);
            return true;
        }
    }
    return false;
}

clConsoleGnomeTerminal::clConsoleGnomeTerminal()
{
    SetTerminalCommand("gnome-terminal --working-directory=%WD% -e '%COMMAND%'");
    SetEmptyTerminalCommand("gnome-terminal --working-directory=%WD%");
}

clConsoleGnomeTerminal::~clConsoleGnomeTerminal() {}

bool clConsoleGnomeTerminal::Start() { return StartProcess(PrepareCommand()); }

bool clConsoleGnomeTerminal::StartForDebugger()
{
    // generate a random value to differntiate this instance of codelite
    // from other instances

    time_t curtime = time(NULL);
    int randomSeed = (curtime % 947);
    wxString secondsToSleep;

    secondsToSleep << (85765 + randomSeed);

    wxString sleepCommand = "/bin/sleep";
    sleepCommand << " " << secondsToSleep;

    wxString homedir = wxGetHomeDir();
    if(homedir.Contains(" ")) {
        homedir.Prepend("\"").Append("\"");
    }
    wxString commandToExecute = GetTerminalCommand();
    commandToExecute.Replace("%WD%", homedir);
    commandToExecute.Replace("%COMMAND%", sleepCommand);
    ::wxExecute(commandToExecute);

    // Let it start ... (wait for it up to 5 seconds)
    for(size_t i = 0; i < 100; ++i) {
        if(FindProcessByCommand(sleepCommand, m_tty, m_pid)) {
            // On GTK, redirection to TTY does not work with lldb
            // as a workaround, we create a symlink with different name

            // Keep the real tty
            m_realPts = m_tty;

            wxString symlinkName = m_tty;
            symlinkName.Replace("/dev/pts/", "/tmp/pts");
            wxString lnCommand;
            lnCommand << "ln -sf " << m_tty << " " << symlinkName;
            if(::system(lnCommand.mb_str(wxConvUTF8).data()) == 0) {
                m_tty.swap(symlinkName);
            }
            break;
        }
        wxThread::Sleep(50);
    }
    return !m_tty.IsEmpty();
}

wxString clConsoleGnomeTerminal::PrepareCommand()
{
    wxString commandToExecute;
    bool hasCommand = !GetCommand().IsEmpty();
    commandToExecute = hasCommand ? GetTerminalCommand() : GetEmptyTerminalCommand();
    if(!IsTerminalNeeded()) {
        commandToExecute = "%COMMAND%";
    }

    if(IsTerminalNeeded()) {
        // set the working directory
        wxString workingDirectory = WrapWithQuotesIfNeeded(GetWorkingDirectory());
        if(workingDirectory.IsEmpty()) {
            workingDirectory = ".";
        }
        commandToExecute.Replace("%WD%", workingDirectory);
    }

    if(hasCommand) {
        wxFileName scriptPath = PrepareExecScript();
        wxString rowCommand;
        rowCommand << "/bin/bash -f \"" << scriptPath.GetFullPath() << "\"";
        commandToExecute.Replace("%COMMAND%", rowCommand);
    }
    return commandToExecute;
}
