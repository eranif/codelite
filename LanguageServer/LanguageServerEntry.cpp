#include "LanguageServerEntry.h"

#include "JSON.h"
#include "LSP/LanguageServerProtocol.h"
#include "StringUtils.h"
#include "globals.h"

LanguageServerEntry::LanguageServerEntry()
    : m_connectionString("stdio")
{
}

LanguageServerEntry::~LanguageServerEntry() {}

void LanguageServerEntry::FromJSON(const JSONItem& json)
{
    m_name = json.namedObject("name").toString();
    m_exepath = json.namedObject("exepath").toString();
    m_args = json.namedObject("args").toString();
    m_workingDirectory = json.namedObject("workingDirectory").toString();
    m_languages = json.namedObject("languages").toArrayString();
    m_enabled = json.namedObject("enabled").toBool(m_enabled);
    m_connectionString = json.namedObject("connectionString").toString("stdio");
    m_disaplayDiagnostics = json.namedObject("displayDiagnostics").toBool(m_disaplayDiagnostics); // defaults to true
    m_initOptions = json["initOptions"].toString();

    // we no longer are using exepath + args, instead a single "command" is used
    wxString commandDefault = m_exepath;
    if(!commandDefault.IsEmpty()) {
        ::WrapWithQuotes(commandDefault);
        if(!m_args.empty()) {
            commandDefault << " " << m_args;
        }
    }
    m_command = json.namedObject("command").toString(commandDefault);
}

JSONItem LanguageServerEntry::ToJSON() const
{
    JSONItem json = JSONItem::createObject();
    json.addProperty("name", m_name);
    json.addProperty("exepath", m_exepath);
    json.addProperty("args", m_args);
    json.addProperty("languages", m_languages);
    json.addProperty("enabled", m_enabled);
    json.addProperty("workingDirectory", m_workingDirectory);
    json.addProperty("connectionString", m_connectionString);
    json.addProperty("displayDiagnostics", m_disaplayDiagnostics);
    json.addProperty("command", m_command);
    json.addProperty("initOptions", m_initOptions);
    return json;
}

eNetworkType LanguageServerEntry::GetNetType() const
{
    wxString connectionString = GetConnectionString();
    connectionString.Trim().Trim(false);
    if(connectionString.CmpNoCase("stdio") == 0) {
        return eNetworkType::kStdio;
    } else {
        return eNetworkType::kTcpIP;
    }
}

bool LanguageServerEntry::IsNull() const { return m_name.empty(); }

bool LanguageServerEntry::IsAutoRestart() const
{
    wxString command = GetCommand();
    command.Trim().Trim(false);
    return !command.IsEmpty();
}

wxString LanguageServerEntry::GetCommand(bool pretty) const
{
    auto cmd_arr = StringUtils::BuildCommandArrayFromString(m_command);
    return StringUtils::BuildCommandStringFromArray(cmd_arr,
                                                    pretty ? StringUtils::WITH_COMMENT_PREFIX : StringUtils::ONE_LINER);
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
    if(str.empty()) {
        return wxEmptyString;
    }

    JSON root{ str };
    if(!root.isOk()) {
        return wxEmptyString;
    }
    return root.toElement().format(pretty);
}
} // namespace

void LanguageServerEntry::SetInitOptions(const wxString& options) { m_initOptions = format_json_str(options, false); }

wxString LanguageServerEntry::GetInitOptions() const { return format_json_str(m_initOptions, true); }