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

    // we no longer are using exepath + args, instead a single "command" is used
    wxString commandDefault = m_exepath;
    if(!commandDefault.IsEmpty()) {
        ::WrapWithQuotes(commandDefault);
        if(!m_args.empty()) { commandDefault << " " << m_args; }
    }
    m_command = json.namedObject("command").toString(commandDefault);
    m_unimplementedMethods.clear();
    wxArrayString methods = json.namedObject("unimplementedMethods").toArrayString();
    for(const wxString& methodName : methods) {
        m_unimplementedMethods.insert(methodName);
    }
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

    wxArrayString methods;
    methods.Alloc(m_unimplementedMethods.size());
    for(const wxString& methodName : m_unimplementedMethods) {
        methods.Add(methodName);
    }

    json.addProperty("unimplementedMethods", methods);
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
    bool is_valid = true;
    if(m_name.IsEmpty()) { return false; }
    return true;
}

void LanguageServerEntry::AddUnImplementedMethod(const wxString& methodName)
{
    m_unimplementedMethods.insert(methodName);
}

bool LanguageServerEntry::IsAutoRestart() const
{
    wxString command = GetCommand();
    command.Trim().Trim(false);
    return !command.IsEmpty();
}
