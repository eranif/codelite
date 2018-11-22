#include "commandlineparser.h"
#include <algorithm>
#include <wx/debug.h>

CommandLineParser::CommandLineParser(int argc, char** argv)
    : m_argc(argc)
    , m_argv(argv)
    , m_curIndex(1)
    , m_used(false)
{
}

CommandLineParser::~CommandLineParser() {}

wxString CommandLineParser::GetArg(const wxString& shortOpt, const wxString& longOption) const
{
    if(m_argsMap.count(shortOpt)) {
        return m_argsMap.find(shortOpt)->second;
        ;
    }
    if(m_argsMap.count(longOption)) {
        return m_argsMap.find(longOption)->second;
        ;
    }
    return "";
}

void CommandLineParser::DoParse()
{
    wxASSERT_MSG(!m_used,
                 "CommandLineParser can be used only once. Please constuct another instance and use it instead");

    if(m_argc <= 1) return;

    // when we see the "--" we consider the rest as the command
    while(HasMore()) {
        wxString optionName = NextToken();
        if(optionName == "--cmd") {
            // Stop parsing, from this point on, everything is the command to execute
            while(HasMore()) {
                wxString cmdToken = NextToken();
                if(cmdToken.Contains(" ")) { cmdToken.Prepend("\"").Append("\""); }
                m_command << cmdToken << " ";
            }
            break;

        } else {
            CommandLineParser::OptionInfo oi = GetOptionInfo(optionName);
            if(oi.IsOk()) {
                if(oi.GetFlags() & CommandLineParser::kOptionWithValue) {
                    wxString value = NextToken();
                    m_argsMap.insert(std::make_pair(StripDash(optionName), value));

                } else {
                    m_argsMap.insert(std::make_pair(StripDash(optionName), ""));
                }
            }
        }
    }
}

bool CommandLineParser::HasMore() { return (m_curIndex < m_argc); }

wxString CommandLineParser::NextToken()
{
    wxASSERT(HasMore());
    return m_argv[m_curIndex++];
}

wxString CommandLineParser::PeekNextToken()
{
    wxASSERT(HasMore());
    return m_argv[m_curIndex];
}

bool CommandLineParser::HasOption(const wxString& shortOpt, const wxString& longOption) const
{
    return m_argsMap.count(shortOpt) || m_argsMap.count(longOption);
}

void CommandLineParser::AddOption(const wxString& shortOpt, const wxString& longOpt, size_t flags)
{
    OptionInfo oi(longOpt, shortOpt);
    oi.SetFlags(flags);
    m_optionsInfo.push_back(oi);
}

CommandLineParser::OptionInfo CommandLineParser::GetOptionInfo(const wxString& name) const
{
    CommandLineParser::OptionInfo::List_t::const_iterator iter =
        std::find_if(m_optionsInfo.begin(), m_optionsInfo.end(), CommandLineParser::OptionInfo(name));
    if(iter == m_optionsInfo.end()) { return CommandLineParser::OptionInfo(); }
    return *iter;
}

wxString CommandLineParser::StripDash(const wxString& opt) const
{
    wxString noPrefix(opt);
    noPrefix.StartsWith("--", &noPrefix);
    noPrefix.StartsWith("-", &noPrefix);
    noPrefix.StartsWith("/", &noPrefix);
    return noPrefix;
}
