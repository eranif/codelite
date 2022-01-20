#include "LanguageServerEntry.h"

#include "LanguageServerProtocol.h"
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
    m_priority = json.namedObject("priority").toInt(m_priority);
    m_disaplayDiagnostics = json.namedObject("displayDiagnostics").toBool(m_disaplayDiagnostics); // defaults to true
    m_remoteLSP = json["remoteLSP"].toBool(m_remoteLSP);
    m_sshAccount = json["sshAccount"].toString(m_sshAccount);

    // we no longer are using exepath + args, instead a single "command" is used
    wxString commandDefault = m_exepath;
    if(!commandDefault.IsEmpty()) {
        ::WrapWithQuotes(commandDefault);
        if(!m_args.empty()) {
            commandDefault << " " << m_args;
        }
    }

    // read the environment variables
    auto env = json["environment"];
    size_t envSize = env.arraySize();
    for(size_t i = 0; i < envSize; ++i) {
        wxString envline = env[i].toString();
        if(envline.IsEmpty()) {
            continue;
        }
        wxString env_name = envline.BeforeFirst('=');
        wxString env_value = envline.AfterFirst('=');
        if(env_name.empty() || env_value.empty()) {
            continue;
        }
        m_env.push_back({ env_name, env_value });
    }

    m_command = json.namedObject("command").toString(commandDefault);
    m_initOptions = json["initOptions"].toString();
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
    json.addProperty("priority", m_priority);
    json.addProperty("displayDiagnostics", m_disaplayDiagnostics);
    json.addProperty("command", m_command);
    json.addProperty("initOptions", m_initOptions);
    json.addProperty("remoteLSP", m_remoteLSP);
    json.addProperty("sshAccount", m_sshAccount);

    // Write the environment variables
    wxArrayString envArr;
    for(const auto& env_entry : m_env) {
        envArr.Add(env_entry.first + "=" + env_entry.second);
    }
    json.addProperty("environment", envArr);
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

bool LanguageServerEntry::IsValid() const
{
    if(m_name.IsEmpty()) {
        return false;
    }
    return true;
}

bool LanguageServerEntry::IsAutoRestart() const
{
    wxString command = GetCommand();
    command.Trim().Trim(false);
    return !command.IsEmpty();
}
