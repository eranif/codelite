#include "LanguageServerEntry.h"

#include "JSON.h"
#include "StringUtils.h"

LanguageServerEntry::LanguageServerEntry()
    : m_connectionString("stdio")
{
}

void LanguageServerEntry::FromJSON(const JSONItem& json)
{
    m_name = json.namedObject("name").toString();
    m_exepath = json.namedObject("exepath").toString();
    m_args = json.namedObject("args").toString();
    m_workingDirectory = json.namedObject("workingDirectory").toString();
    m_languages = json.namedObject("languages").toArrayString();
    m_enabled = json.namedObject("enabled").toBool(m_enabled);
    m_connectionString = json.namedObject("connectionString").toString("stdio");
    m_displayDiagnostics = json.namedObject("displayDiagnostics").toBool(m_displayDiagnostics); // defaults to true
    m_initOptions = json["initOptions"].toString();

    // we no longer are using exepath + args, instead a single "command" is used
    wxString commandDefault = m_exepath;
    if (!commandDefault.IsEmpty()) {
        StringUtils::WrapWithQuotes(commandDefault);
        if (!m_args.empty()) {
            commandDefault << " " << m_args;
        }
    }
    m_command = json.namedObject("command").toString(commandDefault);
}

JSONItem LanguageServerEntry::ToJSON() const
{
    return nlohmann::json{{"name", m_name.ToStdString(wxConvUTF8)},
                          {"exepath", m_exepath.ToStdString(wxConvUTF8)},
                          {"args", m_args.ToStdString(wxConvUTF8)},
                          {"languages", JsonUtils::ToJson(m_languages)},
                          {"enabled", m_enabled},
                          {"workingDirectory", m_workingDirectory.ToStdString(wxConvUTF8)},
                          {"connectionString", m_connectionString.ToStdString(wxConvUTF8)},
                          {"displayDiagnostics", m_displayDiagnostics},
                          {"command", m_command.ToStdString(wxConvUTF8)},
                          {"initOptions", m_initOptions.ToStdString(wxConvUTF8)}};
}

eNetworkType LanguageServerEntry::GetNetType() const
{
    wxString connectionString = GetConnectionString();
    connectionString.Trim().Trim(false);
    if (connectionString.CmpNoCase("stdio") == 0) {
        return eNetworkType::kStdio;
    } else {
        return eNetworkType::kTcpIP;
    }
}

bool LanguageServerEntry::IsNull() const { return m_name.empty(); }
bool LanguageServerEntry::IsBroken() const
{
    auto argv = StringUtils::BuildArgv(GetCommand());
    if (argv.empty()) {
        return true;
    }

    // Check that the first argument (the executable path) exists
    return IsEnabled() && !wxFileName::FileExists(argv[0]);
}

bool LanguageServerEntry::IsAutoRestart() const
{
    wxString command = GetCommand();
    command.Trim().Trim(false);
    return !command.IsEmpty();
}

wxString LanguageServerEntry::GetCommand(bool pretty) const
{
    auto cmd_arr = StringUtils::BuildCommandArrayFromString(m_command);
    return StringUtils::BuildCommandStringFromArray(
        cmd_arr, pretty ? StringUtils::WITH_COMMENT_PREFIX : StringUtils::ONE_LINER);
}

void LanguageServerEntry::SetCommand(const wxString& command)
{
    auto cmd_arr = StringUtils::BuildCommandArrayFromString(command);
    m_command = StringUtils::BuildCommandStringFromArray(cmd_arr);
}

namespace
{
wxString format_json_str(const wxString& str, bool pretty)
{
    if (str.empty()) {
        return wxEmptyString;
    }

    JSON root{str};
    if (!root.isOk()) {
        return wxEmptyString;
    }
    return root.toElement().format(pretty);
}
} // namespace

void LanguageServerEntry::SetInitOptions(const wxString& options) { m_initOptions = format_json_str(options, false); }

wxString LanguageServerEntry::GetInitOptions() const { return format_json_str(m_initOptions, true); }