#include "clConsoleBase.h"

#include "StdToWX.h"
#include "clConsoleAlacritty.hpp"
#include "clConsoleCMD.h"
#include "clConsoleGnomeTerminal.h"
#include "clConsoleKitty.hpp"
#include "clConsoleKonsole.h"
#include "clConsoleLXTerminal.h"
#include "clConsoleMateTerminal.h"
#include "clConsoleOSXTerminal.h"
#include "clConsoleQTerminal.h"
#include "clConsoleRXVTerminal.h"
#include "clConsoleXfce4Terminal.h"
#include "cl_config.h"
#include "file_logger.h"
#include "fileutils.h"

#include <algorithm>
#include <wx/utils.h>

wxDEFINE_EVENT(wxEVT_TERMINAL_EXIT, clProcessEvent);

class ConsoleProcess : public wxProcess
{
public:
    wxEvtHandler* m_sink = nullptr;
    wxString m_uid;

public:
    ConsoleProcess(wxEvtHandler* sink, const wxString& uid)
        : m_sink(sink)
        , m_uid(uid)
    {
    }

    virtual ~ConsoleProcess() { m_sink = NULL; }
    void OnTerminate(int pid, int status)
    {
        clProcessEvent terminateEvent(wxEVT_TERMINAL_EXIT);
        terminateEvent.SetString(m_uid);
        terminateEvent.SetInt(status); // pass the exit code
        m_sink->AddPendingEvent(terminateEvent);
        delete this;
    }
};

clConsoleBase::clConsoleBase() {}

clConsoleBase::~clConsoleBase() {}

clConsoleBase::Ptr_t clConsoleBase::GetTerminal()
{
    clConsoleBase::Ptr_t terminal;
    wxString terminalName = GetSelectedTerminalName();
#ifdef __WXMSW__
    if(terminalName.CmpNoCase("alacritty") == 0) {
        terminal.reset(new clConsoleAlacritty());
    } else {
        terminal.reset(new clConsoleCMD());
    }
#elif defined(__WXGTK__)
    if(terminalName.CmpNoCase("konsole") == 0) {
        terminal.reset(new clConsoleKonsole());
    } else if(terminalName.CmpNoCase("alacritty") == 0) {
        terminal.reset(new clConsoleAlacritty());
    } else if(terminalName.CmpNoCase("kitty") == 0) {
        terminal.reset(new clConsoleKitty());
    } else if(terminalName.CmpNoCase("lxterminal") == 0) {
        terminal.reset(new clConsoleLXTerminal());
    } else if(terminalName.CmpNoCase("mate-terminal") == 0) {
        terminal.reset(new clConsoleMateTerminal());
    } else if(terminalName.CmpNoCase("xfce4-terminal") == 0) {
        terminal.reset(new clConsoleXfce4Terminal());
    } else if(terminalName.CmpNoCase("qterminal") == 0) {
        terminal.reset(new clConsoleQTerminal());
    } else if(terminalName.CmpNoCase("rxvt-unicode") == 0) {
        terminal.reset(new clConsoleRXVTTerminal());
    } else {
        // the default terminal is "gnome-terminal"
        terminal.reset(new clConsoleGnomeTerminal());
    }
#else
    clConsoleOSXTerminal* t = new clConsoleOSXTerminal();
    terminal.reset(t);
    if(terminalName.CmpNoCase("iTerm2") == 0) {
        t->SetTerminalApp("iTerm");
    } else if(terminalName.CmpNoCase("alacritty") == 0) {
        terminal.reset(new clConsoleAlacritty());
    } else if(terminalName.CmpNoCase("kitty") == 0) {
        terminal.reset(new clConsoleKitty());
    }
#endif
    return terminal;
}

wxArrayString clConsoleBase::GetAvailableTerminals()
{
    return StdToWX::ToArrayString({
#ifdef __WXMSW__
        "CMD",
#elif defined(__WXGTK__)
        "konsole", "gnome-terminal", "lxterminal", "mate-terminal", "qterminal", "xfce4-terminal", "rxvt-unicode",
        "Kitty",
#else
        "Terminal", "iTerm2", "Kitty",
#endif
        "alacritty" });
}

void clConsoleBase::AddEnvVariable(const wxString& name, const wxString& value)
{
    m_environment.erase(name);
    m_environment.insert({ name, value });
}

wxString clConsoleBase::GetEnvironmentPrefix() const
{
    wxString strline;
    std::for_each(m_environment.begin(), m_environment.end(),
                  [&](const wxStringMap_t::value_type& vt) { strline << vt.first << "=" << vt.second << " "; });
    return strline;
}

wxString clConsoleBase::WrapWithQuotesIfNeeded(const wxString& s) const
{
    wxString strimmed = s;
    strimmed.Trim().Trim(false);
    if(strimmed.Contains(" ")) {
        strimmed.Prepend("\"").Append("\"");
    }
    return strimmed;
}

wxString clConsoleBase::EscapeString(const wxString& str, const wxString& c) const
{
    wxString escaped = str;
    escaped.Replace(c, wxString() << "\\" << c);
    return escaped;
}

bool clConsoleBase::StartProcess(const wxString& command)
{
    // Apply the environment variables before we launch the process
    clConsoleEnvironment env(GetEnvironment());
    env.Apply();

    wxProcess* callback = nullptr;
    if(m_callback) {
        // user provided callback
        callback = m_callback;
    } else if(m_sink) {
        // using events. This object will get deleted when the process exits
        callback = new ConsoleProcess(m_sink, m_callbackUID);
    }

    clDEBUG() << "Console: running command: `" << command << "`" << endl;

    SetPid(::wxExecute(command, wxEXEC_ASYNC | wxEXEC_MAKE_GROUP_LEADER | GetExecExtraFlags(), callback));
    // reset the m_callback (it will auto-delete itself)
    m_callback = nullptr;
    m_sink = nullptr;
    m_callbackUID.clear();
    return (GetPid() > 0);
}

wxString clConsoleBase::GetSelectedTerminalName()
{
    wxString terminalName = clConfig::Get().Read("Terminal", wxString());
    if(terminalName.empty()) {
#ifdef __WXGTK__
        wxFileName file;
        terminalName = "gnome-terminal";
#elif defined(__WXOSX__)
        terminalName = "Terminal";
#else
        terminalName = "CMD";
#endif
    }
    return terminalName;
}

clConsoleEnvironment::clConsoleEnvironment() {}

clConsoleEnvironment::~clConsoleEnvironment() { UnApply(); }

void clConsoleEnvironment::Add(const wxString& name, const wxString& value)
{
    m_environment.erase(name);
    m_environment.insert({ name, value });
}

void clConsoleEnvironment::Apply()
{
    if(!m_oldEnvironment.empty()) {
        clWARNING() << "Refusing to apply environment. Already in a dirty state";
        return;
    }
    if(m_environment.empty()) {
        return;
    }

    // keep a copy of the old environment before we apply the new values
    m_oldEnvironment.clear();
    std::for_each(m_environment.begin(), m_environment.end(), [&](const wxStringMap_t::value_type& vt) {
        wxString envvalue;
        if(::wxGetEnv(vt.first, &envvalue)) {
            m_oldEnvironment[vt.first] = envvalue;
        } else {
            m_oldEnvironment[vt.first] = "__no_such_env__";
        }
        ::wxSetEnv(vt.first, vt.second);
    });
}

void clConsoleEnvironment::UnApply()
{
    if(m_oldEnvironment.empty()) {
        return;
    }
    std::for_each(m_oldEnvironment.begin(), m_oldEnvironment.end(), [&](const wxStringMap_t::value_type& vt) {
        if(vt.second == "__no_such_env__") {
            ::wxUnsetEnv(vt.second);
        } else {
            ::wxSetEnv(vt.first, vt.second);
        }
    });
    m_oldEnvironment.clear();
}

clConsoleEnvironment::clConsoleEnvironment(const wxStringMap_t& env)
    : m_environment(env)
{
}

#define ADD_CURRENT_TOKEN()          \
    if(!curtoken.IsEmpty()) {        \
        curtoken.Trim().Trim(false); \
        if(!curtoken.IsEmpty()) {    \
            outputArr.Add(curtoken); \
        }                            \
        curtoken.Clear();            \
    }

wxArrayString clConsoleBase::SplitArguments(const wxString& args)
{
    const int STATE_NORMAL = 0;
    const int STATE_STRING = 1;

    int state = STATE_NORMAL;
    wxArrayString outputArr;
    wxString curtoken;
    wxChar prevChar = 0;
    for(size_t i = 0; i < args.size(); ++i) {
        wxChar ch = args[i];
        switch(state) {
        case STATE_NORMAL: {
            switch(ch) {
            case ' ':
            case '\t':
                if(prevChar == '\\') {
                    curtoken << ch;
                } else {
                    ADD_CURRENT_TOKEN();
                }
                break;
            case '"':
            case '\'':
                // we dont want to keep the string markers
                state = STATE_STRING;
                break;
            default:
                curtoken << ch;
                break;
            }
        } break;
        case STATE_STRING: {
            switch(ch) {
            case '"':
            case '\'':
                if(prevChar == '\\') {
                    curtoken << ch;
                } else {
                    // we dont want to keep the string markers
                    state = STATE_NORMAL;
                }
                break;
            default:
                curtoken << ch;
                break;
            }
        } break;
        default:
            break;
        }
        prevChar = ch;
    }
    // if we still got some unprocessed token, add it
    ADD_CURRENT_TOKEN();
    return outputArr;
}

void clConsoleBase::SetEnvironment(const clEnvList_t& environment)
{
    // convert the list into map
    m_environment.clear();
    for(const auto& p : environment) {
        m_environment.insert({ p.first, p.second });
    }
}

void clConsoleBase::MacAddArgsIfNeeded(wxString* outcmd)
{
    if(!IsTerminalNeeded()) {
        return;
    }

#ifndef __WXMAC__
    wxUnusedVar(outcmd);
#else
    if(!GetWorkingDirectory().empty() || !GetCommand().empty()) {
        if(!outcmd->empty()) {
            *outcmd << " ";
        }
        *outcmd << "--args ";
    }
#endif
}
